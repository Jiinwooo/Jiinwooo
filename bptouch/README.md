# BP Touch

Editor de fotos para toque, **64 bits (arm64-v8a)**, com motor próprio em **C++ e OpenGL ES 3.0**
e interface no padrão do Photoshop: ferramentas à esquerda, Camadas puxadas da borda direita,
barra de opções em cima. Feito pensando no celular **deitado**, mas funciona em pé também.

> Nome provisório. Código original do projeto (Trilho B), licença `MIT OR Apache-2.0`.
> Nada aqui vem do Photoshop Touch: ícones, shaders e algoritmos foram escritos do zero.

## O que já faz (v0.1)

**Ferramentas** (mesma ordem e atalhos do Photoshop)
- Mover (V) · Letreiro retangular/elíptico (M) · Laço / Laço poligonal (L)
- Seleção de objeto / Seleção rápida / Varinha mágica (W) · Cortar (C) · Conta-gotas (I)
- Pincel (B) com pressão de caneta · Carimbo (S) · Borracha (E) · Degradê / Lata de tinta (G) · Mão (H)
- Dois dedos em qualquer ferramenta: zoom, giro (com encaixe em 0°/90°) e arrastar

**Seleção sem rebarba**
- *Seleção rápida*: corte de grafo com modelos de cor (GMM) e prior de distância ao traço, seguido
  de filtro guiado na resolução cheia, que encosta a borda no contorno real.
- *Seleção de objeto*: arraste um retângulo em volta do objeto (GrabCut iterativo + refino de borda).
- *Refinar borda*: raio, suavizar, contraste, deslocar borda e difusão, guiado pela foto.
- Modos nova / adicionar / subtrair / interseção, difusão e suavização de serrilhado.

**Camadas**: nova, duplicar, excluir, reordenar, mesclar para baixo, achatar, camada via cópia/recorte,
inserir foto como camada, visibilidade, opacidade, bloquear transparência, renomear (toque longo) e os
**27 modos de mesclagem** do menu do Photoshop.

**Ajustes** (só na seleção, quando houver): Brilho/Contraste, Níveis, Curvas, Exposição,
Temperatura e vibração, Matiz/Saturação, Preto e branco, Inverter, Desfoque gaussiano, Nitidez.

**Imagem**: cortar, cortar para a seleção, girar 90°/180°, espelhar.

**Histórico**: desfazer/refazer ilimitado dentro de 700 MB, painel de Histórico com salto direto.

**Arquivos**: abre JPEG, PNG, WebP, HEIF/AVIF, GIF e o que mais o Android decodificar; salva JPEG,
PNG (com transparência) e WebP sem perdas em *Imagens/BP Touch*; compartilhar.

## Estrutura

```
bptouch/
├─ app/src/main/cpp/        motor C++ (engine, seleção, max-flow, ponte JNI)
├─ app/src/main/assets/     shaders GLSL ES 3.00 do app
├─ app/src/main/java/…      interface em Kotlin + Jetpack Compose
├─ engine-tests/            testes do motor rodando no computador (CPU e OpenGL ES por software)
└─ keystore/                chave de TESTE (não usar para loja)
```
Os shaders compartilhados (composição com 27 modos) ficam em `../telar/filters/`.

## Compilar

Requisitos: JDK 17+, Android SDK 35, NDK 27.2, CMake 3.22.

```bash
cd bptouch
gradle assembleRelease          # APK em app/build/outputs/apk/release/
```

## Testes do motor

```bash
cd bptouch/engine-tests
g++ -O2 -std=c++20 -o test_selection test_selection.cpp ../app/src/main/cpp/{maxflow,selection}.cpp && ./test_selection
# motor GL completo sobre Mesa (llvmpipe): precisa de libegl1, libgles2, libegl-mesa0
g++ -O1 -std=c++20 -DEGL_NO_X11 -I<headers GLES3/EGL> -o test_engine test_engine.cpp ../app/src/main/cpp/{engine,selection,maxflow}.cpp -lEGL -lGLESv2 -lpthread && ./test_engine ..
```
