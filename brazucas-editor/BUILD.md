# Como reconstruir o Brazucas Editor

Pré-requisitos: `java` (JDK 17+), `apktool` 2.10+, `r8`/`d8` (D8), a lib
`apksig`, `uber-apk-signer` (ou `zipalign` + `apksigner`), e a **chave da
equipe** (`brazucas.p12` / `brazucas.pk8` / `brazucas.x509.pem`), que é
distribuída fora deste repositório.

Parâmetros base da adaptação (APK Editor 1.9.0):
- Pacote: `com.gmail.heagoo.apkeditor`
- `libsyscheck.so`: offset da função de verificação por arquitetura —
  arm64-v8a `0x19f4c`, armeabi-v7a `0x1f190`, armeabi `0x1fe2c`, x86 `0x1a643`.
  O patch (`fonte/patch_native.py`) faz essa função retornar `arg ^ 0x55555555`
  (o valor que o app lê como "assinatura original"), desligando o `abort()`
  tardio e a corrupção do nome da chave no lado Java.

## Passos

```bash
# 0) variáveis
ORIG=apk-editor-1.9.0.apk          # APK Editor original
KEY=caminho/para/chave             # pasta com brazucas.pk8 e brazucas.x509.pem
P12=$KEY/brazucas.p12              # keystore (alias: brazucas)

# 1) decodificar
apktool d -f -o editor "$ORIG"

# 2) patch da biblioteca nativa (4 ABIs)
python3 fonte/patch_native.py editor      # edita editor/lib/*/libsyscheck.so

# 3) injetar a classe App e o hook de assinatura
mkdir -p editor/smali/br/brazucas/editor
cp fonte/App.smali editor/smali/br/brazucas/editor/
#    em editor/smali/com/gmail/heagoo/a/c/a.smali, no método de assinatura,
#    trocar o ":goto_0  return-void" pelo bloco de fonte/hook_assinatura.smali

# 4) ajustes de manifesto e recursos
#    - android:name da <application> -> br.brazucas.editor.App
#    - renomear os activity-alias "MainActivity-New<N>" para "MainActivityNew<N>"
#      (no AndroidManifest.xml e em smali/com/gmail/heagoo/seticon/SetIcon.smali)
#    - app_name -> "Brazucas Editor"; minSdk/targetSdk 24 no apktool.yml

# 5) embutir a chave da equipe
cp "$KEY/brazucas.pk8" "$KEY/brazucas.x509.pem" editor/assets/

# 6) compilar o Assinador em dex
javac --release 8 -cp apksig -d classes fonte/Assinador.java
#    (coloque também as classes de apksig em 'classes/')
java -cp r8.jar com.android.tools.r8.D8 --release --min-api 24 \
     --output dexout $(find classes -name '*.class')

# 7) montar o APK
apktool b -o sem_assinatura.apk editor
cp dexout/classes.dex classes2.dex
zip -j sem_assinatura.apk classes2.dex

# 8) zipalign + assinar com a chave da equipe (v1+v2+v3)
java -jar uber-apk-signer.jar -a sem_assinatura.apk \
     --ks "$P12" --ksAlias brazucas --ksPass <senha> --ksKeyPass <senha>

# 9) conferir
java -jar uber-apk-signer.jar -y -a <saida-aligned-signed.apk>
#    deve dizer: zipalign verified / signature verified
```

## Notas

- `minSdk 24` porque a `apksig` usa APIs de Java 8+. O carimbo de origem
  (source stamp, não usado) é a única parte que pediria API 26 — como não é
  chamado, API 24 basta.
- O editor original já fazia a assinatura dele (testkey) antes do nosso hook;
  o hook só reassina por cima com a chave da equipe. É redundante, mas inofensivo.
- Se quiser trocar o pacote (ex.: separar "DLS Prime" do editor público), dá
  para renomear via `apktool` + `--rename-manifest-package`; avise que ajusto a
  receita.
