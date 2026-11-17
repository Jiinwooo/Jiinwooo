#version 300 es
// SPDX-License-Identifier: MIT OR Apache-2.0
// Aplica a máscara do traço na camada: 0 pintar, 1 apagar, 2 carimbo (clonar).
precision highp float;
in vec2 vUV;
out vec4 fragColor;
uniform sampler2D uLayer;
uniform sampler2D uStroke;   // R: cobertura do traço
uniform sampler2D uSel;      // R: seleção (branco se não houver)
uniform int uMode;
uniform vec4 uColor;         // não pré-multiplicada
uniform vec2 uCloneOffset;   // UV
uniform bool uLockAlpha;
void main() {
    vec4 L = texture(uLayer, vUV);
    float m = texture(uStroke, vUV).r * texture(uSel, vUV).r;
    if (uMode == 1) { fragColor = L * (1.0 - m); return; }
    vec4 src = uMode == 2 ? texture(uLayer, vUV - uCloneOffset) : vec4(uColor.rgb * uColor.a, uColor.a);
    vec4 o = src * m + L * (1.0 - src.a * m);
    if (uLockAlpha) o = L.a > 0.0 ? vec4(o.rgb / max(o.a, 1e-5) * L.a, L.a) : L;
    fragColor = o;
}
