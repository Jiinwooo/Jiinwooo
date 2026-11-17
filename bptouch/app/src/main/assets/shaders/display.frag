#version 300 es
// SPDX-License-Identifier: MIT OR Apache-2.0
// Imagem composta sobre xadrez de transparência + formigas marchando na borda da seleção.
precision highp float;
in vec2 vUV;
out vec4 fragColor;
uniform sampler2D uComp;
uniform sampler2D uSel;
uniform bool uHasSel;
uniform float uTime;
uniform float uPixelGrid;   // > 0 quando o zoom é alto: desenha a grade de pixels
uniform vec2 uDocSize;
void main() {
    vec4 c = texture(uComp, vUV);
    vec2 q = floor(gl_FragCoord.xy / 8.0);
    float chk = mod(q.x + q.y, 2.0) < 1.0 ? 1.0 : 0.8;
    vec3 col = c.rgb + vec3(chk) * (1.0 - c.a);
    if (uPixelGrid > 0.0) {
        vec2 f = fract(vUV * uDocSize);
        vec2 w = fwidth(vUV * uDocSize);
        float g = (f.x < w.x || f.y < w.y) ? 1.0 : 0.0;
        col = mix(col, vec3(0.5), g * 0.25 * uPixelGrid);
    }
    if (uHasSel) {
        vec2 dx = dFdx(vUV), dy = dFdy(vUV);
        bool m0 = texture(uSel, vUV).r >= 0.5;
        bool m1 = texture(uSel, vUV + dx).r >= 0.5;
        bool m2 = texture(uSel, vUV + dy).r >= 0.5;
        bool m3 = texture(uSel, vUV - dx).r >= 0.5;
        bool m4 = texture(uSel, vUV - dy).r >= 0.5;
        if (m0 && (!m1 || !m2 || !m3 || !m4)) {
            float stripe = mod(floor((gl_FragCoord.x + gl_FragCoord.y) / 4.0 - uTime * 8.0), 2.0);
            col = vec3(stripe);
        }
    }
    fragColor = vec4(col, 1.0);
}
