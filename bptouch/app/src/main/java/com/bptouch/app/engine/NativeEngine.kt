// SPDX-License-Identifier: MIT OR Apache-2.0
package com.bptouch.app.engine

import android.content.res.AssetManager
import android.graphics.Bitmap

/** Ponte JNI para o motor C++ (libbptouch.so). Todas as chamadas na thread do GL. */
object NativeEngine {
    init { System.loadLibrary("bptouch") }

    @JvmStatic external fun nCreate(am: AssetManager): Long
    @JvmStatic external fun nDestroy(h: Long)
    @JvmStatic external fun nInitGL(h: Long): Boolean
    @JvmStatic external fun nReleaseGL(h: Long)
    @JvmStatic external fun nSurfaceChanged(h: Long, w: Int, hh: Int)
    @JvmStatic external fun nRender(h: Long, t: Float): Boolean
    @JvmStatic external fun nSetView(h: Long, m: FloatArray)
    @JvmStatic external fun nGlInfo(h: Long): String
    @JvmStatic external fun nMaxTex(h: Long): Int
    @JvmStatic external fun nStateVersion(h: Long): Int

    @JvmStatic external fun nNewDocument(h: Long, w: Int, hh: Int, white: Boolean): Boolean
    @JvmStatic external fun nOpenBitmap(h: Long, bmp: Bitmap, name: String): Boolean
    @JvmStatic external fun nAddBitmapLayer(h: Long, bmp: Bitmap, name: String): Boolean
    @JvmStatic external fun nExportToBitmap(h: Long, bmp: Bitmap): Boolean
    @JvmStatic external fun nDocW(h: Long): Int
    @JvmStatic external fun nDocH(h: Long): Int

    @JvmStatic external fun nLayersJson(h: Long): String
    @JvmStatic external fun nHistoryJson(h: Long): String
    @JvmStatic external fun nSetActiveLayer(h: Long, id: Int)
    @JvmStatic external fun nAddLayer(h: Long)
    @JvmStatic external fun nDuplicateLayer(h: Long)
    @JvmStatic external fun nDeleteLayer(h: Long)
    @JvmStatic external fun nMoveLayer(h: Long, delta: Int)
    @JvmStatic external fun nSetLayerVisible(h: Long, id: Int, v: Boolean)
    @JvmStatic external fun nSetLayerOpacity(h: Long, o: Float, fin: Boolean)
    @JvmStatic external fun nSetLayerBlend(h: Long, mode: Int)
    @JvmStatic external fun nSetLayerLockAlpha(h: Long, v: Boolean)
    @JvmStatic external fun nRenameLayer(h: Long, id: Int, name: String)
    @JvmStatic external fun nMergeDown(h: Long)
    @JvmStatic external fun nFlatten(h: Long)
    @JvmStatic external fun nLayerThumb(h: Long, id: Int, bmp: Bitmap): Boolean

    @JvmStatic external fun nStrokeBegin(h: Long, tool: Int, x: Float, y: Float, p: Float, size: Float, hard: Float,
                                         op: Float, rgba: FloatArray, prSize: Boolean, prOp: Boolean)
    @JvmStatic external fun nStrokeMove(h: Long, x: Float, y: Float, p: Float)
    @JvmStatic external fun nStrokeEnd(h: Long)
    @JvmStatic external fun nStrokeCancel(h: Long)
    @JvmStatic external fun nSetCloneSource(h: Long, x: Float, y: Float)
    @JvmStatic external fun nCloneSourceSet(h: Long): Boolean

    @JvmStatic external fun nMoveBegin(h: Long)
    @JvmStatic external fun nMoveUpdate(h: Long, dx: Float, dy: Float)
    @JvmStatic external fun nMoveEnd(h: Long)
    @JvmStatic external fun nMoveCancel(h: Long)

    @JvmStatic external fun nGradientPreview(h: Long, x0: Float, y0: Float, x1: Float, y1: Float, a: FloatArray, b: FloatArray, radial: Boolean)
    @JvmStatic external fun nGradientCommit(h: Long)
    @JvmStatic external fun nGradientCancel(h: Long)
    @JvmStatic external fun nFillSelection(h: Long, rgba: FloatArray)
    @JvmStatic external fun nClearSelectionPixels(h: Long)
    @JvmStatic external fun nBucketFill(h: Long, x: Float, y: Float, tol: Int, contiguous: Boolean, rgba: FloatArray)
    @JvmStatic external fun nSampleColor(h: Long, x: Float, y: Float): Int

    @JvmStatic external fun nSelectPolygon(h: Long, pts: FloatArray, op: Int, feather: Float, aa: Boolean, name: String)
    @JvmStatic external fun nSelectEllipse(h: Long, x0: Float, y0: Float, x1: Float, y1: Float, op: Int, feather: Float, aa: Boolean)
    @JvmStatic external fun nMagicWand(h: Long, x: Float, y: Float, tol: Int, contiguous: Boolean, aa: Boolean, all: Boolean, op: Int)
    @JvmStatic external fun nQuickSelect(h: Long, pts: FloatArray, radius: Float, subtract: Boolean, all: Boolean)
    @JvmStatic external fun nObjectSelect(h: Long, pts: FloatArray, all: Boolean, op: Int)
    @JvmStatic external fun nSelectionBusy(h: Long): Boolean
    @JvmStatic external fun nSelectAll(h: Long)
    @JvmStatic external fun nDeselect(h: Long)
    @JvmStatic external fun nInvertSelection(h: Long)
    @JvmStatic external fun nRefineSelection(h: Long, radius: Float, smooth: Float, contrast: Float, shift: Float, feather: Float)
    @JvmStatic external fun nHasSelection(h: Long): Boolean
    @JvmStatic external fun nSelectionBounds(h: Long): IntArray?

    @JvmStatic external fun nLayerViaCopy(h: Long, cut: Boolean)
    @JvmStatic external fun nCropToSelection(h: Long)
    @JvmStatic external fun nCropRect(h: Long, x0: Int, y0: Int, x1: Int, y1: Int)
    @JvmStatic external fun nRotateCanvas(h: Long, q: Int)
    @JvmStatic external fun nFlipCanvas(h: Long, horizontal: Boolean)

    @JvmStatic external fun nAdjustBegin(h: Long, type: Int)
    @JvmStatic external fun nAdjustSet(h: Long, p: FloatArray, curve: FloatArray?)
    @JvmStatic external fun nAdjustCommit(h: Long)
    @JvmStatic external fun nAdjustCancel(h: Long)

    @JvmStatic external fun nUndo(h: Long): Boolean
    @JvmStatic external fun nRedo(h: Long): Boolean
    @JvmStatic external fun nJumpHistory(h: Long, index: Int)
}
