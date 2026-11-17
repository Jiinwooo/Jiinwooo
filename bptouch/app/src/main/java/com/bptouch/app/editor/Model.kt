// SPDX-License-Identifier: MIT OR Apache-2.0
package com.bptouch.app.editor

import org.json.JSONObject

data class LayerInfo(
    val id: Int, val name: String, val visible: Boolean, val opacity: Float, val blend: Int, val lockAlpha: Boolean,
)

data class DocState(
    val w: Int = 0, val h: Int = 0, val active: Int = 0, val hasSelection: Boolean = false, val busy: Boolean = false,
    val canUndo: Boolean = false, val canRedo: Boolean = false, val layers: List<LayerInfo> = emptyList(),
) {
    val activeLayer: LayerInfo? get() = layers.firstOrNull { it.id == active }
    val open: Boolean get() = w > 0 && h > 0

    companion object {
        fun parse(s: String): DocState {
            val o = JSONObject(s)
            val arr = o.getJSONArray("layers")
            val ls = (0 until arr.length()).map {
                val l = arr.getJSONObject(it)
                LayerInfo(l.getInt("id"), l.getString("name"), l.getInt("visible") == 1, l.getDouble("opacity").toFloat(),
                    l.getInt("blend"), l.getInt("lockAlpha") == 1)
            }
            return DocState(o.getInt("w"), o.getInt("h"), o.getInt("active"), o.getInt("sel") == 1, o.getInt("busy") == 1,
                o.getInt("canUndo") == 1, o.getInt("canRedo") == 1, ls)
        }
    }
}

data class HistoryState(val pos: Int = 0, val items: List<String> = emptyList()) {
    companion object {
        fun parse(s: String): HistoryState {
            val o = JSONObject(s)
            val a = o.getJSONArray("items")
            return HistoryState(o.getInt("pos"), (0 until a.length()).map { a.getString(it) })
        }
    }
}

/** Modos de mesclagem na ordem do menu (índice = uMode do shader). null = separador. */
val BLEND_MENU: List<Pair<Int, String>?> = listOf(
    0 to "Normal", 1 to "Dissolver", null,
    2 to "Escurecer", 3 to "Multiplicação", 4 to "Subexposição de cores", 5 to "Subexposição linear", 6 to "Cor mais escura", null,
    7 to "Clarear", 8 to "Tela", 9 to "Superexposição de cores", 10 to "Superexposição linear (Adicionar)", 11 to "Cor mais clara", null,
    12 to "Sobrepor", 13 to "Luz suave", 14 to "Luz direta", 15 to "Luz brilhante", 16 to "Luz linear", 17 to "Luz do pino", 18 to "Mistura sólida", null,
    19 to "Diferença", 20 to "Exclusão", 21 to "Subtrair", 22 to "Dividir", null,
    23 to "Matiz", 24 to "Saturação", 25 to "Cor", 26 to "Luminosidade",
)

fun blendName(mode: Int): String = BLEND_MENU.firstOrNull { it?.first == mode }?.second ?: "Normal"

/** Ferramentas, na ordem e com os atalhos do Photoshop. */
enum class ToolId(val label: String, val key: Char) {
    MOVE("Mover", 'V'),
    MARQUEE_RECT("Letreiro retangular", 'M'), MARQUEE_ELLIPSE("Letreiro elíptico", 'M'),
    LASSO("Laço", 'L'), LASSO_POLY("Laço poligonal", 'L'),
    OBJECT_SELECT("Seleção de objeto", 'W'), QUICK_SELECT("Seleção rápida", 'W'), MAGIC_WAND("Varinha mágica", 'W'),
    CROP("Cortar", 'C'),
    EYEDROPPER("Conta-gotas", 'I'),
    BRUSH("Pincel", 'B'),
    CLONE("Carimbo", 'S'),
    ERASER("Borracha", 'E'),
    GRADIENT("Degradê", 'G'), BUCKET("Lata de tinta", 'G'),
    HAND("Mão", 'H'),
}

class ToolGroup(val tools: List<ToolId>) { val key get() = tools.first().key }

val TOOL_GROUPS = listOf(
    ToolGroup(listOf(ToolId.MOVE)),
    ToolGroup(listOf(ToolId.MARQUEE_RECT, ToolId.MARQUEE_ELLIPSE)),
    ToolGroup(listOf(ToolId.LASSO, ToolId.LASSO_POLY)),
    ToolGroup(listOf(ToolId.OBJECT_SELECT, ToolId.QUICK_SELECT, ToolId.MAGIC_WAND)),
    ToolGroup(listOf(ToolId.CROP)),
    ToolGroup(listOf(ToolId.EYEDROPPER)),
    ToolGroup(listOf(ToolId.BRUSH)),
    ToolGroup(listOf(ToolId.CLONE)),
    ToolGroup(listOf(ToolId.ERASER)),
    ToolGroup(listOf(ToolId.GRADIENT, ToolId.BUCKET)),
    ToolGroup(listOf(ToolId.HAND)),
)

/** Ajustes, na ordem do menu Imagem › Ajustes. Índice = uType do shader. */
enum class AdjustType(val code: Int, val label: String) {
    BRIGHTNESS(0, "Brilho/Contraste"), LEVELS(4, "Níveis"), CURVES(9, "Curvas"), EXPOSURE(2, "Exposição"),
    VIBRANCE(3, "Temperatura e vibração"), HUE_SAT(1, "Matiz/Saturação"), BW(7, "Preto e branco"), INVERT(8, "Inverter"),
    BLUR(5, "Desfoque gaussiano"), SHARPEN(6, "Nitidez"),
}

data class SliderSpec(val label: String, val min: Float, val max: Float, val def: Float, val unit: String = "", val decimals: Int = 0)

fun adjustSliders(t: AdjustType): List<SliderSpec> = when (t) {
    AdjustType.BRIGHTNESS -> listOf(SliderSpec("Brilho", -150f, 150f, 0f), SliderSpec("Contraste", -50f, 100f, 0f))
    AdjustType.HUE_SAT -> listOf(SliderSpec("Matiz", -180f, 180f, 0f, "°"), SliderSpec("Saturação", -100f, 100f, 0f), SliderSpec("Luminosidade", -100f, 100f, 0f))
    AdjustType.EXPOSURE -> listOf(SliderSpec("Exposição", -5f, 5f, 0f, "", 2), SliderSpec("Deslocamento", -0.5f, 0.5f, 0f, "", 3), SliderSpec("Correção de gama", 0.1f, 3f, 1f, "", 2))
    AdjustType.VIBRANCE -> listOf(SliderSpec("Temperatura", -100f, 100f, 0f), SliderSpec("Tonalidade", -100f, 100f, 0f), SliderSpec("Vibração", -100f, 100f, 0f), SliderSpec("Saturação", -100f, 100f, 0f))
    AdjustType.LEVELS -> listOf(SliderSpec("Entrada: preto", 0f, 253f, 0f), SliderSpec("Entrada: gama", 0.1f, 9.99f, 1f, "", 2), SliderSpec("Entrada: branco", 2f, 255f, 255f), SliderSpec("Saída: preto", 0f, 255f, 0f), SliderSpec("Saída: branco", 0f, 255f, 255f))
    AdjustType.BLUR -> listOf(SliderSpec("Raio", 0.1f, 100f, 4f, " px", 1))
    AdjustType.SHARPEN -> listOf(SliderSpec("Quantidade", 1f, 500f, 100f, "%"), SliderSpec("Raio", 0.1f, 64f, 1.5f, " px", 1), SliderSpec("Limiar", 0f, 255f, 0f, " níveis"))
    else -> emptyList()
}

/** Converte valores dos sliders (unidades da UI) nos parâmetros do shader. */
fun adjustParams(t: AdjustType, v: List<Float>): FloatArray {
    val p = FloatArray(8)
    when (t) {
        AdjustType.BRIGHTNESS -> { p[0] = v[0] / 150f; p[1] = v[1] / 100f }
        AdjustType.HUE_SAT -> { p[0] = v[0]; p[1] = v[1] / 100f; p[2] = v[2] / 100f }
        AdjustType.EXPOSURE -> { p[0] = v[0]; p[1] = v[1]; p[2] = v[2] }
        AdjustType.VIBRANCE -> { p[0] = v[0] / 100f; p[1] = v[1] / 100f; p[2] = v[2] / 100f; p[3] = v[3] / 100f }
        AdjustType.LEVELS -> { p[0] = v[0] / 255f; p[1] = v[1]; p[2] = v[2] / 255f; p[4] = v[3] / 255f; p[5] = v[4] / 255f }
        AdjustType.BLUR -> { p[0] = v[0] }
        AdjustType.SHARPEN -> { p[0] = v[0] / 100f; p[1] = v[1]; p[2] = v[2] / 255f }
        else -> {}
    }
    return p
}
