# Brazucas Editor

Editor de APK da equipe, baseado no **APK Editor 1.9.0**, adaptado para que o
time edite o **Brazucas Pet** (texturas, skins, arquivos internos) e salve o
resultado **já reassinado com a chave única da equipe** — pronto pra instalar
por cima e jogar.

> **O APK pronto e a chave de assinatura NÃO ficam neste repositório** (ele é
> público). Eles são distribuídos direto para o time. Aqui fica só a *receita*
> para reconstruir o editor a partir do APK Editor original + a chave.

## O que a adaptação faz

1. **Assinatura v1 + v2 + v3** em todo build (o original só fazia v1, que o
   Android 11+ recusa na instalação).
2. **Chave única da equipe embutida** → todos os builds, de qualquer editor do
   time, ficam compatíveis entre si (instala por cima sem desinstalar).
3. **Checagem anti-reassinatura da `libsyscheck.so` neutralizada** nas 4
   arquiteturas (o editor original se fechava ~5 s depois de ser reassinado).
4. App renomeado para **"Brazucas Editor"**; `minSdk`/`targetSdk` = 24; classe
   `App` que desliga a checagem de `file://` do instalador em Android novo.

## Conteúdo

| Caminho | O que é |
|---|---|
| `fonte/Assinador.java` | Classe que reassina o APK gerado (usa a lib `apksig`). |
| `fonte/App.smali` | Application custom (StrictMode VmPolicy permissiva). |
| `fonte/hook_assinatura.smali` | Trecho injetado no fluxo de salvar do editor. |
| `fonte/patch_native.py` | Patch que neutraliza a checagem de assinatura nativa. |
| `BUILD.md` | Passo a passo para reconstruir o APK. |

## Reconstruir

Veja [`BUILD.md`](BUILD.md). Em resumo: decodifica o APK Editor original com
`apktool`, aplica o patch nativo e o hook smali, adiciona o `Assinador`
compilado como `classes2.dex`, coloca a chave da equipe em `assets/`,
reconstrói, zipalinha e assina com a chave da equipe.
