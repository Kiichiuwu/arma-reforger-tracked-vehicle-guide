> 🇧🇷 Português · [🇬🇧 English](../en/13-suspension.md)

[← Solução de problemas](12-troubleshooting.md) · [Índice](../../README.pt-BR.md) · [Rodas de apoio, roda-guia, roda motriz e roletes →](14-running-gear.md)

# Suspensão e altura do casco

Este capítulo explica como a simulação de esteira sustenta o casco: o raio da roda, a mola, os amortecedores e o centro de massa. Depois mostra como o casco se move andando, e uma armadilha do Workbench que esconde as mudanças no prefab.

Todos os números são do exemplo do Leopard 2A6 (`Mass` 62300 kg, 7 rodas de apoio por lado) no Workbench 1.8.0.13. Eles vêm da telemetria `LEOSUSP` e `LEOTEL` ([capítulo 16](16-debugging.md)), de textos do executável 1.8.0.13 e dos vídeos de teste. Cada afirmação diz como é conhecida.

### As regras em resumo

| Regra | Como se sabe |
| --- | --- |
| O `SpringRate` está em kN/m (= N/mm). 450 quer dizer 450 000 N/m | Descrição no executável; duas rodadas batem com isso |
| A mola fica solta (força zero) no curso todo para baixo, −`MaxTravelDown` | Bate com o curso parado medido em duas rodadas |
| Curso parado = m·g / (n·k) − `MaxTravelDown` | Medido: −53 mm com 0.15, −3 mm com 0.10 |
| Ponha `MaxTravelDown` ≈ m·g / (n·k) e o casco fica na altura do modelo | Medido |
| O `RoadWheel.Radius` é a distância do centro da roda à face da esteira em contato com o chão | Medido (diferença média de 1,25 mm) |
| O `CenterOfMass` é um deslocamento somado ao centro de massa do próprio modelo | Descrição no executável; medido |
| O `z` do `CenterOfMass` define a inclinação parado: cerca de 2,1° por metro no 2A6 | Calculado (2,06°/m) e medido (2,18°/m) |
| Depois de editar o prefab, feche o mundo, recarregue os scripts e abra o mundo de novo | Observado (rodadas 2 e 3) |

### Sessões de teste usadas neste capítulo

| Sessão (hora local) | Configuração | Usada para |
| --- | --- | --- |
| Rodada 1, 05/10/2026 22:30 | `MaxTravelDown` 0.15, ossos das rodas movidos pelo `CarProcAnimComponent` | Ponto de mola solta, faixa do sinal, perda de contato, um capotamento |
| Rodada 2, 05/10/2026 22:55 | Prefab já em 0.10, mundo não recarregado | A armadilha do cache do prefab |
| Rodada 3, 05/10/2026 23:18 | `MaxTravelDown` 0.10, rodas por script | Curso parado com 0.10 |
| Rodada 4, 05/10/2026 23:20 | Igual, sobre minas | Contato acima do limite do curso |
| 05/10/2026 23:58 e 06/10/2026 00:12 | Igual, rodas por script e corrente de elos por script | Curso parado por roda, agachamento e mergulho, pedras |

### Como uma roda de apoio acha o chão

No prefab do 2A6 só o modelo `RoadWheel` tem um bloco `Suspension`. A roda-guia, a roda motriz e os roletes de retorno ficam nas posições do modelo, presos ao casco.

Cada roda de apoio sonda o chão com um raio (raycast):

| Passo | O que acontece | Como se sabe |
| --- | --- | --- |
| 1 | O raio começa `RayStartOffsetUp` acima do topo do curso, a posição de compressão total | Descrição no executável: "Extension of raycast start above top position of suspension. Suspension total length is not affected." |
| 2 | Ele segue o eixo para baixo do casco | Deduzido: o passo 3 bateu nos eixos do casco |
| 3 | O centro da roda vai para o ponto de contato mais `Radius`, ao longo do eixo para cima do casco | Medido: o próprio sinal de suspensão do motor bateu com (y local do contato + `WheelGetRadius` − altura do modelo) com diferença média de 1,25 mm (rodada 1, 106 quadros com todas as rodas em contato) |
| 4 | O curso é a distância desse centro até a posição modelada (o osso), em metros ao longo do eixo Y do osso. Compressão é positiva | Medido (rodada 1) |
| 5 | O sinal do curso é limitado a −`MaxTravelDown` … +`MaxTravelUp` | Medido: os ossos movidos pelo sinal foram exatamente de −0,1500 a +0,3000 m na rodada 1 |

As alturas das rodas de apoio do 2A6, no espaço do casco (plano do chão em y = 0):

```text
y no casco (m)
  1.219        início do raio = topo do curso + RayStartOffsetUp 0.5    descrição no executável, não medido
  0.719        topo do curso = modelado + MaxTravelUp 0.30              o sinal limita em +0.30
  0.419        centro modelado da roda = osso v_wheel_<s>0<k>           curso 0
  0.418-0.414  parado em chão plano, roda da frente até a de trás       curso -1 ... -5 mm (medido)
  0.319        curso todo para baixo = modelado - MaxTravelDown 0.10    força da mola 0; a roda pende aqui sem contato
  0.001        face da esteira no chão = centro modelado - Radius 0.418
```

**O `Radius` não é o raio do aro.** O centro da roda fica `Radius` acima do contato, então o `Radius` é a distância do centro da roda até a face da esteira em contato com o chão. No 2A6 isso é o aro de 0,345 m mais cerca de uma espessura de elo (0,073 m aqui; a sapata tem 0,076 m): `Radius` 0.418. O osso fica a 0,4186 m, então uma roda na altura do modelo toca o plano do chão. Regra: `Radius` = altura do osso da roda de apoio acima do fundo da esteira. Um `Radius` que não bate faz o casco flutuar ou afundar pela diferença. O [capítulo 14](14-running-gear.md) lista o raio da simulação e o raio visual de cada peça.

![Sobreposição da depuração nativa do veículo sobre um Leopard 2A6 parado: os círculos das rodas desenhados no raio da simulação chegam ao fundo dos elos da esteira](../images/debug-overlay-stationary.jpg)

*A sobreposição da depuração do veículo com o tanque parado (Gear R, Brake 6.0, 700 rpm). Os círculos brancos de 12 lados são desenhados no `Radius` da simulação: os círculos das rodas de apoio (0,418 m) chegam à face dos elos em contato com o chão, abaixo dos aros de 0,345 m. As linhas azuis verticais marcam a suspensão no centro de cada roda. Vídeo de teste A (06/10/2026), foto em 109,0 s · Modelo: buh-late, CC BY 4.0, modificado ([créditos](../../CREDITS.md)).*

**Limites do curso.** O sinal fica dentro de −`MaxTravelDown` … +`MaxTravelUp`. O contato em si pode ficar fora dessa faixa. O curso calculado pelo contato (contato + `Radius` − altura do modelo) chegou a +0,367 m na rodada 1, +0,348 m sobre as minas na rodada 4 e +0,382 m na sessão das 00:12, todos acima do limite de +0,30. Para baixo, passou alguns milímetros do limite (−0,107 m com `MaxTravelDown` 0.10, −0,151 m com 0.15). O raio acha chão acima do topo do curso porque começa 0,5 m mais alto. Chão acima do início do raio não seria achado (deduzido, não testado). A força que a simulação aplica além de +`MaxTravelUp` não foi medida. Um script que move ossos a partir do ponto de contato precisa limitar o curso, como faz o `LEO_RunningGearComponent` ([capítulo 14](14-running-gear.md)).

**Sem contato.** Quando o raio não acha nada (no ar, capotado, sobre um vão), o `WheelHasContact` devolve false e o sinal é exatamente −`MaxTravelDown`: a roda pende no curso todo para baixo. Medido na rodada 1: todos os sinais leram −0,150 enquanto o tanque estava de teto no chão.

![Leopard 2A6 de cabeça para baixo depois de capotar, todas as rodas de apoio penduradas no curso todo para baixo](../images/before-flip-rope-hangs.jpg)

*De cabeça para baixo depois do capotamento da rodada 1 (o `MaxTravelDown` era 0.15 então). Nenhuma roda de apoio toca o chão, então todas pendem no curso todo para baixo. Todos os sinais das rodas leram exatamente −0,150 no log, então o mapeamento errado do `CarProcAnimComponent` daquela versão não muda esta imagem. A esteira pendurada sob o casco é a corda nativa ([capítulo 15](15-track-links.md)). Vídeo de diagnóstico 1 (05/10/2026, clipe das 22:35), foto em 195,0 s (3:15) · Modelo: buh-late, CC BY 4.0, modificado ([créditos](../../CREDITS.md)).*

### Os atributos da suspensão

O bloco `Suspension` fica dentro do modelo `RoadWheel` do `Simulation Tracked`. As descrições são os textos guardados ao lado de cada atributo no executável 1.8.0.13.

| Atributo | Unidade | Descrição no executável | Leopard 2A6 |
| --- | --- | --- | --- |
| `SpringRate` | N/mm = kN/m | "Spring force per mm. Tells how much force is needed to compress the spring by one mm" | 450 |
| `CompressionDamper` | N·s/m | "Compression damper force per 1 m/s. Used when suspension is compressing." | 25000 |
| `RelaxationDamper` | N·s/m | "Relaxation damper force per 1 m/s. Used when suspension is decompressing." | 35000 |
| `MaxTravelUp` | m | "It's the distance that the suspension can be compressed from modeled position." | 0.30 |
| `MaxTravelDown` | m | "It's the distance that the suspension can be expanded from modeled position." | 0.10 |
| `RayStartOffsetUp` | m | "Extension of raycast start above top position of suspension. Suspension total length is not affected." | 0.5 |
| `MaxSteeringAngle` | graus | "Maximum wheel rotation angle (positive number). For non steering wheels just leave it at 0." | 0 |

Mais dois valores fazem parte da altura do casco: o `RoadWheel.Radius` (0.418, veja acima) e o `RigidBody.Mass` (62300).

### O modelo da mola

A mola não tem pré-carga. Ela empurra com força zero no curso todo para baixo e cresce `SpringRate` por milímetro de compressão a partir dali (linear: bate com as duas rodadas; não foi testada ponto a ponto):

```math
F_{\text{spring}} = k \cdot (x + \text{MaxTravelDown}), \qquad -\text{MaxTravelDown} \le x \le \text{MaxTravelUp}
```

Aqui *x* é o curso a partir da posição modelada (positivo para cima) e *k* = `SpringRate` · 1000 N/m. Parado em chão plano, as *n* molas sustentam o casco:

```math
x_{\text{rest}} = \frac{m \cdot g}{n \cdot k} - \text{MaxTravelDown}
```

No 2A6: 62300 · 9,81 / (14 · 450 000) = **0,097 m** de compressão, contados a partir do curso todo para baixo.

Duas rodadas confirmam:

| `MaxTravelDown` | Curso parado previsto | Curso parado medido | Sessão |
| --- | --- | --- | --- |
| 0.15 | −53,0 mm (casco 5,3 cm alto) | −52,4 … −53,4 mm, média das 7 rodas, em 13 de 14 quadros parados (9 deles uma única pose dormindo em −53,0; o 14º, uma parada momentânea entre a ré e a arrancada, leu −42,4) | Rodada 1 |
| 0.10 | −3,0 mm | −3,0 mm, mediana da média por amostra | Rodada 3 e as sessões das 23:58 e 00:12 |

Então a regra é:

```math
\text{MaxTravelDown} \approx \frac{m \cdot g}{n \cdot k}
```

Com ela, o casco fica na altura que você modelou. Um `MaxTravelDown` maior levanta o casco, um menor abaixa, pela diferença.

**Por roda.** Com `MaxTravelDown` 0.10 e `CenterOfMass 0 1 0.46`, o curso parado das rodas de apoio em chão plano é (256 amostras, igual nas duas lagartas):

| Roda de apoio | 1 (frente) | 2 | 3 | 4 | 5 | 6 | 7 (trás) |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Medido (mediana) | −1 mm | −2 mm | −2 mm | −3 mm | −4 mm | −4 mm | −5 mm |
| Modelo da mola | −1,0 mm | −1,7 mm | −2,3 mm | −2,9 mm | −3,6 mm | −4,3 mm | −5,1 mm |

A diferença de 4 mm entre a frente e a traseira é a inclinação de −0,049° deixada pelo centro de massa (veja abaixo).

**A massa do `RigidBody` sozinha bate.** As duas previsões usam só os 62300 kg. Somar as massas das 14 rodas de apoio (14 × 110 kg) preveria 2,4 mm a mais de compressão, o que não foi visto. Então as massas dos modelos de roda parecem não carregar as molas (deduzido pela coincidência).

**Em chão irregular a média se mantém.** Parado em chão inclinado na rodada 1, rodas isoladas foram de −26,7 a −88,1 mm enquanto a média das 7 rodas de uma lagarta ficou em −52,4 … −53,4 mm. Para conferir a altura do casco, sempre tire a média de todas as rodas de apoio de uma lagarta.

**Se você copiou a primeira edição.** Os valores do Leopard 2A7 da primeira edição (`Mass` 64500, `SpringRate` 450, `MaxTravelDown` 0.15) dão 64500 · 9,81 / (14 · 450 000) = 0,100 m de compressão contra 0,15 m de curso para baixo: o casco fica cerca de 5 cm alto (calculado; a altura do 2A7 não foi medida). Ponha `MaxTravelDown` 0.10.

### Dimensionando a suspensão

1. Escolha o `MaxTravelUp` pelo espaço acima da roda no seu modelo. O 2A6 usa 0,30 m.
2. Escolha a mola ou o curso para baixo. O outro sai da conta:

   ```math
   k = \frac{m \cdot g}{n \cdot \text{MaxTravelDown}}, \qquad \text{SpringRate} = \frac{k}{1000}
   ```

   2A6: 62300 · 9,81 / (14 · 0,10) = 436 545 N/m, então `SpringRate` 437 deixaria o casco a 0,1 mm da altura do modelo. O prefab mantém 450 e fica 3 mm alto (curso parado −3 mm).
3. Escolha os amortecedores como uma fração ζ do amortecimento crítico, usando a carga de uma roda (m / n):

   ```math
   c_{\text{crit}} = 2\sqrt{k \cdot \frac{m}{n}}, \qquad c = \zeta \cdot c_{\text{crit}}
   ```

   2A6: c\_crit = 2 · √(450 000 · 4450) = 89 500 N·s/m. `CompressionDamper` 25000 dá ζ = 0,28 e `RelaxationDamper` 35000 dá ζ = 0,39. A frequência vertical sem amortecimento é √(k / (m/n)) / 2π = 1,60 Hz. São valores calculados; o intervalo de 2 s da telemetria não consegue confirmá-los.
4. Copie o `MaxTravelDown` e o `MaxTravelUp` para os atributos `m_fMaxTravelDown` e `m_fMaxTravelUp` do `LEO_RunningGearComponent`, e a altura modelada do centro da roda para o `m_fRestHeight` (usado se um osso de roda não puder ser lido; o script pega as alturas dos ossos) e para o `m_fWheelRestHeight` da telemetria ([capítulo 11](11-scripts.md), [capítulo 14](14-running-gear.md)).
5. Feche o mundo, recarregue e abra de novo (veja [o cache do prefab](#edições-do-prefab-e-o-mundo-aberto)), dirija, e confira se a média do `travel_mm` parado fica perto de 0.
6. Se você usa a corrente de elos nativa, confira de novo a folga dela: o comprimento da corrente é fixo, e o caminho em volta das rodas cresce quando as rodas caem ([capítulo 15](15-track-links.md)).

**O `MaxTravelDown` não dá mais curso para baixo abaixo da posição parada.** Como a mola fica solta no curso todo para baixo, o curso que sobra abaixo da posição parada é sempre m·g / (n·k), qualquer que seja o `MaxTravelDown`: 97 mm no 2A6. Com 0.15 o casco só ficou mais alto; as rodas continuavam podendo cair só 97 mm abaixo da posição parada. Só uma mola mais mole dá às rodas mais espaço para cair (calculado pelo modelo da mola). O espaço que sobra acima da posição parada é `MaxTravelUp` − x\_rest: 0,303 m no 2A6.

### Centro de massa

**O `CenterOfMass` é um deslocamento, não uma posição.** O executável descreve esse atributo do `RigidBody` como "Offset of center of mass. Relative to one defined in model (if applicable)." A opção de importação do modelo `COMAutocenter` é descrita como "Auto center of center of mass".

**O modelo do 2A6 define o próprio centro de massa.** O `.fbx` do casco contém um objeto vazio chamado `COM_hull`, posto pelo script de montagem no centroide sólido do colisor do casco `UCX_hull_01`. No espaço do casco (plano do chão em y = 0) ele fica em x 0,0002, y 1,168, z −0,381.

**Centro de massa da física medido = `COM_hull` + `CenterOfMass`.** O `WheelGetPosition(track, wheel, 0)` devolve o centro da roda no curso todo para baixo em relação ao centro de massa da física ([capítulo 14](14-running-gear.md)). Na rodada 1 ele devolveu (−1,32898; −1,89918; z\_osso − 0,07906) em todas as rodas. O centro no curso todo para baixo está em y = 0,41858 − 0,15 = 0,26858, então o centro de massa está em y = 2,16776 e z = 0,07906. `COM_hull` + (0, 1, 0,46) = (0,0002; 2,1678; 0,0791) bate em 0,01 mm.

Então o `CenterOfMass 0 1 0.46` põe o centro de massa do 2A6 **2,17 m acima do chão**, cerca de 0,47 m acima do teto do casco (cerca de 1,70 m). O `1` em Y o sobe 1 m acima do centroide do colisor. Os primeiros valores testados no 2A6 (`0 1.206 -0.478`) e o conselho da primeira edição ("baixo, perto do piso do casco: `0 1 -0.3`") tratavam o atributo como posição. Não é.

O que isso quer dizer para o seu veículo:

- Se o seu modelo tem um objeto `COM_`, o `CenterOfMass` afasta o centro de massa dele. Comece com `0 0 0`.
- Sem um objeto `COM_`, o ponto de referência que o motor usa não é conhecido (pergunta em aberto).
- No modelo da mola, o `Y` do deslocamento não muda a inclinação parado em chão plano. Ele muda o agachamento, o mergulho e o capotamento (calculado, não testado; veja abaixo).
- O `z` define a inclinação parado. Use-o para nivelar o casco.

#### Nivelando a inclinação

Em chão plano as molas equilibram o peso e o momento dele. Com *z* para a frente, a inclinação parado (positiva com o nariz para cima, em radianos) é:

```math
\theta = -\frac{M \cdot g \cdot (z_{\text{COM}} - \bar z)}{\sum_i k \cdot (z_i - \bar z)^2}
```

A soma passa por todas as rodas de apoio das duas lagartas; *z\_i* são as posições das rodas e *z̄* a média delas. A sensibilidade é:

```math
\left|\frac{d\theta}{dz_{\text{COM}}}\right| = \frac{M \cdot g}{\sum_i k \cdot (z_i - \bar z)^2}
```

2A6: rodas de apoio em z = 2,478, 1,662, 0,889, 0,160, −0,722, −1,596, −2,478 m, então *z̄* = 0,0563 m e Σ k·(z\_i − z̄)² = 1,701 · 10⁷ N·m/rad. Isso dá **2,06° por metro** de deslocamento do centro de massa (calculado). Os dois pontos medidos abaixo dão 2,18°/m.

| `CenterOfMass` | z do centro de massa | Inclinação pela fórmula | Inclinação medida |
| --- | --- | --- | --- |
| `0 1 0` | −0,381 m | +0,900° (nariz para cima) | +0,954° |
| `0 1 0.46` (atual) | +0,079 m | −0,047° | −0,049° (mediana de 256 amostras paradas em chão plano) |
| `0 1 0.437` | +0,056 m | 0,000° | não testado |

Para nivelar o seu próprio casco:

1. Acrescente o `LEO_TelemetryComponent` ([capítulo 11](11-scripts.md)). Ele registra `LEOTEL pose pitch=… roll=…` a cada 0,5 s, mesmo com o motor desligado. A inclinação é positiva com o nariz para cima.
2. Estacione em chão plano e leia a inclinação.
3. Vire o tanque 180° e leia de novo. O 2A6 leu +0,954° nos dois sentidos, o que descarta o terreno.
4. Mova o z do `CenterOfMass` em inclinação / sensibilidade: para a frente (+z) para abaixar o nariz. Use o seu próprio Σ k·(z\_i − z̄)², ou comece com 2°/m para um veículo de massa e espaçamento de rodas parecidos.
5. Feche o mundo, recarregue, abra de novo e leia a inclinação outra vez.

O diagnóstico da depuração do veículo tem uma entrada *Show CoM* (Vehicles > Tracked). Ela não foi usada nestes testes ([capítulo 16](16-debugging.md)).

#### Altura e capotamento

Um centro de massa alto faz o casco inclinar mais acelerando e freando, e facilita o capotamento. Uma medida estática simples é metade da distância entre as rodas de apoio esquerdas e direitas dividida pela altura do centro de massa. No 2A6: 1,3288 / 2,168 = 0,61 g. Com deslocamento Y de 0 (centro de massa a 1,168 m) seria 1,14 g (calculado). Na rodada 1 o tanque capotou a 33–41 km/h em 4ª: a rolagem cresceu até −11° em cerca de 3 s, um impacto levou a velocidade de 34,3 a 13,5 km/h em 0,5 s, e o casco chegou a 177,5° em 2,5 s. O centro de massa alto é um fator de risco calculado nesse capotamento, não uma causa provada.

**Em aberto.** O deslocamento Y não foi reajustado. O `CenterOfMass 0 0 0.437` (centro de massa a 1,17 m, casco nivelado) é o próximo teste óbvio.

### Agachamento, mergulho e comportamento no ar

#### Agachamento e mergulho (medido)

Método: nas sessões das 23:18, 23:58 e 00:12, a inclinação em relação ao chão foi tirada da inclinação frente-trás dos cursos das 7 rodas de apoio (`LEOSUSP`), o que elimina a inclinação do terreno. A aceleração longitudinal veio da diferença de velocidade em ±0,5 s (`LEOTEL`). 395 amostras, `CenterOfMass 0 1 0.46`, `MaxTravelDown` 0.10.

| Aceleração | Amostras | Inclinação (nariz para cima +) |
| --- | --- | --- |
| +2,66 m/s² | 9 | +0,94° |
| +1,40 m/s² | 24 | +0,42° |
| +0,58 m/s² | 52 | +0,21° |
| 0 | 262 | −0,05° |
| −0,51 m/s² | 32 | −0,08° |
| −1,53 m/s² | 6 | −0,35° |
| −2,93 m/s² | 9 | −0,44° |

A inclinação geral é de 0,20° por m/s² (r = 0,56). Os eventos isolados mais fortes:

| Evento | Inclinação | Rodas da frente | Rodas de trás |
| --- | --- | --- | --- |
| Arrancada em 1ª | +1,27 … +1,43° | −57 … −64 mm | +53 … +61 mm |
| Freio 6,0 a −4,3 m/s² (00:01:45) | −1,85° | +72 mm | −88 mm |

Na parada brusca as rodas de trás chegaram a −88 mm, a 12 mm do curso todo para baixo (−100 mm), onde a mola não dá força nenhuma. Uma parada mais forte deixaria as rodas de trás penduradas (calculado pelo modelo da mola).

**O que esperar (calculado).** Se as forças de tração e de freio agem no chão, uma aceleração constante *a* inclina o casco cerca de:

```math
\theta \approx \frac{M \cdot a \cdot h}{\sum_i k \cdot (z_i - \bar z)^2}
```

em que *h* é a altura do centro de massa. Com h = 2,17 m isso dá 0,45° por m/s²; com h = 1,17 m (deslocamento Y 0) daria 0,25° por m/s². A parada brusca acima (1,85° a 4,3 m/s², ou seja 0,43° por m/s²) tem o tamanho da estimativa com 2,17 m. A inclinação média é menor. Onde o motor aplica as forças da esteira, e por que a inclinação média é menor, não foi conferido (em aberto).

A depuração do veículo mostra a inclinação ao vivo no painel Debug como `P`. O `P` é positivo com o nariz para baixo, o contrário do `LEOTEL pose` (observado nos vídeos: negativo na arrancada, positivo freando). A unidade parece ser radianos (o rumo `Y` dá a volta em ±π), mas isso não está confirmado.

![Leopard 2A6 freando forte a partir de 22 km/h com a depuração do veículo ligada; o valor P do diagnóstico sobe enquanto o nariz desce](../images/braking-dive.gif)

*Mergulho na frenagem. Rodando solto em 3ª a cerca de 22 km/h, depois Brake 6.0 em 10,83 s: de 22,0 a 0,4 km/h em cerca de 1,2 s (cerca de 5 m/s², G-Force 1,1–1,2). O `P` do diagnóstico sobe para 0,04–0,05 (nariz para baixo) em 11,2–11,3 s e volta para −0,01 depois da parada (12,5 s). Na foto as esteiras andam a 1,535 m/s enquanto o casco faz 14,4 km/h: as esteiras derrapam. Vídeo de teste B (06/10/2026), 8,4–12,6 s · [foto em tamanho cheio](../images/braking-dive.jpg) em 11,2 s · Modelo: buh-late, CC BY 4.0, modificado ([créditos](../../CREDITS.md)).*

![Leopard 2A6 acelerando em 1ª e passando para a 2ª com a depuração do veículo ligada](../images/launch-upshifts.gif)

*Agachamento na arrancada. O tanque sai em 1ª e depois passa para a 2ª (vista 3/4 traseira direita). Em 94,5 s ele acelera a cerca de 3 m/s² (de 5,4 a 11,0 km/h em 0,5 s, lido no painel) e o `P` do diagnóstico lê −0,03 (nariz para cima). O `P` volta a 0,00 em 95,5 s, quando a 1ª chega ao limite em 13,6 km/h (2926 rpm, torque cortado para 196 Nm). O agachamento é pequeno na tela; leia-o no painel Debug. Vídeo de teste A (06/10/2026), 94,5–99,0 s · [foto em tamanho cheio](../images/launch-upshifts.jpg) em 97,0 s · Modelo: buh-late, CC BY 4.0, modificado ([créditos](../../CREDITS.md)).*

#### No ar e em terreno irregular (medido)

| Evento | O que a telemetria mostrou |
| --- | --- |
| Capotamento, rodada 1 (22:33) | Depois de capotar o tanque ficou de teto no chão (rolagem 168,8°) por cerca de 8,5 s, dormindo, sem nenhuma roda em contato e o motor a 700 rpm, até ser desvirado |
| Minas, rodada 4 (23:20:54–58) | Inclinação −5,5 … −10,1°, rolagem até 10,8°, várias rodas no ar, curso pelo contato +0,348 m (acima do limite de +0,30) e +0,273 m |
| Pedras, 23:59 | A 38 km/h o tanque subiu em pedras e ficou encalhado por cerca de 28 s: inclinação −7 … −13°, rolagem 10 … 18°, só 1–5 rodas de apoio por lado em contato, cerca de 0 km/h com acelerador. Saiu a 5–12 km/h |

As rodas de apoio só sentem o chão embaixo dos raios delas. Uma pedra entre dois raios só encontra os colisores do casco; o encalhe das 23:59 combina com isso (deduzido).

### Lendo a telemetria da suspensão

O `LEO_TelemetryComponent` ([capítulo 11](11-scripts.md)) escreve duas linhas que importam aqui. O log é `Documents\My Games\ArmaReforgerWorkbench\logs\logs_<data>\console.log`.

| Linha | Quando | Campos |
| --- | --- | --- |
| `LEOTEL pose pitch=… roll=…` | A cada 0,5 s, motor ligado ou desligado | Inclinação e rolagem do casco em graus; inclinação positiva com o nariz para cima |
| `LEOSUSP track=l\|r worst_mm=… diff_mm=… travel_mm=… spin=…` | A cada 2 s com o motor ligado | `travel_mm`: por roda de apoio, contato + `Radius` − `m_fWheelRestHeight`, positivo para cima, sem limite, `air` sem contato. `diff_mm`: osso animado menos simulação. `spin`: ângulos dos ossos |

Como são os valores bons em chão plano, parado:

- A média do `travel_mm` de uma lagarta fica perto de 0 (2A6: −3 mm).
- A variação do `travel_mm` da frente para trás é de poucos milímetros (2A6: −1 a −5 mm), e a inclinação fica perto de 0.
- Com o `LEO_RunningGearComponent`, o `diff_mm` fica em 0–3 mm ([capítulo 14](14-running-gear.md)).

**Linhas idênticas repetidas querem dizer que o corpo está dormindo.** Um tanque parado vai dormir (`RigidBody` `LinearSleepingThreashold` 0.4, `AngularSleepingThreashold` 0.5). Na rodada 1, 9 amostras `LEOSUSP` seguidas e as linhas de pose foram idênticas bit a bit. Conte uma série assim como uma amostra só.

O `travel_mm` usa uma única altura modelada para todas as rodas de apoio (`m_fWheelRestHeight` 0.4186 no 2A6). Se as suas rodas de apoio ficam em alturas diferentes, os valores ficam deslocados por roda.

### Edições do prefab e o mundo aberto

Um veículo num mundo que já está aberto mantém os valores de simulação com que foi carregado. Na rodada 2 (22:55) o prefab já dizia `MaxTravelDown` 0.10, mas o tanque continuava parado em −51 … −54 mm, o comportamento do 0.15. Só depois que o mundo foi fechado e recarregado (rodada 3, 23:18) o curso parado passou a −3 mm.

Depois de cada mudança nos valores de `Simulation` ou `RigidBody`:

1. Salve o prefab.
2. Feche o mundo no World Editor.
3. Recarregue os scripts sem mundo aberto. Recarregar com um mundo aberto dispara o assert do Diag "Resources are leaking! Check log!".
4. Abra o mundo de novo: clique duas vezes nele no Resource Browser.
5. Entre no *Play* e confira o novo valor na telemetria (por exemplo, a média do `travel_mm` parado).

### Sintomas e correções

| Sintoma | Causa | Correção |
| --- | --- | --- |
| Casco alto parado | `MaxTravelDown` maior que m·g / (n·k) | Ponha `MaxTravelDown` ≈ m·g / (n·k) (2A6: 0.15 → 0.10) |
| Casco baixo, rodas perto do topo do curso | `MaxTravelDown` menor que m·g / (n·k), ou `SpringRate` mole demais (é kN/m) | A mesma regra; confira a unidade |
| Casco flutua ou afunda uma medida constante; os círculos das rodas na depuração não chegam ao fundo da esteira | O `RoadWheel.Radius` não é a distância do osso à face da esteira em contato com o chão | `Radius` = altura do osso acima do fundo da esteira |
| Casco inclinado parado | z do `CenterOfMass` | Desloque o z em inclinação / sensibilidade (2A6: cerca de 2,1°/m) |
| Agachamento e mergulho fortes; o tanque capota fácil | Provavelmente um centro de massa alto: o Y do `CenterOfMass` é somado ao centro de massa do modelo | Baixe o deslocamento Y (ainda não testado no 2A6) |
| Mudança no prefab sem efeito no *Play* | Mundo ainda aberto com a simulação antiga | Feche o mundo, recarregue os scripts, abra de novo |
| As rodas pendem no ar | Sem contato com o chão: curso todo para baixo | Esperado |
| A telemetria repete os mesmos valores | O corpo rígido está dormindo | Esperado; dirija para acordá-lo |

### Perguntas em aberto

- Que força a simulação aplica quando o contato fica acima de +`MaxTravelUp`?
- Que centro de massa o motor usa quando o modelo não tem objeto `COM_`: o centroide do colisor ou a origem do modelo? A massa da torre entra na conta? (O `WheelGetPosition` bateu exatamente com `COM_hull` + deslocamento.)
- Como o agachamento, o mergulho e o capotamento mudam com um Y menor no `CenterOfMass` (por exemplo `0 0 0.437`)?
- Onde a simulação aplica as forças da esteira, e por que a inclinação média do agachamento (0,20° por m/s²) fica abaixo da estimativa?
- As razões de amortecimento (0,28 e 0,39, calculadas) são o que a simulação mostra? Uma telemetria mais rápida diria.
- Que unidade o parâmetro `displacement` do `WheelGetPosition` usa? Só o 0 foi testado.

---

[← Solução de problemas](12-troubleshooting.md) · [Índice](../../README.pt-BR.md) · [Rodas de apoio, roda-guia, roda motriz e roletes →](14-running-gear.md)
