// SPDX-License-Identifier: MIT OR Apache-2.0
package com.bptouch.app.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.Icon
import androidx.compose.material3.Slider
import androidx.compose.material3.SliderDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.semantics.Role
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import kotlin.math.roundToInt

/** Botão de ícone compacto no estilo dos painéis (alvo de toque de 40 dp). */
@Composable
fun IconBtn(
    icon: ImageVector, desc: String, modifier: Modifier = Modifier, selected: Boolean = false, enabled: Boolean = true,
    size: Dp = 36.dp, iconSize: Dp = 18.dp, tint: Color = Bp.t1, onClick: () -> Unit,
) {
    Box(
        modifier
            .size(size)
            .clip(RoundedCornerShape(5.dp))
            .background(if (selected) Bp.field else Color.Transparent)
            .then(if (selected) Modifier.border(1.dp, Bp.line, RoundedCornerShape(5.dp)) else Modifier)
            .clickable(enabled = enabled, role = Role.Button, onClickLabel = desc, onClick = onClick),
        contentAlignment = Alignment.Center,
    ) {
        Icon(icon, desc, Modifier.size(iconSize), tint = if (enabled) tint else tint.copy(alpha = 0.35f))
    }
}

/** Caixa de seleção pequena com rótulo (barra de opções). */
@Composable
fun CheckOpt(label: String, checked: Boolean, onChange: (Boolean) -> Unit) {
    Row(
        Modifier.clip(RoundedCornerShape(4.dp)).clickable { onChange(!checked) }.padding(horizontal = 4.dp, vertical = 6.dp),
        verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(6.dp),
    ) {
        Box(
            Modifier.size(13.dp).clip(RoundedCornerShape(3.dp))
                .background(if (checked) Bp.accent else Color.Transparent)
                .border(1.dp, if (checked) Bp.accent else Bp.t2, RoundedCornerShape(3.dp)),
            contentAlignment = Alignment.Center,
        ) { if (checked) Icon(Ic.check, null, Modifier.size(11.dp), tint = Color.White) }
        Text(label, style = Bp.uiSmall)
    }
}

/**
 * Campo numérico com rótulo, como os da barra de opções do Photoshop: mostra o valor e,
 * ao tocar, abre um controle deslizante.
 */
@Composable
fun ValueField(
    label: String, value: Float, range: ClosedFloatingPointRange<Float>, unit: String = "", decimals: Int = 0,
    onChange: (Float) -> Unit, onDone: (() -> Unit)? = null,
) {
    var open by remember { mutableStateOf(false) }
    Box {
        Row(
            Modifier.clip(RoundedCornerShape(4.dp)).clickable { open = true }.padding(horizontal = 4.dp, vertical = 6.dp),
            verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(5.dp),
        ) {
            if (label.isNotEmpty()) Text(label, style = Bp.uiSmall)
            Box(Modifier.clip(RoundedCornerShape(4.dp)).background(Bp.field).border(1.dp, Bp.line, RoundedCornerShape(4.dp)).padding(horizontal = 7.dp, vertical = 3.dp)) {
                Text(fmt(value, decimals) + unit, style = Bp.value)
            }
        }
        DropdownMenu(open, onDismissRequest = { open = false; onDone?.invoke() }, containerColor = Bp.panel) {
            Column(Modifier.width(260.dp).padding(horizontal = 14.dp, vertical = 4.dp)) {
                Text("$label ${fmt(value, decimals)}$unit", style = Bp.ui)
                BpSlider(value, range, onChange = onChange, onDone = onDone)
            }
        }
    }
}

fun fmt(v: Float, decimals: Int): String =
    if (decimals == 0) v.roundToInt().toString() else String.format(java.util.Locale.US, "%.${decimals}f", v).replace('.', ',')

@Composable
fun BpSlider(value: Float, range: ClosedFloatingPointRange<Float>, modifier: Modifier = Modifier, onChange: (Float) -> Unit, onDone: (() -> Unit)? = null) {
    Slider(
        value = value.coerceIn(range), onValueChange = onChange, valueRange = range, onValueChangeFinished = onDone,
        modifier = modifier,
        colors = SliderDefaults.colors(thumbColor = Bp.t1, activeTrackColor = Bp.accent, inactiveTrackColor = Bp.line),
    )
}

@Composable
fun TextBtn(label: String, primary: Boolean = false, enabled: Boolean = true, onClick: () -> Unit) {
    Box(
        Modifier.clip(RoundedCornerShape(5.dp))
            .background(if (primary) Bp.accent else Bp.field)
            .border(1.dp, if (primary) Bp.accent else Bp.line, RoundedCornerShape(5.dp))
            .clickable(enabled = enabled, onClick = onClick)
            .padding(horizontal = 10.dp, vertical = 6.dp),
    ) { Text(label, style = Bp.ui.copy(color = if (primary) Color.White else if (enabled) Bp.t1 else Bp.t2)) }
}

@Composable
fun VSep() = Box(Modifier.width(1.dp).size(1.dp, 20.dp).background(Bp.line))
