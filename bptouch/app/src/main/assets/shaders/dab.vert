#version 300 es
// SPDX-License-Identifier: MIT OR Apache-2.0
// Um carimbo de pincel por instância: (x, y, raio) em pixels do documento e alfa.
layout(location = 0) in vec4 aDab;   // x, y, raio, alfa
uniform vec2 uDocSize;
out vec2 vLocal;
out float vAlpha;
out float vRadius;
void main() {
    vec2 corner = vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1)) * 2.0 - 1.0;
    float r = aDab.z + 1.0;                    // +1 px para o antisserrilhado
    vec2 p = aDab.xy + corner * r;
    vLocal = corner * r;
    vAlpha = aDab.w;
    vRadius = aDab.z;
    gl_Position = vec4(p / uDocSize * 2.0 - 1.0, 0.0, 1.0);
}
