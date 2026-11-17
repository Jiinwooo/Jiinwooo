# Trecho inserido em smali/com/gmail/heagoo/a/c/a.smali, dentro do metodo
#   a(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;Ljava/util/Map;Ljava/util/Set;)V
# substituindo o rotulo ":goto_0  return-void" por este bloco.
# Depois que o editor gera/assina o APK (caminho em p2), reassina com a chave da equipe.

    :goto_0
    invoke-virtual {p0}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;
    move-result-object v0
    const-string v1, "brazucas.pk8"
    invoke-virtual {v0, v1}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    move-result-object v1
    const-string v2, "brazucas.x509.pem"
    invoke-virtual {v0, v2}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    move-result-object v2
    invoke-static {v1, v2, p2}, Lbr/brazucas/editor/Assinador;->reassinar(Ljava/io/InputStream;Ljava/io/InputStream;Ljava/lang/String;)V
    return-void
