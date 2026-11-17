#version 300 es
// SPDX-License-Identifier: MIT OR Apache-2.0
// Telar — triângulo de tela cheia, sem buffer de vértices (glDrawArrays(GL_TRIANGLES, 0, 3)).
out vec2 vUV;
void main() {
    vec2 p = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2));
    vUV = p;
    gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);
}
