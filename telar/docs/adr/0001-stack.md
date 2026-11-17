# ADR 0001: Stack do Telar

- **Status:** aceito (pode ser revisto antes do primeiro release)
- **Data:** 2026-11-17
- **Contexto do projeto:** `PROJETO_Charter_OpenSource.md`, `design/telar-tema.html`,
  `RE_PhotoshopTouch_Engine_Resumo.md`

## Contexto

O Telar é um editor de fotos para toque, open-source, inspirado no Photoshop Touch e escrito
do zero (Trilho B). Requisitos que não se negociam:

1. **arm64-v8a nativo**, para rodar nos aparelhos atuais;
2. **OpenGL ES 3.0** como base, com compute do 3.1 quando o aparelho tiver;
3. **UI no padrão do Photoshop**: ferramentas à esquerda, Camadas puxadas da direita, barra de
   opções, **paisagem como modo principal** (ver o protótipo de design);
4. licença `MIT OR Apache-2.0` e build reproduzível por qualquer pessoa;
5. aproveitar o que já está pronto: o shader de mesclagem em **GLSL ES 3.00**, já testado, e o
   resumo de comportamento da engine.

O que exige mais desempenho: compor camadas e aplicar filtros na GPU a 60 fps num documento de
12 a 24 MP; seleção inteligente (graph cut, matting) na CPU com NEON; codecs (JPEG, PNG, PSD).

## Opções avaliadas

| | **A. Kotlin + Compose (UI) + C++ (motor, NDK) + GLES 3** | B. Rust + wgpu (estilo PhotoCraft) | C. Flutter + motor nativo via FFI |
|---|---|---|---|
| GLES 3 / GLSL | **Direto.** Nossos `.frag` rodam como estão | wgpu fala WGSL; GLES é backend secundário no Android; shaders teriam de ser traduzidos | Motor fica em C++ ou Rust de qualquer jeito; Impeller não expõe GL próprio |
| UI de toque no padrão Android | **Compose:** gavetas, gestos de borda, paisagem/retrato, acessibilidade, teclado/caneta nativos | egui/iced não são nativos de toque; gestos e IME dão trabalho | Excelente para UI |
| Integração UI ↔ canvas | `SurfaceView` + EGL próprio dentro do Compose; caminho conhecido | Bom em desktop, mais atrito em Android | Textura externa + FFI: duas fronteiras (Dart↔nativo, Flutter↔GL) |
| Bibliotecas de imagem | libjpeg-turbo, libpng, zlib, lz4, OpenCV 4 opcional: todas C/C++ e com NEON | Crates boas; mais trabalho para NEON/Android | Igual à A, mais a ponte |
| Reaproveitar PhotoCraft | Reimplementar algoritmos com atribuição (já era o plano) | Reuso mais direto do código Rust | Igual à A |
| Linguagens no repositório | 2 (Kotlin, C++) | 1–2 | 3 (Dart, C++, Kotlin para a plataforma) |
| Segurança de memória | Precisa de disciplina: ASan/UBSan, fuzz nos decoders | **Melhor** | Igual à A no motor |
| Desktop no futuro | Motor C++ portátil; UI desktop seria outra (Qt ou ImGui) | Melhor caminho multiplataforma | Bom |

## Decisão

**Opção A: Kotlin + Jetpack Compose na interface, motor em C++20 pelo NDK, renderização
OpenGL ES 3.0 (compute 3.1 opcional), alvo principal arm64-v8a.**

Por quê, em uma frase por requisito:
- é o único caminho em que **GLES 3 e os shaders GLSL que já escrevemos rodam sem tradução**;
- Compose entrega a UI no padrão Photoshop **em paisagem e retrato**, com gaveta puxada da
  borda e caneta com pressão, sem brigar com a plataforma;
- as bibliotecas de imagem com NEON de que precisamos são C/C++;
- o motor em C++ fica portátil para um desktop futuro, sem amarrar a UI.

O ponto fraco (segurança de memória) é tratado na engenharia: sanitizers no CI, fuzzing dos
decoders de arquivo e nada de aritmética de ponteiro fora do núcleo de pixels.

## Como fica a arquitetura

```
telar/
├─ app/                      Kotlin · Compose
│  ├─ ui/                    tema (design/telar-tokens.json → Compose), barra de ferramentas,
│  │                         barra de opções, gaveta de painéis, diálogos
│  ├─ canvas/                SurfaceView + gestos (pinça, giro, caneta) → chamadas JNI
│  └─ doc/                   ViewModels: documento, camadas, histórico
├─ engine/                   C++20 · NDK · CMake
│  ├─ gpu/                   EGL (ES 3.0/3.1), texturas RGBA16F, ping-pong, PBO assíncrono
│  ├─ doc/                   documento em blocos 256×256, camadas, máscaras, desfazer por bloco
│  ├─ select/                varinha (flood fill), seleção rápida (graph cut progressivo),
│  │                         rabisco (GMM + geodésico), refinar borda (guided filter / matting)
│  ├─ io/                    JPEG, PNG, WebP; formato próprio .tlr; PSD (ler/gravar camadas)
│  └─ jni/                   ponte fina Kotlin ↔ engine (API pequena, por comando)
├─ filters/                  shaders GLSL ES 3.00 + descritores JSON (UI gerada pelo descritor)
├─ assets/                   ícones e fontes livres (Inter, JetBrains Mono, Sora · OFL)
└─ docs/                     ADRs, arquitetura, guia de contribuição
```

**Parâmetros de build:**
- `minSdk 26` (Android 8.0): GLES 3.0 garantido na prática e Compose sem limitações;
  `targetSdk` = o mais recente exigido pela Play;
- ABIs: `arm64-v8a` (principal) e `x86_64` (emulador e Chromebook). Sem `armeabi-v7a` no
  começo;
- NDK r27+, CMake, C++20, `-O2` com NEON;
- CI no GitHub Actions: build do APK, testes do motor (GoogleTest), teste dos shaders
  (`filters/tests`), ASan nos testes, release assinado nas tags.

**Contrato UI ↔ motor:** a UI nunca mexe em pixels. Ela manda **comandos**
(`setLayerBlend(id, mode)`, `brushStroke(points…)`, `applyFilter(id, params)`) e o motor
devolve estado e notificações. Isso deixa o histórico (desfazer/refazer) inteiro no motor,
cabe numa API JNI pequena e permite trocar a UI no futuro.

## Consequências

- O próximo trabalho concreto é o **esqueleto**: app Compose com o layout do protótipo em
  paisagem, `SurfaceView` com contexto ES 3.0 mostrando uma imagem, e o
  `composite_blend.frag` compondo duas camadas. É o primeiro APK arm64 instalável.
- O plano do port privado do APK original (Trilho A, AIR 64-bit + ANE nova) continua
  possível e independente; o motor C++ pode até servir de base para essa ANE.
- Se um dia o desktop virar prioridade, revemos a UI, não o motor.

## Revisar se

- o Compose não segurar 60 fps com o canvas aberto em aparelhos intermediários; nesse caso a
  UI de painéis migra para Views clássicas, sem mexer no motor;
- surgir contribuidor forte em Rust e o PhotoCraft virar dependência real; nesse caso
  avaliamos um núcleo Rust atrás da mesma API de comandos.
