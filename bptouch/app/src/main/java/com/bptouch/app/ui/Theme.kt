// SPDX-License-Identifier: MIT OR Apache-2.0
package com.bptouch.app.ui

import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.StrokeJoin
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.graphics.vector.addPathNodes
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

/** Tokens do tema Grafite (design/telar-tokens.json, nível 2 de cor de interface). */
object Bp {
    val pasteboard = Color(0xFF1E1E1E)
    val bar = Color(0xFF2A2A2A)
    val panel = Color(0xFF323232)
    val field = Color(0xFF3E3E3E)
    val hover = Color(0xFF474747)
    val line = Color(0xFF474747)
    val t1 = Color(0xFFE6E6E6)
    val t2 = Color(0xFFABABAB)
    val accent = Color(0xFF3B8EEA)
    val onAccent = Color.White
    val selFill = Color(0x473B8EEA)
    val ruby = Color(0xFFE5484D)

    val ui = TextStyle(fontSize = 12.sp, color = t1)
    val uiSmall = TextStyle(fontSize = 11.sp, color = t2)
    val value = TextStyle(fontSize = 11.sp, fontFamily = FontFamily.Monospace, color = t1)
    val panelTitle = TextStyle(fontSize = 12.sp, fontWeight = FontWeight.SemiBold, color = t1)
}

@Composable
fun BpTheme(content: @Composable () -> Unit) {
    MaterialTheme(
        colorScheme = darkColorScheme(
            primary = Bp.accent, onPrimary = Bp.onAccent, surface = Bp.panel, onSurface = Bp.t1,
            background = Bp.pasteboard, onBackground = Bp.t1, surfaceVariant = Bp.field, onSurfaceVariant = Bp.t2,
            outline = Bp.line, secondary = Bp.accent,
        ),
        content = content,
    )
}

/** Ícones de traço único, desenho próprio, grade 24×24. */
object Ic {
    private val cache = HashMap<String, ImageVector>()
    fun of(name: String, d: String, stroke: Float = 1.6f): ImageVector = cache.getOrPut(name) {
        ImageVector.Builder(name, 24.dp, 24.dp, 24f, 24f).addPath(
            pathData = addPathNodes(d), fill = null, stroke = SolidColor(Color.White), strokeLineWidth = stroke,
            strokeLineCap = StrokeCap.Round, strokeLineJoin = StrokeJoin.Round,
        ).build()
    }
    private fun dashedRect() = "M4 5h3M10 5h4M17 5h3v3M20 11v2M20 16v3h-3M14 19h-4M7 19H4v-3M4 13v-2M4 8V5"
    private fun dashedEllipse() = "M12 6a8 6 0 0 1 5.7 1.8M19.8 10.5a8 6 0 0 1 0 3M17.7 16.2a8 6 0 0 1 -5.7 1.8M8.6 17.6a8 6 0 0 1 -4.4 -4.3M4.2 10.7a8 6 0 0 1 4.4 -4.3"

    val menu get() = of("menu", "M4 7h16M4 12h16M4 17h16")
    val undo get() = of("undo", "M9 7H4v5M4 12a8 8 0 0 1 14-3")
    val redo get() = of("redo", "M15 7h5v5M20 12a8 8 0 0 0-14-3")
    val layers get() = of("layers", "M12 3l9 5-9 5-9-5zM3 13l9 5 9-5")
    val eye get() = of("eye", "M2 12s4-7 10-7 10 7 10 7-4 7-10 7S2 12 2 12zM12 9a3 3 0 1 0 0 6 3 3 0 0 0 0-6z")
    val eyeOff get() = of("eyeoff", "M3 3l18 18M10.6 5.1A10 10 0 0 1 12 5c6 0 10 7 10 7a17 17 0 0 1-3.2 3.9M6.6 6.6C3.8 8.3 2 12 2 12s4 7 10 7a9.6 9.6 0 0 0 5.4-1.6")
    val lock get() = of("lock", "M6 11h12v9H6zM9 11V8a3 3 0 0 1 6 0v3")
    val plus get() = of("plus", "M5 5h14v14H5zM12 9v6M9 12h6")
    val trash get() = of("trash", "M5 7h14M10 7V4h4v3M7 7l1 13h8l1-13")
    val dup get() = of("dup", "M8 8h12v12H8zM4 16V4h12")
    val merge get() = of("merge", "M12 4v10M8 10l4 4 4-4M5 19h14")
    val up get() = of("up", "M12 19V5M6 11l6-6 6 6")
    val down get() = of("down", "M12 5v14M6 13l6 6 6-6")
    val image get() = of("image", "M4 5h16v14H4zM4 16l5-5 4 4 3-3 4 4M15 9.5a1.5 1.5 0 1 0 0-.01")
    val pin get() = of("pin", "M9 3h6l-1 6 4 4H6l4-4zM12 13v8")
    val close get() = of("close", "M6 6l12 12M18 6L6 18")
    val check get() = of("check", "M5 12l5 5 9-10")
    val swap get() = of("swap", "M4 9h14l-3-3M20 15H6l3 3")
    val fit get() = of("fit", "M4 9V4h5M15 4h5v5M20 15v5h-5M9 20H4v-5")
    val copySel get() = of("copysel", "M4 8V4h4M12 4h0M16 4h4v4M20 12h0M8 20H4v-4M10 10h10v10H10z")
    val adjust get() = of("adjust", "M12 4a8 8 0 1 0 0 16zM12 4a8 8 0 0 1 0 16")
    val history get() = of("history", "M4 12a8 8 0 1 0 2.3-5.6M4 4v4h4M12 8v4l3 2")
    val selNew get() = of("selnew", "M5 5h14v14H5z")
    val selAdd get() = of("seladd", "M3 3h11v11H3zM10 10h11v11H10z")
    val selSub get() = of("selsub", "M3 3h11v11H3zM14 10h7v11H10v-7")
    val selInt get() = of("selint", "M3 3h11v11H3zM10 10h11v11H10zM10 10h4v4h-4z")

    fun tool(t: com.bptouch.app.editor.ToolId): ImageVector = when (t) {
        com.bptouch.app.editor.ToolId.MOVE -> of("t_move", "M6 3l11 6-5 1.5-2 5.5zM15 15h6M18 12v6")
        com.bptouch.app.editor.ToolId.MARQUEE_RECT -> of("t_mrect", dashedRect())
        com.bptouch.app.editor.ToolId.MARQUEE_ELLIPSE -> of("t_mell", dashedEllipse())
        com.bptouch.app.editor.ToolId.LASSO -> of("t_lasso", "M12 4c5 0 8 2.5 8 5.5S16.5 15 12 15 4 12.5 4 9.5 7 4 12 4zM7 13.5c-1.5 2-1 4.5 1.5 5.5")
        com.bptouch.app.editor.ToolId.LASSO_POLY -> of("t_lpoly", "M5 17L4 7l8-3 8 6-6 6zM9 20l-4-3")
        com.bptouch.app.editor.ToolId.OBJECT_SELECT -> of("t_obj", "M4 8V4h4M16 4h4v4M20 16v4h-4M8 20H4v-4M8.5 16c0-2.2 1.6-3.5 3.5-3.5s3.5 1.3 3.5 3.5M12 11a2 2 0 1 0 0-4 2 2 0 0 0 0 4z")
        com.bptouch.app.editor.ToolId.QUICK_SELECT -> of("t_quick", "M10 4.5a5.5 5.5 0 0 1 0 11M7.2 14.7a5.5 5.5 0 0 1 -1.7 -7.4M14 14l6 6")
        com.bptouch.app.editor.ToolId.MAGIC_WAND -> of("t_wand", "M4 20L14 10M16 3v3M16 11v3M12.5 7.5h-3M22.5 7.5h-3M13 4.5l1.5 1.5M19 10.5l-1.5-1.5")
        com.bptouch.app.editor.ToolId.CROP -> of("t_crop", "M6 2v16h16M2 6h16v16")
        com.bptouch.app.editor.ToolId.EYEDROPPER -> of("t_drop", "M14 4l6 6-2 2-6-6zM13 9l-7 7-1 3 3-1 7-7")
        com.bptouch.app.editor.ToolId.BRUSH -> of("t_brush", "M18 3l3 3-9 9-3-3zM9 12c-3 0-5 2-5 5 0 1-1 2-2 2 2 2 7 2 8-1 .6-1.7.4-3-1-6z")
        com.bptouch.app.editor.ToolId.CLONE -> of("t_clone", "M9 4h6v5l2 3H7l2-3zM5 15h14v3H5z")
        com.bptouch.app.editor.ToolId.ERASER -> of("t_eraser", "M8 20h12M4 15l9-9 6 6-7 7H8z")
        com.bptouch.app.editor.ToolId.GRADIENT -> of("t_grad", "M4 4h16v16H4zM8 4v16M12 4v16")
        com.bptouch.app.editor.ToolId.BUCKET -> of("t_bucket", "M5 11l7-7 7 7-7 7zM5 11h14M20 15c0 1.5-.8 2.5-1.5 2.5S17 16.5 17 15s1.5-3 1.5-3 1.5 1.5 1.5 3z")
        com.bptouch.app.editor.ToolId.HAND -> of("t_hand", "M8 13V6a1.5 1.5 0 0 1 3 0v5V4.5a1.5 1.5 0 0 1 3 0V11V6a1.5 1.5 0 0 1 3 0v8c0 4-2.5 6-6 6-2.5 0-4-1.2-5.5-3.5L4 13.5a1.5 1.5 0 0 1 2.5-1.6z")
    }
}
