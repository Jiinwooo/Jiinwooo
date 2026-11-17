#version 300 es
// SPDX-License-Identifier: MIT OR Apache-2.0
// Ferramenta Mover: desloca o conteúdo selecionado (ou a camada inteira).
precision highp float;
in vec2 vUV;
out vec4 fragColor;
uniform sampler2D uLayer;
uniform sampler2D uSel;
uniform vec2 uOffset;   // UV
void main() {
    vec2 s = vUV - uOffset;
    bool inside = all(greaterThanEqual(s, vec2(0.0))) && all(lessThanEqual(s, vec2(1.0)));
    vec4 moved = inside ? texture(uLayer, s) * texture(uSel, s).r : vec4(0.0);
    vec4 rest = texture(uLayer, vUV) * (1.0 - texture(uSel, vUV).r);
    fragColor = moved + rest * (1.0 - moved.a);
}
