#version 300 es
// SPDX-License-Identifier: MIT OR Apache-2.0
// Perfil do pincel: dureza 1 = borda dura (com 1 px de antisserrilhado), 0 = totalmente macia.
precision highp float;
in vec2 vLocal;
in float vAlpha;
in float vRadius;
out vec4 fragColor;
uniform float uHardness;
void main() {
    float d = length(vLocal);
    float edge = clamp(vRadius - d + 0.5, 0.0, 1.0);           // corte antisserrilhado
    float t = clamp(d / max(vRadius, 0.5), 0.0, 1.0);
    float inner = uHardness * 0.98;
    float soft = 1.0 - smoothstep(inner, 1.0, t);
    float a = vAlpha * min(edge, uHardness >= 0.999 ? 1.0 : soft);
    fragColor = vec4(a, 0.0, 0.0, 1.0);
}
