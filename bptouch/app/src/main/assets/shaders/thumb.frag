#version 300 es
// SPDX-License-Identifier: MIT OR Apache-2.0
// Miniatura: média de 4x4 amostras por pixel de saída, sobre xadrez.
precision highp float;
in vec2 vUV;
out vec4 fragColor;
uniform sampler2D uSrc;
uniform vec2 uOutSize;
void main() {
    vec4 acc = vec4(0.0);
    for (int j = 0; j < 4; ++j) for (int i = 0; i < 4; ++i) {
        vec2 o = (vec2(float(i), float(j)) + 0.5) / 4.0 - 0.5;
        acc += texture(uSrc, vUV + o / uOutSize);
    }
    acc /= 16.0;
    vec2 c = floor(gl_FragCoord.xy / 6.0);
    float chk = mod(c.x + c.y, 2.0) < 1.0 ? 1.0 : 0.8;
    fragColor = vec4(acc.rgb + vec3(chk) * (1.0 - acc.a), 1.0);
}
