# Engenharia Reversa — Adobe Photoshop Touch (phone) 1.3.7

APK analisado: `adobe-photoshop-touch-1-3-7.apk` · MD5 `99f9fff48de0f5394d8f6fd3dc7ca41e`
Build: 05/03/2015 · Analisado em: 17/11/2026

> **Nota sobre o arquivo:** o APK enviado estava **truncado** (download incompleto): os 318
> cabeçalhos locais e todos os dados dos arquivos estavam presentes, mas o diretório central
> no fim e o registro EOCD estavam cortados. Reconstruí o índice (`zip -FF`) e todos os 318
> arquivos extraíram com integridade — o truncamento atingiu só o índice, não os dados.

---

## 1. Identidade e arquitetura

| Item | Valor |
|---|---|
| Package | `air.com.adobe.pstouchphone` |
| versionName / versionCode | `1.3.7` / `1003007` |
| minSdk / targetSdk | 14 / 12 |
| Runtime | **Adobe AIR** (app Flash/ActionScript empacotado), compilado com **Adobe Flex 4** |
| Activity única | `.AppEntry` (launchMode singleTask, GLES 2 obrigatório) |
| Conteúdo inicial | `TTPixel.swf` (via meta-data `initialcontent`) |

**O ponto-chave:** o `classes.dex` em Java é só o *bootstrap* do AIR. **Toda a lógica do
aplicativo é ActionScript 3**, dentro de `assets/TTPixel.swf`, e o processamento de imagem
pesado é feito em **código nativo C/C++ via OpenGL ES 2**. Ou seja, três camadas:

```
[ Java / AIR bootstrap ]  → com.adobe.air.*  (classes.dex)
          │
[ ActionScript 3 app   ]  → TTPixel.swf  (framework CAB + app ttpixel)
          │  (ponte ANE / FRE)
[ Motor nativo (GPU)   ]  → libTTPixelExtensionAndroid.so + shaders GLSL
```

---

## 2. Conteúdo do APK (318 arquivos)

- `classes.dex` (876 KB) — bootstrap AIR
- `AndroidManifest.xml` — decodificado em `manifest.txt`
- `assets/TTPixel.swf` (6.6 MB comprimido / ~8 MB) — **o app inteiro**
- `assets/resource/application/filters/` — **47 shaders GLSL** (os filtros)
- `assets/*.cabmod` (118) — módulos do framework CAB
- `assets/*.zstrings` (16) — localização (en, de, es, fr, it, ja, pt, ru)
- `lib/armeabi-v7a/` — 9 bibliotecas nativas (ARM32 apenas)

### Bibliotecas nativas
| Lib | Tamanho | Papel |
|---|---|---|
| `libCore.so` | 18.7 MB | runtime Adobe AIR (máquina virtual ActionScript + Flash) |
| `libTTPixelExtensionAndroid.so` | 2.3 MB | **motor de imagem** (filtros GL, alpha matting) |
| `libsibsynclib.so` | 2.3 MB | sincronização Creative Cloud |
| `libTTPixelExtensionAndroid` usa | — | `libGLESv2`, `libEGL`, `libjnigraphics` |
| `libstlport_shared.so`, `libysshared.so` | — | STL / suporte |
| `libstagefright_*.so` | — | codecs de mídia (froyo/honeycomb) |

---

## 3. Permissões e superfície Android

`INTERNET`, `WRITE_EXTERNAL_STORAGE`, `CAMERA`, `ACCESS_NETWORK_STATE`, `ACCESS_WIFI_STATE`,
`WAKE_LOCK`. Features opcionais: câmera, autofoco, wifi. GLES 2 **obrigatório**.

**Intent filters** (o que o app abre):
- `MAIN`/`LAUNCHER`
- `EDIT`/`VIEW` para `application/vnd.adobe.pstouch`, `image/jpeg`, `image/png`, `image/gif`
- deep-link de arquivos `.psdx` via esquemas `file/http/https/content` (pathPattern `.*\.psdx`)

**Push:** receiver C2DM (`air.com.adobe.cc.notifications.c2dm.C2DMReceiver`) — Google Cloud
Messaging legado.

---

## 4. A aplicação ActionScript (TTPixel.swf)

Um único bloco **DoABC** (`frame1`, 2.18 MB) com **20.737 strings** no constant pool. Dois
frameworks:

### 4.1 `com.adobe.cab.*` — framework base dos "Touch apps" da Adobe
Infra compartilhada entre os apps Touch: `animation`, `control`, `core` (`core.air`),
`crypto`, `driver`, `io` (`io.ftp`, `io.local`, `io.ozzy`, `io.px`), `menu`, `net`,
`stage3d`, `theme`, `transition`, `view`. (≈1.182 nomes `com.adobe.*` no total.)

### 4.2 `com.adobe.ttpixel.*` — o Photoshop Touch em si
- `editor.app`, `editor.tool` (+ `editor.tool.selection`), `editor.brush`,
  `editor.processor`, `editor.text`, `editor.components`, `editor.ui`, `editor.utils`
- `gallery` / `gallery.ui`, `convert`
- `media.*`: `dropbox`, `facebook`, `flickr`, `google`, `creativecloud` (`account`,
  `TTCreativeCloudFileSystem`), `oz`, `publish`
- `extension` (`extension.iap` compra no app, `extension.pressure` pressão da caneta)
- `omniture` / `tracking` (analytics), `eula`

### Recursos de edição identificados (por classe/string)
- **Camadas** (`TTLayerManager`): nova/duplicar/excluir/mover, Merge Down, Merge Visible,
  renomear, alpha, **Quick Mask Mode**, limite máximo de camadas.
- **Blend modes**: Normal, Multiply, Screen, Overlay, Darken, Lighten, Difference,
  Subtract, Linear Dodge (Add).
- **Ferramentas**: Brush (com *Edge Aware* e suporte a **Stylus/pressão**), seleção,
  **Magic Wand** (tolerância), seleção por pincel (*scribble*, borda-consciente → alpha
  matting nativo).
- **Ajustes**: Curves, e os 47 shaders abaixo. Processadores como `TTAGALShaderProcessor`,
  `TTAutoFixProcessor`, `TTAquarellProcessor`.

---

## 5. Motor de filtros (GPU)

Os **47 filtros** são shaders **GLSL** em `assets/resource/application/filters/*.fs`
(+ `vertex.vsh`, `vertexTrans.vsh`). Cada `.fs` traz, no topo, um descritor XML
`<filter>` com rótulo (localizável), categoria e parâmetros (ex.: `intensity` com
min/max/default). São carregados e executados pelo motor nativo.

- **effects (31):** aquarell, charcoal, comic, directionalBlur, glass, grainyNight,
  graphicPen, halftone, hdrLook, lighting, monitor, moonlight, multiplyParticle,
  oldPhotoCopy, oldSepia, pencil, posterize, ripple, scratches, sharpen, sleepyHollow,
  softlight, softSkin, stamp, sunnyAfternoon, threshold, thresholdAGAL, tint,
  brushShape, colorize, dodgeBurn…
- **adjustment (10):** colorBalance, contrastBrightness, direction, matchColor, median,
  saturation, selStroke, size, temperature, toneCurve.
- **generic (6):** aquire, autoFix, colormatchaquire, multiGradientMap, palette,
  replaceColor.

### Ponte nativa (AIR Native Extension `com.adobe.ttpixel.extension`)
`libTTPixelExtensionAndroid.so` expõe por JNI o motor GL. Classes/métodos nativos:

- **`ECGLContext`** — `createContext`, `destroyContext`, `getStringInfo`,
  `getLastGLErrorCode` (gerencia o contexto EGL/GL).
- **`ECGLFilter`** — pipeline de filtro: `createFilter`, `shaderCreate`/`shaderDispose`,
  `programCreate`/`programUse`/`programDispose`, `texturePrepare`/`textureUploadData`,
  `setParamFloat(Multi)`/`setParamInt(Multi)`/`setParamFloatMatrix`, `drawArrays`,
  `applyFilter`, `executeSequence`/`asyncExecuteSequence` (+ `asyncReadPixels`,
  `asyncJoin`, `asyncInterrupt`), `readPixels`/`readPixelsRect`.
- **`ECAlphaMatting`** — `getResult`/`release`: recorte inteligente (seleção por scribble).

Ou seja: o AS3 configura o filtro e os parâmetros; o `.so` compila o shader GLSL, sobe a
textura, roda na GPU (OpenGL ES 2) e devolve os pixels — com execução em sequência e
assíncrona para performance.

---

## 6. Integrações de nuvem / rede

| Serviço | Endpoints / uso |
|---|---|
| **Adobe IMS (login Creative Cloud)** | `ims-na1-cc1.adobelogin.com/ims`, `api.adbecrsl.com`, `www.adobe.com/go/cc_signin` |
| **Creative Cloud (arquivos/sync)** | `storage-stage.adobe.com`, engine `libsibsynclib.so` (polling, limites de banda, resolução de conflito) |
| **Dropbox** | `api.dropbox.com/0/`, `api-content.dropbox.com/0/` (API v0) |
| **Flickr** | `api.flickr.com/services/`, auth, photosets |
| **Facebook** | `graph.facebook.com`, `m.facebook.com/dialog`, logout |
| **Google** | busca de imagens `ajax.googleapis.com/ajax/services/search/images` |
| **Analytics** | Omniture/SiteCatalyst `adobemobile.112.2o7.net` |
| **Comando interno** | esquema `pstouch/command/` (ponte UI↔lógica) |

---

## 7. Achados de segurança

1. **Segredo OAuth hardcoded** — `assets/CABConfig.xml` traz em texto claro:
   ```xml
   <configuration name="IMS_clientID" value="pstouchphone-droid" />
   <configuration name="IMS_secret"   value="04528de5-8096-4213-8aa1-f363523fbc97" />
   ```
   Credenciais de cliente Adobe IMS embarcadas no APK (padrão de risco conhecido em apps
   cliente; aqui, de um app descontinuado de 2015). As demais chaves (Dropbox/Flickr/FB)
   aparecem como **nomes de propriedade** no pool AS3 (`consumerKey`, `appSecret`, etc.) —
   os valores são injetados em runtime/config, não ficaram óbvios no bytecode.
2. **Flag de desenvolvimento** curiosa no bytecode: `DO_NOT_CHANGE_SUPER_SECRET_TABLET_MODE`
   (distingue build phone/tablet).
3. `debuggable="false"` (correto). `hardwareAccelerated="false"` no manifest — o app faz a
   aceleração por conta própria via o contexto GL nativo.

---

## 8. Onde estão os artefatos recuperados

Tudo no scratchpad da sessão (temporário):
`/tmp/claude-0/-home-user-Jiinwooo/03780a48-e413-5294-aef4-76dcd5f99ab2/scratchpad/pstouch/`

- `fixed.apk` — APK reparado · `apk_raw/` — tudo extraído
- `TTPixel.uncompressed.swf` — SWF descomprimido
- `abc_strings/frame1.txt` — 20.737 strings do app (classes, métodos, literais)
- `manifest.txt` — manifest decodificado · `axml.py`, `swf_abc.py` — parsers próprios

---

## 9. Limites desta análise / próximos passos

- Não rodei decompiladores Java baixados (jadx/apktool) — o classificador de segurança do
  ambiente bloqueou executar `.jar` baixados. Contornei com parsers próprios em Python
  (AXML + SWF/ABC) e binutils, o que cobriu manifest, estrutura AS3 e libs nativas.
- **Para ir mais fundo**, com sua autorização eu posso:
  - liberar o jadx/apktool para **decompilar o AS3 a código legível** (classes e métodos
    completos, não só os nomes) — precisa você permitir rodar o jar, ou eu escrever um
    desmontador ABC próprio;
  - desmontar as funções do `libTTPixelExtensionAndroid.so`/`libCore.so` em ARM
    (`objdump -d`) — isso já roda aqui sem bloqueio;
  - extrair e catalogar o GLSL completo dos 47 filtros com seus parâmetros.

Quer que eu siga por algum desses?
