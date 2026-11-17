// SPDX-License-Identifier: MIT OR Apache-2.0
package com.bptouch.app.ui

import android.annotation.SuppressLint
import android.opengl.GLSurfaceView
import android.view.MotionEvent
import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.animation.core.tween
import androidx.compose.foundation.ExperimentalFoundationApi
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.clickable
import androidx.compose.foundation.combinedClickable
import androidx.compose.foundation.gestures.Orientation
import androidx.compose.foundation.gestures.draggable
import androidx.compose.foundation.gestures.rememberDraggableState
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.PathEffect
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import com.bptouch.app.editor.Editor
import com.bptouch.app.editor.TOOL_GROUPS
import com.bptouch.app.editor.ToolId
import kotlinx.coroutines.delay
import kotlin.math.abs
import kotlin.math.min
import kotlin.math.roundToInt
import android.content.res.Configuration
import androidx.compose.material3.Icon

/** Ações que dependem da Activity (seletores de arquivo, salvar). */
interface EditorActions {
    fun openPhoto()
    fun addPhotoLayer()
    fun save(format: String)
    fun share()
    fun newDocument()
    fun about()
}

@Composable
fun EditorScreen(editor: Editor, actions: EditorActions) {
    val landscape = LocalConfiguration.current.orientation == Configuration.ORIENTATION_LANDSCAPE
    var panelOpen by remember { mutableStateOf(landscape) }
    var pinned by remember { mutableStateOf(false) }
    var colorDialog by remember { mutableStateOf<Boolean?>(null) }   // true = frente, false = fundo
    var refineDialog by remember { mutableStateOf(false) }

    Column(Modifier.fillMaxSize().background(Bp.pasteboard)) {
        OptionsBar(editor, actions, onPanels = { panelOpen = !panelOpen }, panelOpen = panelOpen, onRefine = { refineDialog = true })
        Row(Modifier.weight(1f).fillMaxWidth()) {
            ToolBar(editor, landscape, onColor = { colorDialog = it })
            BoxWithConstraints(Modifier.weight(1f).fillMaxHeight()) {
                val panelW = if (landscape) 290.dp else maxWidth * 0.86f
                Row(Modifier.fillMaxSize()) {
                    Box(Modifier.weight(1f).fillMaxHeight()) {
                        CanvasArea(editor, actions)
                    }
                    if (pinned && panelOpen) Panels(editor, Modifier.width(panelW).fillMaxHeight(), pinned = true,
                        onPin = { pinned = false }, onClose = { panelOpen = false }, onRefine = { refineDialog = true })
                }
                if (!pinned) Drawer(editor, panelOpen, panelW, onOpen = { panelOpen = it }, onPin = { pinned = true; panelOpen = true },
                    onRefine = { refineDialog = true })
            }
        }
    }
    colorDialog?.let { isFg ->
        ColorPickerDialog(initial = if (isFg) editor.fg else editor.bg, title = if (isFg) "Cor de primeiro plano" else "Cor de fundo",
            onDismiss = { colorDialog = null }) { c -> if (isFg) editor.fg = c else editor.bg = c; colorDialog = null }
    }
    if (refineDialog) RefineDialog(editor) { refineDialog = false }
    editor.toast?.let { msg ->
        LaunchedEffect(msg) { delay(3500); editor.toast = null }
    }
    editor.glError?.let { ErrorBanner(it) }
}

@Composable
private fun ErrorBanner(msg: String) {
    Box(Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
        Text(msg, style = Bp.ui, modifier = Modifier.background(Bp.ruby, RoundedCornerShape(6.dp)).padding(16.dp))
    }
}

// ============================================================ barra de opções

@Composable
private fun OptionsBar(editor: Editor, actions: EditorActions, onPanels: () -> Unit, panelOpen: Boolean, onRefine: () -> Unit) {
    var menu by remember { mutableStateOf(false) }
    val d = editor.doc
    Row(
        Modifier.fillMaxWidth().height(44.dp).background(Bp.bar).padding(horizontal = 6.dp),
        verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(4.dp),
    ) {
        Box {
            IconBtn(Ic.menu, "Menu") { menu = true }
            MainMenu(editor, actions, menu, onDismiss = { menu = false }, onRefine = onRefine)
        }
        VSep()
        Row(
            Modifier.weight(1f).horizontalScroll(rememberScrollState()),
            verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp),
        ) {
            Icon(Ic.tool(editor.tool), null, Modifier.size(20.dp), tint = Bp.t2)
            ToolOptionsRow(editor, onRefine)
        }
        if (d.busy) CircularProgressIndicator(Modifier.size(18.dp), color = Bp.accent, strokeWidth = 2.dp)
        IconBtn(Ic.undo, "Desfazer", enabled = d.canUndo) { editor.undo() }
        IconBtn(Ic.redo, "Refazer", enabled = d.canRedo) { editor.redo() }
        IconBtn(Ic.layers, "Painéis", selected = panelOpen, onClick = onPanels)
        TextBtn("Salvar", primary = true, enabled = d.open) { actions.save("jpeg") }
    }
}

@Composable
private fun MainMenu(editor: Editor, actions: EditorActions, open: Boolean, onDismiss: () -> Unit, onRefine: () -> Unit) {
    val N = com.bptouch.app.engine.NativeEngine
    val d = editor.doc
    DropdownMenu(open, onDismiss, containerColor = Bp.panel) {
        @Composable fun item(label: String, enabled: Boolean = true, hint: String = "", f: () -> Unit) =
            DropdownMenuItem(text = {
                Row(Modifier.width(250.dp)) {
                    Text(label, style = Bp.ui.copy(color = if (enabled) Bp.t1 else Bp.t2), modifier = Modifier.weight(1f))
                    if (hint.isNotEmpty()) Text(hint, style = Bp.uiSmall)
                }
            }, enabled = enabled, onClick = { onDismiss(); f() }, modifier = Modifier.height(40.dp))
        @Composable fun header(t: String) = Text(t.uppercase(), style = Bp.uiSmall, modifier = Modifier.padding(start = 12.dp, top = 8.dp, bottom = 2.dp))

        header("Arquivo")
        item("Abrir foto…", hint = "Ctrl+O") { actions.openPhoto() }
        item("Novo documento…", hint = "Ctrl+N") { actions.newDocument() }
        item("Inserir foto como camada…", d.open) { actions.addPhotoLayer() }
        item("Salvar JPEG na galeria", d.open, "Ctrl+S") { actions.save("jpeg") }
        item("Salvar PNG (com transparência)", d.open) { actions.save("png") }
        item("Salvar WebP sem perdas", d.open) { actions.save("webp") }
        item("Compartilhar…", d.open) { actions.share() }
        HorizontalDivider(color = Bp.line)
        header("Editar")
        item("Preencher com a cor de frente", d.open, "Alt+Del") { val c = editor.fgF(); editor.post { N.nFillSelection(it, c) } }
        item("Limpar", d.open, "Del") { editor.post { N.nClearSelectionPixels(it) } }
        item("Camada via cópia", d.open, "Ctrl+J") { editor.post { N.nLayerViaCopy(it, false) } }
        item("Camada via recorte", d.hasSelection, "Shift+Ctrl+J") { editor.post { N.nLayerViaCopy(it, true) } }
        HorizontalDivider(color = Bp.line)
        header("Selecionar")
        item("Tudo", d.open, "Ctrl+A") { editor.post { N.nSelectAll(it) } }
        item("Desmarcar", d.hasSelection, "Ctrl+D") { editor.post { N.nDeselect(it) } }
        item("Inverter", d.open, "Shift+Ctrl+I") { editor.post { N.nInvertSelection(it) } }
        item("Refinar borda…", d.hasSelection, "Alt+Ctrl+R") { onRefine() }
        HorizontalDivider(color = Bp.line)
        header("Imagem")
        item("Cortar para a seleção", d.hasSelection) { editor.post { N.nCropToSelection(it) } }
        item("Girar 90° horário", d.open) { editor.post { N.nRotateCanvas(it, 1) } }
        item("Girar 90° anti-horário", d.open) { editor.post { N.nRotateCanvas(it, 3) } }
        item("Girar 180°", d.open) { editor.post { N.nRotateCanvas(it, 2) } }
        item("Espelhar na horizontal", d.open) { editor.post { N.nFlipCanvas(it, true) } }
        item("Espelhar na vertical", d.open) { editor.post { N.nFlipCanvas(it, false) } }
        item("Achatar imagem", d.layers.size > 1) { editor.post { N.nFlatten(it) } }
        HorizontalDivider(color = Bp.line)
        item("Ajustar à tela", d.open, "Ctrl+0") { editor.fit() }
        item("100% (pixels reais)", d.open, "Ctrl+1") { editor.zoomTo100() }
        item("Sobre o BP Touch") { actions.about() }
    }
}

@Composable
private fun SelOps(editor: Editor) {
    Row(horizontalArrangement = Arrangement.spacedBy(1.dp)) {
        listOf(Ic.selNew to "Nova seleção", Ic.selAdd to "Adicionar à seleção", Ic.selSub to "Subtrair da seleção", Ic.selInt to "Interseção com a seleção")
            .forEachIndexed { i, (ic, label) -> IconBtn(ic, label, selected = editor.opts.selOp == i, size = 30.dp, iconSize = 16.dp) { editor.opts.selOp = i } }
    }
}

@Composable
private fun ToolOptionsRow(editor: Editor, onRefine: () -> Unit) {
    val o = editor.opts
    val N = com.bptouch.app.engine.NativeEngine
    when (editor.tool) {
        ToolId.MOVE -> Text("Arraste para mover a camada (ou o que estiver selecionado).", style = Bp.uiSmall)
        ToolId.MARQUEE_RECT, ToolId.MARQUEE_ELLIPSE, ToolId.LASSO, ToolId.LASSO_POLY -> {
            SelOps(editor); VSep()
            ValueField("Difusão:", o.feather, 0f..250f, " px", onChange = { o.feather = it })
            CheckOpt("Suavização de serrilhado", o.antialias) { o.antialias = it }
            if (editor.tool == ToolId.LASSO_POLY) TextBtn("Fechar laço") { editor.closePolyLasso() }
            if (editor.doc.hasSelection) TextBtn("Refinar borda…", onClick = onRefine)
        }
        ToolId.QUICK_SELECT -> {
            Row { IconBtn(Ic.selAdd, "Adicionar", selected = !o.quickSubtract, size = 30.dp, iconSize = 16.dp) { o.quickSubtract = false }
                  IconBtn(Ic.selSub, "Subtrair", selected = o.quickSubtract, size = 30.dp, iconSize = 16.dp) { o.quickSubtract = true } }
            VSep()
            ValueField("Tamanho:", o.quickSize, 4f..400f, " px", onChange = { o.quickSize = it })
            CheckOpt("Todas as camadas", o.sampleAll) { o.sampleAll = it }
            if (editor.doc.hasSelection) TextBtn("Refinar borda…", onClick = onRefine)
        }
        ToolId.OBJECT_SELECT -> {
            SelOps(editor); VSep()
            Text("Arraste um retângulo em volta do objeto.", style = Bp.uiSmall)
            CheckOpt("Todas as camadas", o.sampleAll) { o.sampleAll = it }
            if (editor.doc.hasSelection) TextBtn("Refinar borda…", onClick = onRefine)
        }
        ToolId.MAGIC_WAND -> {
            SelOps(editor); VSep()
            ValueField("Tolerância:", o.wandTolerance.toFloat(), 0f..255f, onChange = { o.wandTolerance = it.roundToInt() })
            CheckOpt("Suavização de serrilhado", o.antialias) { o.antialias = it }
            CheckOpt("Contíguo", o.contiguous) { o.contiguous = it }
            CheckOpt("Todas as camadas", o.sampleAll) { o.sampleAll = it }
        }
        ToolId.CROP -> {
            val has = editor.overlay.cropA != null && editor.overlay.cropB != null
            Text(if (has) "Ajuste o retângulo e toque em Aplicar." else "Arraste para marcar a área que fica.", style = Bp.uiSmall)
            TextBtn("Aplicar", primary = true, enabled = has) { editor.applyCrop() }
            TextBtn("Cancelar", enabled = has) { editor.overlay.cropA = null; editor.overlay.cropB = null }
            VSep()
            TextBtn("Girar 90°") { editor.post { N.nRotateCanvas(it, 1) } }
        }
        ToolId.EYEDROPPER -> Text("Toque na imagem para pegar a cor de primeiro plano (amostra de todas as camadas).", style = Bp.uiSmall)
        ToolId.BRUSH -> {
            ValueField("", o.brushSize, 1f..1000f, " px", onChange = { o.brushSize = it })
            ValueField("Dureza:", o.brushHardness * 100, 0f..100f, "%", onChange = { o.brushHardness = it / 100 })
            ValueField("Opacidade:", o.brushOpacity * 100, 1f..100f, "%", onChange = { o.brushOpacity = it / 100 })
            CheckOpt("Pressão: tamanho", o.pressureSize) { o.pressureSize = it }
            CheckOpt("Pressão: opacidade", o.pressureOpacity) { o.pressureOpacity = it }
        }
        ToolId.ERASER -> {
            ValueField("", o.eraserSize, 1f..1000f, " px", onChange = { o.eraserSize = it })
            ValueField("Dureza:", o.eraserHardness * 100, 0f..100f, "%", onChange = { o.eraserHardness = it / 100 })
            ValueField("Opacidade:", o.eraserOpacity * 100, 1f..100f, "%", onChange = { o.eraserOpacity = it / 100 })
            CheckOpt("Pressão: tamanho", o.pressureSize) { o.pressureSize = it }
        }
        ToolId.CLONE -> {
            TextBtn(if (o.cloneSettingSource) "Toque na origem…" else "Definir origem", primary = o.cloneSettingSource) { o.cloneSettingSource = !o.cloneSettingSource }
            ValueField("", o.cloneSize, 1f..1000f, " px", onChange = { o.cloneSize = it })
            ValueField("Dureza:", o.cloneHardness * 100, 0f..100f, "%", onChange = { o.cloneHardness = it / 100 })
            ValueField("Opacidade:", o.cloneOpacity * 100, 1f..100f, "%", onChange = { o.cloneOpacity = it / 100 })
        }
        ToolId.GRADIENT -> {
            Text("Frente → fundo", style = Bp.uiSmall)
            Row { TextBtn("Linear", primary = !o.gradientRadial) { o.gradientRadial = false }; Spacer(Modifier.width(4.dp)); TextBtn("Radial", primary = o.gradientRadial) { o.gradientRadial = true } }
        }
        ToolId.BUCKET -> {
            ValueField("Tolerância:", o.bucketTolerance.toFloat(), 0f..255f, onChange = { o.bucketTolerance = it.roundToInt() })
            CheckOpt("Contíguo", o.contiguous) { o.contiguous = it }
        }
        ToolId.HAND -> {
            TextBtn("Ajustar à tela") { editor.fit() }
            TextBtn("100%") { editor.zoomTo100() }
            Text("Dois dedos: zoom e giro em qualquer ferramenta.", style = Bp.uiSmall)
        }
    }
}

// ============================================================ barra de ferramentas

@OptIn(ExperimentalFoundationApi::class)
@Composable
private fun ToolBar(editor: Editor, landscape: Boolean, onColor: (Boolean) -> Unit) {
    val cols = if (landscape) 2 else 1
    val chosen = remember { mutableStateOf(TOOL_GROUPS.associateWith { it.tools.first() }) }
    var flyout by remember { mutableStateOf<Int?>(null) }
    Column(
        Modifier.fillMaxHeight().width((cols * 40 + 8).dp).background(Bp.bar).padding(4.dp)
            .verticalScroll(rememberScrollState()),
        horizontalAlignment = Alignment.CenterHorizontally,
    ) {
        TOOL_GROUPS.chunked(cols).forEach { row ->
            Row {
                row.forEach { g ->
                    val gi = TOOL_GROUPS.indexOf(g)
                    val current = if (editor.tool in g.tools) editor.tool else chosen.value[g]!!
                    Box {
                        Box(
                            Modifier.size(40.dp).clip(RoundedCornerShape(5.dp))
                                .background(if (editor.tool in g.tools) Bp.field else Color.Transparent)
                                .then(if (editor.tool in g.tools) Modifier.border(1.dp, Bp.line, RoundedCornerShape(5.dp)) else Modifier)
                                .combinedClickable(
                                    onClickLabel = current.label,
                                    onLongClick = { if (g.tools.size > 1) flyout = gi },
                                    onClick = {
                                        if (editor.tool == current && g.tools.size > 1) flyout = gi
                                        else editor.selectTool(current)
                                    },
                                ),
                            contentAlignment = Alignment.Center,
                        ) {
                            Icon(Ic.tool(current), current.label, Modifier.size(20.dp), tint = Bp.t1)
                            if (g.tools.size > 1) Canvas(Modifier.size(40.dp)) {
                                val p = Path().apply { moveTo(size.width - 4f * density, size.height - 9f * density); lineTo(size.width - 4f * density, size.height - 4f * density); lineTo(size.width - 9f * density, size.height - 4f * density); close() }
                                drawPath(p, Bp.t2)
                            }
                        }
                        DropdownMenu(flyout == gi, onDismissRequest = { flyout = null }, containerColor = Bp.panel, offset = androidx.compose.ui.unit.DpOffset(44.dp, (-40).dp)) {
                            g.tools.forEach { t ->
                                DropdownMenuItem(
                                    leadingIcon = { Icon(Ic.tool(t), null, Modifier.size(18.dp), tint = Bp.t1) },
                                    text = { Row(Modifier.width(200.dp)) { Text(t.label, style = Bp.ui, modifier = Modifier.weight(1f)); Text(t.key.toString(), style = Bp.value.copy(color = Bp.t2)) } },
                                    onClick = { chosen.value = chosen.value + (g to t); editor.selectTool(t); flyout = null },
                                )
                            }
                        }
                    }
                }
            }
        }
        Spacer(Modifier.height(10.dp))
        // cores de primeiro plano / fundo, com X e D
        Box(Modifier.size(52.dp)) {
            Box(Modifier.offset(16.dp, 16.dp).size(26.dp).background(Color(editor.bg)).border(1.dp, Bp.t2).clickable { onColor(false) })
            Box(Modifier.offset(4.dp, 4.dp).size(26.dp).background(Color(editor.fg)).border(1.dp, Bp.t2).clickable { onColor(true) })
            Box(Modifier.align(Alignment.TopEnd).size(18.dp).clickable { editor.swapColors() }, contentAlignment = Alignment.Center) {
                Icon(Ic.swap, "Alternar cores (X)", Modifier.size(13.dp), tint = Bp.t2)
            }
            Box(Modifier.align(Alignment.BottomStart).size(16.dp).clickable { editor.defaultColors() }, contentAlignment = Alignment.Center) {
                Box(Modifier.size(9.dp).background(Color.Black).border(1.dp, Color.White))
            }
        }
    }
}

// ============================================================ tela

@SuppressLint("ClickableViewAccessibility")
@Composable
private fun CanvasArea(editor: Editor, actions: EditorActions) {
    Box(Modifier.fillMaxSize()) {
        AndroidView(factory = { ctx ->
            object : GLSurfaceView(ctx) {
                override fun onTouchEvent(event: MotionEvent): Boolean = editor.onTouch(event)
                override fun onHoverEvent(event: MotionEvent): Boolean { editor.onTouch(event); return true }
            }.also { editor.attach(it) }
        }, modifier = Modifier.fillMaxSize())
        OverlayLayer(editor)
        if (!editor.doc.open) EmptyState(actions)
        StatusLine(editor, Modifier.align(Alignment.BottomStart))
        editor.toast?.let {
            Text(it, style = Bp.ui, modifier = Modifier.align(Alignment.BottomCenter).padding(bottom = 36.dp)
                .background(Color(0xE6202020), RoundedCornerShape(6.dp)).padding(horizontal = 14.dp, vertical = 10.dp))
        }
        if (editor.doc.busy) Row(
            Modifier.align(Alignment.TopEnd).padding(10.dp).background(Color(0xE6202020), RoundedCornerShape(6.dp)).padding(horizontal = 12.dp, vertical = 8.dp),
            verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp),
        ) {
            CircularProgressIndicator(Modifier.size(14.dp), color = Bp.accent, strokeWidth = 2.dp)
            Text("Calculando seleção…", style = Bp.ui)
        }
    }
}

@Composable
private fun EmptyState(actions: EditorActions) {
    Box(Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
        Column(horizontalAlignment = Alignment.CenterHorizontally, verticalArrangement = Arrangement.spacedBy(14.dp)) {
            Text("BP Touch", style = Bp.panelTitle.copy(fontSize = androidx.compose.ui.unit.TextUnit(28f, androidx.compose.ui.unit.TextUnitType.Sp)))
            Text("Abra uma foto para começar a editar.", style = Bp.ui.copy(color = Bp.t2))
            Row(horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                TextBtn("Abrir foto", primary = true) { actions.openPhoto() }
                TextBtn("Novo documento") { actions.newDocument() }
            }
        }
    }
}

@Composable
private fun StatusLine(editor: Editor, modifier: Modifier) {
    val d = editor.doc
    if (!d.open) return
    Row(
        modifier.background(Color(0xCC1E1E1E), RoundedCornerShape(topEnd = 6.dp)).padding(horizontal = 10.dp, vertical = 4.dp),
        horizontalArrangement = Arrangement.spacedBy(14.dp),
    ) {
        Text("${(editor.view.s * 100).roundToInt()}%", style = Bp.value.copy(color = Bp.t2))
        Text("${d.w} × ${d.h} px", style = Bp.value.copy(color = Bp.t2))
        Text(editor.tool.label, style = Bp.value.copy(color = Bp.t2))
    }
}

@Composable
private fun OverlayLayer(editor: Editor) {
    val ov = editor.overlay
    val v = editor.view
    Canvas(Modifier.fillMaxSize()) {
        val dash = PathEffect.dashPathEffect(floatArrayOf(6f, 6f))
        fun ants(path: Path) {
            drawPath(path, Color.Black, style = Stroke(1.5f))
            drawPath(path, Color.White, style = Stroke(1.5f, pathEffect = dash))
        }
        val a = ov.rectA; val b = ov.rectB
        if (a != null && b != null) {
            when (editor.tool) {
                ToolId.GRADIENT -> {
                    val p0 = v.toScreen(a.x, a.y); val p1 = v.toScreen(b.x, b.y)
                    drawLine(Color.Black, p0, p1, 3f); drawLine(Color.White, p0, p1, 1.5f)
                    drawCircle(Color.White, 5f, p0); drawCircle(Color.White, 5f, p1)
                }
                ToolId.MARQUEE_ELLIPSE -> {
                    val path = Path()
                    val cx = (a.x + b.x) / 2; val cy = (a.y + b.y) / 2; val rx = abs(b.x - a.x) / 2; val ry = abs(b.y - a.y) / 2
                    for (i in 0..96) {
                        val t = i / 96f * 6.2831855f
                        val s = v.toScreen(cx + rx * kotlin.math.cos(t), cy + ry * kotlin.math.sin(t))
                        if (i == 0) path.moveTo(s.x, s.y) else path.lineTo(s.x, s.y)
                    }
                    ants(path)
                }
                else -> {
                    val path = Path()
                    listOf(a, Offset(b.x, a.y), b, Offset(a.x, b.y)).forEachIndexed { i, o ->
                        val s = v.toScreen(o.x, o.y); if (i == 0) path.moveTo(s.x, s.y) else path.lineTo(s.x, s.y)
                    }
                    path.close(); ants(path)
                }
            }
        }
        if (ov.path.isNotEmpty()) {
            val path = Path()
            ov.path.forEachIndexed { i, o -> val s = v.toScreen(o.x, o.y); if (i == 0) path.moveTo(s.x, s.y) else path.lineTo(s.x, s.y) }
            if (editor.tool == ToolId.QUICK_SELECT) {
                drawPath(path, Color(0x663B8EEA), style = Stroke(ov.pathRadius * 2 * v.s, cap = androidx.compose.ui.graphics.StrokeCap.Round, join = androidx.compose.ui.graphics.StrokeJoin.Round))
            } else {
                ov.polyCursor?.let { val s = v.toScreen(it.x, it.y); path.lineTo(s.x, s.y) }
                ants(path)
                if (editor.tool == ToolId.LASSO_POLY) ov.path.forEach { o -> drawCircle(Color.White, 3.5f, v.toScreen(o.x, o.y)) }
            }
        }
        val ca = ov.cropA; val cb = ov.cropB
        if (editor.tool == ToolId.CROP && ca != null && cb != null) {
            val p0 = v.toScreen(minOf(ca.x, cb.x), minOf(ca.y, cb.y)); val p1 = v.toScreen(maxOf(ca.x, cb.x), maxOf(ca.y, cb.y))
            val r = androidx.compose.ui.geometry.Rect(p0, p1)
            val shade = Color(0x99000000)
            drawRect(shade, Offset.Zero, Size(size.width, r.top))
            drawRect(shade, Offset(0f, r.bottom), Size(size.width, size.height - r.bottom))
            drawRect(shade, Offset(0f, r.top), Size(r.left, r.height))
            drawRect(shade, Offset(r.right, r.top), Size(size.width - r.right, r.height))
            drawRect(Color.White, r.topLeft, r.size, style = Stroke(1.5f))
            for (k in 1..2) {
                drawLine(Color(0x88FFFFFF), Offset(r.left + r.width * k / 3, r.top), Offset(r.left + r.width * k / 3, r.bottom), 1f)
                drawLine(Color(0x88FFFFFF), Offset(r.left, r.top + r.height * k / 3), Offset(r.right, r.top + r.height * k / 3), 1f)
            }
        }
        ov.cloneSrc?.let { if (editor.tool == ToolId.CLONE) { val s = v.toScreen(it.x, it.y)
            drawLine(Color.White, s - Offset(10f, 0f), s + Offset(10f, 0f), 2f); drawLine(Color.White, s - Offset(0f, 10f), s + Offset(0f, 10f), 2f)
            drawCircle(Color.Black, 6f, s, style = Stroke(1.5f)) } }
        ov.cursor?.let { c ->
            if (ov.cursorRadius > 0f) {
                val s = v.toScreen(c.x, c.y); val r = ov.cursorRadius * v.s
                drawCircle(Color.Black, r, s, style = Stroke(2.5f)); drawCircle(Color.White, r, s, style = Stroke(1f))
            }
        }
    }
}

// ============================================================ gaveta de painéis

@Composable
private fun Drawer(editor: Editor, open: Boolean, width: androidx.compose.ui.unit.Dp, onOpen: (Boolean) -> Unit, onPin: () -> Unit, onRefine: () -> Unit) {
    val density = LocalDensity.current
    val wPx = with(density) { width.toPx() }
    var drag by remember { mutableStateOf(0f) }
    val base by animateFloatAsState(if (open) 0f else wPx, tween(200), label = "drawer")
    val offsetX = (base + drag).coerceIn(0f, wPx)
    Box(Modifier.fillMaxSize()) {
        // alça na borda direita
        if (!open) Box(
            Modifier.align(Alignment.CenterEnd).size(16.dp, 84.dp)
                .clip(RoundedCornerShape(topStart = 8.dp, bottomStart = 8.dp)).background(Bp.bar).border(1.dp, Bp.line, RoundedCornerShape(topStart = 8.dp, bottomStart = 8.dp))
                .draggable(rememberDraggableState { drag = (drag + it).coerceIn(-wPx, 0f) }, Orientation.Horizontal,
                    onDragStarted = { drag = 0f }, onDragStopped = { if (drag < -40f) onOpen(true); drag = 0f })
                .clickable { onOpen(true) },
            contentAlignment = Alignment.Center,
        ) { Box(Modifier.size(3.dp, 30.dp).background(Bp.t2, RoundedCornerShape(2.dp))) }
        if (open || drag != 0f || base < wPx) {
            Panels(
                editor,
                Modifier.align(Alignment.CenterEnd).width(width).fillMaxHeight().offset { IntOffset(offsetX.roundToInt(), 0) },
                pinned = false, onPin = onPin, onClose = { onOpen(false) }, onRefine = onRefine,
                headerModifier = Modifier.draggable(rememberDraggableState { drag = (drag + it).coerceIn(0f, wPx) }, Orientation.Horizontal,
                    onDragStarted = { drag = 0f }, onDragStopped = { if (drag > 60f) onOpen(false); drag = 0f }),
            )
        }
    }
}

@Composable
fun Panels(editor: Editor, modifier: Modifier, pinned: Boolean, onPin: () -> Unit, onClose: () -> Unit, onRefine: () -> Unit, headerModifier: Modifier = Modifier) {
    var tab by remember { mutableStateOf(0) }
    Column(modifier.background(Bp.panel).border(1.dp, Bp.line)) {
        Row(Modifier.fillMaxWidth().height(34.dp).background(Bp.bar).then(headerModifier), verticalAlignment = Alignment.CenterVertically) {
            listOf("Camadas", "Ajustes", "Histórico").forEachIndexed { i, t ->
                Box(
                    Modifier.fillMaxHeight().clickable { tab = i }.background(if (tab == i) Bp.panel else Color.Transparent).padding(horizontal = 12.dp),
                    contentAlignment = Alignment.Center,
                ) {
                    Text(t, style = if (tab == i) Bp.panelTitle else Bp.ui.copy(color = Bp.t2))
                    if (tab == i) Box(Modifier.align(Alignment.BottomCenter).fillMaxWidth().height(2.dp).background(Bp.accent))
                }
            }
            Spacer(Modifier.weight(1f))
            IconBtn(Ic.pin, if (pinned) "Soltar painel" else "Fixar painel", selected = pinned, size = 32.dp, iconSize = 16.dp, onClick = onPin)
            IconBtn(Ic.close, "Fechar painel", size = 32.dp, iconSize = 16.dp, onClick = onClose)
        }
        when (tab) {
            0 -> LayersPanel(editor, Modifier.weight(1f))
            1 -> AdjustPanel(editor, Modifier.weight(1f))
            else -> HistoryPanel(editor, Modifier.weight(1f))
        }
    }
}

@Suppress("unused")
private fun clampMin(a: Float, b: Float) = min(a, b)
