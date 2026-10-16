> 🇧🇷 Português · [🇬🇧 English](../en/05-video-examples.md)

[← Passo a passo do zero](04-from-scratch.md) · [Índice](../../README.pt-BR.md) · [Convertendo um veículo existente →](06-converting.md)

# Exemplos em vídeo

Estes exemplos vêm de dois vídeos de teste do mod de exemplo do **Leopard 2A6**, gravados em 06/10/2026 no Workbench 1.8.0.13 (exe Diag, modo Play). Nos dois, a sobreposição de depuração nativa do veículo está ligada, as rodas de apoio, a roda-guia, a roda motriz e os roletes de retorno são movidos por script, e os elos são a corrente de elos por script. Uma última seção mostra três vídeos de diagnóstico de 05/10/2026, gravados antes dessas correções.

Cada exemplo tem um GIF curto (ou uma foto), o momento exato no vídeo original e os números lidos nos painéis de depuração. Esses números são observados no vídeo, lidos quadro a quadro. Relações como "Feedback = rpm da roda motriz × relação da marcha × 4,6" foram conferidas em vários quadros. "Telemetria" quer dizer as linhas `LEOTEL`/`LEOSUSP`/`LEOREV` no log do Workbench. "Engenharia reversa" quer dizer lido no executável 1.8.0.13.

Toda a mídia mostra o mod de exemplo do Leopard 2A6. O modelo é ["Leopard 2 A6"](https://sketchfab.com/3d-models/leopard-2-a6-7cb23d5322df4b409a880de635826067), de [buh-late](https://sketchfab.com/buh-late), licenciado sob [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/deed.pt_BR), publicado em 17/01/2024. Ele foi modificado para a Enfusion (separado em peças, hierarquia nova, ossos e rig de suspensão, colisores, LODs, texturas refeitas). O autor não endossa este projeto. Créditos completos: [CREDITS.md](../../CREDITS.md).

A edição anterior deste capítulo usou um teste de 2 min 13 s do Leopard 2A7 (modelo "Leopard 2 A7" de king_st0ne, CC BY-SA 4.0). O texto e os GIFs dela ficam no histórico do git, na tag [`v1-leopard2a7`](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/blob/v1-leopard2a7/docs/pt-BR/05-video-examples.md).

## Os vídeos

| Nome | Gravado | Duração | Conteúdo |
| --- | --- | --- | --- |
| Vídeo de teste A | 06/10/2026 | 5:00 | Pista, depois grama, pedras e uma cerca de arame. Depuração do veículo ligada; a sobreposição "Draw tracks" fica desligada a partir de 206,3 s |
| Vídeo de teste B | 06/10/2026 | 1:17 | Recorte de outra gravação da mesma sessão de Play (pista e grama), uma tomada contínua sem cortes. Cobre o mesmo trecho que A 95,5–172,5 s: some cerca de 95,5 s a um tempo de B para achar o mesmo momento em A (casado pelas freadas fortes, giros e arrancadas de ré) |
| Vídeo de diagnóstico 1 | 05/10/2026 22:35 | 4:30 | Rodas pelo `CarProcAnimComponent` e a corda da corrente nativa. Capotamento em 189–194 s |
| Vídeo de diagnóstico 2 | 05/10/2026 22:56 | 1:10 | Rodas pelo `CarProcAnimComponent` com parâmetros reordenados e a corda da corrente nativa |
| Vídeo de diagnóstico 3 | 05/10/2026 23:21 | 0:40 | Rodas por script e a corda da corrente nativa com folga extra. Minas em 22,75–24 s |

Configuração dos vídeos de teste A e B:

| Item | Valor |
| --- | --- |
| Mundo | Subcena de teste do mundo Game Master de Arland (`GM_Arland`): pista do aeródromo, grama ao lado, pedras e uma cerca de arame |
| Prefab | `Leopard2A6_base.et`: `Mass` 62300, `MaxTravelDown` 0.10, 73 elos com `TrackLength` 0.2110, `CarProcAnimComponent` vazio ([blocos do prefab](../../examples/leopard2a6/README.md)) |
| Rodas de apoio, roda-guia, roda motriz, roletes | `LEO_RunningGearComponent` ([Rodas de apoio, roda-guia, roda motriz e roletes](14-running-gear.md)) |
| Elos | `LEO_TrackChain`. Os elos da corda nativa ficam escondidos, mas a corda continua rodando, então a sobreposição de depuração ainda a desenha ([Elos da esteira](15-track-links.md)) |
| Ré | Empurrão do `LEO_TrackedReverseComponent`. O log imprime `LEOREV APPLY` em cada arrancada de ré ([Scripts prontos](11-scripts.md)) |
| Opções do diagnóstico | Show vehicle debug, Player vehicle only, Show engine, Show suspension, Wheel engine brake e MT simulation ligados; Show contacts ligado em 41 s ([foto do menu em tamanho cheio](../images/diag-menu-tracked.jpg)) |
| Alinhamento com o log | Horário do log do Workbench = 00:14:14,8 + tempo do vídeo de teste A (conferido com a velocidade e a marcha do HUD) |
| Captura | 1152×720 a cerca de 60 fps; os GIFs aqui têm 480 px de largura a 10 fps |

A câmera em terceira pessoa muitas vezes gira junto com o casco. Curvas e giros então aparecem principalmente como o fundo girando.

<a id="reading-the-overlay"></a>

## Lendo o HUD e os painéis de depuração

![Leopard 2A6 parado, lado direito, com a sobreposição de depuração nativa desenhada sobre as duas lagartas e os painéis Debug e Engine no canto superior esquerdo](../images/debug-overlay-both-tracks.jpg)

*Foto: vídeo de teste A, 30,0 s. Tudo o que a depuração do veículo desenha, sobre as duas lagartas, com o tanque parado. Ilustra: [Diagnóstico do veículo e telemetria](16-debugging.md)*

A sobreposição só existe no exe Diag. Abra o menu de diagnóstico no Play com Win esquerdo + Alt ([detalhes](16-debugging.md)).

**HUD (canto inferior esquerdo).** Velocímetro de 0–120 km/h, conta-giros em rpm × 1000 (0–3) e um medidor de combustível. O velocímetro não tem lado negativo, então leia a velocidade de ré no painel Debug. Em ré, o conta-giros fica no topo da escala por causa do bug da ré nativa ([Arrancada de ré](#reverse-launch)).

**Painel Debug (canto superior esquerdo).**

| Campo | Significado (como se sabe) |
| --- | --- |
| Steering | -1 a +1. +1 vira para a direita: num giro no lugar a lagarta esquerda anda para a frente e Y aumenta (observado) |
| Clutch | 0 = aberta, 1 = acoplada. Na marcha lenta pode ler 1,0 enquanto RPM e Feedback diferem, então nem sempre está travada (observado) |
| Brake | 0 a 1 pela entrada de freio. **6,0** é o valor do freio de mão: `SetBreak(x, true)` guarda 6,0 (engenharia reversa). Aparece em toda parada segurada, em todo giro no lugar e nas freadas fortes (0,0 → 6,0 de uma vez, ou 0,7 → 1,0 → 6,0 em 0,4 s) |
| Throttle | 0,8 na condução normal; 1,0 nos giros, na ré e quando travado. Telemetria anterior liga o 1,0 a acelerador mais turbo (`ThrottleTurbo` 0.2); não conferido nestes vídeos |
| Gearbox Gear | R, N ou 1–4. Parado com o freio segurado, o controlador seleciona R (medido) |
| Speed | Velocidade do casco em kph, negativa em ré |
| Slope | -1 % em todo quadro dos dois vídeos, mesmo com P em 0,10–0,12: não é uma leitura do terreno (observado) |
| G-Force | Aceleração total em g: 1,0 parado, 1,1–1,2 em freada forte |
| Common: Y, P, R | Guinada, arfagem e rolagem, provavelmente em radianos (Y dá a volta em ±π). Y cresce em curvas para a direita; P > 0 é nariz para baixo |
| Inertia | Só aparece com Show inertia: 327602.7, 395692.4, 94730.0 (vídeo de teste A, 46,5 s) |

**Painel Engine (abaixo dele).**

| Campo | Significado (conferido nos quadros lidos) |
| --- | --- |
| Running | yes / no |
| Load | \|Torque\| / `MaxTorque` 4700 como fração de 0–1, apesar do sinal "%" (3409 Nm → 0,73, 4601 Nm → 0,98) |
| Torque | Torque do motor em Nm. Com acelerador 0 ele é negativo (freio-motor) e bate com -470 × (RPM - 700) / (3000 - 700) com até 2 Nm de diferença em todos os quadros lidos menos dois, com limite em -471 Nm (`Friction` 470, `RpmIdle` 700, `RpmMax` 3000) |
| Power | Torque × RPM × 2π/60, em kW (3409 Nm a 2700 rpm = 964 kW) |
| RPM | Rotação do motor |
| Feedback | Rotação do lado da transmissão = rpm da roda motriz × relação da marcha × `Ratio` 4,6 (27,6 em 1ª, 15,18 em 2ª, 8,28 em 3ª, 10,58 em R). É igual ao RPM com a embreagem travada. Num giro neutro é a média de \|rpm das lagartas\| × `NeutralSteeringRatio` 12 |
| Gráfico | Um traço branco de RPM e um traço amarelo |

**Caixa por lagarta (uma por lagarta).** Numa vista de lado, a caixa de baixo é da lagarta mais próxima.

| Linha | Significado |
| --- | --- |
| `Sim: Paused` | Paused em todo quadro lido, mesmo a 27 km/h. O exe tem três textos de estado ao lado desse campo: `Paused`, `Running` e `SubStepping`. Significado em aberto |
| `Substep: 0/25` | Sempre 0/25. O passo da corda roda 25 iterações de restrição por subpasso (engenharia reversa); se este campo mostra essa contagem não está confirmado |
| `Momentum`, `Energy` | Sempre 0.000000. Significado em aberto |
| `AngVel` | Velocidade angular da roda motriz em rad/s, com rpm entre parênteses |
| `Speed` | Velocidade da corrente em m/s. É igual a AngVel × 0,34 (o `Radius` da roda motriz) em todo quadro lido |

**Sobreposição no trem de rolamento** (Tracked > Draw tracks ligado):

- Um círculo branco de 12 lados para cada um dos 13 TrackParts de uma lagarta, desenhado no `Radius` **da simulação**. Os círculos das rodas de apoio (0,418) chegam à face de contato dos elos, abaixo dos aros visíveis (0,345), então os círculos das rodas da frente se sobrepõem. Um raio branco gira com cada roda.
- A corda nativa como uma linha poligonal vermelha e verde com uma marca ciano em cada ponto. O significado das cores não foi estabelecido; a corda mais próxima ficou quase toda verde na freada forte em ré do vídeo de teste B (74,5 s).
- Um número em cada roda motriz. Ele diminui andando para a frente, aumenta em ré e dá a volta entre 72 e 0 (73 pontos). Provavelmente é o índice do ponto da corda preso pela roda motriz (não confirmado).
- Linhas verticais azuis e cruzes vermelhas/verdes nos centros das rodas, desenhadas com Show suspension ligado (as cruzes também aparecem com Show contacts desligado, por exemplo em 62 s e 198 s). Elas continuam quando Draw tracks está desligado.
- A sobreposição da lagarta do outro lado é desenhada através do casco.

As opções Show … ficam na lista de diagnóstico do veículo, ao lado dos submenus Helicopter e Tracked. O submenu Tracked tem só Steering mode, TrackSim, Draw tracks, Track dynamic, Track gravity e Reset tracks. Draw tracks liga e desliga os círculos das rodas, as linhas poligonais da corda e as caixas (vídeo de teste A, 13,5–18 s e 206,3 s). Desligar TrackSim ou Track gravity por um momento não mudou nada visível. Reset tracks não foi testado. Uma foto de perto da sobreposição parada: [debug-overlay-stationary.jpg](../images/debug-overlay-stationary.jpg) (vídeo de teste A, 109,0 s). Nesse quadro, a linha vermelha da corda corre exatamente pela face de contato dos elos por script.

## Resumo

| Exemplo | Vídeo e tempo | Ilustra |
| --- | --- | --- |
| [Arrancada e trocas automáticas](#launch-upshifts) | Vídeo de teste A, 94,5–99,0 s | [Ajuste com dados reais](10-tuning.md) |
| [Virando andando: as velocidades das lagartas se separam](#steering-track-speeds) | Vídeo de teste A, 31,0–33,2 s | [Como funciona por dentro](08-internals.md) |
| [Giro no lugar: lagartas em sentidos opostos](#pivot-counter-rotating) | Vídeo de teste A, 177,0–181,5 s | [Como funciona por dentro](08-internals.md) |
| [Arrancada de ré](#reverse-launch) | Vídeo de teste A, 134,0–137,6 s | [Bugs e limitações](09-bugs.md) |
| [Ré a fundo, depois uma freada forte](#reverse-hard-stop) | Vídeo de teste B, 70,0–75,2 s | [Bugs e limitações](09-bugs.md) |
| [Freada forte e mergulho do nariz](#braking-dive) | Vídeo de teste B, 8,4–12,6 s | [Suspensão e altura do casco](13-suspension.md) |
| [Saindo da pista para a grama](#offroad-grass) | Vídeo de teste A, 192,0–195,5 s | [Referência de atributos](07-attribute-reference.md) |
| [Dando ré para sair das pedras, depois virando no freio](#rough-ground) | Vídeo de teste A, 284,5–288,5 s | [Elos da esteira: a corda nativa e a corrente por script](15-track-links.md) |
| [Elos por script envolvendo a roda motriz](#sprocket-wrap) | Vídeo de teste A, 208,8–211,0 s | [Elos da esteira: a corda nativa e a corrente por script](15-track-links.md) |
| [Saída com a sobreposição de depuração](#debug-pullaway) | Vídeo de teste A, 0,0–4,4 s | [Diagnóstico do veículo e telemetria](16-debugging.md) |
| [Vista de lado em movimento: a corda nativa sob os elos por script](#hero-debug-side) | Vídeo de teste A, 186,0–191,0 s | [Elos da esteira: a corda nativa e a corrente por script](15-track-links.md) |
| [Antes das correções](#before-the-fixes) | Vídeos de diagnóstico 1–3 | [Rodas de apoio, roda-guia, roda motriz e roletes](14-running-gear.md), [Elos da esteira](15-track-links.md) |

<a id="launch-upshifts"></a>

## Arrancada e trocas automáticas

![Leopard 2A6 acelerando na pista visto de trás pela direita em três quartos, sobreposição de depuração nas duas lagartas, da 1ª para a 2ª marcha](../images/launch-upshifts.gif)

Uma vista firme de trás pela direita em três quartos, na pista, com a sobreposição nas duas lagartas. O tanque sai em 1ª com Throttle 0,8: 8,2 km/h em 94,5 s, 12,6 km/h em 95,0 s e 13,6 km/h em 95,5 s, cerca de 2,1 m/s². A 13,6 km/h a 1ª chega ao limite. O motor chega a 2926–2935 rpm, entre `RpmRedline` 2700 e `RpmMax` 3000, e o limitador corta o torque para 139–196 Nm. As caixas das lagartas leem 11.134745 rad/s (106,33 rpm) e 3,786 m/s, que dá 13,6 km/h. A mesma velocidade sai do motor: 2935 rpm / 27,6 (marcha 6,0 × `Ratio` 4,6) = 106,3 rpm da roda motriz, e 106,3 rpm × 2π/60 × 0,34 m = 3,78 m/s.

O câmbio passa para a 2ª em 96,5 s (14,3 km/h, 1941 rpm, 3489 Nm). Na foto (97,0 s) ele está em 2ª a 15,9 km/h, com RPM = Feedback = 1969, 3462 Nm e 714 kW. A 2ª puxa cerca de 1,2 m/s² (de 14,3 a 23,2 km/h em 2 s). Em 99 s as caixas leem 19.334555 rad/s (184,63 rpm) = 6,574 m/s = 23,7 km/h, igual à velocidade do casco, e Feedback 2803 = 184,63 × 15,18 (marcha 3,3 × 4,6). A troca para a 3ª começa aí. Os números de índice dos elos nas rodas motrizes diminuem rápido enquanto a corrente anda.

A telemetria anterior dá os mesmos pontos de troca: mediana de 2924 rpm para 1 → 2 e 2812 rpm para 2 → 3. Os vídeos de teste ficam abaixo de 28 km/h (27,7 km/h em 100,5 s). A velocidade máxima do 2A6 não foi medida de novo: a teoria dá cerca de 72–75 km/h com `Ratio` 4.6, e a versão anterior, do 2A7, mediu 74,3 km/h.

*Vídeo: vídeo de teste A 94,5–99,0 s · [foto em tamanho cheio](../images/launch-upshifts.jpg) (97,0 s) · Ilustra: [Ajuste com dados reais](10-tuning.md)*

<a id="steering-track-speeds"></a>

## Virando andando: as velocidades das lagartas se separam

![Lado direito do Leopard 2A6 saindo numa curva para a esquerda, as duas caixas por lagarta mostrando velocidades diferentes](../images/steering-track-speeds.gif)

O lado direito, com a câmera quase parada. O tanque sai do repouso em 1ª. Em 32,0 s as duas lagartas andam a 2,934 rad/s (28,02 rpm, 0,998 m/s = 3,6 km/h) enquanto o casco faz 1,7 km/h: as lagartas patinam na saída (Throttle 0,8, Clutch 0,9, 3684 Nm).

O motorista então vira à esquerda. Em 32,75 s (Steering -0,5, 9,3 km/h) a lagarta de fora (direita) lê 9.678348 rad/s (92,42 rpm, 3,29 m/s) e a de dentro 4.940294 rad/s (47,18 rpm), cerca de 2:1. A média delas, 7,31 rad/s × 0,34 m = 2,49 m/s (9,0 km/h), bate com a velocidade do casco. Em 33,0 s (Steering -0,8, 8,0 km/h) a separação é 7,37 contra 2,25 rad/s. A câmera começa a orbitar por volta de 33 s.

Logo depois do GIF (34,0 s, Steering -0,9, Throttle 0) a lagarta de dentro quase para: 1,42 rpm contra 27,50 rpm. O tanque gira em volta da lagarta de dentro em vez de girar as duas em sentidos opostos, e a curva mata a velocidade: de 9,3 a 2,2 km/h em cerca de 1,25 s. Com o volante todo virado, a lagarta de dentro primeiro anda a exatamente 1/6 da de fora (99,08 contra 16,51 rpm em 149 s), o que bate com `SteeringRatio` 6; mais adiante na mesma curva a relação é 6,2–6,9. O casco então vira num raio de cerca de 1,6–1,8 m (vídeo de teste A 149–156 s, vídeo de teste B 51–57 s).

*Vídeo: vídeo de teste A 31,0–33,2 s · [foto em tamanho cheio](../images/steering-track-speeds.jpg) (32,75 s) · Ilustra: [Como funciona por dentro](08-internals.md)*

<a id="pivot-counter-rotating"></a>

## Giro no lugar: lagartas em sentidos opostos

![Leopard 2A6 girando no lugar na pista, as duas voltas da corda desenhadas e as caixas por lagarta mostrando velocidades opostas](../images/pivot-counter-rotating.gif)

Um giro no lugar na pista com Steering 1,0, Throttle 1,0, Gear R, Clutch 0,0 e Brake 6,0. O motor fica em 2700 rpm (`RpmRedline`) com 3409 Nm e 964 kW, enquanto o Feedback continua baixo. Em 177,0 s o casco está parado (-0,0 km/h) e as caixas leem +1.595173 e -1.619690 rad/s (+0,542 e -0,551 m/s): um giro neutro de verdade, com as lagartas iguais e opostas. A lagarta esquerda anda para a frente (Steering +1 vira para a direita). Enquanto o casco gira, a lagarta que anda para trás acelera e o casco começa a recuar:

| Tempo | Lagarta para a frente | Lagarta para trás | Casco | Feedback |
| --- | --- | --- | --- | --- |
| 177,0 s | +0,542 m/s | -0,551 m/s | -0,0 km/h | 185 |
| 178,5 s | +0,698 m/s | -1,105 m/s | | |
| 180,0 s | +0,533 m/s | -1,890 m/s | -2,2 km/h | 408 |
| 181,0 s | +0,410 m/s | -2,361 m/s | -3,4 a -3,7 km/h | |

O Feedback segue a regra do giro neutro: em 180,0 s as lagartas giram a 14,97 e 53,07 rpm, e a média delas, 34,02, × `NeutralSteeringRatio` 12 = 408. O tanque gira cerca de 200° entre 175,5 e 182 s. Em 180 s as velocidades das lagartas preveem 0,91 rad/s de guinada (diferença de velocidade / 2,658 m entre as lagartas), mas Y muda só 0,69–0,78 rad/s, então as lagartas patinam (deduzido).

Aparecem dois tipos de giro nos vídeos. Com Clutch 0,0 (este GIF) o motor roda no limitador de 2700 rpm e o Feedback fica baixo. Com Clutch 1,0 (vídeo de teste A 158–164 s, vídeo de teste B 64,3–68 s) o motor é arrastado para 700 rpm com 4601 Nm (Load 0,98), e mesmo assim o casco gira mais rápido na pista: até 1,13 rad/s, contra 0,83 rad/s no giro a 2700 rpm do vídeo de teste B. Na grama (vídeo de teste A 294–296 s) o mesmo estado de 700 rpm e 4601 Nm quase não girou o casco. Brake lê 6,0 nos dois tipos. Em quase todo giro a lagarta que anda para trás anda mais rápido e o casco recua (-0,1 a -3,8 km/h). Qual lógica do controlador puxa o freio de mão aqui, e por que o recuo acontece, está em aberto.

*Vídeo: vídeo de teste A 177,0–181,5 s · [foto em tamanho cheio](../images/pivot-counter-rotating.jpg) (180,0 s) · Ilustra: [Como funciona por dentro](08-internals.md)*

<a id="reverse-launch"></a>

## Arrancada de ré

![Vista baixa do lado esquerdo do Leopard 2A6 saindo em ré, com a sobreposição de depuração sobre os elos por script](../images/reverse-launch.gif)

Uma vista baixa e firme do lado esquerdo. O tanque sai para trás: de 0 a -10,5 km/h em cerca de 1,2 s (134,0–135,4 s), cerca de 2,4 m/s². As rodas e os elos por script giram visivelmente para trás, e o número de índice dos elos na roda motriz aumenta (40 → 48 em 0,7 s). Os círculos das rodas de apoio chegam à face de contato dos elos, não aos aros, porque são desenhados no `Radius` 0.418 da simulação. A linha poligonal da corda corre pela parte de baixo dos elos por script e envolve o círculo da roda motriz.

O painel Engine mostra o bug da ré nativa. Em R, o RPM pula 901 → 3146 → 3584 → 3766 em 0,4 s e fica em 3766–3768, acima de `RpmMax` 3000. O torque lê 0 Nm com acelerador a fundo e -471 Nm com acelerador 0. 3766 rpm é o valor que a fórmula do bug obtida por engenharia reversa prevê para o 2A6 (`RpmMax` + 766 rpm, veja [Bugs e limitações](09-bugs.md)). Com torque zero no motor, o tanque só se move porque o `LEO_TrackedReverseComponent` empurra o casco: o log imprime `LEOREV APPLY force=00159947 N v=-04.0 km/h` em 134,4 s. O Feedback continua seguindo as lagartas. Na foto (136,0 s, acelerador solto) as caixas leem -8.213704 rad/s (-78,44 rpm), -2,79 m/s = -10,1 km/h, e Feedback 830 = 78,44 × 10,58 (`Reverse` 2,3 × 4,6).

O conta-giros do HUD fica no topo da escala e o velocímetro não mostra sinal de ré. Só o painel Debug mostra a velocidade negativa.

*Vídeo: vídeo de teste A 134,0–137,6 s · [foto em tamanho cheio](../images/reverse-launch.jpg) (136,0 s) · Ilustra: [Bugs e limitações](09-bugs.md), [Scripts prontos](11-scripts.md)*

<a id="reverse-hard-stop"></a>

## Ré a fundo, depois uma freada forte

![Vista de lado do lado esquerdo do Leopard 2A6 dando ré a cerca de 16 km/h, depois freando forte até parar](../images/reverse-hard-stop.gif)

A mesma vista firme do lado esquerdo, no vídeo de teste B. O tanque dá ré de -1,9 km/h em 70,0 s a -16,6 km/h em 73,0 s. As duas caixas continuam lendo `Sim: Paused`, `Substep: 0/25`, `Momentum` 0 e `Energy` 0; só AngVel e Speed sobem. O motor fica em 3766–3768 rpm com Clutch 1,0 e Torque 0 Nm, enquanto o Feedback sobe de 234 para 1366. Em 73,0 s (acelerador solto) a caixa mais próxima lê -13.530499 rad/s (-129,21 rpm) e -4,600 m/s = -16,6 km/h, e Feedback 1366 = 129,21 × 10,58.

A partir de 73,8 s o freio sobe 0,7 → 1,0 → 6,0 e o tanque para em cerca de 1 s, com pico de cerca de 5,7 m/s² quando Brake lê 6,0, G-Force 1,2. P vai a -0,04 (a traseira abaixa), depois volta a +0,01. As lagartas derrapam: em 74,4 s elas leem -1,656 m/s enquanto o casco ainda faz -10,6 km/h (-2,94 m/s). Durante a parada o RPM dispara para 4721–4804 com -471 Nm, e quase toda a linha da corda mais próxima fica verde.

Empurrões curtos de ré como este terminam em -16 a -17 km/h porque o acelerador é solto depois de cerca de 3 s; o empurrão por script também diminui com a velocidade (0,3 g parado, zero a 31 km/h). Um empurrão mais longo chegou a -27,8 km/h depois de cerca de 7,6 s (telemetria, 05/10/2026 23:19).

*Vídeo: vídeo de teste B 70,0–75,2 s · [foto em tamanho cheio](../images/reverse-hard-stop.jpg) (73,0 s) · Ilustra: [Bugs e limitações](09-bugs.md)*

<a id="braking-dive"></a>

## Freada forte e mergulho do nariz

![Lado direito do Leopard 2A6 freando forte de cerca de 20 km/h na pista, câmera orbitando para a frente pela direita](../images/braking-dive.gif)

O lado direito; a câmera orbita devagar da vista de lado para três quartos pela frente à direita. O tanque desliza em 3ª a cerca de 23 km/h com Throttle 0. Em 9,0 s ele faz 23,5 km/h (6,53 m/s) com -161 Nm a 1485 rpm, e as lagartas andam a 6,398 m/s, cerca de 2 % mais devagar que o casco sob freio-motor.

Em 10,83 s o Brake pula de 0,0 para 6,0. A velocidade cai de 19,8 km/h para 0,9 km/h em 11,83 s, cerca de 5,3 m/s², com G-Force 1,1–1,2. P vai 0,01, 0,03, 0,05 (pico em 11,17 s), 0,04, 0,03, 0,02, depois volta a -0,01 por volta de 12,3 s: um mergulho pequeno do nariz. O câmbio desce de 3 para 2 a cerca de 10,9 km/h e seleciona R parado.

As lagartas travam e derrapam: em 11,2 s as duas caixas leem 1,535 m/s enquanto o casco ainda faz 13,5–14,4 km/h (3,75–4,0 m/s). O mesmo acontece em toda freada forte lida nestes vídeos (0,96–1,66 m/s de lagarta contra 2,6–4,0 m/s de casco). Os 5–5,7 m/s² medidos ficam um pouco abaixo de uma estimativa grosseira pelos torques de freio, calculada com o `BrakeTorque` cheio de cada peça: (14 × 6000 / 0,418 + 2 × 30000 / 0,34) N / 62300 kg ≈ 6,1 m/s².

O mergulho é difícil de ver na tela. Numa rodada anterior a telemetria da suspensão mediu 1,85° de nariz para baixo a -4,3 m/s² com Brake 6,0 (rodas da frente +72 mm, rodas de trás -88 mm).

*Vídeo: vídeo de teste B 8,4–12,6 s · [foto em tamanho cheio](../images/braking-dive.jpg) (11,2 s) · Ilustra: [Suspensão e altura do casco](13-suspension.md), [Ajuste com dados reais](10-tuning.md)*

<a id="offroad-grass"></a>

## Saindo da pista para a grama

![Vista de trás pela esquerda em três quartos do Leopard 2A6 com a lagarta esquerda na grama e a direita no concreto, as duas voltas da corda desenhadas](../images/offroad-grass.gif)

Uma vista de trás pela esquerda em três quartos com a sobreposição completa; as duas voltas da corda estão desenhadas. O tanque sai da pista com a lagarta esquerda na grama e a direita no concreto. Ele desliza a partir de cerca de 15 km/h, reduz para a 1ª em 194 s com um freio leve (0,3, G-Force 1,1) e termina a 4,2 km/h.

Em 194,5 s as caixas leem 5.481618 rad/s (52,35 rpm, 1,864 m/s) e 5.436948 rad/s (51,92 rpm, 1,849 m/s). Com Steering 0,0 as duas lagartas diferem só 0,8 %, uma na grama e outra no concreto. O casco faz 6,8 km/h (1,89 m/s), cerca de 1,5 % à frente das lagartas enquanto desliza. A relação da 1ª confere: 1433 rpm / 52,35 = 27,4 (6,0 × 4,6 = 27,6).

O chão na beira da pista é plano, então a suspensão quase não se mexe.

*Vídeo: vídeo de teste A 192,0–195,5 s · [foto em tamanho cheio](../images/offroad-grass.jpg) (194,5 s) · Ilustra: [Referência de atributos](07-attribute-reference.md)*

<a id="rough-ground"></a>

## Dando ré para sair das pedras, depois virando no freio

![Lado esquerdo do Leopard 2A6 na grama irregular com pedras, dando ré e depois girando com freio, só os marcadores da suspensão desenhados](../images/rough-ground.gif)

O lado esquerdo na grama irregular com pedras, com a câmera seguindo. Draw tracks está desligado a partir de 206,3 s, então só os marcadores da suspensão aparecem. Antes deste trecho o tanque ficou preso contra pedras por cerca de 7 s (276–282 s). Acelerador a fundo em 1ª segurou o motor em 700 rpm com 4601 Nm e Feedback 381 enquanto o casco fazia 0,1–0,3 km/h. As lagartas provavelmente patinavam no lugar (deduzido; a grama esconde o ramo de baixo).

O motorista dá ré para sair: Gear R, -0,5, depois -5,5, depois -12,1 km/h. Em 285 s o motor dispara para 3745 rpm com Torque 0 (o bug da ré de novo), enquanto o Feedback 450 ainda bate com as lagartas (10,58 × velocidade do casco / 0,34 m dá 454). O empurrão por script está fazendo o trabalho: o log imprime `LEOREV APPLY force=00149825 N v=-05.7 km/h` com `steer=-1.00` em 284,7 s.

Depois vem uma virada no freio ainda rolando para trás: Steering -1,0, Brake 6,0 (depois 1,0), RPM 700, 4601 Nm. Y vai de -0,98 (286 s) para -1,52 (287 s) e -1,80 (288 s), cerca de 0,8 rad em 2 s, e o casco vira de frente para a câmera entre cerca de 286,8 e 287,6 s. O trecho termina com a volta da frente das duas lagartas à vista. Os elos por script ficam nas rodas-guia e motrizes o tempo todo.

As pedras nesta parte do vídeo ficam ao lado das lagartas, não debaixo delas, então o trecho não mostra a suspensão trabalhando sobre uma pedra. A grama esconde a maior parte do ramo de baixo.

*Vídeo: vídeo de teste A 284,5–288,5 s · [foto em tamanho cheio](../images/rough-ground.jpg) (287,0 s) · Ilustra: [Elos da esteira: a corda nativa e a corrente por script](15-track-links.md), [Bugs e limitações](09-bugs.md)*

<a id="sprocket-wrap"></a>

## Elos por script envolvendo a roda motriz

![Vista em três quartos do lado direito do Leopard 2A6 na grama, os elos por script envolvendo a roda motriz traseira e passando sob as rodas de apoio](../images/sprocket-wrap.gif)

Uma vista clara em três quartos do lado direito na grama. O tanque vira de leve (Steering 0,4, depois -0,7) a cerca de 8–10 km/h, com rolagem R em 0,12–0,15. Na foto (209,5 s) ele está em 1ª a 7,8 km/h, com Throttle 0,8, RPM = Feedback = 2719 e 2458 Nm. Os elos por script sobem da última roda de apoio, envolvem a roda motriz traseira e correm para a frente por cima dos roletes de retorno. No ramo de baixo eles passam sob as 7 rodas de apoio. Só os marcadores pequenos da suspensão aparecem (Draw tracks desligado).

Os elos são colocados a cada frame num caminho feito a partir dos círculos das rodas nos raios da simulação, então por construção não saem da roda motriz. Eles ainda não passaram por um capotamento nem por uma explosão de mina.

*Vídeo: vídeo de teste A 208,8–211,0 s · [foto em tamanho cheio](../images/sprocket-wrap.jpg) (209,5 s) · Ilustra: [Elos da esteira: a corda nativa e a corrente por script](15-track-links.md)*

<a id="debug-pullaway"></a>

## Saída com a sobreposição de depuração

![Vista de lado completa do trem de rolamento esquerdo do Leopard 2A6 saindo e trocando de marcha, com os painéis Debug e Engine legíveis](../images/debug-pullaway.gif)

Uma vista de lado completa do trem de rolamento esquerdo enquanto o tanque sai e troca de marcha; os painéis Debug e Engine ficam legíveis o tempo todo. Começa com um giro neutro parado em 0,0 s: Steering 1,0, Clutch 0,0, Brake 6,0, Throttle 1,0, 1ª marcha. O motor roda a 2700 rpm com 3409 Nm, e as lagartas giram em sentidos opostos, desiguais: -1.405754 rad/s (-13,42 rpm, -0,478 m/s) e +2.175743 rad/s (20,78 rpm, +0,740 m/s). O Feedback 206 é a média de \|rpm das lagartas\| × `NeutralSteeringRatio` 12 (17,1 × 12 = 205).

| Tempo | Entradas | Casco | RPM / Feedback | Lagartas |
| --- | --- | --- | --- | --- |
| 0,5 s | Steering 0,7, Clutch 0,5, Throttle 0,8 | 1,0 km/h | 1003 / 431 | 3,306 rad/s (31,57 rpm) e 0: a lagarta de dentro fica parada |
| 1,0 s | Steering -0,1, Clutch 1,0, Throttle 0,8 | 6,0 km/h | 1589 / 1589 (3702 Nm) | cerca de 2,04 m/s cada, 20 % mais rápido que o casco (patinação) |
| 2,0 s | Clutch 1,0, Throttle 0,8, 1ª marcha | 13,2 km/h | 2803 / 2803 | 10.634644 rad/s (101,55 rpm), 3,615 m/s |
| 4,0 s | Throttle 0,0, 2ª marcha | 18,0 km/h | 2009 / 2009 (-268 Nm) | 13,856 rad/s (132,32 rpm), 4,71 m/s |

Em 0,5 s, o Feedback 431 é mais ou menos a média do rpm das lagartas × 27,6 (436). Em 2,0 s, 101,55 rpm × 27,6 (1ª) = 2803, e Speed / AngVel = 0,340 m, o `Radius` da roda motriz. Em 4,0 s, 132,32 × 15,18 (2ª) = 2009. Com a embreagem travada, o RPM é igual ao Feedback. De 4,0 a 7,0 s o tanque desliza em 2ª com acelerador 0, de 17,5 a 12,9 km/h (cerca de 0,43 m/s²), e o torque de freio-motor cai de -265 para -159 Nm conforme o rpm baixa.

*Vídeo: vídeo de teste A 0,0–4,4 s · [foto em tamanho cheio](../images/debug-pullaway.jpg) (2,0 s) · Ilustra: [Diagnóstico do veículo e telemetria](16-debugging.md)*

<a id="hero-debug-side"></a>

## Vista de lado em movimento: a corda nativa sob os elos por script

![Vista de lado firme do lado esquerdo do Leopard 2A6 acelerando na pista, círculos das rodas da simulação e a linha da corda nativa desenhados sobre os elos por script](../images/hero-debug-side.gif)

Uma vista de lado firme do lado esquerdo na pista. O tanque passa para a 2ª por volta de 186,5 s (a velocidade cai de 13,0 para 10,3 km/h com Clutch 0,2), acelera até 21,8 km/h em 189 s e depois desliza. Cada TrackPart das duas lagartas tem o seu círculo branco da simulação com um raio girando, e os elos por script passam sob os círculos das rodas de apoio e em volta da roda motriz.

A corda nativa continua sendo simulada sob os elos por script; só os elos dela ficam escondidos. O ramo de cima dela chicoteia visivelmente a 17–21 km/h. Um trecho sobe acima da linha dos roletes de retorno perto da roda motriz (melhor por volta de 188,3 s), e por volta de 189,7–190,0 s ele corta na diagonal para baixo, atravessando as rodas de apoio de trás. Os elos por script e o ramo de baixo da corda ficam arrumados, e as caixas continuam lendo `Sim: Paused`, `Substep: 0/25`, `Momentum` 0, `Energy` 0. A folga da corda também aparece a 25 km/h por volta de 100 s no vídeo de teste A, onde ela se junta no ramo de cima logo à frente da roda motriz traseira. É a folga que `TrackLength` 0.2110 dá à corda. Ela fica invisível no jogo normal porque os elos nativos estão escondidos.

| Tempo | Estado | Casco | Caixa da lagarta | Motor |
| --- | --- | --- | --- | --- |
| 187,0 s | 2ª marcha, Throttle 0,8 | 10,7 km/h (2,97 m/s) | 9.571692 rad/s (91,40 rpm), 3,254 m/s: 9,5 % mais rápido que o casco | 1467 rpm, 3722 Nm, 572 kW |
| 188,0 s (foto) | 2ª marcha, Throttle 0,8 | 16,2 km/h | 13.620472 rad/s (130,07 rpm), 4,631 m/s | 1995 rpm, 3442 Nm, 719 kW |
| 189,83 s | 2ª marcha | 21,3 km/h | 16.684423 rad/s (159,32 rpm), 5,673 m/s | 2417 rpm; 2417 / 159,32 = 15,17 = 3,3 × 4,6 |
| 190,0 s | 2ª marcha, Throttle 0 (deslizando) | 20,8 km/h | 16.363831 rad/s (156,26 rpm), 5,564 m/s = 20,0 km/h: 4 % mais devagar que o casco | 2370 rpm, -342 Nm, -85 kW |

*Vídeo: vídeo de teste A 186,0–191,0 s · [foto em tamanho cheio](../images/hero-debug-side.jpg) (188,0 s) · Ilustra: [Elos da esteira: a corda nativa e a corrente por script](15-track-links.md), [Diagnóstico do veículo e telemetria](16-debugging.md)*

<a id="before-the-fixes"></a>

## Antes das correções

Três vídeos de diagnóstico de 05/10/2026, gravados enquanto o trem de rolamento e os elos eram corrigidos. Eles mostram as falhas que levaram às rodas por script e à corrente de elos por script. Não têm sobreposição de depuração, então velocidades e rpm vêm dos ponteiros do HUD (cerca de ±3 km/h e ±150 rpm).

| Vídeo | Rodas | Corrente de elos | Suspensão na simulação |
| --- | --- | --- | --- |
| Vídeo de diagnóstico 1 (22:35) | `CarProcAnimComponent` com `wheel_suspension.pap`, parâmetros na ordem por lado (rodada 1) | Corda nativa, 73 × `TrackLength` 0.2101 | `MaxTravelDown` 0.15: casco cerca de 5 cm alto parado (curso parado -53 mm, medido) |
| Vídeo de diagnóstico 2 (22:56) | `CarProcAnimComponent`, parâmetros intercalados L01, R01, L02, … (rodada 2) | Corda nativa | Ainda se comportando como a rodada 1 (curso parado -51 a -54 mm): as edições do prefab não tinham chegado ao mundo aberto |
| Vídeo de diagnóstico 3 (23:21) | `LEO_RunningGearComponent` (por script) | Corda nativa, 73 × `TrackLength` 0.2110 | `MaxTravelDown` 0.10: casco na altura do modelo |

Mecanismo mais provável das falhas da corda (deduzido do passo da corda obtido por engenharia reversa): todo ponto da corda tem gravidade, e a corda nunca colide com o chão. Um ponto só é segurado embaixo de uma roda de apoio enquanto essa roda tem contato com o chão e o ponto fica abaixo do centro da roda, a até 0,1 m dele ao longo do casco. Por isso a folga pende entre as rodas e em volta da roda motriz e da roda-guia. Quando as rodas perdem o contato (capotamento, explosão), essa trava desliga e as rodas caem até o curso todo para baixo, o que consome toda a folga da corda ou mais. O passo de restrição então arrasta pontos através dos círculos das rodas, e o passo de colisão empurra cada ponto para fora pelo lado mais próximo. Uma corda puxada além do centro da roda motriz acaba do lado de fora dela e fica lá.

### Rodas de apoio flutuando acima da corrente (CarProcAnimComponent)

![Vista de lado baixa e parada do lado direito do Leopard 2A6: a roda de apoio da frente fica sobre os elos, as rodas 2 a 7 flutuam acima deles](../images/before-wheels-floating.jpg)

Vídeo de diagnóstico 2, uma vista baixa e parada do lado direito com o motor em marcha lenta a 0 km/h. A roda de apoio 1 (da frente, à direita da imagem) fica sobre os elos. As rodas 2–7 flutuam acima da corrente, com luz e os chifres-guia dos elos visíveis debaixo delas: cerca de 5–8 cm, estimados pelo aro de 0,69 m. A câmera não se mexe de 7,0 a 21,5 s, e o vão continua o mesmo andando a cerca de 10 km/h (23,3–28,8 s). O vídeo de diagnóstico 1 mostra a mesma falha a cerca de 22 km/h (15–19 s).

Telemetria desta rodada: os ossos R02–R07 ficavam 51–54 mm acima da roda deles na simulação, e o R01 a até 4 mm dela. O `CarProcAnimComponent` entregou à maioria dos ossos das rodas os sinais de outra roda. Na rodada 1, os ossos L01/L03/L05/L07 seguiram as rodas esquerdas 0/1/2/3 com 0,9–1,7 mm de diferença. Na rodada 2, os ossos esquerdos pares L02/L04/L06 giraram exatamente como o osso direito R01. Não use o `CarProcAnimComponent` no trem de rolamento de esteira na 1.8.0.13. Os ossos agora são movidos pelo `LEO_RunningGearComponent`.

*Foto: vídeo de diagnóstico 2, 9,0 s · Ilustra: [Rodas de apoio, roda-guia, roda motriz e roletes](14-running-gear.md)*

### Corda pendendo abaixo das rodas

![Vista alta em três quartos pela frente à direita do Leopard 2A6 estacionado: o ramo de baixo da corda nativa fica abaixo das rodas de apoio](../images/before-rope-sag.jpg)

Vídeo de diagnóstico 3, estacionado na pista com o motor desligado. As rodas por script estão na altura certa, mas o ramo de baixo da corda nativa fica abaixo delas, cerca de 5–10 cm (estimado) sob as rodas 2–6, e se enrola solto atrás. Com `TrackLength` 0.2110 a corrente tem 73 × 0,2110 = 15,403 m, cerca de 12 cm a mais que o caminho de 15,284 m em volta dos círculos das rodas em repouso. A corda tem gravidade mas não colide com o chão, então o comprimento extra provavelmente pende (deduzido). O afundamento aparece bem num recorte ampliado 2×, mas é sutil no quadro inteiro.

*Foto: vídeo de diagnóstico 3, 8,0 s · Ilustra: [Elos da esteira: a corda nativa e a corrente por script](15-track-links.md)*

### Folga se acumulando atrás da roda motriz em movimento

![Vista de lado acompanhando o lado direito do Leopard 2A6 a cerca de 27 km/h: os elos com folga fazem zigue-zague e batem atrás da roda motriz](../images/before-rope-sag-sprocket.gif)

Vídeo de diagnóstico 3, uma vista acompanhando o lado direito a cerca de 27 km/h e cerca de 2000 rpm. A folga se junta entre a roda motriz e a roda de apoio 7. Os elos fazem zigue-zague, se empilham e batem para fora abaixo e atrás da roda motriz, com picos por volta de 17,2–17,4 s e 18,8 s. O ramo de baixo também pende um pouco abaixo das rodas.

*Vídeo: vídeo de diagnóstico 3 15,5–19,6 s · [foto em tamanho cheio](../images/before-rope-sag-sprocket.jpg) (17,4 s) · Ilustra: [Elos da esteira: a corda nativa e a corrente por script](15-track-links.md)*

### De cabeça para baixo: a corda pende pela gravidade

![Leopard 2A6 de teto para baixo na grama, rodas no curso todo para baixo, o ramo de retorno da corda pendendo abaixo do casco invertido](../images/before-flip-rope-hangs.gif)

Vídeo de diagnóstico 1. Em 189–194 s o tanque sobe um barranco de grama ao lado da pista, tomba de lado e cai de teto para baixo. Telemetria desse momento: 33–41 km/h em 4ª com a rolagem crescendo até -11° em cerca de 3 s, depois um impacto de 34,3 para 13,5 km/h em 0,5 s e uma rolagem até 177,5° em 2,5 s. O centro de massa nessa versão estava a 2,17 m do chão, porque `CenterOfMass` 0 1 0.46 é somado ao ponto COM do próprio modelo. Isso dá um fator de estabilidade estática de só cerca de 0,61 g (calculado, não uma causa provada; veja [Suspensão e altura do casco](13-suspension.md)).

Deitado invertido, o lado mais próximo é o esquerdo. As rodas de apoio não têm contato e ficam penduradas no curso todo para baixo, apontando para cima. O ramo de contato da corda fica esticado sobre elas, e o antigo ramo de retorno pende abaixo do casco invertido como uma catenária: a corda tem gravidade e não colide com o chão. Com todas as rodas a 0,15 m de queda, o caminho em volta dos círculos das rodas fica mais longo que a corrente de 73 × 0,2101 m: faltam cerca de 10–14 cm, conforme se conte o arco ou o polígono de elos. Um ícone do Game Master cobre o casco a partir de cerca de 197,2 s; o tanque foi desvirado por volta de 198–203 s.

*Vídeo: vídeo de diagnóstico 1 194,6–197,0 s · [foto em tamanho cheio](../images/before-flip-rope-hangs.jpg) (195,0 s) · Ilustra: [Elos da esteira: a corda nativa e a corrente por script](15-track-links.md), [Suspensão e altura do casco](13-suspension.md)*

### Corda fora da roda motriz depois do capotamento

![Vista em três quartos pela frente à esquerda do Leopard 2A6 na pista: a roda motriz esquerda flutua nua atrás do fim da corrente](../images/before-rope-off-flip.jpg)

Vídeo de diagnóstico 1, depois de o tanque ser desvirado, na pista (vista parada em três quartos pela frente à esquerda, cerca de 5–7 km/h). A corda esquerda envolve a última roda de apoio (7) em vez da roda motriz. A roda motriz flutua nua atrás e acima do fim da corrente, com luz debaixo dela. A falha continua o tempo todo na pista (249–257 s): a corda não volta sozinha para o lugar, e a API de script não tem chamada para reiniciá-la. As rodas de apoio 2–4 ficam visivelmente acima da corrente (a troca do `CarProcAnimComponent`). Em 261,5 s uma vista de trás mostra a roda motriz esquerda nua e a direita ainda envolvida: só o lado que saiu continua fora.

*Foto: vídeo de diagnóstico 1, 255,0 s · Ilustra: [Elos da esteira: a corda nativa e a corrente por script](15-track-links.md)*

### Corda fora depois de uma explosão de mina

![Vista em três quartos de trás pela esquerda do Leopard 2A6 entre pedras: a corda esquerda corre como um fio solto da roda motriz até uma pedra](../images/before-rope-off-mines.gif)

Vídeo de diagnóstico 3. Em 22,75–24 s o tanque passa por cima de uma fileira de minas, e o casco é jogado para cima em 24,4–25,0 s. Telemetria dessa rodada: arfagem de -5,5 a -10,1°, rolagem até 10,8°, várias rodas no ar e curso de contato até +0,348 m, acima de `MaxTravelUp` 0.30. De 25,0 a 26,2 s a corda esquerda forma um laço solto abaixo e atrás da roda motriz, e um laço também pende na traseira direita. De 26,3 a 28,0 s a corda esquerda corre como um fio fino na diagonal da roda motriz até uma pedra, presa ao tanque só atrás, enquanto as rodas de apoio passam sobre a pedra sem elos debaixo delas (cerca de 10 km/h, cerca de 2700 rpm). Um arbusto tapa a vista a partir de cerca de 28,3 s.

*Vídeo: vídeo de diagnóstico 3 25,0–28,0 s · [foto em tamanho cheio](../images/before-rope-off-mines.jpg) (27,3 s) · Ilustra: [Elos da esteira: a corda nativa e a corrente por script](15-track-links.md)*

### Roda motriz nua no fim

![Vista de lado quase parada do lado direito do Leopard 2A6 entre pedras: dentes da roda motriz nus e nenhum elo sob as rodas de apoio](../images/before-bare-sprocket.jpg)

Vídeo de diagnóstico 3, fim do trecho: o lado direito, parado entre pedras e grama. Os dentes da roda motriz estão nus e nenhum elo aparece sob as rodas de apoio, então a corda direita também saiu (a roda motriz já parece nua por volta de 33 s). Depois desta rodada os elos da corda nativa foram escondidos e a corrente de elos por script tomou o lugar deles.

*Foto: vídeo de diagnóstico 3, 38,0 s · Ilustra: [Elos da esteira: a corda nativa e a corrente por script](15-track-links.md)*

<a id="what-the-videos-reveal"></a>

## O que os vídeos revelam

A maior parte destes comportamentos vale para qualquer tanque feito com este guia; os números são os do Leopard. Os tempos estão em segundos; "A" e "B" são os vídeos de teste.

| Comportamento | Onde | Situação |
| --- | --- | --- |
| Rodas de apoio, roda-guia, roda motriz e roletes de retorno giram e acompanham a suspensão | A e B o tempo todo (pouco curso de suspensão neste chão plano) | ✅ Por script, `LEO_RunningGearComponent` ([detalhes](14-running-gear.md)) |
| Com o `CarProcAnimComponent`, as rodas de apoio direitas 2–7 flutuam cerca de 5–8 cm acima da corrente (a telemetria acrescenta: a maioria dos ossos recebe os sinais de outra roda, por exemplo os ossos esquerdos pares giram como o osso direito R01) | Vídeo de diagnóstico 2, 7–28,8 · vídeo de diagnóstico 1, 15–19 | ❌ O `CarProcAnimComponent` troca as rodas de esteira na 1.8.0.13; substituído pelo script ([detalhes](14-running-gear.md)) |
| Os elos por script envolvem a roda motriz e a roda-guia e ficam no lugar na grama, contra pedras e atravessando uma cerca de arame, sem pender | A 208,8–211,0, 251–259,8, 284,5–292,2 · todo o B | ✅ Corrente por script ([detalhes](15-track-links.md)). Por construção não sai do lugar; ainda não testada capotando nem com mina |
| A corda nativa com cerca de 12 cm de folga pende abaixo das rodas e se acumula atrás da roda motriz | Vídeo de diagnóstico 3, 5,6–9,0, 15,5–19,6 | ⚠️ Corda nativa: gravidade, sem colisão com o chão ([detalhes](15-track-links.md)) |
| A corda nativa sai da roda motriz depois de um capotamento e fica fora | Vídeo de diagnóstico 1, 189–197, 249–262,8 | ⚠️ Sem API de script para reiniciá-la; só o menu Diag tem Reset tracks (não testado) |
| A corda nativa sai depois que minas jogam o tanque | Vídeo de diagnóstico 3, 22,75–28,0, 33–39,7 | ⚠️ Mesmo mecanismo (deduzido) |
| A corda nativa escondida continua rodando sob os elos por script; o ramo de cima dela chicoteia a 17–21 km/h e a folga dela se junta à frente da roda motriz traseira | A 100, 188,3, 189,7–190,0 | Cosmético, porque os elos dela estão escondidos. Se o RPM das rodas depende da corda está em aberto |
| A depuração do veículo funciona com uma corrente de elos presente: Show vehicle debug ficou ligado a sessão inteira, sem travar ao alternar Show inertia, Show forces, Show engine, Show contacts, Show raycast, Show wheels, Show Controller Diags, Draw tracks, TrackSim e Track gravity | A 0–89, 196–207 · todo o B | ✅ Travou em 02/10/2026, quando o veículo não tinha corrente ([detalhes](16-debugging.md)) |
| As caixas da corda leem `Sim: Paused`, `Substep: 0/25`, `Momentum` 0 e `Energy` 0 em todo quadro, mesmo a 27 km/h | Os dois vídeos de teste | Em aberto: significado desconhecido |
| Velocidade da corrente = AngVel × 0,34 m (`Radius` da roda motriz); Feedback = rpm da roda motriz × relação da marcha × 4,6 | Todo quadro lido (Feedback: fora dos giros neutros), por exemplo A 2,0, 99, 136 | Medido pela sobreposição ([detalhes](08-internals.md)) |
| Patinação das lagartas: as lagartas andam 3–20 % mais rápido que o casco com potência (cerca de 110 % na saída, A 32,0) e 2–6 % mais devagar deslizando | A 1,0, 32,0, 187, 190, 205 · B 2, 9 | Medido pela sobreposição |
| A 1ª chega ao limite em 13,6 km/h a cerca de 2935 rpm, onde o limitador corta o torque; então o câmbio sobe uma marcha (nem sempre: em A 117–120 ficou em 1ª) | A 95,5–96,5, 127–129 · B 0–1 | Medido ([detalhes](10-tuning.md)) |
| Torque de freio-motor = -470 × (RPM - 700) / (3000 - 700), com limite em -471 Nm | A 102–106, 263 · B 5 | Medido (bate com até 2 Nm em todos os quadros menos dois) |
| Brake 6,0 para o tanque a cerca de 5–5,7 m/s² (G-Force 1,1–1,2); as lagartas derrapam; P chega a +0,05 para a frente e -0,04 em ré | B 10,8–12,3, 35, 73,8–75 · A 106–107,5, 130,6–131,6, 169,2–170,4 | Medido pela sobreposição ([detalhes](13-suspension.md)) |
| Com o volante todo virado, a lagarta de dentro anda a 1/6 a 1/7 da de fora (exatamente 1/6 em A 149, batendo com `SteeringRatio` 6); o casco vira num raio de cerca de 1,6–1,8 m | A 149–156 · B 51–57 | Medido pela sobreposição |
| Giro neutro: as lagartas giram em sentidos opostos com Brake 6,0. Com Clutch 0 o motor fica em 2700 rpm; com Clutch 1 ele é arrastado para 700 rpm e o casco gira mais rápido na pista (na grama, em A 294–296, quase não girou). A lagarta que anda para trás é mais rápida e o casco recua até 3,8 km/h | A 0, 109,5–115, 158–164, 177–182 · B 14–19,5, 64,3–68 | Observado no vídeo; causa em aberto ([detalhes](08-internals.md)) |
| Ré: o motor fica preso em 3766–3768 rpm com Torque 0 (até 4469–4804 rpm quando o acelerador é solto ou o freio é aplicado); o tanque só se move pelo empurrão por script | A 115,5, 134–137,6, 165,8–170,4, 285 · B 38,5–41,9, 70–75,2 | ⚠️ Bug nativo, contorno por script ([detalhes](09-bugs.md)) |
| O HUD não mostra a ré: o velocímetro não tem lado negativo e o conta-giros fica no topo | Toda ré | Efeito colateral do bug da ré |
| Empurrões curtos de ré chegam a -16 a -17 km/h | A 168,2 · B 73 | Esperado: o acelerador é solto depois de cerca de 3 s, e o empurrão diminui com a velocidade. Um empurrão de 7,6 s chegou a -27,8 km/h (telemetria) |
| Com acelerador 0 em 1ª o tanque continua rolando no concreto plano: de 3,2 a 1,7 km/h em 31 s | A 60–91 | Observado no vídeo: quase nenhuma resistência à rolagem em passo de caminhada |
| "Slope" lê -1 % em todo lugar, e "Load" é uma fração de 0–1 apesar do sinal "%" | Os dois vídeos de teste | Observado no vídeo: peculiaridades dos painéis |
| Show inertia imprime 327602.7, 395692.4, 94730.0, compatível com uma caixa maciça de 62,3 t com cerca de 3,96 m de largura, 1,60 m de altura e 7,78 m de comprimento | A 45–47,5 | Observado; o ajuste da caixa é deduzido ([detalhes](16-debugging.md)) |
| Show forces desenha uma linha de cada roda que sai da tela com 62 t | A 50–51,5 | Observado: ilegível com esta massa |

---

[← Passo a passo do zero](04-from-scratch.md) · [Índice](../../README.pt-BR.md) · [Convertendo um veículo existente →](06-converting.md)
