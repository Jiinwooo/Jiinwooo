#version 300 es
// SPDX-License-Identifier: MIT OR Apache-2.0
// Cópia com transformação de UV (recorte, giro, espelho) e máscara opcional.
precision highp float;
in vec2 vUV;
out vec4 fragColor;
uniform sampler2D uSrc;
uniform sampler2D uMask;     // R; branca se não houver
uniform mat3 uXform;         // UV de destino → UV de origem
void main() {
    vec2 uv = (uXform * vec3(vUV, 1.0)).xy;
    if (uv.x < 0.0 || uv.y < 0.0 || uv.x > 1.0 || uv.y > 1.0) { fragColor = vec4(0.0); return; }
    fragColor = texture(uSrc, uv) * texture(uMask, vUV).r;
}
