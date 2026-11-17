# Photoshop Touch — Roadmap de Modernização (GLES 3, ARM64, Ferramentas, UI)

Documento 4. Consolida as quatro frentes do projeto e como elas se encaixam.
Complementa: doc 1 (identidade/segurança), doc 2 (arquitetura/modding), doc 3 (port ARM64).

As quatro frentes **convergem num único ponto**: a reconstrução da extensão nativa de imagem
para arm64. É nela que entram GLES 3.0, as ferramentas novas e a base pra UI moderna. Por
isso o plano abaixo trata tudo como um esforço só, em fases.

---

## Frente 1 — OpenGL ES 2.0 → 3.0 (remover a limitação dos filtros)

**Situação atual:** os 47 shaders são **GLSL ES 1.00** (GLES2) — nenhum tem `#version`,
usam `varying`/`texture2D`, `precision highp float`. Limitações que te travam:
- sem `texelFetch` (amostragem exata por pixel inteiro);
- sem operações com inteiros / bit a bit;
- loops só com limite constante;
- sem múltiplos render targets, sem transform feedback;
- precisão e acesso a texturas restritos.

**O que o GLES 3.0 destrava** (suportado pela grande maioria dos celulares desde ~2013):
- `#version 300 es`, `in/out`, `texture()`, `texelFetch`, inteiros, `textureSize`;
- múltiplos render targets (MRT), UBOs, instancing;
- com **GLES 3.1** ainda: **compute shaders** → filtros pesados (blur grande, convolução,
  histograma, seleção) em paralelo, sem gambiarra de ping-pong de textura.

**Como encaixa:** o upgrade é praticamente **de graça dentro do port**, porque vamos
reescrever o `ECGLContext` de qualquer forma (Frente 2). Basta o contexto EGL pedir
`EGL_OPENGL_ES3_BIT` e os `.fs` migrarem para `#version 300 es`. Estratégia recomendada:
- novo contexto GLES 3.0 (fallback 2.0 em aparelhos antigos);
- migrar os 47 shaders para `300 es` (mecânico: `varying`→`in/out`, `texture2D`→`texture`,
  declarar `out vec4 fragColor`);
- habilitar um caminho **GLES 3.1 compute** para os filtros novos mais pesados.

> Decisão a tomar: alvo mínimo **GLES 3.0** (cobertura quase total) e usar **3.1 compute**
> como "turbo" opcional onde o device suportar.

---

## Frente 2 — Port ARM64 + reconstrução da ANE (o eixo central)

Já detalhado no doc 3. Resumo: SWF reusa como está (CPU-agnóstico); runtime AIR é trocado
por um AIR 64-bit (HARMAN); a **extensão nativa (122 funções JNI) é reimplementada** para
arm64. O Ghidra entra aqui: estou decompilando `libTTPixelExtensionAndroid.so` para C, para
documentar o que cada função faz e guiar a reimplementação — especialmente os 3 algoritmos
proprietários (alpha matting, scribble, quick-selection).

**Sinergia:** ao reescrever a ANE nós já (a) entramos em 64 bits, (b) subimos para GLES 3,
(c) ganhamos o lugar certo para plugar ferramentas novas (Frente 3).

---

## Frente 3 — Ferramentas novas, usando o PhotoCraft como referência

`storytold/photocraft` ("ArtCraft/PhotoCraft") é uma **reimplementação clean-room do
Photoshop em Rust**, licença **MIT OR Apache-2.0** (permissiva: podemos estudar e **adaptar
com atribuição**; não copiamos trechos grandes verbatim). É nativo wgpu/WGSL, então serve de
**referência algorítmica** — reescrevemos a lógica como shader GLSL / processor AS3 no Touch.

**O que dá pra trazer (mapa de origem → destino):**

| Ferramenta (PhotoCraft) | Onde está lá | Como entra no Touch |
|---|---|---|
| **Blend modes** completos (Color Dodge/Burn, Vivid/Linear/Pin Light, Hard Mix, Hue, Saturation, Color, Luminosity, Difference, Exclusion, Divide...) | `crates/compose/src/psblend.rs`, `crates/color/src/blend.rs` | Shader de composição GLES3 + opção no `TTLayerView` (hoje só tem 9 modos) |
| **Curves** (ponto-a-ponto) | `adjust_editors/curves.rs`, `widgets/point_curve.rs` | Já existe `TTCurvesProcessor`; estender a curva/UI |
| **Levels** (com clipping) | `adjust_editors/levels.rs`, `levels_clipping.rs` | `TTLevelsProcessor` + shader |
| **Hue/Saturation** | `adjust_editors/hue_saturation.rs` | novo `.fs` + processor |
| **Selective Color** | `adjust_editors/selective_color.rs` | novo `.fs` + processor |
| **Gradient Map** | `adjust_editors/gradient_map.rs` | já há `TTGradientProcessor`; estender |
| **Brush engine** (pressão, dynamics) | `crates/paint/` | casa com a ANE de pressão existente |
| **PSD real** (camadas/máscaras) | `crates/format/` (psd) | melhora import/export além do `.psdx` |

**Os blend modes são o ganho mais rápido e visível:** a matemática é padrão (não
copyrightável), e no Touch hoje só há 9 modos — adicionar os ~16 restantes do Photoshop é um
único shader de composição parametrizado + entradas na UI. Ótimo primeiro "feature" pós-port.

---

## Frente 4 — Modernizar a UI (tirar o visual de 2014)

A estética é controlável por **dados**, não por código — melhor do que parecia:

1. **`theme.cabres`** (XML de tema, editável): define `smallFontSize/normalFontSize/
   largeFontSize`, `fontHeight`, `darkTheme`, `hiliteColor` (hoje `1174C7`),
   alphas (disabled/hilite/selected), margens e offsets de janela/aba/dock. ➜ Ajustar
   paleta, tipografia e espaçamento aqui já renova muito.
2. **SWFs de tema** (gráficos vetoriais): `cab/controls.swf` (skins de botão),
   `toolicons.swf`, `panelicons.swf`, `cursor.swf`, `welcome.swf`. ➜ Redesenhar ícones
   e skins modernos (flat, cantos, contraste) substituindo esses SWFs.
3. **`.cabmod`** (layouts): posições/tamanhos `x/y/width/height`, `sizeX/sizeY=scale`. ➜
   Respiro entre controles, toque maior (targets ≥ 48dp), reorganização de painéis.

**Limites honestos da UI:** é um layout Flash de coordenadas fixas — não é CSS/flexbox. Dá
pra fazer um *refresh* forte (cores, tipografia, ícones flat, espaçamento, densidade de
toque), mas um redesign totalmente fluido/responsivo tem teto. O salto maior de UX só viria
trocando a camada de UI — fora do escopo de um *reskin*.

---

## Plano unificado em fases

**Fase A — Fundação do port (destrava tudo).**
- Projeto AIR 64-bit (HARMAN), empacotar `TTPixel.swf` + assets.
- Ghidra → documentar a ANE; definir contrato JNI (122 funções).
- ANE stub que faz o app **abrir** em arm64.

**Fase B — Motor de imagem em GLES 3.**
- Reimplementar `ECGLContext` (contexto GLES 3.0) + `ECGLFilter`.
- Migrar os 47 `.fs` para `#version 300 es`.
- `ECUtils` + codecs (JPEG/PNG/LZ4/zlib). ➜ editar, filtrar e salvar funcionando.

**Fase C — Ferramentas novas (PhotoCraft como guia).**
- Blend modes completos (1º feature, alto impacto).
- Curves/Levels/Hue-Sat/Selective Color estendidos.
- (3.1 compute) filtros pesados novos.

**Fase D — Seleção inteligente.**
- Reimplementar/aproximar matting, scribble, quick-select (a parte incerta).

**Fase E — UI moderna.**
- Novo `theme.cabres` (paleta/tipografia), ícones SWF redesenhados, `.cabmod` com
  espaçamento/toque modernos.

**Fase F — Acabamento.**
- Remover integrações mortas (CC/IMS/push), scoped storage, targetSdk novo, assinatura v2/v3.

---

## Próximos passos imediatos (nesta sessão)

1. ⏳ **Ghidra** terminando a decompilação da `libTTPixelExtensionAndroid.so` → vou entregar
   o `.c` decompilado + um resumo por função (foco em GL e matting).
2. Posso já **migrar 1 shader** para `#version 300 es` como prova de conceito (ex.: `threshold.fs`).
3. Posso escrever o **shader de blend modes completo** (GLES3) adaptando a matemática padrão
   (referência PhotoCraft, com atribuição) — o primeiro feature novo.
4. Posso montar um **`theme.cabres` modernizado** (paleta + tipografia) pra você ver o reskin.

Me diz qual desses quer ver primeiro assim que o Ghidra fechar.

---

### Nota de licença/atribuição
PhotoCraft é MIT/Apache-2.0. Qualquer algoritmo adaptado dele entra com crédito no `NOTICE`
do projeto. Fórmulas matemáticas de blend/ajuste são padrão de domínio público. Nada de
copiar código Rust verbatim — reimplementamos em GLSL/AS3.
