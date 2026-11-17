# Filtros do Telar (GLSL ES 3.00)

Shaders originais do Telar, `#version 300 es`, para o motor GLES 3.0 (arm64). Licença
`MIT OR Apache-2.0`. Nada aqui foi extraído do Photoshop Touch.

| Arquivo | O que faz |
|---|---|
| `composite_blend.vert` | Triângulo de tela cheia sem buffer de vértices (`glDrawArrays(GL_TRIANGLES, 0, 3)`). |
| `composite_blend.frag` | Compõe uma camada sobre o fundo com os **27 modos de mesclagem** do menu de Camadas, opacidade, preenchimento e máscara. |
| `tests/blend_webgl2_test.js` | Compila o shader em WebGL2 (mesmo GLSL ES 3.00), compara cada modo com uma referência em JS e gera `tests/blend_modes_chart.png`. |

## Contrato do `composite_blend.frag`

| Uniform | Tipo | Significado |
|---|---|---|
| `uBackdrop` | `sampler2D` | Tudo abaixo da camada, RGBA **pré-multiplicado** |
| `uLayer` | `sampler2D` | A camada, RGBA pré-multiplicado |
| `uMask` | `sampler2D` | Máscara de camada no canal R (textura 1×1 branca se não houver) |
| `uMode` | `int` | 0 Normal · 1 Dissolver · 2 Escurecer · 3 Multiplicação · 4 Subexposição de cores · 5 Subexposição linear · 6 Cor mais escura · 7 Clarear · 8 Tela · 9 Superexposição de cores · 10 Superexposição linear (Adicionar) · 11 Cor mais clara · 12 Sobrepor · 13 Luz suave · 14 Luz direta · 15 Luz brilhante · 16 Luz linear · 17 Luz do pino · 18 Mistura sólida · 19 Diferença · 20 Exclusão · 21 Subtrair · 22 Dividir · 23 Matiz · 24 Saturação · 25 Cor · 26 Luminosidade |
| `uOpacity`, `uFill` | `float` | 0..1, multiplicados entre si |
| `uDissolveSeed` | `uint` | Semente do padrão do Dissolver |

Matemática: equação de composição e modos separáveis/não separáveis do W3C *Compositing and
Blending Level 1*; os demais modos pelas definições usuais (ver comentários no arquivo).
A mesclagem é feita nos valores armazenados (com gama), como os editores de foto fazem por
padrão.

**Limitações conhecidas:** "Mistura sólida" com Preenchimento abaixo de 100% tem, em alguns
editores, um comportamento especial que ainda não reproduzimos. Os modos são aplicados em RGB;
CMYK e Lab ficam para depois.

## Rodar o teste

```bash
node telar/filters/tests/blend_webgl2_test.js
```

Precisa de `playwright` com Chromium. Resultado atual: **27 de 27 modos batem com a referência**
(erro máximo < 3·10⁻⁷ em float32). No empate exato `b + s = 1` da Mistura sólida, o resultado
é indefinido e o teste ignora o caso.
