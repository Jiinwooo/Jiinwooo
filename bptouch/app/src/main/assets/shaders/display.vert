#version 300 es
// SPDX-License-Identifier: MIT OR Apache-2.0
// Desenha o documento na tela com a transformação de vista (zoom, giro, deslocamento).
uniform vec2 uDocSize;
uniform vec2 uScreen;
uniform mat3 uDocToScreen;
out vec2 vUV;
void main() {
    vec2 c = vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1));
    vUV = c;
    vec3 s = uDocToScreen * vec3(c * uDocSize, 1.0);
    gl_Position = vec4(s.x / uScreen.x * 2.0 - 1.0, 1.0 - s.y / uScreen.y * 2.0, 0.0, 1.0);
}
