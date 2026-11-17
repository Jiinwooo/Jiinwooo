// SPDX-License-Identifier: MIT OR Apache-2.0
package com.bptouch.app.editor

import android.content.Context
import android.graphics.Bitmap
import android.opengl.GLSurfaceView
import android.os.Handler
import android.os.Looper
import android.os.SystemClock
import android.view.MotionEvent
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateMapOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.ImageBitmap
import androidx.compose.ui.graphics.asImageBitmap
import com.bptouch.app.engine.NativeEngine as N
import java.util.concurrent.ConcurrentLinkedQueue
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import kotlin.math.atan2
import kotlin.math.cos
import kotlin.math.hypot
import kotlin.math.max
import kotlin.math.min
import kotlin.math.sin

/** Opções das ferramentas (barra de opções). */
class ToolOptions {
    var brushSize by mutableFloatStateOf(30f)
    var brushHardness by mutableFloatStateOf(0.8f)
    var brushOpacity by mutableFloatStateOf(1f)
    var eraserSize by mutableFloatStateOf(40f)
    var eraserHardness by mutableFloatStateOf(0.6f)
    var eraserOpacity by mutableFloatStateOf(1f)
    var cloneSize by mutableFloatStateOf(60f)
    var cloneHardness by mutableFloatStateOf(0.5f)
    var cloneOpacity by mutableFloatStateOf(1f)
    var pressureSize by mutableStateOf(true)
    var pressureOpacity by mutableStateOf(false)
    var selOp by mutableIntStateOf(0)            // 0 nova, 1 adicionar, 2 subtrair, 3 interseção
    var feather by mutableFloatStateOf(0f)
    var antialias by mutableStateOf(true)
    var wandTolerance by mutableIntStateOf(32)
    var contiguous by mutableStateOf(true)
    var sampleAll by mutableStateOf(false)
    var quickSize by mutableFloatStateOf(40f)
    var quickSubtract by mutableStateOf(false)
    var gradientRadial by mutableStateOf(false)
    var bucketTolerance by mutableIntStateOf(32)
    var cloneSettingSource by mutableStateOf(false)
}

/** Formas desenhadas por cima da tela (em coordenadas do documento). */
class Overlay {
    var rectA by mutableStateOf<Offset?>(null)
    var rectB by mutableStateOf<Offset?>(null)
    val path = mutableStateListOf<Offset>()
    var pathRadius by mutableFloatStateOf(0f)
    var polyCursor by mutableStateOf<Offset?>(null)
    var cropA by mutableStateOf<Offset?>(null)
    var cropB by mutableStateOf<Offset?>(null)
    var cloneSrc by mutableStateOf<Offset?>(null)
    var cursor by mutableStateOf<Offset?>(null)
    var cursorRadius by mutableFloatStateOf(0f)
    fun clearDrag() { rectA = null; rectB = null; path.clear(); polyCursor = null }
}

/** Transformação de vista: tela = R(θ)·s·doc + t. */
data class ViewXf(val s: Float = 1f, val rot: Float = 0f, val tx: Float = 0f, val ty: Float = 0f) {
    fun toScreen(x: Float, y: Float): Offset {
        val c = cos(rot) * s; val sn = sin(rot) * s
        return Offset(c * x - sn * y + tx, sn * x + c * y + ty)
    }
    fun toDoc(x: Float, y: Float): Offset {
        val dx = x - tx; val dy = y - ty
        val c = cos(rot) / s; val sn = sin(rot) / s
        return Offset(c * dx + sn * dy, -sn * dx + c * dy)
    }
    fun matrix(): FloatArray {
        val c = cos(rot) * s; val sn = sin(rot) * s
        return floatArrayOf(c, sn, -sn, c, tx, ty)
    }
}

class Editor(context: Context) {
    val handle: Long = N.nCreate(context.assets)
    private val main = Handler(Looper.getMainLooper())
    private val queue = ConcurrentLinkedQueue<(Long) -> Unit>()
    var glView: GLSurfaceView? = null
        private set

    // estado observado pela UI
    var doc by mutableStateOf(DocState())
    var history by mutableStateOf(HistoryState())
    val thumbs = mutableStateMapOf<Int, ImageBitmap>()
    var view by mutableStateOf(ViewXf())
    var viewW by mutableIntStateOf(0)
    var viewH by mutableIntStateOf(0)
    var glInfo by mutableStateOf("")
    var maxTex by mutableIntStateOf(4096)
    var glError by mutableStateOf<String?>(null)

    var tool by mutableStateOf(ToolId.QUICK_SELECT)
    val opts = ToolOptions()
    val overlay = Overlay()
    var fg by mutableIntStateOf(0xFF000000.toInt())
    var bg by mutableIntStateOf(0xFFFFFFFF.toInt())
    var adjusting by mutableStateOf<AdjustType?>(null)
    var toast by mutableStateOf<String?>(null)

    private var glReady = false
    private var lastVersion = -1
    private var continuous = false
    private var fitPending = true
    private val t0 = SystemClock.elapsedRealtime()

    // ------------------------------------------------------------------ GL

    fun attach(v: GLSurfaceView) {
        glView = v
        v.setEGLContextClientVersion(3)
        v.setEGLConfigChooser(8, 8, 8, 8, 0, 0)
        v.preserveEGLContextOnPause = true
        v.setRenderer(object : GLSurfaceView.Renderer {
            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
                if (glReady) { N.nReleaseGL(handle); main.post { toast = "O contexto gráfico foi recriado; o documento precisou ser reaberto." } }
                val ok = N.nInitGL(handle)
                glReady = ok
                val info = N.nGlInfo(handle); val mt = N.nMaxTex(handle)
                main.post { glInfo = info; maxTex = mt; if (!ok) glError = "Falha ao compilar os shaders do motor (OpenGL ES 3.0)." }
                lastVersion = -1
            }
            override fun onSurfaceChanged(gl: GL10?, w: Int, h: Int) {
                N.nSurfaceChanged(handle, w, h)
                main.post { viewW = w; viewH = h; if (fitPending && doc.open) fit() }
            }
            override fun onDrawFrame(gl: GL10?) {
                if (!glReady) return
                while (true) { val job = queue.poll() ?: break; job(handle) }
                N.nSetView(handle, view.matrix())
                val cont = N.nRender(handle, (SystemClock.elapsedRealtime() - t0) / 1000f)
                if (cont != continuous) {
                    continuous = cont
                    v.renderMode = if (cont) GLSurfaceView.RENDERMODE_CONTINUOUSLY else GLSurfaceView.RENDERMODE_WHEN_DIRTY
                }
                syncState()
            }
        })
        v.renderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY
    }

    /** Executa no thread do GL e redesenha. */
    fun post(job: (Long) -> Unit) {
        queue.add(job)
        glView?.requestRender()
    }

    fun requestRender() = glView?.requestRender()

    private fun syncState() {
        val ver = N.nStateVersion(handle)
        if (ver == lastVersion) return
        lastVersion = ver
        val d = DocState.parse(N.nLayersJson(handle))
        val hs = HistoryState.parse(N.nHistoryJson(handle))
        val th = HashMap<Int, ImageBitmap>()
        if (d.open) {
            val tw = 72; val tht = (tw * d.h / d.w.toFloat()).toInt().coerceIn(18, 72)
            for (l in d.layers) {
                val b = Bitmap.createBitmap(tw, tht, Bitmap.Config.ARGB_8888)
                if (N.nLayerThumb(handle, l.id, b)) th[l.id] = b.asImageBitmap()
            }
        }
        main.post {
            val sizeChanged = d.w != doc.w || d.h != doc.h
            doc = d; history = hs
            thumbs.clear(); thumbs.putAll(th)
            if (sizeChanged || fitPending) fit()
        }
    }

    fun fit() {
        if (!doc.open || viewW == 0) { fitPending = true; return }
        fitPending = false
        val m = 24f
        val s = min((viewW - 2 * m) / doc.w, (viewH - 2 * m) / doc.h)
        view = ViewXf(s, 0f, (viewW - doc.w * s) / 2f, (viewH - doc.h * s) / 2f)
        requestRender()
    }

    fun zoomTo100() {
        if (!doc.open) return
        val c = view.toDoc(viewW / 2f, viewH / 2f)
        view = ViewXf(1f, 0f, viewW / 2f - c.x, viewH / 2f - c.y)
        requestRender()
    }

    // ------------------------------------------------------------------ documento

    fun openBitmap(b: Bitmap, name: String) {
        fitPending = true
        post { h ->
            val ok = N.nOpenBitmap(h, b, name)
            main.post { if (!ok) toast = "Não foi possível abrir a imagem (${b.width}×${b.height})." else { overlay.cropA = null; overlay.cropB = null } }
        }
    }

    fun newDocument(w: Int, h: Int, white: Boolean) {
        fitPending = true
        post { N.nNewDocument(it, w, h, white) }
    }

    fun addBitmapLayer(b: Bitmap, name: String) = post { N.nAddBitmapLayer(it, b, name) }

    fun export(onReady: (Bitmap?) -> Unit) {
        post { h ->
            val w = N.nDocW(h); val hh = N.nDocH(h)
            val b = if (w > 0) Bitmap.createBitmap(w, hh, Bitmap.Config.ARGB_8888) else null
            val ok = b != null && N.nExportToBitmap(h, b)
            main.post { onReady(if (ok) b else null) }
        }
    }

    fun undo() = post { N.nUndo(it) }
    fun redo() = post { N.nRedo(it) }

    private fun colorF(c: Int) = floatArrayOf(((c shr 16) and 255) / 255f, ((c shr 8) and 255) / 255f, (c and 255) / 255f, ((c ushr 24) and 255) / 255f)
    fun fgF() = colorF(fg)
    fun bgF() = colorF(bg)
    fun swapColors() { val t = fg; fg = bg; bg = t }
    fun defaultColors() { fg = 0xFF000000.toInt(); bg = 0xFFFFFFFF.toInt() }

    fun selectTool(t: ToolId) {
        if (t == tool) return
        cancelTransient()
        tool = t
        overlay.clearDrag()
        if (t != ToolId.CROP) { overlay.cropA = null; overlay.cropB = null }
        opts.cloneSettingSource = false
    }

    private fun cancelTransient() {
        if (polyPts.isNotEmpty()) { polyPts.clear(); overlay.clearDrag() }
    }

    // ------------------------------------------------------------------ toque

    private enum class Mode { NONE, PENDING, TOOL, VIEW }
    private var mode = Mode.NONE
    private var downX = 0f; private var downY = 0f; private var downT = 0L
    private var gStartXf = ViewXf(); private var gCx = 0f; private var gCy = 0f; private var gDist = 1f; private var gAng = 0f
    private var toolStartedAt = 0L
    private var docStart = Offset.Zero
    private val polyPts = ArrayList<Offset>()
    private var lastTapT = 0L

    private fun pressure(e: MotionEvent, i: Int): Float {
        val t = e.getToolType(i)
        return if (t == MotionEvent.TOOL_TYPE_STYLUS || t == MotionEvent.TOOL_TYPE_ERASER) e.getPressure(i).coerceIn(0.05f, 1f) else 1f
    }

    fun onTouch(e: MotionEvent): Boolean {
        if (!doc.open) return true
        when (e.actionMasked) {
            MotionEvent.ACTION_DOWN -> {
                downX = e.x; downY = e.y; downT = e.eventTime
                mode = Mode.PENDING
                // ferramentas de toque único respondem já
                if (tool == ToolId.HAND) { mode = Mode.VIEW; startGesture(e) }
                else beginTool(e)
            }
            MotionEvent.ACTION_POINTER_DOWN -> {
                if (mode == Mode.TOOL) {
                    // segundo dedo logo depois de começar = era gesto de vista: desfaz o início
                    if (e.eventTime - toolStartedAt < 300) cancelTool() else endTool(e, true)
                } else if (mode == Mode.PENDING) cancelTool()
                mode = Mode.VIEW
                startGesture(e)
            }
            MotionEvent.ACTION_MOVE -> when (mode) {
                Mode.VIEW -> updateGesture(e)
                Mode.TOOL, Mode.PENDING -> { mode = Mode.TOOL; moveTool(e) }
                else -> {}
            }
            MotionEvent.ACTION_POINTER_UP -> if (mode == Mode.VIEW) {
                // recomeça o gesto com os dedos que sobraram
                val keep = (0 until e.pointerCount).filter { it != e.actionIndex }
                if (keep.size >= 1) startGesture(e, skip = e.actionIndex)
            }
            MotionEvent.ACTION_UP -> {
                if (mode == Mode.TOOL || mode == Mode.PENDING) endTool(e, false)
                else if (mode == Mode.VIEW && tool == ToolId.HAND && e.eventTime - downT < 250 && hypot(e.x - downX, e.y - downY) < 12) {
                    if (e.eventTime - lastTapT < 350) fit()
                    lastTapT = e.eventTime
                }
                mode = Mode.NONE
            }
            MotionEvent.ACTION_CANCEL -> { if (mode == Mode.TOOL || mode == Mode.PENDING) cancelTool(); mode = Mode.NONE }
            MotionEvent.ACTION_HOVER_MOVE -> {
                val d = view.toDoc(e.x, e.y)
                overlay.cursor = d
                overlay.cursorRadius = brushRadiusFor(tool)
            }
            MotionEvent.ACTION_HOVER_EXIT -> overlay.cursor = null
        }
        return true
    }

    private fun startGesture(e: MotionEvent, skip: Int = -1) {
        val idx = (0 until e.pointerCount).filter { it != skip }
        gStartXf = view
        if (idx.size >= 2) {
            val a = idx[0]; val b = idx[1]
            gCx = (e.getX(a) + e.getX(b)) / 2; gCy = (e.getY(a) + e.getY(b)) / 2
            gDist = max(1f, hypot(e.getX(a) - e.getX(b), e.getY(a) - e.getY(b)))
            gAng = atan2(e.getY(b) - e.getY(a), e.getX(b) - e.getX(a))
        } else if (idx.isNotEmpty()) {
            gCx = e.getX(idx[0]); gCy = e.getY(idx[0]); gDist = 0f
        }
    }

    private fun updateGesture(e: MotionEvent) {
        val n = e.pointerCount
        val anchor = gStartXf.toDoc(gCx, gCy)
        if (n >= 2 && gDist > 0f) {
            val cx = (e.getX(0) + e.getX(1)) / 2; val cy = (e.getY(0) + e.getY(1)) / 2
            val d = max(1f, hypot(e.getX(0) - e.getX(1), e.getY(0) - e.getY(1)))
            val ang = atan2(e.getY(1) - e.getY(0), e.getX(1) - e.getX(0))
            val s = (gStartXf.s * d / gDist).coerceIn(0.02f, 64f)
            var rot = gStartXf.rot + (ang - gAng)
            // encaixa em 0°/90°/180°/270° quando perto
            val q = Math.round(rot / (Math.PI.toFloat() / 2)) * (Math.PI.toFloat() / 2)
            if (kotlin.math.abs(rot - q) < 0.06f) rot = q
            val c = cos(rot) * s; val sn = sin(rot) * s
            view = ViewXf(s, rot, cx - (c * anchor.x - sn * anchor.y), cy - (sn * anchor.x + c * anchor.y))
        } else {
            val dx = e.x - gCx; val dy = e.y - gCy
            view = gStartXf.copy(tx = gStartXf.tx + dx, ty = gStartXf.ty + dy)
        }
        requestRender()
    }

    private fun brushRadiusFor(t: ToolId) = when (t) {
        ToolId.BRUSH -> opts.brushSize / 2
        ToolId.ERASER -> opts.eraserSize / 2
        ToolId.CLONE -> opts.cloneSize / 2
        ToolId.QUICK_SELECT -> opts.quickSize / 2
        else -> 0f
    }

    private fun beginTool(e: MotionEvent) {
        val d = view.toDoc(e.x, e.y)
        docStart = d
        toolStartedAt = e.eventTime
        val p = pressure(e, 0)
        when (tool) {
            ToolId.BRUSH -> { val c = fgF(); post { N.nStrokeBegin(it, 0, d.x, d.y, p, opts.brushSize, opts.brushHardness, opts.brushOpacity, c, opts.pressureSize, opts.pressureOpacity) }; mode = Mode.TOOL }
            ToolId.ERASER -> { val c = fgF(); post { N.nStrokeBegin(it, 1, d.x, d.y, p, opts.eraserSize, opts.eraserHardness, opts.eraserOpacity, c, opts.pressureSize, opts.pressureOpacity) }; mode = Mode.TOOL }
            ToolId.CLONE -> {
                if (opts.cloneSettingSource) {
                    post { N.nSetCloneSource(it, d.x, d.y) }
                    overlay.cloneSrc = d
                    opts.cloneSettingSource = false
                    mode = Mode.NONE
                } else if (overlay.cloneSrc == null) {
                    toast = "Toque em “Definir origem” e depois no ponto que será copiado."
                    mode = Mode.NONE
                } else {
                    val c = fgF(); post { N.nStrokeBegin(it, 2, d.x, d.y, p, opts.cloneSize, opts.cloneHardness, opts.cloneOpacity, c, opts.pressureSize, opts.pressureOpacity) }
                    mode = Mode.TOOL
                }
            }
            ToolId.MOVE -> { post { N.nMoveBegin(it) }; mode = Mode.TOOL }
            ToolId.MARQUEE_RECT, ToolId.MARQUEE_ELLIPSE, ToolId.OBJECT_SELECT -> { overlay.rectA = d; overlay.rectB = d }
            ToolId.CROP -> { overlay.cropA = d; overlay.cropB = d }
            ToolId.LASSO -> { overlay.path.clear(); overlay.path.add(d); overlay.pathRadius = 0f }
            ToolId.LASSO_POLY -> {}
            ToolId.QUICK_SELECT -> { overlay.path.clear(); overlay.path.add(d); overlay.pathRadius = opts.quickSize / 2 }
            ToolId.GRADIENT -> { overlay.rectA = d; overlay.rectB = d }
            ToolId.EYEDROPPER -> pick(d)
            else -> {}
        }
        overlay.cursor = if (tool in listOf(ToolId.BRUSH, ToolId.ERASER, ToolId.CLONE, ToolId.QUICK_SELECT)) d else null
        overlay.cursorRadius = brushRadiusFor(tool)
    }

    private fun moveTool(e: MotionEvent) {
        val d = view.toDoc(e.x, e.y)
        when (tool) {
            ToolId.BRUSH, ToolId.ERASER, ToolId.CLONE -> {
                // eventos históricos dão traço mais liso
                val pts = ArrayList<FloatArray>()
                for (i in 0 until e.historySize) {
                    val hd = view.toDoc(e.getHistoricalX(i), e.getHistoricalY(i))
                    pts.add(floatArrayOf(hd.x, hd.y, if (e.getToolType(0) == MotionEvent.TOOL_TYPE_STYLUS) e.getHistoricalPressure(i).coerceIn(0.05f, 1f) else 1f))
                }
                pts.add(floatArrayOf(d.x, d.y, pressure(e, 0)))
                post { h -> for (q in pts) N.nStrokeMove(h, q[0], q[1], q[2]) }
                overlay.cursor = d
            }
            ToolId.MOVE -> { val dx = d.x - docStart.x; val dy = d.y - docStart.y; post { N.nMoveUpdate(it, dx, dy) } }
            ToolId.MARQUEE_RECT, ToolId.MARQUEE_ELLIPSE, ToolId.OBJECT_SELECT -> overlay.rectB = constrain(d)
            ToolId.CROP -> overlay.cropB = d
            ToolId.LASSO -> overlay.path.add(d)
            ToolId.LASSO_POLY -> overlay.polyCursor = d
            ToolId.QUICK_SELECT -> { overlay.path.add(d); overlay.cursor = d }
            ToolId.GRADIENT -> {
                overlay.rectB = d
                val a = docStart; val r = opts.gradientRadial; val c0 = fgF(); val c1 = bgF()
                post { N.nGradientPreview(it, a.x, a.y, d.x, d.y, c0, c1, r) }
            }
            ToolId.EYEDROPPER -> pick(d)
            else -> {}
        }
    }

    private fun constrain(d: Offset) = d

    private fun endTool(e: MotionEvent, interrupted: Boolean) {
        val d = view.toDoc(e.x, e.y)
        val moved = hypot(e.x - downX, e.y - downY) > 10
        val op = opts.selOp
        when (tool) {
            ToolId.BRUSH, ToolId.ERASER, ToolId.CLONE -> post { N.nStrokeEnd(it) }
            ToolId.MOVE -> post { N.nMoveEnd(it) }
            ToolId.MARQUEE_RECT, ToolId.MARQUEE_ELLIPSE -> {
                val a = overlay.rectA; val b = overlay.rectB
                if (a != null && b != null && moved) {
                    if (tool == ToolId.MARQUEE_RECT) {
                        val pts = floatArrayOf(a.x, a.y, b.x, a.y, b.x, b.y, a.x, b.y)
                        val f = opts.feather; val aa = opts.antialias
                        post { N.nSelectPolygon(it, pts, op, f, aa, "Letreiro retangular") }
                    } else {
                        val f = opts.feather; val aa = opts.antialias
                        post { N.nSelectEllipse(it, a.x, a.y, b.x, b.y, op, f, aa) }
                    }
                } else if (!moved && op == 0) post { N.nDeselect(it) }
                overlay.clearDrag()
            }
            ToolId.OBJECT_SELECT -> {
                val a = overlay.rectA; val b = overlay.rectB
                if (a != null && b != null && moved) {
                    val pts = floatArrayOf(a.x, a.y, b.x, a.y, b.x, b.y, a.x, b.y)
                    val all = opts.sampleAll
                    post { N.nObjectSelect(it, pts, all, op) }
                }
                overlay.clearDrag()
            }
            ToolId.LASSO -> {
                overlay.path.add(d)
                if (overlay.path.size >= 3 && moved) {
                    val pts = FloatArray(overlay.path.size * 2)
                    overlay.path.forEachIndexed { i, o -> pts[i * 2] = o.x; pts[i * 2 + 1] = o.y }
                    val f = opts.feather; val aa = opts.antialias
                    post { N.nSelectPolygon(it, pts, op, f, aa, "Laço") }
                } else if (!moved && op == 0) post { N.nDeselect(it) }
                overlay.clearDrag()
            }
            ToolId.LASSO_POLY -> {
                // toque adiciona vértice; toque perto do primeiro (ou duplo) fecha
                val first = polyPts.firstOrNull()
                val closeDist = 24f / view.s
                val dbl = e.eventTime - lastTapT < 300
                lastTapT = e.eventTime
                if (first != null && (hypot(d.x - first.x, d.y - first.y) < closeDist || dbl) && polyPts.size >= 3) {
                    val pts = FloatArray(polyPts.size * 2)
                    polyPts.forEachIndexed { i, o -> pts[i * 2] = o.x; pts[i * 2 + 1] = o.y }
                    val f = opts.feather; val aa = opts.antialias
                    post { N.nSelectPolygon(it, pts, op, f, aa, "Laço poligonal") }
                    polyPts.clear(); overlay.clearDrag()
                } else {
                    polyPts.add(d)
                    overlay.path.clear(); overlay.path.addAll(polyPts); overlay.polyCursor = null
                }
            }
            ToolId.QUICK_SELECT -> {
                overlay.path.add(d)
                val pts = FloatArray(overlay.path.size * 2)
                overlay.path.forEachIndexed { i, o -> pts[i * 2] = o.x; pts[i * 2 + 1] = o.y }
                val r = opts.quickSize / 2; val sub = opts.quickSubtract || op == 2; val all = opts.sampleAll
                post { N.nQuickSelect(it, pts, r, sub, all) }
                overlay.clearDrag()
                overlay.cursor = null
            }
            ToolId.MAGIC_WAND -> {
                val tol = opts.wandTolerance; val c = opts.contiguous; val aa = opts.antialias; val all = opts.sampleAll
                post { N.nMagicWand(it, d.x, d.y, tol, c, aa, all, op) }
            }
            ToolId.GRADIENT -> {
                if (moved) post { N.nGradientCommit(it) } else post { N.nGradientCancel(it) }
                overlay.clearDrag()
            }
            ToolId.BUCKET -> {
                val tol = opts.bucketTolerance; val c = opts.contiguous; val col = fgF()
                post { N.nBucketFill(it, d.x, d.y, tol, c, col) }
            }
            ToolId.CROP -> if (!moved) { overlay.cropA = null; overlay.cropB = null }
            else -> {}
        }
        if (tool != ToolId.QUICK_SELECT) overlay.cursor = null
        if (interrupted) overlay.clearDrag()
    }

    private fun cancelTool() {
        when (tool) {
            ToolId.BRUSH, ToolId.ERASER, ToolId.CLONE -> post { N.nStrokeCancel(it) }
            ToolId.MOVE -> post { N.nMoveCancel(it) }
            ToolId.GRADIENT -> post { N.nGradientCancel(it) }
            ToolId.CROP -> { overlay.cropA = null; overlay.cropB = null }
            else -> {}
        }
        if (tool != ToolId.LASSO_POLY) overlay.clearDrag()
        overlay.cursor = null
        mode = Mode.NONE
    }

    private fun pick(d: Offset) {
        post { h ->
            val c = N.nSampleColor(h, d.x, d.y)
            main.post { if ((c ushr 24) != 0) fg = c or 0xFF000000.toInt() }
        }
    }

    fun applyCrop() {
        val a = overlay.cropA ?: return; val b = overlay.cropB ?: return
        val x0 = min(a.x, b.x).toInt(); val y0 = min(a.y, b.y).toInt()
        val x1 = max(a.x, b.x).toInt(); val y1 = max(a.y, b.y).toInt()
        overlay.cropA = null; overlay.cropB = null
        post { N.nCropRect(it, x0, y0, x1, y1) }
    }

    fun closePolyLasso() {
        if (polyPts.size < 3) return
        val pts = FloatArray(polyPts.size * 2)
        polyPts.forEachIndexed { i, o -> pts[i * 2] = o.x; pts[i * 2 + 1] = o.y }
        val op = opts.selOp; val f = opts.feather; val aa = opts.antialias
        post { N.nSelectPolygon(it, pts, op, f, aa, "Laço poligonal") }
        polyPts.clear(); overlay.clearDrag()
    }

    fun destroy() {
        glView?.queueEvent { N.nDestroy(handle) }
    }
}
