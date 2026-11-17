# Photoshop Touch 1.3.7 — Avaliação de Port para ARM64 (64-bit)

Documento 3. Complementa os docs 1 (identidade/segurança) e 2 (arquitetura/modding).

**Pergunta:** dá para portar este app (hoje só ARM32) para ARM64?
**Resposta curta:** sim, mas não editando o `.so` atual. O port se divide em três
sub-problemas com dificuldades muito diferentes, e o trabalho real está em **uma** parte:
reconstruir a extensão nativa de imagem para arm64.

---

## 1. Situação confirmada

- Única ABI no APK: `lib/armeabi-v7a/` → **tudo ELF32 / Machine ARM**.
- Nenhum artefato `arm64-v8a` / `aarch64` / `x86_64` em lugar nenhum do pacote.
- As ANEs só embarcam a fatia `Android-ARM` (32-bit).
- Runtime AIR é *captive* (embutido como `libCore.so`).
- `minSdkVersion=14`, `targetSdkVersion=12` — extremamente antigo.

**Por que portar importa:** desde 2019 a Google Play exige 64 bits, e dispositivos
recentes **removeram** as bibliotecas 32-bit do sistema — nesses aparelhos um app só-ARM32
**não inicia**. Então o port é o que mantém o app executável no futuro.

---

## 2. O princípio central: três camadas, três "portes" diferentes

| Camada | Arquivo | Depende de CPU? | O que fazer |
|---|---|---|---|
| App ActionScript | `TTPixel.swf` (ABC) | ❌ Não | **Reusar como está** |
| Runtime AIR | `libCore.so` (18 MB) | ✅ Sim | **Substituir** por AIR 64-bit |
| Extensões nativas | `libTTPixelExtensionAndroid.so` (122 fn JNI), `libsibsynclib.so`, ANEs sync/push | ✅ Sim | **Reimplementar / descartar** |

### 2.1 App ActionScript — já é portável
Bytecode ABC roda na VM do AIR independentemente da CPU. As 1.209 classes que desmontei
**não precisam de port**. (No máximo, ajustes por APIs AIR deprecadas no runtime novo.)

### 2.2 Runtime AIR — troca, não porte
Não há fonte do `libCore.so`. Mas o AIR hoje é da **HARMAN**, cujo SDK gera runtime
**arm64-v8a**. Caminho: **reempacotar** o app com um AIR SDK moderno 64-bit (o runtime
inteiro é trocado por uma build oficial 64-bit). É um processo de build, não de engenharia
reversa do binário.

### 2.3 Extensões nativas — o trabalho de verdade
Um processo 64-bit **não carrega** `.so` de 32 bits. As ANEs só existem em ARM32 e **sem
fonte**. Como obter as versões arm64 é impossível (app descontinuado) e portar o binário à
mão é inviável, a saída é **reimplementar a ANE para arm64 contra a mesma interface JNII** —
que eu já mapeei por completo (seção 3).

---

## 3. A interface JNI completa a reimplementar — 122 funções

Levantei todos os símbolos exportados de `libTTPixelExtensionAndroid.so`. Isso é a
especificação exata do que uma extensão arm64 nova precisa prover para o SWF funcionar sem
alterações.

| Grupo (classe JNI) | Nº | Papel | Dificuldade de reimplementar |
|---|---|---|---|
| `gl.ECGLFilter` + `gl.ECGLContext` | ~45 | Plumbing OpenGL ES 2 (criar shader/program, texturas, draw, readPixels, execução sync/async) | 🟢 GL padrão — **a matemática dos filtros está nos `.fs`, fora do `.so`** |
| `utils.ECUtils` | ~30 | Bitmap: copy, resample, get/setPixels, alphaBlend, isolateColor, I/O de arquivo, LZ4, contagem de CPU, timestamp | 🟢 Padrão |
| `TTPixelExtensionContextImpExp` | ~12 | Encode/decode JPEG, PNG, ZLib, LZ4; premultiply/unpremultiply; progresso | 🟢 libjpeg-turbo / libpng / zlib / lz4 |
| `ByteBufferFactory` | 5 | Buffers diretos: alloc/copy/free, swap de canais, flip | 🟢 Padrão |
| `am.ECAlphaMatting` | 5 | Alpha matting (recorte fino de bordas) | 🔴 **Algoritmo proprietário de CV** |
| `ss.ECScribbleSegment` | 5 | Segmentação por "scribble" (seleção por rabisco) | 🔴 **Algoritmo proprietário de CV** |
| `...ContextQuickSelection` | ~12 | Seleção rápida / varinha inteligente (brushHit, mask, modos) | 🔴 **Algoritmo proprietário de CV** |
| `...ContextPressureJaJa` | 8 | Fila de eventos de pressão/botão da caneta | 🟡 Secundário |
| `httpd.Httpd` | 3 | Mini servidor HTTP embutido (transferência local de arquivos) | 🟡 Descartável ou nghttp/mongoose |
| `bigdata.ECBitmapPreflight` | 4 | Preflight de bitmaps grandes (tiling) | 🟡 Médio |
| `utils.AIRRuntimeHelper` | 2 | Bridge FRE ↔ BitmapData do AIR | 🟡 Depende do SDK FRE 64-bit |
| `FnCompressBitmapRLE` | 1 | Compressão RLE | 🟢 Trivial |
| `TTPixelExtension.initIDs` | 1 | Cache de IDs JNI na init | 🟢 Trivial |

**Leitura disto:** ~70% das funções (GL, bitmap, codecs, buffers) são **reimplementáveis
com bibliotecas padrão**, porque a inteligência dos filtros vive nos `.fs` editáveis. O
núcleo difícil são os **3 algoritmos de seleção inteligente** (matting, scribble, quick
select) — CV proprietária que precisaria ser reescrita ou aproximada (há literatura e libs
open-source de matting/graph-cut que servem de base).

`libsibsynclib.so` (sync CC) e as ANEs de sync/push apontam para **serviços da Adobe que já
não existem** → não precisam de port; devem ser **removidas ou stubadas**.

---

## 4. Plano de port em fases

**Fase 0 — Decisão de estratégia (sem código).**
Confirmar a rota "reempacotar com Harman AIR 64-bit + ANE nativa nova". Levantar licença do
AIR SDK atual e o SDK Android (NDK r26+ para arm64).

**Fase 1 — Esqueleto que sobe.**
1. Novo projeto AIR com Harman SDK (arm64-v8a + armeabi-v7a).
2. Empacotar o `TTPixel.swf` existente + assets como conteúdo.
3. ANE *stub*: implementar só as funções triviais (initIDs, ECUtils básico, ByteBuffer,
   GL context) para o app **iniciar** em 64 bits — mesmo sem todos os filtros.
   → Marco: app abre e mostra a UI num device arm64.

**Fase 2 — Pipeline de imagem.**
4. Reimplementar `ECGLContext` + `ECGLFilter` (GL ES 2 real) → **os 47 filtros `.fs`
   voltam a funcionar** (eles são dados, não código nativo).
5. Reimplementar `ECUtils` + `ImpExp` (bitmap ops + encode JPEG/PNG) → abrir/salvar/camadas.
   → Marco: editar, aplicar filtros, salvar — paridade de ~70%.

**Fase 3 — Seleção inteligente (o núcleo difícil).**
6. Reimplementar/aproximar `QuickSelection`, `AlphaMatting`, `ScribbleSegment`.
   → Marco: ferramentas de seleção inteligente.

**Fase 4 — Acabamento.**
7. Pressão da caneta, preflight big-data, (opcional) httpd.
8. Remover integrações mortas (CC sync/push/IMS); apontar import/export para local.
9. Bump de `targetSdk`, permissões modernas (scoped storage), assinatura v2/v3.

---

## 5. Esforço e riscos — honestamente

- **Fases 1–2** (app 64-bit abrindo + filtros + salvar): esforço **médio** e bem
  de-riscado, porque a interface está mapeada e a lógica dos filtros é externa ao binário.
- **Fase 3** (seleção inteligente): **alto** e incerto — é reescrever CV proprietária.
  Dá para entregar o resto sem ela e marcar essas ferramentas como "beta/aproximada".
- **Riscos reais:**
  - *Compatibilidade do SWF com AIR novo*: APIs Flash/AIR deprecadas podem exigir patches no
    ABC (eu tenho a desmontagem; patcho pontualmente).
  - *FRE 64-bit*: a bridge BitmapData↔nativo depende do header FRE do AIR 64-bit.
  - *Licenciamento*: redistribuir um app da Adobe tem implicações legais — isto faz sentido
    como projeto de estudo/uso próprio, não de redistribuição.
- **O que NÃO funciona:** editar o `libCore.so`/`.so` atuais para "virar 64-bit"
  (impossível), ou recompilar o SWF a partir da desmontagem (não é recompilável).

---

## 6. O primeiro passo concreto que eu posso dar agora

Para arrancar a Fase 1, o artefato-chave é a **especificação JNI exata** (assinaturas de
tipo de cada uma das 122 funções) — o "contrato" que a nova `.so` arm64 tem que cumprir.
Isso sai de duas fontes que eu já tenho acesso aqui:
1. os símbolos exportados (feito — as 122 funções);
2. o lado AS3 da ANE em `assets/META-INF/AIR/extensions/com.adobe.ttpixel.extension/
   library.swf` (declara os métodos nativos com seus tipos) — posso desmontar com o mesmo
   desmontador ABC.

E, para guiar a reimplementação dos grupos difíceis, posso **desmontar o ARM32** das funções
de `ECGLFilter`/`AlphaMatting` com `objdump -d` (roda aqui sem bloqueio) e documentar o que
cada uma faz.

**Quer que eu comece gerando:**
- (a) a especificação completa de assinaturas JNI (o header-contrato da ANE arm64), e/ou
- (b) a desmontagem ARM32 comentada das funções nucleares (GL + matting)?
