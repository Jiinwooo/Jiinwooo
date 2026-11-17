#version 300 es
// SPDX-License-Identifier: MIT OR Apache-2.0
// Preencher / limpar / degradê dentro da seleção.
// uMode: 0 cor sólida, 1 limpar, 2 degradê linear, 3 degradê radial.
precision highp float;
in vec2 vUV;
out vec4 fragColor;
uniform sampler2D uLayer;
uniform sampler2D uSel;
uniform sampler2D uRegion;   // máscara extra (lata de tinta); branca se não houver
uniform int uMode;
uniform vec4 uColorA;        // não pré-multiplicadas
uniform vec4 uColorB;
uniform vec2 uP0;            // pixels do documento
uniform vec2 uP1;
uniform vec2 uDocSize;
void main() {
    vec4 L = texture(uLayer, vUV);
    float m = texture(uSel, vUV).r * texture(uRegion, vUV).r;
    if (uMode == 1) { fragColor = L * (1.0 - m); return; }
    vec4 c = uColorA;
    if (uMode >= 2) {
        vec2 p = vUV * uDocSize;
        vec2 d = uP1 - uP0;
        float t = uMode == 2 ? dot(p - uP0, d) / max(dot(d, d), 1e-6) : length(p - uP0) / max(length(d), 1e-6);
        t = clamp(t, 0.0, 1.0);
        c = mix(uColorA, uColorB, t);
        // pontilhado leve contra faixas
        float n = fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453);
        c.rgb += (n - 0.5) / 255.0;
    }
    vec4 src = vec4(c.rgb * c.a, c.a);
    fragColor = src * m + L * (1.0 - src.a * m);
}
