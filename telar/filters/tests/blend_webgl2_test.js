// SPDX-License-Identifier: MIT OR Apache-2.0
// Teste do composite_blend.frag: compila em WebGL2 (= GLSL ES 3.00, o mesmo do GLES 3.0),
// renderiza uma grade de casos para cada um dos 27 modos e compara com uma referência em
// JavaScript escrita à parte. Também gera uma prancha visual dos modos.
//
// Uso:  node telar/filters/tests/blend_webgl2_test.js [saida.png]
// Requer: playwright + chromium (o WebGL2 roda via SwiftShader em modo headless).

const fs = require('fs');
const path = require('path');
const { chromium } = require('playwright');

const dir = path.join(__dirname, '..');
const FRAG = fs.readFileSync(path.join(dir, 'composite_blend.frag'), 'utf8');
const VERT = fs.readFileSync(path.join(dir, 'composite_blend.vert'), 'utf8');
const OUT_PNG = process.argv[2] || path.join(__dirname, 'blend_modes_chart.png');

const MODES = ['Normal', 'Dissolver', 'Escurecer', 'Multiplicação', 'Subexposição de cores',
  'Subexposição linear', 'Cor mais escura', 'Clarear', 'Tela', 'Superexposição de cores',
  'Superexposição linear', 'Cor mais clara', 'Sobrepor', 'Luz suave', 'Luz direta',
  'Luz brilhante', 'Luz linear', 'Luz do pino', 'Mistura sólida', 'Diferença', 'Exclusão',
  'Subtrair', 'Dividir', 'Matiz', 'Saturação', 'Cor', 'Luminosidade'];

// ---------- roda no navegador ----------
function inPage({ VERT, FRAG, MODES }) {
  // Referência em JS (independente do GLSL)
  const L = c => 0.3 * c[0] + 0.59 * c[1] + 0.11 * c[2];
  const clip = c => {
    const l = L(c), n = Math.min(...c), x = Math.max(...c);
    if (n < 0) c = c.map(v => l + (v - l) * l / Math.max(l - n, 1e-6));
    if (x > 1) c = c.map(v => l + (v - l) * (1 - l) / Math.max(x - l, 1e-6));
    return c;
  };
  const setLum = (c, l) => { const d = l - L(c); return clip(c.map(v => v + d)); };
  const S = c => Math.max(...c) - Math.min(...c);
  const setSat = (c, s) => { const mn = Math.min(...c), r = Math.max(...c) - mn; return r > 1e-6 ? c.map(v => (v - mn) * s / r) : [0, 0, 0]; };
  const dodge = (b, s) => b <= 0 ? 0 : s >= 1 ? 1 : Math.min(1, b / (1 - s));
  const burn = (b, s) => b >= 1 ? 1 : s <= 0 ? 0 : 1 - Math.min(1, (1 - b) / s);
  const hard = (b, s) => s <= .5 ? 2 * b * s : 1 - 2 * (1 - b) * (1 - s);
  const soft = (b, s) => s <= .5 ? b - (1 - 2 * s) * b * (1 - b) : b + (2 * s - 1) * ((b <= .25 ? ((16 * b - 12) * b + 4) * b : Math.sqrt(b)) - b);
  const sepFn = {
    2: Math.min, 3: (b, s) => b * s, 4: burn, 5: (b, s) => Math.max(b + s - 1, 0), 7: Math.max,
    8: (b, s) => b + s - b * s, 9: dodge, 10: (b, s) => Math.min(b + s, 1), 12: (b, s) => hard(s, b),
    13: soft, 14: hard, 15: (b, s) => s <= .5 ? burn(b, 2 * s) : dodge(b, 2 * s - 1),
    16: (b, s) => Math.min(1, Math.max(0, b + 2 * s - 1)),
    17: (b, s) => s <= .5 ? Math.min(b, 2 * s) : Math.max(b, 2 * s - 1),
    18: (b, s) => b + s >= 1 ? 1 : 0, 19: (b, s) => Math.abs(b - s), 20: (b, s) => b + s - 2 * b * s,
    21: (b, s) => Math.max(b - s, 0), 22: (b, s) => s <= 0 ? (b > 0 ? 1 : 0) : Math.min(1, b / s),
  };
  function ref(mode, Bp, Sp, opacity) {
    const ab = Bp[3], as = Sp[3] * opacity;
    const un = c => c[3] > 0 ? [0, 1, 2].map(i => Math.min(1, c[i] / c[3])) : [0, 0, 0];
    const Cb = un(Bp), Cs = un(Sp);
    let B;
    if (sepFn[mode]) B = Cb.map((b, i) => sepFn[mode](b, Cs[i]));
    else if (mode === 23) B = setLum(setSat(Cs, S(Cb)), L(Cb));
    else if (mode === 24) B = setLum(setSat(Cb, S(Cs)), L(Cb));
    else if (mode === 25) B = setLum(Cs, L(Cb));
    else if (mode === 26) B = setLum(Cb, L(Cs));
    else if (mode === 6) B = L(Cs) < L(Cb) ? Cs : Cb;
    else if (mode === 11) B = L(Cs) > L(Cb) ? Cs : Cb;
    else B = Cs;
    const mix = Cs.map((s, i) => (1 - ab) * s + ab * Math.min(1, Math.max(0, B[i])));
    return [...mix.map((m, i) => as * m + (1 - as) * Bp[i]), as + ab * (1 - as)];
  }

  // ---------- GL ----------
  const cv = document.createElement('canvas');
  const gl = cv.getContext('webgl2', { premultipliedAlpha: true, preserveDrawingBuffer: true });
  if (!gl) return { error: 'WebGL2 indisponível' };
  const floatRT = !!gl.getExtension('EXT_color_buffer_float');
  function sh(type, src) { const s = gl.createShader(type); gl.shaderSource(s, src); gl.compileShader(s); if (!gl.getShaderParameter(s, gl.COMPILE_STATUS)) throw new Error(gl.getShaderInfoLog(s)); return s; }
  let prog;
  try {
    prog = gl.createProgram();
    gl.attachShader(prog, sh(gl.VERTEX_SHADER, VERT)); gl.attachShader(prog, sh(gl.FRAGMENT_SHADER, FRAG));
    gl.linkProgram(prog); if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) throw new Error(gl.getProgramInfoLog(prog));
  } catch (e) { return { error: 'compilação: ' + e.message }; }
  gl.useProgram(prog);
  gl.bindVertexArray(gl.createVertexArray());
  const U = n => gl.getUniformLocation(prog, n);

  function tex(w, h, data, float) {
    const t = gl.createTexture(); gl.bindTexture(gl.TEXTURE_2D, t);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST); gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE); gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    if (float) gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA32F, w, h, 0, gl.RGBA, gl.FLOAT, data);
    else gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA8, w, h, 0, gl.RGBA, gl.UNSIGNED_BYTE, data);
    return t;
  }
  function run(mode, w, h, back, layer, opacity, float) {
    const tb = tex(w, h, back, float), tl = tex(w, h, layer, float);
    const tm = tex(1, 1, float ? new Float32Array([1, 1, 1, 1]) : new Uint8Array([255, 255, 255, 255]), float);
    const out = gl.createTexture(); gl.bindTexture(gl.TEXTURE_2D, out);
    if (float) gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA32F, w, h, 0, gl.RGBA, gl.FLOAT, null);
    else gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA8, w, h, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
    const fb = gl.createFramebuffer(); gl.bindFramebuffer(gl.FRAMEBUFFER, fb);
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, out, 0);
    gl.viewport(0, 0, w, h);
    [[tb, 'uBackdrop'], [tl, 'uLayer'], [tm, 'uMask']].forEach(([t, n], i) => { gl.activeTexture(gl.TEXTURE0 + i); gl.bindTexture(gl.TEXTURE_2D, t); gl.uniform1i(U(n), i); });
    gl.uniform1i(U('uMode'), mode); gl.uniform1f(U('uOpacity'), opacity); gl.uniform1f(U('uFill'), 1); gl.uniform1ui(U('uDissolveSeed'), 7);
    gl.drawArrays(gl.TRIANGLES, 0, 3);
    const px = float ? new Float32Array(w * h * 4) : new Uint8Array(w * h * 4);
    gl.readPixels(0, 0, w, h, gl.RGBA, float ? gl.FLOAT : gl.UNSIGNED_BYTE, px);
    return px;
  }

  // ---------- 1) exatidão numérica ----------
  const vals = [0, 0.1, 0.25, 0.4, 0.5, 0.6, 0.75, 0.9, 1];
  const alphas = [1, 0.6];
  const cases = [];
  for (const b of vals) for (const s of vals) for (const a of alphas) {
    const bc = [b, (b * 0.7 + 0.2) % 1, 1 - b], sc = [s, 1 - s, (s * 0.5 + 0.3) % 1];
    cases.push({ Bp: [...bc.map(v => v * a), a], Sp: [...sc, 1] });
  }
  const N = cases.length, W = N, H = 1;
  const back = new Float32Array(N * 4), layer = new Float32Array(N * 4);
  cases.forEach((c, i) => { back.set(c.Bp, i * 4); layer.set(c.Sp, i * 4); });
  const results = [];
  for (let m = 0; m < MODES.length; m++) {
    if (m === 1) { results.push({ mode: MODES[m], maxErr: null, note: 'aleatório por definição: verificado à parte' }); continue; }
    const opacity = 0.8;
    const px = run(m, W, H, floatRT ? back : Uint8Array.from(back, v => Math.round(v * 255)), floatRT ? layer : Uint8Array.from(layer, v => Math.round(v * 255)), opacity, floatRT);
    let maxErr = 0, worst = null;
    cases.forEach((c, i) => {
      const e = ref(m, c.Bp, c.Sp, opacity);
      // Mistura sólida tem degrau em b + s = 1: no empate exato float32 (GPU) e double (JS)
      // podem cair em lados diferentes. Esses casos são indefinidos e ficam de fora.
      if (m === 18) {
        const un = q => q[3] > 0 ? [0, 1, 2].map(j => q[j] / q[3]) : [0, 0, 0];
        const cb = un(c.Bp), cs = un(c.Sp);
        if (cb.some((b, j) => Math.abs(b + cs[j] - 1) < 1e-5)) return;
      }
      for (let k = 0; k < 4; k++) {
        const g = floatRT ? px[i * 4 + k] : px[i * 4 + k] / 255;
        const d = Math.abs(g - e[k]); if (d > maxErr) { maxErr = d; worst = { i, k, g, e: e[k] }; }
      }
    });
    results.push({ mode: MODES[m], maxErr, worst });
  }
  // Dissolver: com opacidade 0,5, ~50% dos pixels devem ser a camada pura
  const DW = 128, dissB = new Float32Array(DW * DW * 4), dissL = new Float32Array(DW * DW * 4);
  for (let i = 0; i < DW * DW; i++) { dissB.set([0, 0, 0, 1], i * 4); dissL.set([1, 1, 1, 1], i * 4); }
  const dpx = run(1, DW, DW, floatRT ? dissB : Uint8Array.from(dissB, v => v * 255), floatRT ? dissL : Uint8Array.from(dissL, v => v * 255), 0.5, floatRT);
  let on = 0, bad = 0; for (let i = 0; i < DW * DW; i++) { const v = floatRT ? dpx[i * 4] : dpx[i * 4] / 255; if (v > .99) on++; else if (v > .01) bad++; }
  results[1].maxErr = bad ? 1 : 0; results[1].note = `fração visível ${(on / (DW * DW)).toFixed(3)} (esperado ≈ 0,5), pixels parciais: ${bad}`;

  // ---------- 2) prancha visual (8 bits, como na tela) ----------
  const TW = 160, TH = 110, cols = 6, rows = Math.ceil(MODES.length / cols);
  const bimg = new Uint8Array(TW * TH * 4), limg = new Uint8Array(TW * TH * 4);
  for (let y = 0; y < TH; y++) for (let x = 0; x < TW; x++) {
    const i = (y * TW + x) * 4, u = x / (TW - 1), v = y / (TH - 1);
    // fundo: céu de fim de tarde em degradê vertical + faixa de cinza na base
    const sky = [0.17 + 0.75 * v, 0.23 + 0.35 * v, 0.48 - 0.1 * v];
    const g = v > 0.82 ? [u, u, u] : sky;
    bimg.set([...g.map(c => Math.round(Math.min(1, c) * 255)), 255], i);
    // camada: roda de matiz na horizontal, do escuro ao claro na vertical
    const h = u * 6, f = h - Math.floor(h), q = [[1, f, 0], [1 - f, 1, 0], [0, 1, f], [0, 1 - f, 1], [f, 0, 1], [1, 0, 1 - f]][Math.floor(h) % 6];
    const k = 1 - Math.abs(v * 2 - 1), base = v < .5 ? 0 : (v - .5) * 2;
    limg.set([...q.map(c => Math.round((base + (c - base) * k) * 255 * 0.999)), 255], i);
  }
  const chart = document.createElement('canvas'); chart.width = cols * (TW + 12) + 12; chart.height = rows * (TH + 34) + 12;
  const cx = chart.getContext('2d'); cx.fillStyle = '#1E1E1E'; cx.fillRect(0, 0, chart.width, chart.height);
  cx.font = '12px sans-serif'; cx.textBaseline = 'top';
  for (let m = 0; m < MODES.length; m++) {
    const px = run(m, TW, TH, bimg, limg, 1, false);
    const id = cx.createImageData(TW, TH);
    id.data.set(px); // a linha 0 da textura é a linha de cima da imagem
    const ox = 12 + (m % cols) * (TW + 12), oy = 12 + Math.floor(m / cols) * (TH + 34);
    cx.putImageData(id, ox, oy);
    cx.fillStyle = '#E6E6E6'; cx.fillText(`${m} · ${MODES[m]}`, ox, oy + TH + 6);
  }
  return { floatRT, renderer: gl.getParameter(gl.SHADING_LANGUAGE_VERSION), results, png: chart.toDataURL('image/png') };
}

(async () => {
  const browser = await chromium.launch({ args: ['--use-gl=swiftshader', '--enable-unsafe-swiftshader', '--ignore-gpu-blocklist'] });
  const page = await browser.newPage();
  await page.setContent('<!doctype html><title>blend test</title>');
  const r = await page.evaluate(inPage, { VERT, FRAG, MODES });
  await browser.close();
  if (r.error) { console.error('FALHOU:', r.error); process.exit(1); }
  console.log(`GLSL: ${r.renderer} · alvo float: ${r.floatRT}`);
  const tol = r.floatRT ? 1e-4 : 1.5 / 255;
  let fail = 0;
  for (const x of r.results) {
    const ok = x.maxErr !== null && x.maxErr <= tol;
    if (!ok) fail++;
    console.log(`${ok ? 'ok  ' : 'ERRO'} ${String(MODES.indexOf(x.mode)).padStart(2)} ${x.mode.padEnd(34)} erro máx ${x.maxErr === null ? '-' : x.maxErr.toExponential(2)}${x.note ? '  ' + x.note : ''}${!ok && x.worst ? '  ' + JSON.stringify(x.worst) : ''}`);
  }
  fs.writeFileSync(OUT_PNG, Buffer.from(r.png.split(',')[1], 'base64'));
  console.log(`prancha: ${OUT_PNG}`);
  console.log(fail ? `${fail} modo(s) com erro` : 'TODOS OS 27 MODOS OK');
  process.exit(fail ? 1 : 0);
})();
