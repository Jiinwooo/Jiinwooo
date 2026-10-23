# Downloads de FTS15 e logos

## FTS15

- [teams.pak atualizado](fts15/teams.pak)
- [ZIP do teams.pak com relatório e instruções](fts15/FTS15_teams_logos_atualizadas.zip)
- [Relatório de todas as entradas](fts15/relatorio.csv)
- [Amostra antes e depois](fts15/amostra-antes-depois.jpg)

Foram atualizadas **387 logos principais e suas miniaturas existentes**, totalizando 740 imagens. Outras 113 logos principais foram preservadas por serem históricas, genéricas, sem identificação segura ou sem correspondência no catálogo. Não é uma atualização integral.

Faça backup do arquivo instalado, feche o jogo e substitua apenas o `teams.pak` da mesma versão pelo arquivo baixado. O arquivo foi validado estruturalmente e suas 882 imagens foram verificadas, mas ainda precisa ser testado dentro do FTS15.

## Todas as logos do site

O pacote contém **5.128 logos e variantes em PNG**, organizadas por país, baixadas de https://football-logos.cc/ em 23/10/2026. Inclui o catálogo e o manifesto das imagens.

O ZIP completo tem 288.451.578 bytes e foi dividido em três partes para respeitar o limite do GitHub. Baixe **as três partes** para a mesma pasta:

1. [logos-times-football-logos.zip.001](logos/logos-times-football-logos.zip.001)
2. [logos-times-football-logos.zip.002](logos/logos-times-football-logos.zip.002)
3. [logos-times-football-logos.zip.003](logos/logos-times-football-logos.zip.003)

Na página de cada arquivo no GitHub, use **Download raw file** (ícone de download). Não salve a página HTML.

Com 7-Zip ou um descompactador compatível com arquivos divididos, abra o `.001` para extrair; as outras duas partes precisam estar na mesma pasta.

Se seu aplicativo não reconhecer as partes, recomponha o ZIP primeiro:

Windows (Prompt de Comando):

```bat
copy /b logos-times-football-logos.zip.001+logos-times-football-logos.zip.002+logos-times-football-logos.zip.003 logos-times-football-logos.zip
```

Linux/macOS:

```sh
cat logos-times-football-logos.zip.001 logos-times-football-logos.zip.002 logos-times-football-logos.zip.003 > logos-times-football-logos.zip
```

Depois, extraia `logos-times-football-logos.zip` normalmente. [Checksums SHA-256](logos/checksums.json) e [catálogo](logos/catalogo.csv).
