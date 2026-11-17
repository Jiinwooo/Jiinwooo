#version 300 es
// SPDX-License-Identifier: MIT OR Apache-2.0
// Desfoque gaussiano separável (um eixo por passada), em pré-multiplicado.
precision highp float;
in vec2 vUV;
out vec4 fragColor;
uniform sampler2D uSrc;
uniform vec2 uDir;       // (1/largura, 0) ou (0, 1/altura)
uniform float uSigma;    // em pixels
void main() {
    if (uSigma < 0.3) { fragColor = texture(uSrc, vUV); return; }
    int r = int(ceil(uSigma * 3.0));
    float k = -0.5 / (uSigma * uSigma);
    vec4 acc = texture(uSrc, vUV);
    float wsum = 1.0;
    // pares de amostras com filtragem linear: metade das leituras
    for (int i = 1; i <= r; i += 2) {
        float w0 = exp(float(i * i) * k), w1 = exp(float((i + 1) * (i + 1)) * k);
        float w = w0 + w1;
        float o = (float(i) * w0 + float(i + 1) * w1) / w;
        acc += (texture(uSrc, vUV + uDir * o) + texture(uSrc, vUV - uDir * o)) * w;
        wsum += 2.0 * w;
    }
    fragColor = acc / wsum;
}
