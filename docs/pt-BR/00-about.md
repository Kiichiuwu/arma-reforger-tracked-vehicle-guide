> 🇧🇷 Português · [🇬🇧 English](../en/00-about.md)

[Índice](../../README.pt-BR.md) · [Visão geral →](01-overview.md)

# Sobre este guia

**Finalidade.** Este guia documenta o recurso de veículo de esteira nativo do Arma Reforger (`TrackedVehicle_Base` e `VehicleTrackedSimulation`). Ele ensina a usá-lo em mods futuros, do modelo 3D até um tanque que anda, com rodas e elos de esteira em movimento.

**Escopo.** Só o veículo de esteira nativo: a simulação, o controlador, a suspensão, as rodas, as esteiras e seus elos, a animação deles, o ajuste, a telemetria e a depuração. Armas, blindagem e tripulação ficam de fora.

**Como foi feito.** Tudo aqui foi descoberto e testado durante a construção de um mod de Leopard 2, só com o jogo base. O trabalho teve duas etapas:

| Edição | Modelo | O que foi descoberto |
| --- | --- | --- |
| Primeira | Leopard 2A7 (versão anterior) | Transmissão, controlador, o bug da ré e o script dele, velocidade máxima, os primeiros travamentos |
| Esta | Leopard 2A6 | Suspensão e altura do casco, centro de massa, rodas de apoio e roletes de retorno, o bug de mapeamento do `CarProcAnimComponent`, a corrente de elos nativa, o trem de rolamento e a corrente de elos por script, a sobreposição de depuração nativa |

Os valores citados são os do Leopard 2A6, a não ser que o texto diga "medido na versão anterior, do 2A7".

**Como cada fato é conhecido.** O guia marca como sabe cada fato que importa:

| Rótulo | Significado |
| --- | --- |
| Medido | Lido no modo Play do Workbench pela telemetria (linhas `LEOTEL`, `LEOSUSP` e `LEOGEAR` no `console.log`) ou pela sobreposição de depuração nativa |
| Engenharia reversa | Lido no executável Diag do Workbench 1.8.0.13: textos de atributos, mensagens de erro, desmontagem |
| Observado em vídeo | Visto num trecho de teste gravado. A legenda ou o texto dá o vídeo e o tempo |
| Não testado / Em aberto | Uma expectativa fundamentada ou uma pergunta em aberto. Não conte com ela |

Muitas descobertas foram conferidas dos dois jeitos, no jogo e no executável, e as principais passaram por uma segunda análise independente.

**Exemplo de referência.** O prefab do casco do Leopard 2A6 (`Leopard2A6_base.et` no mod) é o exemplo em todo o guia. O bloco de simulação e as sobrescritas de componentes do casco estão em [examples/leopard2a6](../../examples/leopard2a6/README.md). Os scripts que ele usa estão em [scripts/](../../scripts/), e a ferramenta que gerou o seu `TrackPositions` é a [tools/trackpositions.py](../../tools/trackpositions.py). Os modelos e texturas do mod não estão neste repositório.

**Modelo 3D usado.** ["Leopard 2 A6"](https://sketchfab.com/3d-models/leopard-2-a6-7cb23d5322df4b409a880de635826067), de [buh-late](https://sketchfab.com/buh-late), licenciado sob [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/deed.pt_BR), publicado no Sketchfab em 17/01/2024. Ele foi modificado para a Enfusion: separado em peças, com hierarquia nova, ossos e rig de suspensão, colisores, LODs e texturas reempacotadas. O autor não endossa este projeto. A lista completa de alterações está em [CREDITS.md](../../CREDITS.md).

A CC BY 4.0 pede isto de quem compartilha o modelo ou uma versão modificada dele, inclusive a do mod:

- dar crédito ao autor;
- indicar a licença com um link;
- dizer que houve alterações;
- não sugerir que o autor endossa você ou o seu uso.

Ela não tem cláusula de compartilhamento pela mesma licença (ShareAlike). O texto e as imagens do guia continuam sob CC BY-SA 4.0 ([LICENSE-docs](../../LICENSE-docs)). O modelo que aparece nas imagens continua sendo obra de buh-late, sob CC BY 4.0.

**Edição anterior.** A primeira edição deste guia usou um Leopard 2A7 feito a partir de "Leopard 2 A7", de king_st0ne (CC BY-SA 4.0). O texto, as imagens e o bloco de prefab dela ficam no histórico do git, na tag [`v1-leopard2a7`](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/tree/v1-leopard2a7). Os capítulos 00-12 mantêm os nomes de arquivo, então links antigos continuam funcionando. O apêndice antigo, `13-appendix.md`, agora é o [17-appendix.md](17-appendix.md). Onde uma medida antiga do 2A7 ainda é a única evidência, como a velocidade máxima de 74,3 km/h e a tabela do Δ do bug da ré, esta edição diz que ela foi medida na versão anterior, do 2A7.

**Versão testada.** Workbench e Arma Reforger 1.8.0.13, em outubro de 2026. Os testes do Leopard 2A6 foram de 04/10/2026 a 06/10/2026, na versão Diag do Workbench (`ArmaReforgerWorkbenchSteamDiag.exe`). As entradas nativas de depuração do veículo ("Show vehicle debug", "Draw tracks" e "Reset tracks" da corda, a caixa de estatísticas por lagarta) só existem nas versões Diag (o Workbench e o `ArmaReforgerSteamDiag.exe`); o executável do jogo de varejo (`ArmaReforgerSteam.exe`) não as tem (busca de textos nos executáveis 1.8.0.13). Atualizações do jogo podem mudar nomes, padrões e bugs.

---

[Índice](../../README.pt-BR.md) · [Visão geral →](01-overview.md)
