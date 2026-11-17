// SPDX-License-Identifier: MIT OR Apache-2.0
package com.bptouch.app.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.ExperimentalFoundationApi
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.combinedClickable
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.aspectRatio
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.text.font.FontStyle
import androidx.compose.ui.unit.dp
import com.bptouch.app.editor.AdjustType
import com.bptouch.app.editor.BLEND_MENU
import com.bptouch.app.editor.Editor
import com.bptouch.app.editor.adjustParams
import com.bptouch.app.editor.adjustSliders
import com.bptouch.app.editor.blendName
import com.bptouch.app.engine.NativeEngine as N
import kotlin.math.abs
import kotlin.math.roundToInt

// ============================================================ Camadas

@OptIn(ExperimentalFoundationApi::class)
@Composable
fun LayersPanel(editor: Editor, modifier: Modifier) {
    val d = editor.doc
    val active = d.activeLayer
    var blendOpen by remember { mutableStateOf(false) }
    var renaming by remember { mutableStateOf<Int?>(null) }
    var dragOpacity by remember { mutableStateOf<Float?>(null) }
    Column(modifier) {
        Column(Modifier.fillMaxWidth().padding(8.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
            Box {
                Row(
                    Modifier.fillMaxWidth().height(30.dp).clip(RoundedCornerShape(4.dp)).background(Bp.field).border(1.dp, Bp.line, RoundedCornerShape(4.dp))
                        .clickable(enabled = active != null) { blendOpen = true }.padding(horizontal = 8.dp),
                    verticalAlignment = Alignment.CenterVertically,
                ) {
                    Text(blendName(active?.blend ?: 0), style = Bp.ui, modifier = Modifier.weight(1f))
                    Text("▾", style = Bp.uiSmall)
                }
                DropdownMenu(blendOpen, { blendOpen = false }, containerColor = Bp.panel, modifier = Modifier.height(420.dp)) {
                    BLEND_MENU.forEach { e ->
                        if (e == null) HorizontalDivider(color = Bp.line)
                        else DropdownMenuItem(
                            text = { Text(e.second, style = Bp.ui.copy(color = if (active?.blend == e.first) Bp.accent else Bp.t1)) },
                            onClick = { blendOpen = false; val m = e.first; editor.post { N.nSetLayerBlend(it, m) } },
                            modifier = Modifier.height(34.dp),
                        )
                    }
                }
            }
            Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(6.dp)) {
                Text("Opacidade:", style = Bp.uiSmall)
                val shown = dragOpacity ?: (active?.opacity ?: 1f)
                BpSlider(shown, 0f..1f, Modifier.weight(1f), onChange = { v ->
                    dragOpacity = v; editor.post { N.nSetLayerOpacity(it, v, false) }
                }, onDone = {
                    val v = dragOpacity ?: return@BpSlider
                    editor.post { N.nSetLayerOpacity(it, v, true) }; dragOpacity = null
                })
                Text("${(shown * 100).roundToInt()}%", style = Bp.value, modifier = Modifier.width(40.dp))
            }
            Row(verticalAlignment = Alignment.CenterVertically) {
                CheckOpt("Bloquear transparência", active?.lockAlpha == true) { v -> editor.post { N.nSetLayerLockAlpha(it, v) } }
            }
        }
        HorizontalDivider(color = Bp.line)
        LazyColumn(Modifier.weight(1f).fillMaxWidth()) {
            itemsIndexed(d.layers, key = { _, l -> l.id }) { _, l ->
                val sel = l.id == d.active
                Row(
                    Modifier.fillMaxWidth().height(52.dp).background(if (sel) Bp.selFill else Color.Transparent)
                        .combinedClickable(onClick = { val id = l.id; editor.post { N.nSetActiveLayer(it, id) } }, onLongClick = { renaming = l.id })
                        .padding(end = 8.dp),
                    verticalAlignment = Alignment.CenterVertically,
                ) {
                    IconBtn(if (l.visible) Ic.eye else Ic.eyeOff, if (l.visible) "Ocultar camada" else "Mostrar camada", size = 40.dp, iconSize = 16.dp,
                        tint = if (l.visible) Bp.t1 else Bp.t2) { val id = l.id; val v = !l.visible; editor.post { N.nSetLayerVisible(it, id, v) } }
                    val th = editor.thumbs[l.id]
                    Box(Modifier.size(52.dp, 38.dp).border(1.dp, if (sel) Bp.accent else Bp.line), contentAlignment = Alignment.Center) {
                        if (th != null) Image(th, null, Modifier.fillMaxSize(), contentScale = ContentScale.Fit)
                    }
                    Column(Modifier.weight(1f).padding(start = 8.dp)) {
                        Text(l.name, style = Bp.ui.copy(fontStyle = if (l.name == "Fundo") FontStyle.Italic else FontStyle.Normal), maxLines = 1)
                        if (l.blend != 0 || l.opacity < 0.999f) Text("${blendName(l.blend)} · ${(l.opacity * 100).roundToInt()}%", style = Bp.uiSmall, maxLines = 1)
                    }
                    if (l.lockAlpha) Icon(Ic.lock, "Transparência bloqueada", Modifier.size(13.dp), tint = Bp.t2)
                }
                HorizontalDivider(color = Bp.line)
            }
        }
        Row(
            Modifier.fillMaxWidth().height(40.dp).background(Bp.bar).padding(horizontal = 2.dp),
            verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.SpaceEvenly,
        ) {
            val p = { f: (Long) -> Unit -> editor.post(f) }
            IconBtn(Ic.up, "Trazer para frente", size = 34.dp, iconSize = 16.dp) { p { N.nMoveLayer(it, 1) } }
            IconBtn(Ic.down, "Enviar para trás", size = 34.dp, iconSize = 16.dp) { p { N.nMoveLayer(it, -1) } }
            IconBtn(Ic.merge, "Mesclar para baixo", size = 34.dp, iconSize = 16.dp, enabled = d.layers.size > 1) { p { N.nMergeDown(it) } }
            IconBtn(Ic.copySel, "Camada via cópia", size = 34.dp, iconSize = 16.dp) { p { N.nLayerViaCopy(it, false) } }
            IconBtn(Ic.dup, "Duplicar camada", size = 34.dp, iconSize = 16.dp) { p { N.nDuplicateLayer(it) } }
            IconBtn(Ic.plus, "Nova camada", size = 34.dp, iconSize = 16.dp) { p { N.nAddLayer(it) } }
            IconBtn(Ic.trash, "Excluir camada", size = 34.dp, iconSize = 16.dp, enabled = d.layers.size > 1) { p { N.nDeleteLayer(it) } }
        }
    }
    renaming?.let { id ->
        val current = d.layers.firstOrNull { it.id == id }?.name ?: ""
        RenameDialog(current, onDismiss = { renaming = null }) { name -> editor.post { N.nRenameLayer(it, id, name) }; renaming = null }
    }
}

// ============================================================ Ajustes

@Composable
fun AdjustPanel(editor: Editor, modifier: Modifier) {
    val cur = editor.adjusting
    if (cur == null) {
        Column(modifier.verticalScroll(rememberScrollState()).padding(vertical = 4.dp)) {
            Text(if (editor.doc.hasSelection) "Os ajustes valem só dentro da seleção." else "Os ajustes valem para a camada ativa.",
                style = Bp.uiSmall, modifier = Modifier.padding(10.dp))
            AdjustType.entries.forEach { t ->
                Row(
                    Modifier.fillMaxWidth().height(42.dp).clickable(enabled = editor.doc.open) {
                        if (t == AdjustType.INVERT || t == AdjustType.BW) {
                            val c = t.code; editor.post { N.nAdjustBegin(it, c); N.nAdjustCommit(it) }
                        } else {
                            val c = t.code; editor.post { N.nAdjustBegin(it, c) }
                            editor.adjusting = t
                        }
                    }.padding(horizontal = 12.dp),
                    verticalAlignment = Alignment.CenterVertically,
                ) {
                    Icon(Ic.adjust, null, Modifier.size(16.dp), tint = Bp.t2)
                    Text(t.label, style = Bp.ui, modifier = Modifier.padding(start = 10.dp).weight(1f))
                    if (t == AdjustType.INVERT || t == AdjustType.BW) Text("aplica direto", style = Bp.uiSmall)
                }
            }
        }
        return
    }
    val specs = remember(cur) { adjustSliders(cur) }
    val values = remember(cur) { mutableStateListOf<Float>().apply { addAll(specs.map { it.def }) } }
    val curvePts = remember(cur) { mutableStateListOf(Offset(0f, 0f), Offset(1f, 1f)) }
    fun push() {
        val p = adjustParams(cur, values)
        val lut = if (cur == AdjustType.CURVES) curveLut(curvePts) else null
        editor.post { N.nAdjustSet(it, p, lut) }
    }
    Column(modifier.verticalScroll(rememberScrollState()).padding(12.dp), verticalArrangement = Arrangement.spacedBy(4.dp)) {
        Text(cur.label, style = Bp.panelTitle)
        if (cur == AdjustType.CURVES) {
            CurveEditor(curvePts) { push() }
            Text("Toque para adicionar ponto; arraste para ajustar; arraste para fora para remover.", style = Bp.uiSmall)
        }
        specs.forEachIndexed { i, s ->
            Row(verticalAlignment = Alignment.CenterVertically) {
                Text(s.label, style = Bp.uiSmall, modifier = Modifier.weight(1f))
                Text(fmt(values[i], s.decimals) + s.unit, style = Bp.value)
            }
            BpSlider(values[i], s.min..s.max, onChange = { values[i] = it; push() })
        }
        Row(Modifier.fillMaxWidth().padding(top = 10.dp), horizontalArrangement = Arrangement.spacedBy(8.dp, Alignment.End)) {
            TextBtn("Redefinir") { specs.forEachIndexed { i, s -> values[i] = s.def }; curvePts.clear(); curvePts.addAll(listOf(Offset(0f, 0f), Offset(1f, 1f))); push() }
            TextBtn("Cancelar") { editor.post { N.nAdjustCancel(it) }; editor.adjusting = null }
            TextBtn("OK", primary = true) { editor.post { N.nAdjustCommit(it) }; editor.adjusting = null }
        }
    }
}

/** Curva monótona (Fritsch–Carlson) pelos pontos → tabela de 256 valores. */
fun curveLut(pts: List<Offset>): FloatArray {
    val p = pts.sortedBy { it.x }
    val n = p.size
    val out = FloatArray(256)
    if (n < 2) { for (i in 0 until 256) out[i] = i / 255f; return out }
    val dx = FloatArray(n - 1) { p[it + 1].x - p[it].x }
    val sl = FloatArray(n - 1) { if (dx[it] > 1e-6f) (p[it + 1].y - p[it].y) / dx[it] else 0f }
    val m = FloatArray(n)
    m[0] = sl[0]; m[n - 1] = sl[n - 2]
    for (i in 1 until n - 1) m[i] = if (sl[i - 1] * sl[i] <= 0f) 0f else (sl[i - 1] + sl[i]) / 2f
    for (i in 0 until n - 1) {
        if (abs(sl[i]) < 1e-6f) { m[i] = 0f; m[i + 1] = 0f; continue }
        val a = m[i] / sl[i]; val b = m[i + 1] / sl[i]
        val s = a * a + b * b
        if (s > 9f) { val t = 3f / kotlin.math.sqrt(s); m[i] = t * a * sl[i]; m[i + 1] = t * b * sl[i] }
    }
    for (k in 0 until 256) {
        val x = k / 255f
        val y = when {
            x <= p[0].x -> p[0].y
            x >= p[n - 1].x -> p[n - 1].y
            else -> {
                var i = 0
                while (i < n - 2 && x > p[i + 1].x) i++
                val h = dx[i]; val t = (x - p[i].x) / h
                val h00 = (1 + 2 * t) * (1 - t) * (1 - t); val h10 = t * (1 - t) * (1 - t)
                val h01 = t * t * (3 - 2 * t); val h11 = t * t * (t - 1)
                h00 * p[i].y + h10 * h * m[i] + h01 * p[i + 1].y + h11 * h * m[i + 1]
            }
        }
        out[k] = y.coerceIn(0f, 1f)
    }
    return out
}

@Composable
private fun CurveEditor(pts: MutableList<Offset>, onChange: () -> Unit) {
    var dragIdx by remember { mutableStateOf(-1) }
    Canvas(
        Modifier.fillMaxWidth().aspectRatio(1f).background(Bp.field).border(1.dp, Bp.line)
            .pointerInput(Unit) {
                detectTapGestures { o ->
                    val x = (o.x / size.width).coerceIn(0f, 1f); val y = 1f - (o.y / size.height).coerceIn(0f, 1f)
                    if (pts.none { abs(it.x - x) < 0.03f }) { pts.add(Offset(x, y)); pts.sortBy { it.x }; onChange() }
                }
            }
            .pointerInput(Unit) {
                detectDragGestures(
                    onDragStart = { o ->
                        val x = o.x / size.width; val y = 1f - o.y / size.height
                        dragIdx = pts.indices.minByOrNull { (pts[it].x - x) * (pts[it].x - x) + (pts[it].y - y) * (pts[it].y - y) } ?: -1
                    },
                    onDragEnd = { dragIdx = -1 },
                ) { change, _ ->
                    if (dragIdx < 0) return@detectDragGestures
                    val o = change.position
                    val x = o.x / size.width; val y = 1f - o.y / size.height
                    val endpoint = dragIdx == 0 || dragIdx == pts.lastIndex
                    if (!endpoint && (y < -0.15f || y > 1.15f)) { pts.removeAt(dragIdx); dragIdx = -1; onChange(); return@detectDragGestures }
                    val lo = if (dragIdx > 0) pts[dragIdx - 1].x + 0.01f else 0f
                    val hi = if (dragIdx < pts.lastIndex) pts[dragIdx + 1].x - 0.01f else 1f
                    pts[dragIdx] = Offset(if (endpoint) (if (dragIdx == 0) x.coerceIn(0f, hi) else x.coerceIn(lo, 1f)) else x.coerceIn(lo, hi), y.coerceIn(0f, 1f))
                    onChange()
                }
            },
    ) {
        for (k in 1..3) {
            val f = k / 4f
            drawLine(Bp.line, Offset(size.width * f, 0f), Offset(size.width * f, size.height))
            drawLine(Bp.line, Offset(0f, size.height * f), Offset(size.width, size.height * f))
        }
        drawLine(Bp.hover, Offset(0f, size.height), Offset(size.width, 0f))
        val lut = curveLut(pts)
        val path = Path()
        for (i in 0 until 256) {
            val x = i / 255f * size.width; val y = (1f - lut[i]) * size.height
            if (i == 0) path.moveTo(x, y) else path.lineTo(x, y)
        }
        drawPath(path, Bp.t1, style = Stroke(2f))
        pts.forEach { drawCircle(Bp.t1, 6f, Offset(it.x * size.width, (1 - it.y) * size.height), style = Stroke(2f)) }
    }
}

// ============================================================ Histórico

@Composable
fun HistoryPanel(editor: Editor, modifier: Modifier) {
    val h = editor.history
    LazyColumn(modifier.fillMaxWidth()) {
        itemsIndexed(h.items) { i, name ->
            val applied = i < h.pos
            val current = i == h.pos - 1
            Row(
                Modifier.fillMaxWidth().height(38.dp).background(if (current) Bp.selFill else Color.Transparent)
                    .clickable { editor.post { N.nJumpHistory(it, i) } }.padding(horizontal = 12.dp),
                verticalAlignment = Alignment.CenterVertically,
            ) {
                Icon(Ic.history, null, Modifier.size(14.dp), tint = Bp.t2)
                Spacer(Modifier.width(10.dp))
                Text(name, style = Bp.ui.copy(color = if (applied) Bp.t1 else Bp.t2.copy(alpha = 0.6f)))
            }
            HorizontalDivider(color = Bp.line)
        }
    }
    LaunchedEffect(h.items.size) {}
}
