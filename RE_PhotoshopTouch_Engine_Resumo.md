# Photoshop Touch: resumo da engine nativa (`libTTPixelExtensionAndroid.so`)

Documento 5 · **Trilho A (privado, referência)**. Resumo em palavras nossas do que a
decompilação do Ghidra mostrou: 3.183 funções, das quais 122 são a ponte JNI. Serve para
duas coisas:
1. guiar uma ANE arm64 que substitua esta (port privado do app original);
2. dar o **blueprint de comportamento** para a engine clean-room do Telar, **sem copiar
   código**. Cada módulo abaixo termina com "Como refazer", apontando a literatura pública ou
   a biblioteca livre que resolve o mesmo problema.

Fontes: `ttpixel_decompiled.c` (scratchpad, 7,5 MB), `RE_TTPixel_JNI_decompiled.c` (repo),
`strings` do binário.

---

## 1. Do que a biblioteca é feita

Só uma parte pequena do `.so` é código próprio da Adobe. O resto são bibliotecas de terceiros
compiladas estaticamente, muitas com os símbolos renomeados:

| Componente | Evidência | Funções (aprox.) | No port / no Telar |
|---|---|---|---|
| **OpenCV 1.x/2.x, API C** (`cv*`, `icv*`) | `cvKMeans2`, `cvCreateGraph`, `<opencv_storage>`, distance transform | ~1.000 | Só usado pelo matting e pelo scribble. No Telar: código próprio ou OpenCV 4 (Apache-2.0) |
| **libjpeg 6b** renomeada `ttpixel_jpeg_*` | string `6b 27-Mar-1998` | ~150 | **libjpeg-turbo** (BSD/IJG), que tem NEON em arm64 |
| **libpng 1.5.2** | `libpng version 1.5.2 - March 31, 2011` | ~120 | libpng 1.6 atual |
| **zlib 1.2.3** | `deflate*`, `inflate*` | ~60 | zlib do NDK |
| **LZ4 (r-series antiga)** | `LZ4_compressCtx`, `LZ4_compress64kCtx` | ~10 | lz4 1.9+ (BSD), formato de bloco compatível |
| **STLport** | `libstlport_shared.so` ao lado | — | libc++ do NDK |
| **Código Adobe** (`EC*`, `qs*`, `MWSegment`, `CMatteSolver`…) | nomes de classe, mensagens de log | ~600 | É isto que este documento resume |

Conclusão prática: das ~3.200 funções, só umas 600 precisam de reimplementação de verdade, e
dessas a maior parte é o encanamento de GL e de bitmap. O "cérebro" são quatro algoritmos de
seleção (seção 5).

---

## 2. Mapa de módulos (código Adobe)

```
JNI (122)  ─┬─ gl.ECGLContext / ECGLFilter ──► ECGL*        (motor de filtros GPU)
            ├─ utils.ECUtils / ImpExp ───────► ECBitmap*, ECAsync*Exporter, ECLz4*, ECPreMultiply*
            ├─ ByteBufferFactory ────────────► buffers diretos (alloc/copy/swap/flip)
            ├─ bigdata.ECBitmapPreflight ────► ECBitmapPreflight* (pré-checagem de imagens grandes)
            ├─ ContextQuickSelection ────────► QuickSelectTool, BrushHit, qsBrushHit, TiledImageGraph
            ├─ am.ECAlphaMatting ────────────► MultiScaleMatting → patchMatting → CMatteSolver
            ├─ ss.ECScribbleSegment ─────────► addOneScribble → GMM + geoSegment (geodésico)
            ├─ httpd.Httpd ──────────────────► ECHttpd* (servidor de arquivos na rede local)
            └─ ContextPressureJaJa ──────────► fila de eventos de caneta
(interno)       MWSegmentSeedFill<…>  → preenchimento por semente (varinha mágica / lata de tinta)
```

---

## 3. Motor de filtros GPU (`ECGL*`): como funciona

**Contexto.** `ECGLCreateContext` abre um display EGL, liga a API OpenGL ES, escolhe uma config
e cria um contexto **fora da tela** (pbuffer). A UI do AIR não desenha com este contexto: ele
só processa imagens. Os erros saem como códigos negativos próprios (`-1005`, `-1200`…) mais o
último `eglGetError`, guardados na struct do contexto.

**Workarounds.** `ECGLGetWorkarounds` lê `GL_EXTENSIONS` uma vez e liga uma flag quando
encontra uma extensão específica, usada para contornar bug de driver. No GLES 3 isso tende a
desaparecer, já que o núcleo exigido é maior.

**Texturas.** `ECGLTexturePrepare` cria texturas RGBA 8 bits com wrap `CLAMP_TO_EDGE` e filtro
`NEAREST`/`LINEAR` configurável, **arredondando largura e altura para a próxima potência de
dois**. É a marca registrada do GLES 2 em GPUs antigas (NPOT limitado) e desperdiça até 4× de
memória numa foto de 2048×1536 (vira 2048×2048). **No GLES 3, NPOT é nativo: some.**

**Programa / parâmetros.** `ECGLShaderCreate(Ex)` compila os `.fs`/`.vs` que vêm do SWF,
`ECGLProgramCreate` linka, e `ECGLSetParam{Float,Int,FloatMulti,IntMulti,FloatMatrix}`
alimenta os uniforms pelo nome. As matrizes de vértice são calculadas na CPU
(`ECGLMatrixSetOrthographic`, `Multiply`, `Identity`).

**Execução de um filtro (`ECGLApplyFilter`).** É um **ping-pong de duas texturas**:
1. o sampler de entrada é ligado na unidade 0;
2. para cada passe, a textura de destino é presa num framebuffer (`glFramebufferTexture2D`),
   o status do FBO é conferido, limpa, `glDrawArrays` com o quad;
3. o resultado vira entrada do passe seguinte, alternando as unidades 1 e 2;
4. o **último passe** desenha num **renderbuffer**, que é o que `ECGLReadPixels(Rect)` lê de
   volta para a CPU.

**Sequências e threads.** `ECGLExecuteSequence` roda uma lista de filtros em série;
`ECGLAsyncExecuteSequence` faz o mesmo numa `pthread` própria, com `Join`/`Interrupt` para o AS3
esperar ou cancelar, e `ECGLAsyncReadPixels` para ler sem travar a UI. O estado do GL
(programa, texturas, viewport) é salvo e restaurado em volta de cada chamada
(`Get/SetServerState`), porque o contexto é compartilhado entre chamadas do AS3.

**Como refazer (Telar / ANE arm64):**
- contexto EGL **ES 3.0** (`EGL_OPENGL_ES3_BIT`), com 3.1 opcional para compute;
- texturas de tamanho exato (sem potência de dois) e **RGBA16F** como formato de trabalho, o
  que acaba com o banding em ajustes encadeados;
- ping-pong mantido, mas com `glReadPixels` assíncrono via **PBO**, no lugar da thread que
  copia;
- para filtros pesados (blur grande, histograma), compute shader no 3.1.

---

## 4. Bitmap, codecs e I/O

- `ECBitmapData*` / `ECBitmapFile*`: cópia, reamostragem (`Resample`), leitura e gravação de um
  formato bruto próprio de bitmap em arquivo, usado como cache de camada em disco.
- `ECPreMultiplyAlpha*` e `ECAlphaBlend`: premultiplicar/despremultiplicar, e composição
  simples na CPU.
- `ECAsync{JPEG,PNG,ZLib,Lz4,Bitmap}Exporter`: uma classe base `ECAsyncExporter` com um
  `ECAsyncWorker` em thread. Cada subclasse codifica em segundo plano e informa progresso, que
  o AS3 consulta por polling.
- `ECBitmapPreflight*`: antes de abrir uma imagem grande, estima memória e decide se cabe ou se
  precisa ser processada em blocos. Também roda assíncrono e devolve um relatório em texto
  (`OutputToString`).
- `AndroidBitmap_*` e `ANativeWindow_*`: ponte direta com `android.graphics.Bitmap` e com a
  superfície de vídeo.
- `ECUtils*`: contagem de CPUs, detecção de SIMD (NEON), cronômetros para profiling.

**Como refazer:** puro padrão. libjpeg-turbo, libpng, zlib, lz4, `std::thread`, e o formato de
cache de camada passa a ser nosso (por exemplo, blocos 256×256 comprimidos com LZ4).

---

## 5. Os quatro algoritmos de seleção

### 5.1 Seleção rápida (`QuickSelectTool`, `BrushHit`, `qsBrushHit`)

**O que faz:** você pincela sobre o objeto e a seleção "gruda" nas bordas enquanto o dedo anda.

**Como faz, pelo grafo de chamadas:**
1. **Ao criar a ferramenta:** a imagem é reduzida para uma pirâmide de resoluções
   (`DownsizeImage`, `GetHigherPyramidLevel`). Na resolução baixa é montado um **grafo com um
   nó por pixel** (`BuildNodesSegmentation`) e calculadas as energias (`SetGraphEnergies`).
2. **Energias:** os pesos entre vizinhos caem exponencialmente com a diferença de cor
   (aparecem `exp`/`pow` e tabelas de distância inversa para vizinhos diagonais), e as ligações
   com fonte e sorvedouro usam **log-verossimilhança** de modelos de cor (`log`). É a formulação
   clássica de **graph cut** para segmentação.
3. **A cada pincelada:** o pincel é "carimbado" na máscara como restrição rígida
   (`StampBrushOntoMask`), só as capacidades afetadas são atualizadas
   (`update_tlink_capacity`, `update_edge`) e o **max-flow é retomado de onde parou**
   (`add_to_active_list`: corte de grafo **dinâmico/incremental**, estilo Boykov-Kolmogorov).
   Por isso responde em tempo real.
4. **Refinamento de borda:** depois do corte em baixa resolução, `RefineEdgesSegmentation` pega
   só a faixa da fronteira, divide em **ladrilhos** (`TiledImageGraph`, `IndexedTiles`) e refaz o
   corte em resolução mais alta só ali, congelando o que já é certamente frente ou fundo
   (`BuildAndFreezeForeground/BackgroundNodeEnergiesAlong`).
5. `switchMode` alterna adicionar/subtrair; `getMask`/`setMask` trocam a máscara com o AS3.

**Literatura equivalente (pública):** Liu, Sun & Shum, *Paint Selection* (SIGGRAPH 2009):
seleção progressiva por pincel com graph cut multi-resolução e otimização local, exatamente
este comportamento. Boykov & Kolmogorov (2004) para o max-flow.
**Como refazer:** max-flow BK próprio (é curto) ou `maxflow`/`gridcut` com licença compatível;
pirâmide + faixa de borda como acima. Para o Telar dá para ir além e somar um modelo de
segmentação leve no aparelho (tipo "Selecionar assunto").

### 5.2 Alpha matting (`ECAlphaMatting` → `MultiScaleMatting` → `patchMatting` → `CMatteSolver`)

**O que faz:** dado um trimap (frente certa, fundo certo, faixa incerta), calcula a
transparência fina da borda, como cabelo e pelo.

**Como faz:** `MultiScaleMatting` resolve primeiro em escala reduzida e propaga para cima,
informando progresso (`setProgress`/`setSubProgress`). Em cada escala, `patchMatting` divide a
região incerta em **blocos** e chama `CMatteSolver::Run` em cada um. O nome, mais o uso de
janelas locais e a solução por sistema linear esparso, batem com **Closed-Form Matting** (Levin,
Lischinski & Weiss, 2008): o alfa é localmente uma função linear da cor, o que vira um sistema
laplaciano esparso.

**Como refazer:** implementação própria do closed-form em blocos (solver de gradiente
conjugado) ou, mais rápido no celular, **guided filter** (He et al., 2010) sobre o trimap, que
dá 90% do resultado a uma fração do custo e roda bem em shader.

### 5.3 Segmentação por rabisco (`ECScribbleSegment` → `addOneScribble`)

**O que faz:** você rabisca "isso é objeto" e "isso é fundo" e ele separa os dois.

**Como faz:**
1. `callGMMModel` → `trainGMM` treina **modelos de mistura de gaussianas** de cor para frente e
   fundo, inicializados por **k-means** (`CKMeanCluster`) e refinados por **EM** (`EMUpdate`).
2. `calGMMProbMap` gera um mapa de probabilidade por pixel.
3. `geoSegment` → `CalGeoDesiceDis` calcula a **distância geodésica** de cada pixel até cada
   rabisco sobre esse mapa (caminho "barato" dentro de regiões parecidas) e atribui cada pixel
   ao rabisco mais próximo.
4. A cada rabisco novo, só a região afetada é recalculada (`calUpdateRegion`,
   `getOneUpdateScribble`) e o resultado vira **trimap** (`initializeTrimap`, `combineTrimap`),
   que normalmente alimenta o matting (5.2) para acabar a borda.

**Literatura equivalente:** Bai & Sapiro, *A Geodesic Framework for Fast Interactive Image and
Video Segmentation and Matting* (ICCV 2007), com GMM de cor como em GrabCut (Rother et al.,
2004).
**Como refazer:** GMM + EM e distância geodésica (varredura rápida ou Dijkstra em grade) são
algoritmos de livro; cabem em algumas centenas de linhas próprias.

### 5.4 Preenchimento por semente (`MWSegmentSeedFill<…>`)

Template instanciado para **Gray, RGB e CMYK** em **8, 16 e 32 bits float**. É o flood fill
com tolerância da **varinha mágica** e da **lata de tinta**. Algoritmo de scanline padrão.
**Como refazer:** trivial; no Telar, uma única versão sobre RGBA16F.

---

## 6. O resto

- **`ECHttpd*`:** mini servidor HTTP com arquivo de senha (`ModifyPasswdFile`) para mandar
  arquivos pela rede local. Pode ser descartado no port. No Telar, compartilhamento pelo
  sistema (share sheet) resolve.
- **Pressão (`PressureJaJa`):** fila de eventos de caneta vinda do Java. No Telar vem direto de
  `MotionEvent.getPressure()`/`getToolType()`.

---

## 7. O que isto muda nos planos

| Item | Antes deste resumo | Agora |
|---|---|---|
| Esforço da ANE arm64 | "122 funções a reimplementar" | ~600 funções reais; 80% é encanamento GL/bitmap com substitutos prontos |
| Seleção inteligente | "CV proprietária, incerta" | 4 algoritmos **identificados** com artigo público equivalente: Paint Selection, Closed-Form Matting, segmentação geodésica + GMM, flood fill |
| GLES 3 | upgrade desejável | ganho concreto: some a textura potência de dois (até 4× menos memória), entra RGBA16F, PBO e compute |
| Telar (clean-room) | — | implementar pelos artigos, não pelo binário. Este doc serve só para confirmar **qual** técnica entrega o comportamento que os usuários do PS Touch conhecem |

**Regra do Trilho B:** quem escrever o código do Telar trabalha a partir dos artigos citados e
deste resumo de comportamento, **nunca** a partir do `.c` decompilado.
