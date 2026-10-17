# Arma Reforger — Guia da esteira nativa

> [🇬🇧 English](README.md) · 🇧🇷 Português

![Leopard 2A6 andando na pista com a sobreposição de depuração nativa do veículo: círculos das rodas, corda da corrente e estatísticas por lagarta desenhados sobre as rodas e os elos movidos por script](docs/images/hero-debug-side.gif)

*Leopard 2A6 na simulação de esteira nativa, lado esquerdo, acelerando em 2ª de cerca de 11 a 21 km/h (vídeo de teste A, 06/10/2026, 3:06–3:11). A sobreposição de depuração nativa está ligada: os círculos brancos são as rodas da simulação no raio da simulação, a linha vermelha e verde é a corda da corrente nativa, e as caixas são as estatísticas dela por lagarta. As rodas e os elos que você vê são movidos por script. Modelo "Leopard 2 A6" de buh-late, CC BY 4.0, modificado. Mais em [Exemplos em vídeo](docs/pt-BR/05-video-examples.md).*

Um guia completo e testado do recurso de **veículo de esteira nativo** do Arma Reforger
(`TrackedVehicle_Base` e `VehicleTrackedSimulation`): o que é, como funciona por dentro
e como colocá-lo no seu próprio tanque, passo a passo.

Tudo aqui foi descoberto e testado durante a construção de um mod de Leopard 2 só com o
jogo base. O exemplo prático agora é um **Leopard 2A6**. Os primeiros testes de transmissão e
de ré rodaram numa versão anterior, um Leopard 2A7; o guia avisa sempre que um número ainda vem dela.

Esta edição acrescenta **física da esteira, suspensão e mais**: a suspensão nativa e como acertar
a altura do casco, as rodas de apoio, a roda-guia, a roda motriz e os roletes de retorno, a corrente
de elos nativa e por que ela falha, um trem de rolamento e uma corrente de elos por script que
funcionam, e a sobreposição de depuração nativa do veículo. Cada fato está marcado como medido,
obtido por engenharia reversa, observado em vídeo ou não testado.

**Testado em:** Arma Reforger / Enfusion Workbench **1.8.0.13** (outubro de 2026).

## Mod no Workshop e download do modelo

| | |
| --- | --- |
| **Mod no Workshop** | [Leopard 2A6 - Tracked Base](https://reforger.armaplatform.com/workshop/7E4ABBD41FEDAC42), versão 0.9.1. Um prefab base para você montar os seus próprios tanques. Multiplayer: veja o [capítulo 18](docs/pt-BR/18-multiplayer.md) |
| **GUID do mod** | `7E4ABBD41FEDAC42`. Acrescente em `Dependencies` no seu `addon.gproj` para herdar os prefabs e os scripts do Leopard no seu mod |
| **Licença do mod** | [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/deed.pt_BR): use, modifique e republique à vontade, só mantenha os créditos |

**Modelo 3D:** [⬇ Baixar o modelo modificado (FBX + texturas, 32 MB)](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/releases/download/leopard2a6-model-v1/Leopard2A6_Enfusion_model_v1.zip) · [Modelo original no Sketchfab](https://sketchfab.com/3d-models/leopard-2-a6-7cb23d5322df4b409a880de635826067)

O modelo modificado é o que o mod usa: dividido em casco, torre, periscópio e elo da esteira, com os ossos do trem de rolamento e o rig de suspensão, colisores `UCX_`, o ponto `COM_hull`, LODs e as texturas reorganizadas para o Enfusion. Os dois são CC BY 4.0 do buh-late; os detalhes estão [na release](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/releases/tag/leopard2a6-model-v1).

## Por que existe

O jogo base traz uma simulação de esteira, mas nenhum veículo vanilla a usa e a
configuração padrão nem carrega. Os mods de tanque publicados imitam a esteira com a
simulação de rodas. Este guia documenta o sistema de verdade, com os bugs e os contornos,
para o próximo modder não ter que fazer engenharia reversa de novo.

## O que funciona

| Recurso | Situação |
| --- | --- |
| Andar para a frente, marchas automáticas | ✅ Funciona. Velocidade máxima não medida de novo no 2A6: a teoria dá 72–75 km/h em 4ª com redução final 4,6. A versão anterior, do 2A7, mediu 74,3 km/h |
| Virar andando | ✅ Funciona |
| Girar no lugar | ✅ Funciona. No giro, a lagarta que anda para trás costuma girar mais rápido, então o casco recua devagar, até cerca de 4 km/h (observado em vídeo); causa em aberto |
| Ré | ⚠️ A ré nativa está quebrada na 1.8.0.13 (no 2A6 o motor trava em 3766 rpm). Resolvida com um script pequeno ([por quê](docs/pt-BR/09-bugs.md)) |
| Suspensão | ✅ Funciona de forma nativa. Use `MaxTravelDown ≈ m·g/(n·k)` para o casco ficar na altura do modelo ([capítulo 13](docs/pt-BR/13-suspension.md)) |
| Roletes de retorno | ✅ Funcionam (`Type "Return Roller"`) |
| Animação das rodas de apoio, roda-guia, roda motriz e roletes | ✅ Por script (`LEO_RunningGearComponent`). O `CarProcAnimComponent` ❌ entrega às rodas de esteira os sinais de outra roda na 1.8.0.13 ([capítulo 14](docs/pt-BR/14-running-gear.md)) |
| Elos da esteira | Corda da corrente nativa ⚠️: ela pende (não colide com o chão), sai da roda motriz depois de capotar ou de uma explosão, e não tem API para reiniciar. Corrente por script ✅ (`LEO_TrackChain`): por construção não sai do lugar, mas ainda não foi testada capotando nem com mina ([capítulo 15](docs/pt-BR/15-track-links.md)) |
| Sobreposição de depuração nativa do veículo | ✅ Rodou num teste de 5 minutos com uma corrente de elos presente. Sem corrente ela derrubou o Workbench em 02/10/2026, provavelmente pela falta da corrente ([capítulo 16](docs/pt-BR/16-debugging.md)) |
| Multiplayer | ⚠️ Em parte. Servidor dedicado: giro no lugar / direção neutra e ré funcionam com o `LEO_TrackedReverseComponent` v5 (o motor descarta o freio de mão do move de rede; o script repõe). **O motorista num cliente** fica com a cabeça congelada, olhar lento e comandos presos: uma limitação do motor no caminho do cliente dono de esteira, sem correção por script. Outros assentos, hosts e singleplayer funcionam ([capítulo 18](docs/pt-BR/18-multiplayer.md)) |

## O que há de novo nesta edição

- **Multiplayer (17/10/2026).** Um capítulo novo, [Multiplayer](docs/pt-BR/18-multiplayer.md): como o
  veículo de esteira é replicado, por que um servidor dedicado nunca gira no lugar (o freio de mão é
  descartado do move de rede), o script de ré v5 que corrige isso, a limitação do motorista em cliente
  que ele não corrige, e o Peer Tool do Workbench.
- **Aderência.** `FrictionCurveLongitudinal/Lateral` nunca são lidas no 1.8.0.13; os atributos de
  atrito V2 são, e a curva de posição vanilla deixa a roda de apoio traseira sem aderência lateral.
  Reajustado, com o centro de massa mais baixo ([capítulo 10](docs/pt-BR/10-tuning.md)).
- **Exemplo novo.** Um Leopard 2A6 feito a partir do modelo de buh-late (CC BY 4.0). A primeira
  edição (Leopard 2A7, modelo de king_st0ne) fica no histórico do git, na tag
  [`v1-leopard2a7`](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/tree/v1-leopard2a7).
- **Quatro capítulos novos.** [Suspensão e altura do casco](docs/pt-BR/13-suspension.md),
  [Rodas de apoio, roda-guia, roda motriz e roletes](docs/pt-BR/14-running-gear.md),
  [Elos da esteira](docs/pt-BR/15-track-links.md) e
  [Diagnóstico do veículo e telemetria](docs/pt-BR/16-debugging.md). O apêndice foi para
  [`17-appendix.md`](docs/pt-BR/17-appendix.md).
- **Suspensão, medida.** `SpringRate` está em kN/m, a mola fica livre no curso todo para baixo, e o
  afundamento estático é `m·g/(n·k)`. Daí vem a regra do `MaxTravelDown` para a altura do casco.
- **Trem de rolamento.** Os roletes de retorno funcionam. As duas primeiras peças de cada lagarta
  precisam ser rodas de apoio, então liste as rodas de apoio primeiro. A API de script lê cada roda
  por (lagarta, peça). O `CarProcAnimComponent` troca as rodas de esteira entre si, então um script
  move os ossos no lugar dele.
- **Elos da esteira.** A corrente de elos nativa, por engenharia reversa: uma corda 2D com
  gravidade, contato com as rodas e tração pela roda motriz, mas sem colisão com o chão. Esta edição
  traz um gerador offline de `TrackPositions` e uma corrente por script que substitui a corda no
  visual.
- **Depuração.** A sobreposição de depuração nativa do veículo (círculos das rodas, corda, painel do
  motor) e a telemetria de suspensão `LEOSUSP`.
- **Correções.** `CenterOfMass` é um deslocamento a partir do centro de massa do próprio modelo, não
  um ponto perto do assoalho. O travamento da sobreposição de depuração provavelmente vinha da falta
  da corrente de elos: com corrente, ela rodou num teste de 5 minutos sem travar. O script de ré
  agora também desliga o motor quando o motorista sai.
- **Mídia nova.** GIFs e fotos dos vídeos de teste do 2A6 com a sobreposição de depuração ligada, e
  trechos de antes das correções ([Exemplos em vídeo](docs/pt-BR/05-video-examples.md)).

## Conteúdo

| # | Capítulo |
| --- | --- |
| 0 | [Sobre este guia](docs/pt-BR/00-about.md) |
| 1 | [Visão geral](docs/pt-BR/01-overview.md) |
| 2 | [Arquitetura](docs/pt-BR/02-architecture.md) |
| 3 | [Requisitos do modelo 3D](docs/pt-BR/03-model-requirements.md) |
| 4 | [Passo a passo do zero](docs/pt-BR/04-from-scratch.md) |
| 5 | [Exemplos em vídeo](docs/pt-BR/05-video-examples.md) |
| 6 | [Convertendo um veículo existente](docs/pt-BR/06-converting.md) |
| 7 | [Referência de atributos](docs/pt-BR/07-attribute-reference.md) |
| 8 | [Como funciona por dentro](docs/pt-BR/08-internals.md) |
| 9 | [Bugs e limitações (1.8.0.13)](docs/pt-BR/09-bugs.md) |
| 10 | [Ajuste com dados reais](docs/pt-BR/10-tuning.md) |
| 11 | [Scripts prontos](docs/pt-BR/11-scripts.md) |
| 12 | [Solução de problemas](docs/pt-BR/12-troubleshooting.md) |
| 13 | [Suspensão e altura do casco](docs/pt-BR/13-suspension.md) *(novo)* |
| 14 | [Rodas de apoio, roda-guia, roda motriz e roletes](docs/pt-BR/14-running-gear.md) *(novo)* |
| 15 | [Elos da esteira: a corda nativa e a corrente por script](docs/pt-BR/15-track-links.md) *(novo)* |
| 16 | [Diagnóstico do veículo e telemetria](docs/pt-BR/16-debugging.md) *(novo)* |
| 17 | [Apêndice](docs/pt-BR/17-appendix.md) |
| 18 | [Multiplayer](docs/pt-BR/18-multiplayer.md) *(novo)* |

## Início rápido

1. Faça o prefab do casco herdar de `{0608D8FA71FD3433}Prefabs/Vehicles/Core/TrackedVehicle_Base.et`.
2. Preencha o bloco `Simulation Tracked`: curvas de atrito, modelos de roda (`RoadWheel`, `Idler`,
   `Sprocket`, `Roller`) e exatamente duas lagartas. Em cada lagarta, liste primeiro as rodas de
   apoio (da frente para trás), depois a roda-guia, a roda motriz e os roletes de retorno, na mesma
   ordem dos dois lados ([capítulo 4](docs/pt-BR/04-from-scratch.md); um bloco completo e funcionando
   está em [examples/leopard2a6](examples/leopard2a6/VehicleTrackedSimulation_Leopard2A6.et.txt)).
3. Dimensione a suspensão para o casco ficar na altura do modelo: `MaxTravelDown ≈ m·g/(n·k)`,
   com `n` rodas de apoio e `k` = `SpringRate` × 1000 N/m. Leopard: 62300 × 9,81 / (14 × 450000)
   = 0,097 m, então `MaxTravelDown 0.10` ([capítulo 13](docs/pt-BR/13-suspension.md)).
4. Adicione o [`LEO_TrackedReverseComponent`](scripts/LEO_TrackedReverseComponent.c) para a ré
   andar de verdade, e defina `ThrottleReverseTarget 1` no controlador.
5. Para rodas e elos em movimento: monte os ossos das rodas, deixe o `CarProcAnimComponent` vazio e
   adicione o [`LEO_RunningGearComponent`](scripts/LEO_RunningGearComponent.c) (ele precisa do
   [`LEO_TrackChain.c`](scripts/LEO_TrackChain.c)) e o
   [`LEO_TrackBeltComponent`](scripts/LEO_TrackBeltComponent.c)
   ([capítulo 14](docs/pt-BR/14-running-gear.md)).
6. Dê também uma corrente de elos à simulação: `TrackSegment` (o seu modelo de elo), `TrackPositions`
   (gerado por [`tools/trackpositions.py`](tools/trackpositions.py)), `TrackLength` e
   `TrackOffset1/2`. A sobreposição de depuração provavelmente precisa dela (sem ela travou), e o
   script do trem de rolamento só foi testado com uma corrente presente
   ([capítulo 15](docs/pt-BR/15-track-links.md)).
7. Depois de cada edição do prefab, feche o mundo, recarregue os scripts sem mundo aberto e abra o
   mundo de novo. Um veículo num mundo que já está aberto mantém os valores antigos da simulação
   ([capítulo 13](docs/pt-BR/13-suspension.md#edições-do-prefab-e-o-mundo-aberto)).

## Organização do repositório

```
docs/en/              Capítulos em inglês (00-17)
docs/pt-BR/           Capítulos em português (mesmos nomes de arquivo)
docs/images/          GIFs e fotos dos vídeos de teste do Leopard 2A6
scripts/              Componentes em Enforce Script (veja a tabela abaixo)
tools/                trackpositions.py: gerador offline de TrackPositions
examples/leopard2a6/  Blocos de prefab do Leopard 2A6: simulação com corrente de elos, componentes do casco
```

| Arquivo | O que faz |
| --- | --- |
| [`scripts/LEO_TrackedReverseComponent.c`](scripts/LEO_TrackedReverseComponent.c) | Ré por script (v4); desliga o motor quando o banco do motorista fica vazio (`m_bStopEngineOnExit`) |
| [`scripts/LEO_TelemetryComponent.c`](scripts/LEO_TelemetryComponent.c) | Linhas `LEOTEL` da transmissão e `LEOSUSP` da suspensão no `console.log` |
| [`scripts/LEO_RunningGearComponent.c`](scripts/LEO_RunningGearComponent.c) | Curso e giro das rodas de apoio, giro da roda-guia, da roda motriz e dos roletes; cuida das correntes de elos por script e esconde os elos da corda nativa |
| [`scripts/LEO_TrackChain.c`](scripts/LEO_TrackChain.c) | A corrente de elos por script de uma lagarta (classe usada pelo `LEO_RunningGearComponent`) |
| [`scripts/LEO_TrackBeltComponent.c`](scripts/LEO_TrackBeltComponent.c) | Esconde as malhas estáticas das esteiras do casco quando já existem entidades de elo |
| [`tools/trackpositions.py`](tools/trackpositions.py) | Gera `TrackPositions` a partir dos círculos das rodas (envoltória convexa, reamostrada pelo comprimento do arco) |

## Créditos

- **Modelo 3D:** ["Leopard 2 A6"](https://sketchfab.com/3d-models/leopard-2-a6-7cb23d5322df4b409a880de635826067)
  de [buh-late](https://sketchfab.com/buh-late), licenciado sob
  [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/deed.pt_BR), publicado em 17/01/2024.
  Modificado para a Enfusion (separado em peças, hierarquia nova, ossos e rig de suspensão,
  colisores, LODs, texturas reempacotadas). O autor não endossa este projeto. Veja [CREDITS.md](CREDITS.md).
- **Modelo do exemplo anterior** (primeira edição, só na tag `v1-leopard2a7`): "Leopard 2 A7" de
  king_st0ne, licenciado sob [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.pt_BR).
- **Guia, pesquisa e scripts:** Kiichi.

## Licença

| O quê | Licença |
| --- | --- |
| Documentação e imagens (`docs/`, READMEs, `CREDITS.md`) | [CC BY-SA 4.0](LICENSE-docs) |
| Código (`scripts/`, `tools/`, blocos de prefab e trechos de código em `examples/`) | [MIT](LICENSE) |
| Modelo 3D do Leopard 2A6, que aparece nas imagens e é usado pelo mod de exemplo | [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/deed.pt_BR), crédito a buh-late, modificado |
| Mod no Workshop e o download do modelo modificado | [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/deed.pt_BR), crédito a buh-late (modelo) e Kiichi |
| Modelo 3D do Leopard 2A7 que aparece nas imagens da primeira edição (só na tag `v1-leopard2a7`) | [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.pt_BR), crédito a king_st0ne |

Arma Reforger e Enfusion são marcas da Bohemia Interactive a.s. Este projeto não tem
vínculo com a Bohemia Interactive nem é endossado por ela.

## Contribuindo

Issues e pull requests são bem-vindos, principalmente resultados de versões mais novas do jogo e
testes em multiplayer. Mantenha os capítulos em inglês e em português sincronizados.
