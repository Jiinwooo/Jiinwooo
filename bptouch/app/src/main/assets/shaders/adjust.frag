#version 300 es
// SPDX-License-Identifier: MIT OR Apache-2.0
// Ajustes de imagem com aplicação pela seleção.
// uType: 0 Brilho/Contraste · 1 Matiz/Saturação · 2 Exposição · 3 Temperatura/Vibração
//        4 Níveis · 5 Desfoque gaussiano · 6 Nitidez · 7 Preto e branco · 8 Inverter
//        9 Curvas (tabela 256)
precision highp float;
in vec2 vUV;
out vec4 fragColor;
uniform sampler2D uSrc;
uniform sampler2D uBlur;     // versão desfocada (tipos 5 e 6)
uniform sampler2D uSel;
uniform sampler2D uCurve;    // 256x1, R=curva mestre
uniform int uType;
uniform vec4 uP;             // parâmetros
uniform vec4 uQ;

vec3 rgb2hsl(vec3 c) {
    float mx = max(max(c.r, c.g), c.b), mn = min(min(c.r, c.g), c.b);
    float l = (mx + mn) * 0.5, h = 0.0, s = 0.0, d = mx - mn;
    if (d > 1e-6) {
        s = l > 0.5 ? d / (2.0 - mx - mn) : d / (mx + mn);
        if (mx == c.r) h = (c.g - c.b) / d + (c.g < c.b ? 6.0 : 0.0);
        else if (mx == c.g) h = (c.b - c.r) / d + 2.0;
        else h = (c.r - c.g) / d + 4.0;
        h /= 6.0;
    }
    return vec3(h, s, l);
}
float hue2(float p, float q, float t) {
    t = fract(t);
    if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
    if (t < 0.5) return q;
    if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
    return p;
}
vec3 hsl2rgb(vec3 h) {
    if (h.y < 1e-6) return vec3(h.z);
    float q = h.z < 0.5 ? h.z * (1.0 + h.y) : h.z + h.y - h.z * h.y, p = 2.0 * h.z - q;
    return vec3(hue2(p, q, h.x + 1.0 / 3.0), hue2(p, q, h.x), hue2(p, q, h.x - 1.0 / 3.0));
}
float luma(vec3 c) { return dot(c, vec3(0.2126, 0.7152, 0.0722)); }
// sRGB <-> linear para exposição fisicamente coerente
vec3 toLin(vec3 c) { return mix(c / 12.92, pow((c + 0.055) / 1.055, vec3(2.4)), step(0.04045, c)); }
vec3 toSrgb(vec3 c) { c = max(c, 0.0); return mix(c * 12.92, 1.055 * pow(c, vec3(1.0 / 2.4)) - 0.055, step(0.0031308, c)); }

vec3 adjustColor(vec3 c) {
    if (uType == 0) {                       // brilho -1..1, contraste -1..1
        float b = uP.x, k = uP.y;
        c = c + b * 0.5 * (1.0 - abs(b) * 0.25) ;
        float f = k >= 0.0 ? 1.0 / max(1.0 - k * 0.98, 0.02) : 1.0 + k;
        c = (c - 0.5) * f + 0.5;
    } else if (uType == 1) {                // matiz -180..180, saturação -1..1, luminosidade -1..1
        vec3 h = rgb2hsl(clamp(c, 0.0, 1.0));
        h.x = fract(h.x + uP.x / 360.0);
        h.y = uP.y >= 0.0 ? h.y + (1.0 - h.y) * uP.y * h.y * 1.5 : h.y * (1.0 + uP.y);
        h.y = clamp(h.y, 0.0, 1.0);
        c = hsl2rgb(h);
        c = uP.z >= 0.0 ? mix(c, vec3(1.0), uP.z) : c * (1.0 + uP.z);
    } else if (uType == 2) {                // exposição (EV), deslocamento, gama
        vec3 l = toLin(clamp(c, 0.0, 1.0)) * exp2(uP.x) + uP.y;
        c = toSrgb(l);
        c = pow(max(c, 0.0), vec3(1.0 / max(uP.z, 0.05)));
    } else if (uType == 3) {                // temperatura -1..1, tonalidade -1..1, vibração -1..1, saturação -1..1
        vec3 l = toLin(clamp(c, 0.0, 1.0));
        // temperatura: quente = mais vermelho, menos azul; tonalidade: + magenta, − verde
        l *= vec3(1.0 + 0.30 * uP.x + 0.08 * uP.y, 1.0 - 0.22 * uP.y, 1.0 - 0.30 * uP.x + 0.08 * uP.y);
        c = toSrgb(l);
        float y = luma(c);
        float sat = max(max(c.r, c.g), c.b) - min(min(c.r, c.g), c.b);
        float v = uP.z * (1.0 - sat);       // vibração: mais forte em cores pouco saturadas
        c = mix(vec3(y), c, 1.0 + v + uP.w);
    } else if (uType == 4) {                // níveis: preto, gama, branco de entrada; saída preto, branco
        c = clamp((c - uP.x) / max(uP.z - uP.x, 1e-3), 0.0, 1.0);
        c = pow(c, vec3(1.0 / max(uP.y, 0.05)));
        c = mix(vec3(uQ.x), vec3(uQ.y), c);
    } else if (uType == 7) {                // preto e branco, com mistura
        c = vec3(luma(c));
    } else if (uType == 8) {
        c = 1.0 - c;
    } else if (uType == 9) {
        c = vec3(texture(uCurve, vec2(c.r, 0.5)).r, texture(uCurve, vec2(c.g, 0.5)).r, texture(uCurve, vec2(c.b, 0.5)).r);
    }
    return clamp(c, 0.0, 1.0);
}

void main() {
    vec4 s = texture(uSrc, vUV);
    float m = texture(uSel, vUV).r;
    vec4 o;
    if (uType == 5) {
        o = texture(uBlur, vUV);
    } else if (uType == 6) {                // máscara de nitidez: quantidade, raio (no desfoque), limiar
        vec4 b = texture(uBlur, vUV);
        vec3 cs = s.a > 0.0 ? s.rgb / s.a : vec3(0.0);
        vec3 cb = b.a > 0.0 ? b.rgb / b.a : vec3(0.0);
        vec3 d = cs - cb;
        d *= step(uP.z, abs(luma(d)));
        o = vec4(clamp(cs + d * uP.x, 0.0, 1.0) * s.a, s.a);
    } else {
        vec3 c = s.a > 0.0 ? s.rgb / s.a : vec3(0.0);
        o = vec4(adjustColor(c) * s.a, s.a);
    }
    fragColor = mix(s, o, m);
}
