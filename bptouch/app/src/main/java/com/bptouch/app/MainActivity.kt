// SPDX-License-Identifier: MIT OR Apache-2.0
package com.bptouch.app

import android.content.ContentValues
import android.content.Intent
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.ImageDecoder
import android.graphics.Matrix
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.provider.MediaStore
import android.view.KeyEvent
import android.view.WindowManager
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.result.PickVisualMediaRequest
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat
import androidx.exifinterface.media.ExifInterface
import com.bptouch.app.editor.Editor
import com.bptouch.app.editor.TOOL_GROUPS
import com.bptouch.app.engine.NativeEngine as N
import com.bptouch.app.ui.AboutDialog
import com.bptouch.app.ui.BpTheme
import com.bptouch.app.ui.EditorActions
import com.bptouch.app.ui.EditorScreen
import com.bptouch.app.ui.NewDocumentDialog
import java.io.File
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
import java.util.concurrent.Executors
import kotlin.math.max

class MainActivity : ComponentActivity(), EditorActions {
    private lateinit var editor: Editor
    private val io = Executors.newSingleThreadExecutor()
    private var pickForLayer = false
    private var newDocDialog by mutableStateOf(false)
    private var aboutDialog by mutableStateOf(false)
    private var maxSide by mutableIntStateOf(4096)
    private var lastSaved: Uri? = null

    private val pickPhoto = registerForActivityResult(ActivityResultContracts.PickVisualMedia()) { uri -> uri?.let { load(it, pickForLayer) } }
    private val saveLegacy = registerForActivityResult(ActivityResultContracts.CreateDocument("image/jpeg")) { uri -> uri?.let { writeTo(it, "jpeg") } }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        WindowCompat.setDecorFitsSystemWindows(window, false)
        WindowInsetsControllerCompat(window, window.decorView).apply {
            hide(WindowInsetsCompat.Type.systemBars())
            systemBarsBehavior = WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
        }
        if (Build.VERSION.SDK_INT >= 28) window.attributes.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES
        maxSide = getPreferences(MODE_PRIVATE).getInt("maxSide", 4096)
        editor = Editor(this)
        setContent {
            BpTheme {
                EditorScreen(editor, this)
                if (newDocDialog) NewDocumentDialog(max(editor.viewW, 1), max(editor.viewH, 1), editor.maxTex, { newDocDialog = false }) { w, h, white ->
                    newDocDialog = false; editor.newDocument(w, h, white)
                }
                if (aboutDialog) AboutDialog(editor.glInfo, BuildConfigInfo.version(this), maxSide, { maxSide = it; getPreferences(MODE_PRIVATE).edit().putInt("maxSide", it).apply() }) { aboutDialog = false }
            }
        }
        handleIntent(intent)
    }

    override fun onNewIntent(intent: Intent) { super.onNewIntent(intent); handleIntent(intent) }

    private fun handleIntent(i: Intent?) {
        val uri: Uri? = when (i?.action) {
            Intent.ACTION_SEND -> if (Build.VERSION.SDK_INT >= 33) i.getParcelableExtra(Intent.EXTRA_STREAM, Uri::class.java) else @Suppress("DEPRECATION") i.getParcelableExtra(Intent.EXTRA_STREAM)
            Intent.ACTION_VIEW, Intent.ACTION_EDIT -> i.data
            else -> null
        }
        uri?.let { load(it, false) }
    }

    override fun onResume() { super.onResume(); editor.glView?.onResume() }
    override fun onPause() { editor.glView?.onPause(); super.onPause() }
    override fun onDestroy() { editor.destroy(); super.onDestroy() }

    // ------------------------------------------------------------ abrir

    override fun openPhoto() { pickForLayer = false; pickPhoto.launch(PickVisualMediaRequest(ActivityResultContracts.PickVisualMedia.ImageOnly)) }
    override fun addPhotoLayer() { pickForLayer = true; pickPhoto.launch(PickVisualMediaRequest(ActivityResultContracts.PickVisualMedia.ImageOnly)) }
    override fun newDocument() { newDocDialog = true }
    override fun about() { aboutDialog = true }

    private fun load(uri: Uri, asLayer: Boolean) {
        val limit = minOf(maxSide, editor.maxTex)
        io.execute {
            val bmp = try { decode(uri, limit) } catch (t: Throwable) { null }
            runOnUiThread {
                if (bmp == null) { editor.toast = "Não consegui ler essa imagem."; return@runOnUiThread }
                val name = uri.lastPathSegment ?: "foto"
                if (asLayer && editor.doc.open) editor.addBitmapLayer(bmp, "Foto") else editor.openBitmap(bmp, name)
            }
        }
    }

    /** Decodifica qualquer formato que o Android suporta (JPEG, PNG, WebP, HEIF/AVIF, GIF, DNG em parte),
     *  aplica a rotação do EXIF e reduz para caber no limite. Sempre ARGB_8888 sRGB. */
    private fun decode(uri: Uri, limit: Int): Bitmap? {
        if (Build.VERSION.SDK_INT >= 28) {
            val src = ImageDecoder.createSource(contentResolver, uri)
            val b = ImageDecoder.decodeBitmap(src) { dec, info, _ ->
                dec.allocator = ImageDecoder.ALLOCATOR_SOFTWARE
                dec.setTargetColorSpace(android.graphics.ColorSpace.get(android.graphics.ColorSpace.Named.SRGB))
                val w = info.size.width; val h = info.size.height
                val s = max(w, h)
                if (s > limit) { val k = limit.toFloat() / s; dec.setTargetSize(max(1, (w * k).toInt()), max(1, (h * k).toInt())) }
            }
            return if (b.config != Bitmap.Config.ARGB_8888) b.copy(Bitmap.Config.ARGB_8888, false) else b
        }
        val opts = BitmapFactory.Options().apply { inJustDecodeBounds = true }
        contentResolver.openInputStream(uri)?.use { BitmapFactory.decodeStream(it, null, opts) }
        var sample = 1
        while (max(opts.outWidth, opts.outHeight) / (sample * 2) >= limit) sample *= 2
        val o2 = BitmapFactory.Options().apply { inSampleSize = sample; inPreferredConfig = Bitmap.Config.ARGB_8888 }
        var b = contentResolver.openInputStream(uri)?.use { BitmapFactory.decodeStream(it, null, o2) } ?: return null
        val rot = contentResolver.openInputStream(uri)?.use { ExifInterface(it).rotationDegrees } ?: 0
        val s = max(b.width, b.height)
        if (rot != 0 || s > limit) {
            val m = Matrix()
            if (s > limit) { val k = limit.toFloat() / s; m.postScale(k, k) }
            if (rot != 0) m.postRotate(rot.toFloat())
            b = Bitmap.createBitmap(b, 0, 0, b.width, b.height, m, true)
        }
        return b
    }

    // ------------------------------------------------------------ salvar

    override fun save(format: String) {
        if (!editor.doc.open) return
        if (Build.VERSION.SDK_INT < 29) {
            if (format == "jpeg") saveLegacy.launch(fileName("jpg")) else editor.toast = "Neste Android, salve em JPEG."
            return
        }
        editor.export { bmp ->
            if (bmp == null) { editor.toast = "Falha ao gerar a imagem."; return@export }
            io.execute {
                val (mime, ext) = when (format) { "png" -> "image/png" to "png"; "webp" -> "image/webp" to "webp"; else -> "image/jpeg" to "jpg" }
                val values = ContentValues().apply {
                    put(MediaStore.Images.Media.DISPLAY_NAME, fileName(ext))
                    put(MediaStore.Images.Media.MIME_TYPE, mime)
                    put(MediaStore.Images.Media.RELATIVE_PATH, Environment.DIRECTORY_PICTURES + "/BP Touch")
                    put(MediaStore.Images.Media.IS_PENDING, 1)
                }
                val uri = contentResolver.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values)
                var ok = false
                if (uri != null) {
                    contentResolver.openOutputStream(uri)?.use { ok = compress(bmp, format, it) }
                    values.clear(); values.put(MediaStore.Images.Media.IS_PENDING, 0)
                    contentResolver.update(uri, values, null, null)
                }
                runOnUiThread {
                    lastSaved = uri
                    editor.toast = if (ok) "Salvo em Imagens/BP Touch (${bmp.width}×${bmp.height})." else "Não foi possível salvar."
                }
            }
        }
    }

    private fun writeTo(uri: Uri, format: String) {
        editor.export { bmp ->
            if (bmp == null) return@export
            io.execute {
                val ok = contentResolver.openOutputStream(uri)?.use { compress(bmp, format, it) } ?: false
                runOnUiThread { lastSaved = uri; editor.toast = if (ok) "Imagem salva." else "Não foi possível salvar." }
            }
        }
    }

    private fun compress(b: Bitmap, format: String, out: java.io.OutputStream): Boolean {
        // JPEG não tem transparência: compõe sobre branco
        return when (format) {
            "png" -> b.compress(Bitmap.CompressFormat.PNG, 100, out)
            "webp" -> b.compress(if (Build.VERSION.SDK_INT >= 30) Bitmap.CompressFormat.WEBP_LOSSLESS else @Suppress("DEPRECATION") Bitmap.CompressFormat.WEBP, 100, out)
            else -> {
                val flat = Bitmap.createBitmap(b.width, b.height, Bitmap.Config.ARGB_8888)
                val c = android.graphics.Canvas(flat); c.drawColor(android.graphics.Color.WHITE); c.drawBitmap(b, 0f, 0f, null)
                flat.compress(Bitmap.CompressFormat.JPEG, 95, out)
            }
        }
    }

    override fun share() {
        editor.export { bmp ->
            if (bmp == null) return@export
            io.execute {
                val dir = File(cacheDir, "share").apply { mkdirs() }
                val f = File(dir, "bptouch.png")
                f.outputStream().use { bmp.compress(Bitmap.CompressFormat.PNG, 100, it) }
                val uri = androidx.core.content.FileProvider.getUriForFile(this, "$packageName.files", f)
                runOnUiThread {
                    val i = Intent(Intent.ACTION_SEND).setType("image/png").putExtra(Intent.EXTRA_STREAM, uri).addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                    startActivity(Intent.createChooser(i, "Compartilhar imagem"))
                }
            }
        }
    }

    private fun fileName(ext: String) = "BPTouch_" + SimpleDateFormat("yyyyMMdd_HHmmss", Locale.US).format(Date()) + "." + ext

    // ------------------------------------------------------------ teclado físico (Chromebook / teclado BT)

    override fun dispatchKeyEvent(event: KeyEvent): Boolean {
        if (event.action != KeyEvent.ACTION_DOWN) return super.dispatchKeyEvent(event)
        val ctrl = event.isCtrlPressed; val shift = event.isShiftPressed; val alt = event.isAltPressed
        when {
            ctrl && event.keyCode == KeyEvent.KEYCODE_Z -> { if (shift) editor.redo() else editor.undo(); return true }
            ctrl && event.keyCode == KeyEvent.KEYCODE_Y -> { editor.redo(); return true }
            ctrl && event.keyCode == KeyEvent.KEYCODE_O -> { openPhoto(); return true }
            ctrl && event.keyCode == KeyEvent.KEYCODE_N -> { newDocument(); return true }
            ctrl && event.keyCode == KeyEvent.KEYCODE_S -> { save("jpeg"); return true }
            ctrl && event.keyCode == KeyEvent.KEYCODE_A -> { editor.post { N.nSelectAll(it) }; return true }
            ctrl && event.keyCode == KeyEvent.KEYCODE_D -> { editor.post { N.nDeselect(it) }; return true }
            ctrl && shift && event.keyCode == KeyEvent.KEYCODE_I -> { editor.post { N.nInvertSelection(it) }; return true }
            ctrl && event.keyCode == KeyEvent.KEYCODE_J -> { editor.post { N.nLayerViaCopy(it, shift) }; return true }
            ctrl && event.keyCode == KeyEvent.KEYCODE_0 -> { editor.fit(); return true }
            ctrl && event.keyCode == KeyEvent.KEYCODE_1 -> { editor.zoomTo100(); return true }
            alt && (event.keyCode == KeyEvent.KEYCODE_DEL || event.keyCode == KeyEvent.KEYCODE_FORWARD_DEL) -> { val c = editor.fgF(); editor.post { N.nFillSelection(it, c) }; return true }
            event.keyCode == KeyEvent.KEYCODE_DEL || event.keyCode == KeyEvent.KEYCODE_FORWARD_DEL -> { editor.post { N.nClearSelectionPixels(it) }; return true }
            !ctrl && !alt && event.keyCode == KeyEvent.KEYCODE_X -> { editor.swapColors(); return true }
            !ctrl && !alt && event.keyCode == KeyEvent.KEYCODE_D -> { editor.defaultColors(); return true }
            !ctrl && !alt -> {
                val ch = event.unicodeChar.toChar().uppercaseChar()
                val g = TOOL_GROUPS.firstOrNull { it.key == ch }
                if (g != null) {
                    val i = g.tools.indexOf(editor.tool)
                    editor.selectTool(if (shift && i >= 0) g.tools[(i + 1) % g.tools.size] else if (i >= 0) editor.tool else g.tools.first())
                    return true
                }
            }
        }
        return super.dispatchKeyEvent(event)
    }
}

object BuildConfigInfo {
    fun version(a: android.app.Activity): String = try { a.packageManager.getPackageInfo(a.packageName, 0).versionName ?: "" } catch (_: Throwable) { "" }
}
