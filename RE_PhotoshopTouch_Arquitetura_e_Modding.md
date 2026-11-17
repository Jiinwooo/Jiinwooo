# Photoshop Touch 1.3.7 — Arquitetura Completa e Guia de Modificação

Documento 2 de 2. (O documento 1, `RE_PhotoshopTouch_1.3.7.md`, cobre a identidade do APK,
permissões e achados de segurança.)

Objetivo: entender **como tudo funciona** para poder **alterar layouts, estruturas e
integrar novas funcionalidades** (incluindo trazer recursos do Photoshop desktop).

> Base desta análise: desmontei todo o bytecode ActionScript com um desmontador AVM2/ABC
> próprio (Python). Resultado: **1.209 classes, 15.525 métodos, 15.421 corpos de método**
> desmontados, em `as3_disasm/` (árvore por pacote). Isso é código legível em nível de
> opcode — não é AS recompilável, mas revela toda a lógica.

---

## 1. As três camadas (e o que dá pra mexer em cada uma)

```
┌─────────────────────────────────────────────────────────────┐
│ CAMADA 1 — Bootstrap AIR (Java/DEX)   com.adobe.air.*         │  ❌ não mexer
│   Só inicializa o runtime Flash. Genérico da Adobe.          │
├─────────────────────────────────────────────────────────────┤
│ CAMADA 2 — App ActionScript (TTPixel.swf)                    │
│   • Framework CAB  (com.adobe.cab.*)                         │
│   • App Photoshop  (com.adobe.ttpixel.*)                     │
│   ──> LAYOUT/UI: XML declarativo (.cabmod/.cabapp)  ✅ FÁCIL  │
│   ──> LÓGICA: bytecode ABC dentro do SWF         ⚠️ MÉDIO     │
├─────────────────────────────────────────────────────────────┤
│ CAMADA 3 — Motor nativo (GPU)  libTTPixelExtensionAndroid.so │
│   • Shaders GLSL (.fs)  ──> FILTROS            ✅ FÁCIL       │
│   • Código ARM (.so)                           🔴 DIFÍCIL    │
└─────────────────────────────────────────────────────────────┘
```

**Resumo da viabilidade** (detalhado na seção 8):

| O que você quer fazer | Onde | Dificuldade |
|---|---|---|
| Mover/redimensionar botões e painéis | `.cabmod` XML | ✅ Fácil (editar texto) |
| Adicionar um filtro/efeito novo (GLSL) | `.fs` + descritor XML | ✅ Fácil |
| Mudar textos, idiomas, ícones | `.zstrings` / theme SWF | ✅ Fácil |
| Adicionar um painel/janela novo | `.cabmod` + classe AS3 existente | 🟡 Médio |
| Mudar comportamento/lógica | patch no bytecode ABC | ⚠️ Médio-difícil |
| Operação de imagem nova em nível de pixel | `.so` nativo (ARM) ou shader | 🔴 Difícil / 🟡 via shader |

---

## 2. O framework CAB — o coração declarativo

O app não "desenha" a UI em código. Ele a **declara em XML** e o CAB instancia classes AS3
a partir dessas tags. Prova disso está em `application.cabapp`:

```xml
<ttapplication id="ttapplication" mainwindow="window1" ...>
  <objects>
    <shaderManager id="messageHandler"/>      <!-- instancia TTShaderManager -->
    <ttfontmanager id="messageHandler1"/>
  </objects>
  <classes>
    <class className="ttapplication"           <!-- tag XML -->
           packageName="com.adobe.ttpixel.app"
           codeName="TTApplication">           <!-- classe AS3 real -->
      <var name="mainwindow" codeName="iMainWindow" className="CABWindow"/>
      ...
```

**Como ler isto:** cada `<class>` associa um **nome de tag XML** a uma **classe
ActionScript**, e cada `<var>` liga um **atributo XML** a uma **variável do código**
(`codeName`). É um sistema de *data-binding* declarativo. Então, quando você escreve
`<button type="tool" .../>` num `.cabmod`, o CAB cria um objeto da classe AS3 registrada
para `button`, com as propriedades vindas dos atributos.

Mensageria interna: o CAB usa um barramento de mensagens (`CABMessageHandler`,
`CABControlMessage`, `referenceDispatcher`, atributos `target`/`selector`). Botões disparam
`selector="fxCategoryChange"` para um `target` — como *actions/outlets*. Isso é o que conecta
UI → lógica **sem** recompilar código: você aponta um botão novo para um selector já
existente.

Comunicação com o navegador/host via esquema `pstouch/command/` (ponte URL → comando).

---

## 3. Sistema de LAYOUT (.cabmod / .cabapp) — totalmente editável

Os `.cabmod` são **XML em texto puro** (confirmado: `file` reporta ASCII). Exemplo real
(`tttoolspanelmodule.cabmod`):

```xml
<ttpanelmodule id="module" view="view1" refdispatcher="referenceDispatcher">
  <windows>
    <window id="window" width="300" height="300" x="0" y="0" initVisible="false">
      <ttpanelview id="view1" width="271" height="251" x="12" y="9" module="module">
        <button id="button" width="40" height="28" x="11" y="8" toggles="true" type="tool"/>
      </ttpanelview>
    </window>
  </windows>
```

Propriedades-chave que você controla: `x`, `y`, `width`, `height`, `sizeX/sizeY`
(`scale`/fixo), `layer` (z-order), `type` (`tool`/...), `toggles`, `initVisible`,
`addToWindowMenu`, `title` (com chave de localização `$$$/.../=Texto`), `target`/`selector`.

**Onde ficam os layouts** (em `assets/resource/application/`):
- `application.cabapp` — app raiz, janelas, registro de classes, objetos globais.
- `editor/panels/*.cabmod` — painéis do editor (ferramentas, camadas, adicionar camada,
  tipos de marquee/lasso, rotação, sliders...).
- `components/*.cabmod` — listas (efeitos `tteffectslistpanelmodule`, ajustes
  `ttadjustmentlistpanelmodule`), barras, controles.
- `editor/toolsettings/*.cabmod` — configurações por ferramenta.
- `*.cabmod` na raiz — diálogos (salvar, abrir, cor, câmera, alertas, teclado...).

**Listas de efeitos/ajustes** usam `<switchBar>` com `<part>` por categoria
(Basic / Stylize / Artistic…), cada `part` com `selector` que muda a categoria exibida.
É aqui que você adiciona/reordena categorias de filtros na UI.

### Receita A — mover/adicionar um botão
1. Abrir o `.cabmod` do painel (ex.: `editor/panels/tttoolspanelmodule.cabmod`).
2. Editar `x`/`y`/`width`/`height`, ou copiar um `<button>` e ajustar `id`, posição e
   `selector` (apontando para um comando já existente).
3. Reempacotar o APK (seção 9). Nenhum código precisa ser recompilado.

---

## 4. Modelo de documento, camadas e histórico

Pacote `com.adobe.ttpixel.editor.app`:
- **`TTModel`** — o documento aberto (imagem, estado). Alvo de comandos como
  `Apply Curves`, `Curves`.
- **`TTLayer`**, **`TTLayerBigData`** — camadas e seus dados de pixel.
- **`TTLayerManager`** — operações: New/Delete/Duplicate/Move/Select Layer, Merge Down,
  Merge Visible, Set Layer Alpha/Blend Mode/Name, Show Layer, **Quick Mask Mode**,
  Select Pixels. Tem limite máximo de camadas (`NOMorelayer...`).
- **Blend modes** (em `TTLayerView`): Normal, Multiply, Screen, Overlay, Darken, Lighten,
  Difference, Subtract, Linear Dodge (Add). ➜ Para **adicionar um blend mode do Photoshop**
  (ex.: Color Dodge, Hard Light), o caminho mais limpo é via shader de composição (camada 3),
  já que a mistura final passa pela GPU.
- **Undo/histórico**: cada operação registra um "undo title" (`getUndoTitle()`), então o
  sistema de histórico é por-comando — importante ao criar features novas (implemente
  `getUndoTitle` para integrar no desfazer).

Ferramentas (`com.adobe.ttpixel.editor.tool`, `.tool.selection`): Brush (com *Edge Aware* e
pressão de caneta via ANE `extension.pressure`), Magic Wand (tolerância), seleção por
pincel/scribble (ligada ao **alpha matting nativo** `ECAlphaMatting`), marquee, lasso,
transform, crop, text.

---

## 5. Pipeline de FILTROS (o mais fácil de estender)

### 5.1 Como um filtro é definido
Cada filtro é um par **shader GLSL + descritor XML**, no mesmo arquivo `.fs`
(`assets/resource/application/filters/`). Exemplo (`softlight.fs`):

```glsl
/*$$$
<filter id="softlightShader" label="$$$/Shader/Soft Light=Soft Light"
        category="effects" vartype="intensity" source1="blur"
        blurPercentAmount="7" pixelLocal="false">
  <var varname="intensity" label="$$$/Shader/Intensity=Intensity"
       type="number" numFactor="100" min="0" max="1" default="0.75" />
</filter>
$$$*/
precision highp float;
uniform sampler2D tt_uniform_maintexture;
uniform vec2 tt_uniform_surfacesize;
uniform float intensity;           // <- o <var> acima vira este uniform
varying vec2 mainTexCoord;
void main() { ... }
```

O descritor controla:
- `id` — identificador do shader (referenciado pelo `TTShaderProcessor.iShaderID`).
- `label` / `category` — como aparece na UI (effects / adjustment / generic).
- `vartype` — `novars` / `intensity` / `single` / `custom` (define o tipo de painel).
- `source1="blur"` — pede uma textura auxiliar (ex.: versão borrada) ao motor.
- `<var>` — **gera automaticamente o controle na UI e o uniform no shader**. Tipos
  suportados (de `TTShaderManager`): `boolean`, `number`, `angle`, `color`, `coloralpha`,
  `int`, `layer`, `choice`.

### 5.2 Como é executado
`TTShaderProcessor` (base dos 37 processors) faz:
`readXML()` → lê o descritor · `createParamUI()` → monta a UI dos `<var>` sozinho ·
`setParamsToShader()` → envia os valores como uniforms · `previewShader()` → preview ·
tiling (`startTilingProcess`/`processTile`) para imagens grandes.

A execução real desce para a ANE `com.adobe.ttpixel.extension.gl.ECGLFilter` (JNI →
`libTTPixelExtensionAndroid.so`): `shaderCreate` → `programCreate` → `textureUploadData`
→ `setParamFloat/Int` → `applyFilter`/`executeSequence` (sync ou async) → `readPixels`.
Uniforms padrão disponíveis: `tt_uniform_maintexture`, `tt_uniform_texture1`,
`tt_uniform_surfacesize`, `image_pixeloffsetscale`, `image_blursize`, `mainTexCoord`.

### 5.3 Receita B — adicionar um filtro do Photoshop (ex.: "Gaussian Blur" variante, "Vibrance")
1. Escrever `meufiltro.fs` em `assets/resource/application/filters/` com o bloco
   `<filter>` + os `<var>` dos parâmetros + o `void main()` GLSL ES 2.0.
2. Escolher `category` e `vartype` para a UI aparecer automaticamente.
3. Registrá-lo na lista da UI: incluir na categoria certa do
   `components/tteffectslistpanelmodule.cabmod` (ou `ttadjustmentlistpanelmodule.cabmod`)
   referenciando o `id` do shader.
4. Se a lógica for coberta por `TTShaderProcessor` (a maioria dos filtros é), **não precisa
   tocar em bytecode**. Se precisar de pré-processamento especial, aí sim cria um processor
   novo (camada 2 → patch ABC).
5. Reempacotar (seção 9).

> Muitos efeitos "de Photoshop" (ajustes de cor, mistura, estilização, meio-tom, nitidez,
> HDR, etc.) são **puramente shader** e cabem nesse fluxo sem mexer em código. Esse é o
> vetor de integração de funcionalidades mais produtivo.

---

## 6. Integrações de mídia e nuvem

`com.adobe.ttpixel.media.*`: dropbox, facebook, flickr, google (busca de imagens),
creativecloud (login IMS + sistema de arquivos `TTCreativeCloudFileSystem` + sync via
`libsibsynclib.so`), oz, publish. ⚠️ **Realidade:** o Photoshop Touch foi descontinuado pela
Adobe em 2015; os serviços de nuvem/login (IMS, CC sync, Ozzy) provavelmente **não respondem
mais**. Para um app modificado, o mais sensato é desativar/encobrir essas integrações mortas
e apontar import/export para **armazenamento local** (já suportado: `localFileSystem` no
`.cabapp`, permissão `WRITE_EXTERNAL_STORAGE`).

---

## 7. Fluxo de inicialização (resumido)

1. Android lança `.AppEntry` → runtime AIR (`libCore.so`) sobe.
2. AIR carrega `initialcontent = TTPixel.swf`.
3. O SWF roda `frame1` (o único DoABC) → inicializa o framework CAB.
4. CAB lê `application.cabapp` → cria `TTApplication`, janelas, `objects` globais
   (`shaderManager`, `ttfontmanager`, `localFS`).
5. Carrega recursos via `resources.cabtoc` → `main.cabres`, `cab.cabres`, `theme.cabres`,
   `fonts.cabres`.
6. Monta a UI a partir dos `.cabmod`; registra ferramentas, filtros, painéis.
7. ANEs inicializadas: TTPixel (imagem), CC Sync, Push C2DM.

---

## 8. Integrar recursos do Photoshop desktop — o que é realista

**Vetor 1 — Filtros/ajustes por shader (RECOMENDADO).** Qualquer efeito expressável como
função de pixel em GLSL entra pela Receita B, sem recompilar código. Cobre a maioria dos
ajustes e efeitos do Photoshop (curvas extras, vibrance, color balance avançado, novos
blends, estilizações, grão, blur direcional, etc.).

**Vetor 2 — Reorganização de UI/UX.** Rearranjar ferramentas, painéis, barras, categorias,
e até expor controles escondidos — tudo via `.cabmod`/`.cabapp`. Sem código.

**Vetor 3 — Lógica nova (patch de bytecode).** Para comportamento que o AS3 não expõe
declarativamente, é preciso **editar o bytecode ABC** dentro do SWF. Eu já tenho a
desmontagem completa; para *reescrever*, o caminho é um round-trip de ABC
(desmontar → editar → remontar) — eu posso escrever o remontador, ou usamos o fluxo
RABCDAsm. É factível, mas é o passo mais delicado (sem checagem de tipo do compilador).

**Vetor 4 — Operações nativas novas.** Se a feature exige algo que nem shader nem AS
resolvem (ex.: um algoritmo que precise rodar no `.so`), aí é engenharia de binário ARM
(Ghidra + patch no `libTTPixelExtensionAndroid.so`). É o caminho 🔴 mais caro e o único que
realmente precisa do Ghidra/REA que você mencionou.

**Limites honestos:**
- Não dá pra "colar" um módulo do Photoshop desktop (C++/CEP) aqui — é outra plataforma.
- Serviços de nuvem da Adobe para esse app estão mortos (2015).
- Recompilar o SWF inteiro a partir da desmontagem **não** é possível; mudanças de lógica
  são *patches*, não recompilação.
- GLES 2.0 limita os shaders (sem compute shaders; precisão/loops limitados).

---

## 9. Reempacotar e reinstalar (fluxo de modificação)

Toda modificação de **assets** (`.cabmod`, `.fs`, `.zstrings`, theme) segue:
1. Editar os arquivos extraídos em `apk_raw/`.
2. Reconstruir o ZIP/APK (realinhar com zipalign).
3. **Reassinar** (APK v2); sem assinatura o Android não instala.
4. Instalar o APK modificado (precisa desinstalar o original — assinaturas diferentes).

> Observação do ambiente: aqui na nuvem o classificador de segurança bloqueou executar
> `.jar` baixados (apktool/jadx). O **empacotamento/assinatura final** é melhor feito na sua
> máquina (com `apksigner`/`zipalign` do Android SDK), ou você me autoriza a rodar essas
> ferramentas aqui. Toda a edição de assets e a análise eu faço aqui sem bloqueio.

---

## 10. Artefatos gerados (no scratchpad da sessão)

`/tmp/claude-0/.../scratchpad/pstouch/`
- `as3_disasm/` — **1.209 classes desmontadas** (árvore por pacote) + `00_INDEX.txt`
- `abc_disasm.py` — o desmontador AVM2 (meu, reaproveitável)
- `apk_raw/` — APK extraído (todos os assets editáveis)
- `TTPixel.uncompressed.swf`, `frame1.abc`, `abc_strings/frame1.txt`
- `manifest.txt`

Como o scratchpad é temporário, vou te entregar a desmontagem como arquivo e posso commitar
o que você quiser manter no repositório.

---

## 11. Próximos passos sugeridos

1. **Quer um filtro novo já de exemplo?** Eu escrevo um `.fs` (ex.: Vibrance ou um blend
   novo) e mostro o registro na lista — pronto pra empacotar.
2. **Quer mexer num layout específico?** Me diga qual painel e eu edito o `.cabmod`.
3. **Precisa mudar lógica?** Eu escrevo o remontador de ABC para fecharmos o round-trip de
   patch de bytecode.
4. **Operação nativa?** Aí partimos para `objdump`/Ghidra no `.so` (posso desmontar o ARM
   aqui com binutils).

Me diga por onde começar.
