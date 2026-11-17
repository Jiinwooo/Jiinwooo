#version 300 es
// SPDX-License-Identifier: MIT OR Apache-2.0
// Telar — composição de camada com os 27 modos de mesclagem do menu de Camadas.
//
// Código original do projeto Telar. As fórmulas são matemática pública: W3C "Compositing and
// Blending Level 1" (modos separáveis e não separáveis, e a equação de composição) e as
// definições usuais dos modos "Linear", "Vivid/Pin Light", "Hard Mix", "Subtract", "Divide",
// "Darker/Lighter Color" e "Dissolve".
//
// Convenções
//   * Texturas de entrada e saída em alfa PRÉ-multiplicado (padrão do motor do Telar).
//   * A mesclagem acontece nos valores armazenados (espaço com gama, como os editores de foto
//     fazem por padrão). Para mesclar em linear, o motor converte antes e depois.
//   * Uma passada: lê o fundo (uBackdrop) e a camada (uLayer) e escreve o composto.
//   * uOpacity e uFill multiplicam-se. São separados porque Preenchimento, ao contrário de
//     Opacidade, não atinge estilos de camada, que são compostos em outro passe.

precision highp float;
precision highp int;

in vec2 vUV;
out vec4 fragColor;

uniform sampler2D uBackdrop;   // tudo que está abaixo da camada, pré-multiplicado
uniform sampler2D uLayer;      // a camada, pré-multiplicada
uniform sampler2D uMask;       // máscara de camada (canal R); branca se não houver
uniform int   uMode;           // ver tabela BLEND_* abaixo
uniform float uOpacity;        // 0..1
uniform float uFill;           // 0..1
uniform uint  uDissolveSeed;   // muda o padrão do Dissolver por camada

// ---- IDs: mesma ordem e agrupamento do menu de mesclagem -------------------------------
const int BLEND_NORMAL        = 0;
const int BLEND_DISSOLVE      = 1;
const int BLEND_DARKEN        = 2;
const int BLEND_MULTIPLY      = 3;
const int BLEND_COLOR_BURN    = 4;
const int BLEND_LINEAR_BURN   = 5;
const int BLEND_DARKER_COLOR  = 6;
const int BLEND_LIGHTEN       = 7;
const int BLEND_SCREEN        = 8;
const int BLEND_COLOR_DODGE   = 9;
const int BLEND_LINEAR_DODGE  = 10;  // "Adicionar"
const int BLEND_LIGHTER_COLOR = 11;
const int BLEND_OVERLAY       = 12;
const int BLEND_SOFT_LIGHT    = 13;
const int BLEND_HARD_LIGHT    = 14;
const int BLEND_VIVID_LIGHT   = 15;
const int BLEND_LINEAR_LIGHT  = 16;
const int BLEND_PIN_LIGHT     = 17;
const int BLEND_HARD_MIX      = 18;
const int BLEND_DIFFERENCE    = 19;
const int BLEND_EXCLUSION     = 20;
const int BLEND_SUBTRACT      = 21;
const int BLEND_DIVIDE        = 22;
const int BLEND_HUE           = 23;
const int BLEND_SATURATION    = 24;
const int BLEND_COLOR         = 25;
const int BLEND_LUMINOSITY    = 26;

// ---- modos separáveis, por canal: b = fundo, s = camada (não pré-multiplicados) --------

float colorDodge(float b, float s) {
    if (b <= 0.0) return 0.0;
    if (s >= 1.0) return 1.0;
    return min(1.0, b / (1.0 - s));
}

float colorBurn(float b, float s) {
    if (b >= 1.0) return 1.0;
    if (s <= 0.0) return 0.0;
    return 1.0 - min(1.0, (1.0 - b) / s);
}

float hardLight(float b, float s) {
    return s <= 0.5 ? b * 2.0 * s
                    : 1.0 - (1.0 - b) * (1.0 - (2.0 * s - 1.0));   // tela com 2s-1
}

float softLight(float b, float s) {
    if (s <= 0.5) return b - (1.0 - 2.0 * s) * b * (1.0 - b);
    float d = b <= 0.25 ? ((16.0 * b - 12.0) * b + 4.0) * b : sqrt(b);
    return b + (2.0 * s - 1.0) * (d - b);
}

float vividLight(float b, float s) {
    return s <= 0.5 ? colorBurn(b, 2.0 * s) : colorDodge(b, 2.0 * s - 1.0);
}

float pinLight(float b, float s) {
    return s <= 0.5 ? min(b, 2.0 * s) : max(b, 2.0 * s - 1.0);
}

float divideCh(float b, float s) {
    if (s <= 0.0) return b > 0.0 ? 1.0 : 0.0;
    return min(1.0, b / s);
}

vec3 blendSeparable(int m, vec3 b, vec3 s) {
    switch (m) {
        case BLEND_DARKEN:       return min(b, s);
        case BLEND_MULTIPLY:     return b * s;
        case BLEND_COLOR_BURN:   return vec3(colorBurn(b.r, s.r), colorBurn(b.g, s.g), colorBurn(b.b, s.b));
        case BLEND_LINEAR_BURN:  return max(b + s - 1.0, 0.0);
        case BLEND_LIGHTEN:      return max(b, s);
        case BLEND_SCREEN:       return b + s - b * s;
        case BLEND_COLOR_DODGE:  return vec3(colorDodge(b.r, s.r), colorDodge(b.g, s.g), colorDodge(b.b, s.b));
        case BLEND_LINEAR_DODGE: return min(b + s, 1.0);
        case BLEND_OVERLAY:      return vec3(hardLight(s.r, b.r), hardLight(s.g, b.g), hardLight(s.b, b.b));
        case BLEND_SOFT_LIGHT:   return vec3(softLight(b.r, s.r), softLight(b.g, s.g), softLight(b.b, s.b));
        case BLEND_HARD_LIGHT:   return vec3(hardLight(b.r, s.r), hardLight(b.g, s.g), hardLight(b.b, s.b));
        case BLEND_VIVID_LIGHT:  return vec3(vividLight(b.r, s.r), vividLight(b.g, s.g), vividLight(b.b, s.b));
        case BLEND_LINEAR_LIGHT: return clamp(b + 2.0 * s - 1.0, 0.0, 1.0);
        case BLEND_PIN_LIGHT:    return vec3(pinLight(b.r, s.r), pinLight(b.g, s.g), pinLight(b.b, s.b));
        case BLEND_HARD_MIX:     return step(1.0, b + s);
        case BLEND_DIFFERENCE:   return abs(b - s);
        case BLEND_EXCLUSION:    return b + s - 2.0 * b * s;
        case BLEND_SUBTRACT:     return max(b - s, 0.0);
        case BLEND_DIVIDE:       return vec3(divideCh(b.r, s.r), divideCh(b.g, s.g), divideCh(b.b, s.b));
        default:                 return s;   // Normal / Dissolver
    }
}

// ---- modos não separáveis (luminosidade / saturação) ------------------------------------

float lum(vec3 c) { return dot(c, vec3(0.30, 0.59, 0.11)); }

vec3 clipColor(vec3 c) {
    float l = lum(c);
    float n = min(min(c.r, c.g), c.b);
    float x = max(max(c.r, c.g), c.b);
    if (n < 0.0) c = l + (c - l) * l / max(l - n, 1e-6);
    if (x > 1.0) c = l + (c - l) * (1.0 - l) / max(x - l, 1e-6);
    return c;
}

vec3 setLum(vec3 c, float l) { return clipColor(c + (l - lum(c))); }

float sat(vec3 c) { return max(max(c.r, c.g), c.b) - min(min(c.r, c.g), c.b); }

// Reescala c para ter saturação s, mantendo a ordem dos canais (o menor vai a 0).
vec3 setSat(vec3 c, float s) {
    float mn = min(min(c.r, c.g), c.b);
    float mx = max(max(c.r, c.g), c.b);
    float range = mx - mn;
    return range > 1e-6 ? (c - mn) * (s / range) : vec3(0.0);
}

vec3 blendNonSeparable(int m, vec3 b, vec3 s) {
    switch (m) {
        case BLEND_HUE:           return setLum(setSat(s, sat(b)), lum(b));
        case BLEND_SATURATION:    return setLum(setSat(b, sat(s)), lum(b));
        case BLEND_COLOR:         return setLum(s, lum(b));
        case BLEND_LUMINOSITY:    return setLum(b, lum(s));
        case BLEND_DARKER_COLOR:  return lum(s) < lum(b) ? s : b;
        case BLEND_LIGHTER_COLOR: return lum(s) > lum(b) ? s : b;
        default:                  return s;
    }
}

bool isNonSeparable(int m) {
    return m == BLEND_HUE || m == BLEND_SATURATION || m == BLEND_COLOR || m == BLEND_LUMINOSITY
        || m == BLEND_DARKER_COLOR || m == BLEND_LIGHTER_COLOR;
}

// ---- Dissolver: ruído estável por pixel ------------------------------------------------

float hash01(uvec2 p, uint seed) {
    uint h = p.x * 0x8da6b343u ^ p.y * 0xd8163841u ^ seed * 0xcb1ab31fu;
    h ^= h >> 16; h *= 0x7feb352du; h ^= h >> 15; h *= 0x846ca68bu; h ^= h >> 16;
    return float(h) * (1.0 / 4294967296.0);
}

vec3 unpremul(vec4 c) { return c.a > 0.0 ? clamp(c.rgb / c.a, 0.0, 1.0) : vec3(0.0); }

void main() {
    vec4 Bp = texture(uBackdrop, vUV);
    vec4 Sp = texture(uLayer, vUV);
    float mask = texture(uMask, vUV).r;

    float ab = Bp.a;
    float as = Sp.a * uOpacity * uFill * mask;

    if (uMode == BLEND_DISSOLVE) {
        // cada pixel aparece inteiro ou some; a chance é a opacidade efetiva
        as = hash01(uvec2(gl_FragCoord.xy), uDissolveSeed) < as ? 1.0 : 0.0;
    }

    vec3 Cb = unpremul(Bp);
    vec3 Cs = unpremul(Sp);

    vec3 B = isNonSeparable(uMode) ? blendNonSeparable(uMode, Cb, Cs)
                                   : blendSeparable(uMode, Cb, Cs);

    // W3C: onde o fundo é transparente vale a cor da camada pura
    vec3 Cmix = (1.0 - ab) * Cs + ab * clamp(B, 0.0, 1.0);

    // source-over em pré-multiplicado
    float ao = as + ab * (1.0 - as);
    vec3  co = as * Cmix + (1.0 - as) * Bp.rgb;
    fragColor = vec4(co, ao);
}
