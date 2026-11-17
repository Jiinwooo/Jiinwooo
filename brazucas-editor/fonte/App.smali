.class public Lbr/brazucas/editor/App;
.super Landroid/support/multidex/MultiDexApplication;
.source "App.java"

# Brazucas Editor: com targetSdk >= 24 o Android bloqueia Intent com file:// (FileUriExposedException).
# O APK Editor usa file:// para abrir o instalador; zerar a VmPolicy desliga essa checagem.

.method public constructor <init>()V
    .locals 0
    invoke-direct {p0}, Landroid/support/multidex/MultiDexApplication;-><init>()V
    return-void
.end method

.method public onCreate()V
    .locals 1
    invoke-super {p0}, Landroid/support/multidex/MultiDexApplication;->onCreate()V
    new-instance v0, Landroid/os/StrictMode$VmPolicy$Builder;
    invoke-direct {v0}, Landroid/os/StrictMode$VmPolicy$Builder;-><init>()V
    invoke-virtual {v0}, Landroid/os/StrictMode$VmPolicy$Builder;->build()Landroid/os/StrictMode$VmPolicy;
    move-result-object v0
    invoke-static {v0}, Landroid/os/StrictMode;->setVmPolicy(Landroid/os/StrictMode$VmPolicy;)V
    return-void
.end method
