// SPDX-License-Identifier: MIT OR Apache-2.0
// BP Touch — ponte JNI entre o Kotlin (com.bptouch.app.engine.NativeEngine) e o motor C++.
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/bitmap.h>
#include <string>
#include <vector>
#include "engine.h"

using namespace bp;

namespace {

struct Handle {
    AAssetManager* am = nullptr;
    jobject amRef = nullptr;
    std::unique_ptr<Engine> engine;
};

inline Engine& E(jlong h) { return *reinterpret_cast<Handle*>(h)->engine; }

std::string str(JNIEnv* env, jstring s) {
    if (!s) return {};
    const char* c = env->GetStringUTFChars(s, nullptr);
    std::string r(c);
    env->ReleaseStringUTFChars(s, c);
    return r;
}

std::vector<Pt> pts(JNIEnv* env, jfloatArray a) {
    std::vector<Pt> v;
    if (!a) return v;
    jsize n = env->GetArrayLength(a);
    std::vector<float> f(n);
    env->GetFloatArrayRegion(a, 0, n, f.data());
    for (jsize i = 0; i + 1 < n; i += 2) v.push_back({f[i], f[i + 1]});
    return v;
}

void floats(JNIEnv* env, jfloatArray a, float* out, int n) {
    for (int i = 0; i < n; ++i) out[i] = 0;
    if (!a) return;
    jsize len = std::min<jsize>(n, env->GetArrayLength(a));
    env->GetFloatArrayRegion(a, 0, len, out);
}

// Bitmap ARGB_8888 (bytes RGBA, pré-multiplicado) → vetor compacto
bool readBitmap(JNIEnv* env, jobject bmp, std::vector<uint8_t>& out, int& w, int& h) {
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bmp, &info) != ANDROID_BITMAP_RESULT_SUCCESS) return false;
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) return false;
    void* p = nullptr;
    if (AndroidBitmap_lockPixels(env, bmp, &p) != ANDROID_BITMAP_RESULT_SUCCESS) return false;
    w = (int)info.width; h = (int)info.height;
    out.resize((size_t)w * h * 4);
    for (int y = 0; y < h; ++y) memcpy(&out[(size_t)y * w * 4], (uint8_t*)p + (size_t)y * info.stride, (size_t)w * 4);
    AndroidBitmap_unlockPixels(env, bmp);
    return true;
}

bool writeBitmap(JNIEnv* env, jobject bmp, const std::vector<uint8_t>& in, int w, int h) {
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bmp, &info) != ANDROID_BITMAP_RESULT_SUCCESS) return false;
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 || (int)info.width != w || (int)info.height != h) return false;
    void* p = nullptr;
    if (AndroidBitmap_lockPixels(env, bmp, &p) != ANDROID_BITMAP_RESULT_SUCCESS) return false;
    for (int y = 0; y < h; ++y) memcpy((uint8_t*)p + (size_t)y * info.stride, &in[(size_t)y * w * 4], (size_t)w * 4);
    AndroidBitmap_unlockPixels(env, bmp);
    return true;
}

} // namespace

#define FN(ret, name) extern "C" JNIEXPORT ret JNICALL Java_com_bptouch_app_engine_NativeEngine_##name

FN(jlong, nCreate)(JNIEnv* env, jclass, jobject assetManager) {
    auto* h = new Handle();
    h->amRef = env->NewGlobalRef(assetManager);
    h->am = AAssetManager_fromJava(env, assetManager);
    AAssetManager* am = h->am;
    h->engine = std::make_unique<Engine>([am](const std::string& name) -> std::string {
        AAsset* a = AAssetManager_open(am, name.c_str(), AASSET_MODE_BUFFER);
        if (!a) return {};
        std::string s((const char*)AAsset_getBuffer(a), (size_t)AAsset_getLength(a));
        AAsset_close(a);
        return s;
    });
    return reinterpret_cast<jlong>(h);
}

FN(void, nDestroy)(JNIEnv* env, jclass, jlong h) {
    auto* p = reinterpret_cast<Handle*>(h);
    env->DeleteGlobalRef(p->amRef);
    delete p;
}

FN(jboolean, nInitGL)(JNIEnv*, jclass, jlong h) { return E(h).initGL(); }
FN(void, nReleaseGL)(JNIEnv*, jclass, jlong h) { E(h).releaseGL(); }
FN(void, nSurfaceChanged)(JNIEnv*, jclass, jlong h, jint w, jint hh) { E(h).surfaceChanged(w, hh); }
FN(jboolean, nRender)(JNIEnv*, jclass, jlong h, jfloat t) { return E(h).render(t); }
FN(void, nSetView)(JNIEnv* env, jclass, jlong h, jfloatArray m) { float v[6]; floats(env, m, v, 6); E(h).setView(v); }
FN(jstring, nGlInfo)(JNIEnv* env, jclass, jlong h) { return env->NewStringUTF(E(h).glInfo().c_str()); }
FN(jint, nMaxTex)(JNIEnv*, jclass, jlong h) { return E(h).maxTextureSize(); }
FN(jint, nStateVersion)(JNIEnv*, jclass, jlong h) { return (jint)E(h).stateVersion(); }

FN(jboolean, nNewDocument)(JNIEnv*, jclass, jlong h, jint w, jint hh, jboolean white) { return E(h).newDocument(w, hh, white); }
FN(jboolean, nOpenBitmap)(JNIEnv* env, jclass, jlong h, jobject bmp, jstring name) {
    std::vector<uint8_t> px; int w, hh;
    if (!readBitmap(env, bmp, px, w, hh)) return false;
    return E(h).openImage(px.data(), w, hh, str(env, name));
}
FN(jboolean, nAddBitmapLayer)(JNIEnv* env, jclass, jlong h, jobject bmp, jstring name) {
    std::vector<uint8_t> px; int w, hh;
    if (!readBitmap(env, bmp, px, w, hh)) return false;
    return E(h).addImageLayer(px.data(), w, hh, str(env, name));
}
FN(jboolean, nExportToBitmap)(JNIEnv* env, jclass, jlong h, jobject bmp) {
    std::vector<uint8_t> px;
    if (!E(h).exportComposite(px)) return false;
    return writeBitmap(env, bmp, px, E(h).docW(), E(h).docH());
}
FN(jint, nDocW)(JNIEnv*, jclass, jlong h) { return E(h).docW(); }
FN(jint, nDocH)(JNIEnv*, jclass, jlong h) { return E(h).docH(); }

FN(jstring, nLayersJson)(JNIEnv* env, jclass, jlong h) { return env->NewStringUTF(E(h).layersJson().c_str()); }
FN(jstring, nHistoryJson)(JNIEnv* env, jclass, jlong h) { return env->NewStringUTF(E(h).historyJson().c_str()); }
FN(void, nSetActiveLayer)(JNIEnv*, jclass, jlong h, jint id) { E(h).setActiveLayer(id); }
FN(void, nAddLayer)(JNIEnv*, jclass, jlong h) { E(h).addLayer(); }
FN(void, nDuplicateLayer)(JNIEnv*, jclass, jlong h) { E(h).duplicateLayer(); }
FN(void, nDeleteLayer)(JNIEnv*, jclass, jlong h) { E(h).deleteLayer(); }
FN(void, nMoveLayer)(JNIEnv*, jclass, jlong h, jint d) { E(h).moveLayer(d); }
FN(void, nSetLayerVisible)(JNIEnv*, jclass, jlong h, jint id, jboolean v) { E(h).setLayerVisible(id, v); }
FN(void, nSetLayerOpacity)(JNIEnv*, jclass, jlong h, jfloat o, jboolean fin) { E(h).setLayerOpacity(o, fin); }
FN(void, nSetLayerBlend)(JNIEnv*, jclass, jlong h, jint m) { E(h).setLayerBlend(m); }
FN(void, nSetLayerLockAlpha)(JNIEnv*, jclass, jlong h, jboolean v) { E(h).setLayerLockAlpha(v); }
FN(void, nRenameLayer)(JNIEnv* env, jclass, jlong h, jint id, jstring n) { E(h).renameLayer(id, str(env, n)); }
FN(void, nMergeDown)(JNIEnv*, jclass, jlong h) { E(h).mergeDown(); }
FN(void, nFlatten)(JNIEnv*, jclass, jlong h) { E(h).flatten(); }
FN(jboolean, nLayerThumb)(JNIEnv* env, jclass, jlong h, jint id, jobject bmp) {
    AndroidBitmapInfo info;
    if (AndroidBitmap_getInfo(env, bmp, &info) != ANDROID_BITMAP_RESULT_SUCCESS) return false;
    std::vector<uint8_t> px;
    if (!E(h).layerThumbnail(id, (int)info.width, (int)info.height, px)) return false;
    return writeBitmap(env, bmp, px, (int)info.width, (int)info.height);
}

FN(void, nStrokeBegin)(JNIEnv* env, jclass, jlong h, jint tool, jfloat x, jfloat y, jfloat p, jfloat size, jfloat hard,
                       jfloat op, jfloatArray rgba, jboolean prSize, jboolean prOp) {
    float c[4]; floats(env, rgba, c, 4);
    Tool t = tool == 1 ? Tool::Eraser : tool == 2 ? Tool::Clone : Tool::Brush;
    E(h).strokeBegin(t, x, y, p, size, hard, op, c, prSize, prOp);
}
FN(void, nStrokeMove)(JNIEnv*, jclass, jlong h, jfloat x, jfloat y, jfloat p) { E(h).strokeMove(x, y, p); }
FN(void, nStrokeEnd)(JNIEnv*, jclass, jlong h) { E(h).strokeEnd(); }
FN(void, nStrokeCancel)(JNIEnv*, jclass, jlong h) { E(h).strokeCancel(); }
FN(void, nMoveCancel)(JNIEnv*, jclass, jlong h) { E(h).moveCancel(); }
FN(void, nSetCloneSource)(JNIEnv*, jclass, jlong h, jfloat x, jfloat y) { E(h).setCloneSource(x, y); }
FN(jboolean, nCloneSourceSet)(JNIEnv*, jclass, jlong h) { return E(h).cloneSourceSet(); }

FN(void, nMoveBegin)(JNIEnv*, jclass, jlong h) { E(h).moveBegin(); }
FN(void, nMoveUpdate)(JNIEnv*, jclass, jlong h, jfloat dx, jfloat dy) { E(h).moveUpdate(dx, dy); }
FN(void, nMoveEnd)(JNIEnv*, jclass, jlong h) { E(h).moveEnd(); }

FN(void, nGradientPreview)(JNIEnv* env, jclass, jlong h, jfloat x0, jfloat y0, jfloat x1, jfloat y1, jfloatArray a, jfloatArray b, jboolean radial) {
    float ca[4], cb[4]; floats(env, a, ca, 4); floats(env, b, cb, 4);
    E(h).gradientPreview(x0, y0, x1, y1, ca, cb, radial);
}
FN(void, nGradientCommit)(JNIEnv*, jclass, jlong h) { E(h).gradientCommit(); }
FN(void, nGradientCancel)(JNIEnv*, jclass, jlong h) { E(h).gradientCancel(); }
FN(void, nFillSelection)(JNIEnv* env, jclass, jlong h, jfloatArray rgba) { float c[4]; floats(env, rgba, c, 4); E(h).fillSelection(c); }
FN(void, nClearSelectionPixels)(JNIEnv*, jclass, jlong h) { E(h).clearSelectionPixels(); }
FN(void, nBucketFill)(JNIEnv* env, jclass, jlong h, jfloat x, jfloat y, jint tol, jboolean contig, jfloatArray rgba) {
    float c[4]; floats(env, rgba, c, 4); E(h).bucketFill(x, y, tol, contig, c);
}
FN(jint, nSampleColor)(JNIEnv*, jclass, jlong h, jfloat x, jfloat y) { return (jint)E(h).sampleColor(x, y); }

FN(void, nSelectPolygon)(JNIEnv* env, jclass, jlong h, jfloatArray p, jint op, jfloat feather, jboolean aa, jstring name) {
    std::string n = str(env, name);
    E(h).selectPolygon(pts(env, p), op, feather, aa, n.c_str());
}
FN(void, nSelectEllipse)(JNIEnv*, jclass, jlong h, jfloat x0, jfloat y0, jfloat x1, jfloat y1, jint op, jfloat feather, jboolean aa) {
    E(h).selectEllipse(x0, y0, x1, y1, op, feather, aa);
}
FN(void, nMagicWand)(JNIEnv*, jclass, jlong h, jfloat x, jfloat y, jint tol, jboolean contig, jboolean aa, jboolean all, jint op) {
    E(h).magicWand(x, y, tol, contig, aa, all, op);
}
FN(void, nQuickSelect)(JNIEnv* env, jclass, jlong h, jfloatArray p, jfloat r, jboolean sub, jboolean all) {
    E(h).quickSelect(pts(env, p), r, sub, all);
}
FN(void, nObjectSelect)(JNIEnv* env, jclass, jlong h, jfloatArray p, jboolean all, jint op) { E(h).objectSelect(pts(env, p), all, op); }
FN(jboolean, nSelectionBusy)(JNIEnv*, jclass, jlong h) { return E(h).selectionBusy(); }
FN(void, nSelectAll)(JNIEnv*, jclass, jlong h) { E(h).selectAll(); }
FN(void, nDeselect)(JNIEnv*, jclass, jlong h) { E(h).deselect(); }
FN(void, nInvertSelection)(JNIEnv*, jclass, jlong h) { E(h).invertSelection(); }
FN(void, nRefineSelection)(JNIEnv*, jclass, jlong h, jfloat radius, jfloat smooth, jfloat contrast, jfloat shift, jfloat feather) {
    RefineParams p; p.radius = radius; p.smooth = smooth; p.contrast = contrast; p.shift = shift; p.feather = feather;
    E(h).refineSelection(p);
}
FN(jboolean, nHasSelection)(JNIEnv*, jclass, jlong h) { return E(h).hasSelection(); }
FN(jintArray, nSelectionBounds)(JNIEnv* env, jclass, jlong h) {
    int b[4];
    if (!E(h).selectionBounds(b[0], b[1], b[2], b[3])) return nullptr;
    jintArray r = env->NewIntArray(4);
    env->SetIntArrayRegion(r, 0, 4, b);
    return r;
}

FN(void, nLayerViaCopy)(JNIEnv*, jclass, jlong h, jboolean cut) { E(h).layerViaCopy(cut); }
FN(void, nCropToSelection)(JNIEnv*, jclass, jlong h) { E(h).cropToSelection(); }
FN(void, nCropRect)(JNIEnv*, jclass, jlong h, jint x0, jint y0, jint x1, jint y1) { E(h).cropRect(x0, y0, x1, y1); }
FN(void, nRotateCanvas)(JNIEnv*, jclass, jlong h, jint q) { E(h).rotateCanvas(q); }
FN(void, nFlipCanvas)(JNIEnv*, jclass, jlong h, jboolean hz) { E(h).flipCanvas(hz); }

FN(void, nAdjustBegin)(JNIEnv*, jclass, jlong h, jint t) { E(h).adjustBegin(t); }
FN(void, nAdjustSet)(JNIEnv* env, jclass, jlong h, jfloatArray p, jfloatArray curve) {
    float v[8]; floats(env, p, v, 8);
    if (curve) { float c[256]; floats(env, curve, c, 256); E(h).adjustSet(v, c); }
    else E(h).adjustSet(v, nullptr);
}
FN(void, nAdjustCommit)(JNIEnv*, jclass, jlong h) { E(h).adjustCommit(); }
FN(void, nAdjustCancel)(JNIEnv*, jclass, jlong h) { E(h).adjustCancel(); }

FN(jboolean, nUndo)(JNIEnv*, jclass, jlong h) { return E(h).undo(); }
FN(jboolean, nRedo)(JNIEnv*, jclass, jlong h) { return E(h).redo(); }
FN(void, nJumpHistory)(JNIEnv*, jclass, jlong h, jint i) { E(h).jumpHistory(i); }
