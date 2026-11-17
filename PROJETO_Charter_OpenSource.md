# Project Charter — Editor de Imagens Open-Source (inspirado no Photoshop Touch)

> Documento de intenção. **Nada é publicado ainda.** Guardado para a fase final, quando
> criaremos o repositório público. Registra nome, requisitos, modelo legal e estrutura.

---

## 1. Visão

Um editor de imagens **para toque/mobile**, **open-source**, **64-bit nativo**, moderno e
bonito — *inspirado* na arquitetura e nas ideias do Photoshop Touch (que estudamos por
engenharia reversa), mas **escrito do zero**, como o PhotoCraft fez com o Photoshop desktop.
Disponível para download, fácil de instalar, para quem quiser.

## 2. Requisitos não-negociáveis

1. **ARM64 (arm64-v8a) obrigatório** — roda em aparelhos atuais (que removeram o 32-bit).
   `armeabi-v7a` é opcional/legado.
2. **GLES 3.0+ (com caminho 3.1 compute)** — remove as limitações do GLES2 original.
3. **Clean-room / 100% original** — ver seção 4. É o que torna a publicação legal.
4. **Licença permissiva** — `MIT OR Apache-2.0` (dual), igual ao PhotoCraft.

## 3. Nome (a definir — ver proposta)

Candidatos (todos originais, sem marca de terceiros):
- **Telar** *(recomendado)* — de "tela" (canvas) + eco de *atelier*; curto, brandável.
- **Retoque** — português direto para "retouch".
- **Pixora** — coined, pixel + aura.
- **Camada** — "layer" em PT, elegante.

> Decisão do dono do projeto. Placeholder no código: `TELAR` até confirmar.

## 4. Modelo legal — os DOIS TRILHOS (crítico)

O PhotoCraft é open-source porque é **clean-room**: nenhum código/asset da Adobe. Para
publicarmos da mesma forma, seguimos o mesmo princípio.

### Trilho A — Pesquisa (PRIVADO, nunca publicado)
Material derivado da Adobe, usado só como **referência de interoperabilidade/estudo**:
- APK original, `TTPixel.swf`, bytecode AS3 desmontado
- `.so` nativas e sua decompilação (Ghidra)
- shaders `.fs`, ícones e temas originais da Adobe
- qualquer "port" do APK original (serve para **uso pessoal**, não para distribuição)

➜ Fica fora de qualquer repositório público. No repo atual (privado) já está isolado e o
`.gitignore` exclui os binários grandes.

### Trilho B — Projeto clean-room (PÚBLICO)
Tudo **original nosso**:
- código-fonte novo (engine, UI, ferramentas)
- assets novos (ícones, tema, cursores) — desenhados por nós ou de bibliotecas livres
- **conhecimento** levado do Trilho A: a *arquitetura* (camadas, processors, pipeline de
  filtro), e **matemática de domínio público** (fórmulas de blend/ajuste) — reimplementada
  em código novo. Nunca copiar código/asset da Adobe.

**Regra de ouro:** se um arquivo foi extraído/derivado do APK da Adobe, ele é Trilho A.
Se foi escrito/desenhado por nós do zero, é Trilho B (publicável).

### Sobre referências externas
- **PhotoCraft** (`storytold/photocraft`): MIT/Apache. Podemos **adaptar algoritmos com
  atribuição** no `NOTICE`. Não copiar trechos grandes verbatim; reimplementar em GLSL/stack
  nosso.
- Fórmulas de blend modes e ajustes (Multiply, Screen, Overlay, Curves…) são **domínio
  público** (matemática), não copyrightáveis.

## 5. Stack do projeto público (a decidir na fase final)

Como é clean-room e 64-bit nativo, **não** ressuscitamos o Adobe AIR. Opções de stack,
todas ARM64-nativas:
- **Rust + wgpu/GLES3** (trilha PhotoCraft; portátil, moderno)
- **Kotlin + C++ (NDK) + GLES3/Vulkan** (Android-first, nativo)
- **Flutter + Impeller / plugin GLES** (UI moderna rápida, multiplataforma)

Decisão fica para quando formos começar o Trilho B de fato. A RE que fizemos serve de
**blueprint** em qualquer um deles.

## 6. O que levamos da RE para o projeto público (blueprint)

- **Arquitetura de 3 camadas** e o padrão *processor* por filtro (base → shader → UI gerada
  por descritor). ← ótimo design, reimplementável.
- **Pipeline de filtro GPU**: shader + parâmetros declarativos → UI automática. Vamos
  reescrever os ~47 efeitos como shaders GLES3 **novos** (mesma matemática pública).
- **Modelo de documento**: camadas, blend modes, máscara/quick-mask, histórico por comando.
- **Layout declarativo** (ideia do `.cabmod`/tema XML) → nosso próprio sistema de tema/UI,
  já moderno desde o início.
- **Ferramentas** (do blueprint + referência PhotoCraft): blend modes completos, curves,
  levels, hue/sat, selective color, seleção inteligente.

## 7. Estrutura prevista do repositório público (rascunho)

```
telar/                      (nome a confirmar)
├─ LICENSE-MIT  LICENSE-APACHE  NOTICE   (atribuições)
├─ README.md    ROADMAP.md
├─ app/            # UI + shell (stack escolhido)
├─ engine/         # núcleo de imagem (camadas, compor, histórico)
├─ filters/        # shaders GLES3 novos + descritores
├─ tools/          # pincel, seleção, transform…
├─ assets/         # ícones/tema ORIGINAIS
└─ docs/           # arquitetura, contribuição
```

## 8. Estado atual / pendências para a fase de publicação

- [ ] Confirmar **nome**.
- [ ] Escolher **stack** do Trilho B.
- [ ] Criar os **assets originais** (ícones/tema) — nada da Adobe.
- [ ] Reescrever os filtros como **shaders GLES3 novos**.
- [ ] Só então: criar repositório público + README/LICENSE/ROADMAP + releases para download.

> Até lá, seguimos no Trilho A (estudo/port privado) e **preparando** peças originais do
> Trilho B que já forem seguras (shaders novos, design de UI, arquitetura).
