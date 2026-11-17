// SPDX-License-Identifier: MIT OR Apache-2.0
package com.bptouch.app.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.Dialog
import com.bptouch.app.editor.Editor
import com.bptouch.app.engine.NativeEngine as N

@Composable
private fun DialogFrame(title: String, onDismiss: () -> Unit, content: @Composable () -> Unit) {
    Dialog(onDismissRequest = onDismiss) {
        Column(
            Modifier.width(360.dp).background(Bp.panel, RoundedCornerShape(10.dp)).border(1.dp, Bp.line, RoundedCornerShape(10.dp))
                .verticalScroll(rememberScrollState()).padding(16.dp),
            verticalArrangement = Arrangement.spacedBy(10.dp),
        ) {
            Text(title, style = Bp.panelTitle)
            content()
        }
    }
}

@Composable
private fun Field(value: String, onChange: (String) -> Unit, modifier: Modifier = Modifier, numeric: Boolean = false) {
    BasicTextField(
        value, onChange, singleLine = true, textStyle = Bp.ui, cursorBrush = SolidColor(Bp.accent),
        keyboardOptions = if (numeric) KeyboardOptions(keyboardType = KeyboardType.Number) else KeyboardOptions.Default,
        modifier = modifier.background(Bp.field, RoundedCornerShape(4.dp)).border(1.dp, Bp.line, RoundedCornerShape(4.dp)).padding(horizontal = 8.dp, vertical = 8.dp),
    )
}

// ------------------------------------------------------------ seletor de cor (HSV)

private fun hsvToColor(h: Float, s: Float, v: Float): Int = android.graphics.Color.HSVToColor(floatArrayOf(h, s, v))

@Composable
fun ColorPickerDialog(initial: Int, title: String, onDismiss: () -> Unit, onPick: (Int) -> Unit) {
    val hsv0 = remember { FloatArray(3).also { android.graphics.Color.colorToHSV(initial, it) } }
    var h by remember { mutableFloatStateOf(hsv0[0]) }
    var s by remember { mutableFloatStateOf(hsv0[1]) }
    var v by remember { mutableFloatStateOf(hsv0[2]) }
    val cur = hsvToColor(h, s, v)
    var hex by remember(cur) { mutableStateOf(String.format("%06X", cur and 0xFFFFFF)) }
    DialogFrame(title, onDismiss) {
        Box(
            Modifier.fillMaxWidth().height(200.dp)
                .pointerInput(Unit) { detectTapGestures { o -> s = (o.x / size.width).coerceIn(0f, 1f); v = 1f - (o.y / size.height).coerceIn(0f, 1f) } }
                .pointerInput(Unit) { detectDragGestures { c, _ -> s = (c.position.x / size.width).coerceIn(0f, 1f); v = 1f - (c.position.y / size.height).coerceIn(0f, 1f) } },
        ) {
            Canvas(Modifier.fillMaxWidth().height(200.dp)) {
                drawRect(Color(hsvToColor(h, 1f, 1f)))
                drawRect(Brush.horizontalGradient(listOf(Color.White, Color.Transparent)))
                drawRect(Brush.verticalGradient(listOf(Color.Transparent, Color.Black)))
                val p = Offset(s * size.width, (1 - v) * size.height)
                drawCircle(Color.Black, 9f, p, style = Stroke(3f)); drawCircle(Color.White, 9f, p, style = Stroke(1.5f))
            }
        }
        Box(
            Modifier.fillMaxWidth().height(26.dp)
                .pointerInput(Unit) { detectTapGestures { o -> h = (o.x / size.width).coerceIn(0f, 1f) * 360f } }
                .pointerInput(Unit) { detectDragGestures { c, _ -> h = (c.position.x / size.width).coerceIn(0f, 1f) * 360f } },
        ) {
            Canvas(Modifier.fillMaxWidth().height(26.dp)) {
                drawRect(Brush.horizontalGradient((0..6).map { Color(hsvToColor(it * 60f, 1f, 1f)) }))
                val x = h / 360f * size.width
                drawRect(Color.White, Offset(x - 2f, 0f), androidx.compose.ui.geometry.Size(4f, size.height))
            }
        }
        Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(10.dp)) {
            Box(Modifier.size(40.dp, 30.dp).background(Color(initial)).border(1.dp, Bp.line))
            Box(Modifier.size(40.dp, 30.dp).background(Color(cur)).border(1.dp, Bp.line))
            Text("#", style = Bp.ui)
            Field(hex, { t ->
                hex = t.uppercase().filter { it in "0123456789ABCDEF" }.take(6)
                if (hex.length == 6) { val c = (0xFF000000 or hex.toLong(16)).toInt(); val o = FloatArray(3); android.graphics.Color.colorToHSV(c, o); h = o[0]; s = o[1]; v = o[2] }
            }, Modifier.width(110.dp))
        }
        Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(6.dp)) {
            listOf(0xFF000000, 0xFFFFFFFF, 0xFF808080, 0xFFE5484D, 0xFFF5B53F, 0xFF3DD68C, 0xFF3B8EEA, 0xFF8E4EC6).forEach { c ->
                Box(Modifier.size(28.dp).background(Color(c.toInt())).border(1.dp, Bp.line).clickable {
                    val o = FloatArray(3); android.graphics.Color.colorToHSV(c.toInt(), o); h = o[0]; s = o[1]; v = o[2]
                })
            }
        }
        Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp, Alignment.End)) {
            TextBtn("Cancelar", onClick = onDismiss)
            TextBtn("OK", primary = true) { onPick(cur or 0xFF000000.toInt()) }
        }
    }
}

// ------------------------------------------------------------ refinar borda

@Composable
fun RefineDialog(editor: Editor, onDismiss: () -> Unit) {
    var radius by remember { mutableFloatStateOf(8f) }
    var smooth by remember { mutableFloatStateOf(0f) }
    var contrast by remember { mutableFloatStateOf(30f) }
    var shift by remember { mutableFloatStateOf(0f) }
    var feather by remember { mutableFloatStateOf(0f) }
    DialogFrame("Refinar borda", onDismiss) {
        Text("Usa a própria foto como guia: a borda da seleção “gruda” nos contornos reais, inclusive cabelo e pelos.", style = Bp.uiSmall)
        @Composable fun s(label: String, v: Float, r: ClosedFloatingPointRange<Float>, unit: String, f: (Float) -> Unit) {
            Row { Text(label, style = Bp.uiSmall, modifier = Modifier.weight(1f)); Text(fmt(v, if (r.endInclusive <= 5f) 1 else 0) + unit, style = Bp.value) }
            BpSlider(v, r, onChange = f)
        }
        s("Raio", radius, 1f..60f, " px") { radius = it }
        s("Suavizar", smooth, 0f..100f, "") { smooth = it }
        s("Contraste", contrast, 0f..100f, "%") { contrast = it }
        s("Deslocar borda", shift, -100f..100f, "%") { shift = it }
        s("Difusão", feather, 0f..50f, " px") { feather = it }
        Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp, Alignment.End)) {
            TextBtn("Cancelar", onClick = onDismiss)
            TextBtn("Aplicar", primary = true) {
                val r = radius; val sm = smooth / 100f; val c = contrast / 100f; val sh = shift / 100f; val f = feather
                editor.post { N.nRefineSelection(it, r, sm, c, sh, f) }
                onDismiss()
            }
        }
    }
}

// ------------------------------------------------------------ renomear camada

@Composable
fun RenameDialog(current: String, onDismiss: () -> Unit, onOk: (String) -> Unit) {
    var name by remember { mutableStateOf(current) }
    DialogFrame("Nome da camada", onDismiss) {
        Field(name, { name = it.take(60) }, Modifier.fillMaxWidth())
        Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp, Alignment.End)) {
            TextBtn("Cancelar", onClick = onDismiss)
            TextBtn("OK", primary = true, enabled = name.isNotBlank()) { onOk(name.trim()) }
        }
    }
}

// ------------------------------------------------------------ novo documento

@Composable
fun NewDocumentDialog(screenW: Int, screenH: Int, maxTex: Int, onDismiss: () -> Unit, onCreate: (Int, Int, Boolean) -> Unit) {
    var w by remember { mutableStateOf("1920") }
    var h by remember { mutableStateOf("1080") }
    var white by remember { mutableStateOf(true) }
    DialogFrame("Novo documento", onDismiss) {
        val presets = listOf("Tela do aparelho" to (screenW to screenH), "Quadrado 1080" to (1080 to 1080), "Full HD 1920×1080" to (1920 to 1080),
            "Story 1080×1920" to (1080 to 1920), "4K 3840×2160" to (3840 to 2160), "A4 300 ppi" to (2480 to 3508))
        presets.forEach { (label, sz) ->
            Text(label + "  ·  ${sz.first} × ${sz.second}", style = Bp.ui,
                modifier = Modifier.fillMaxWidth().clickable { w = sz.first.toString(); h = sz.second.toString() }.padding(vertical = 6.dp))
        }
        Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Text("L", style = Bp.uiSmall); Field(w, { w = it.filter(Char::isDigit).take(5) }, Modifier.width(90.dp), numeric = true)
            Text("A", style = Bp.uiSmall); Field(h, { h = it.filter(Char::isDigit).take(5) }, Modifier.width(90.dp), numeric = true)
            Text("px", style = Bp.uiSmall)
        }
        CheckOpt("Fundo branco (desmarcado: transparente)", white) { white = it }
        val wi = w.toIntOrNull() ?: 0; val hi = h.toIntOrNull() ?: 0
        val ok = wi in 1..maxTex && hi in 1..maxTex
        if (!ok) Text("Tamanho máximo neste aparelho: $maxTex px por lado.", style = Bp.uiSmall.copy(color = Bp.ruby))
        Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp, Alignment.End)) {
            TextBtn("Cancelar", onClick = onDismiss)
            TextBtn("Criar", primary = true, enabled = ok) { onCreate(wi, hi, white) }
        }
    }
}

// ------------------------------------------------------------ sobre

@Composable
fun AboutDialog(glInfo: String, version: String, maxSide: Int, onMaxSide: (Int) -> Unit, onDismiss: () -> Unit) {
    DialogFrame("BP Touch $version", onDismiss) {
        Text("Editor de fotos para toque, 64 bits (arm64), motor próprio em C++ com OpenGL ES 3.0.", style = Bp.ui)
        Text("GPU: $glInfo", style = Bp.uiSmall)
        Text("Resolução máxima ao abrir fotos (lado maior):", style = Bp.ui)
        Row(horizontalArrangement = Arrangement.spacedBy(6.dp)) {
            listOf(2048, 4096, 6144, 8192).forEach { s -> TextBtn("$s", primary = s == maxSide) { onMaxSide(s) } }
        }
        Text("Fotos maiores que isso são reduzidas ao abrir. Valores altos usam mais memória.", style = Bp.uiSmall)
        Text("Código aberto: MIT OR Apache-2.0. Ícones e interface próprios.", style = Bp.uiSmall)
        Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.End) { TextBtn("Fechar", primary = true, onClick = onDismiss) }
    }
}
