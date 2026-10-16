> 🇧🇷 Português · [🇬🇧 English](../en/06-converting.md)

[← Exemplos em vídeo](05-video-examples.md) · [Índice](../../README.pt-BR.md) · [Referência de atributos →](07-attribute-reference.md)

# Convertendo um veículo existente

A maioria dos tanques de mods usa a simulação de rodas (`VehicleWheeledSimulation`) com muitos eixos. Converter para o sistema de esteira nativo é trocar a base, traduzir os blocos e refazer o que a versão de rodas resolvia de outro jeito: a altura do casco, a animação das rodas e as esteiras.

Converta nesta ordem: primeiro faça o veículo andar e ficar na altura certa (passos 1–6), depois acrescente a animação (passo 7). Os valores citados são os do Leopard 2A6 (o [Passo a passo do zero](04-from-scratch.md) tem os blocos completos).

### 1. Troque a herança

Faça backup do prefab. Depois mude a primeira linha para herdar do `TrackedVehicle_Base.et`:

```
Vehicle : "{0608D8FA71FD3433}Prefabs/Vehicles/Core/TrackedVehicle_Base.et" {
```

Se o prefab herdava de um veículo vanilla (por exemplo `LAV25_base.et`), ele perde tudo o que vinha de lá. Copie para o seu prefab os componentes que você ainda quer.

### 2. Remova os componentes do carro

As duas bases usam IDs diferentes para alguns componentes. Se você só trocar a herança, todo bloco que o seu prefab declara com um ID da versão de rodas fica como componente extra: dois nós, dois controladores, dois componentes de animação procedural.

| Remover (rodas) | Fica no lugar (esteira, herdado) |
| --- | --- |
| `VehicleWheeledSimulation "{731B26FCA2F19855}"` | `VehicleTrackedSimulation "{6160DE6434513C4D}"` |
| `BaseVehicleNodeComponent "{20FB66C5B2237133}"` | `BaseVehicleNodeComponent "{5D6501223785D0E7}"` |
| `SCR_CarControllerComponent` (dentro do nó) | `SCR_TrackedControllerComponent "{60F8D3F71B2F1D79}"` |
| `NwkCarMovementComponent "{5D6CA5AFEC980F35}"` | `NwkTrackedMovementComponent "{5D6CA5AFEC980F35}"` |
| `CarProcAnimComponent "{A201E6A6F82F2CDA}"` (o ID da base de rodas) com os `ProcAnimParams` das rodas | `CarProcAnimComponent "{5D6501223785D009}"`, vazio. Deixe-o vazio (passo 7) |

O movimento em rede usa o **mesmo ID** nas duas bases, mas a classe é outra. Apague o bloco do carro para ele não sobrescrever o da esteira.

Se o seu nó antigo tinha outros componentes próprios, mova-os para o nó `{5D6501223785D0E7}`.

### 3. Traduza os atributos

| Simulação de rodas | Esteira nativa | Observação |
| --- | --- | --- |
| `Engine`, `Clutch`, `Gearbox` | Os mesmos blocos | Mesmos nomes. Mas veja a [seção de bugs](09-bugs.md): a saída do câmbio e a embreagem funcionam de outro jeito |
| `Axle` → `Wheel { Radius, BrakeTorque }` | `RoadWheel { Radius, BrakeTorque, Mass }` | Um único modelo para todas as rodas de apoio. `Radius` = distância do centro da roda até a face de contato da esteira ([capítulo 3](03-model-requirements.md#o-que-o-osso-de-uma-peça-significa-para-a-simulação)) |
| `Axle` → `Suspension { ... }` | `RoadWheel` → `Suspension { ... }` | Mesma classe e mesmos nomes de atributo, mas recalcule os valores (abaixo) |
| `MaxSteeringAngle 25` | `MaxSteeringAngle 0` | Um veículo de esteira vira pela diferença de velocidade |
| `WheelPosition Wheel_L01 { PivotID "v_wheel_l01" }` | `TrackPart Wheel_L01 { Pivot Pivot "{id}" { Bone "v_wheel_l01" } }` | Os ossos antigos funcionam sem mudar para andar |
| — | Modelos `Idler`, `Sprocket` e, com roletes de retorno, `Roller`, mais um `Type` nessas entradas `TrackPart` | Novo; veja o passo 4 |
| Diferencial por eixo | `Differential TripleDifferential` | `Ratio`, `SteeringRatio`, `NeutralSteeringRatio` |
| Velocidade pelo raio da roda | Velocidade pelo raio da **roda motriz** | Recalcule o `Ratio` ([seção de ajuste](10-tuning.md)) |

**Recalcule a suspensão.** Na simulação de esteira, `SpringRate` está em N/mm, ou seja, kN/m, e a mola fica livre (força zero) no curso todo para baixo (descrição no executável e medidas no 2A6). Então o `MaxTravelDown` define a altura do casco. Use a compressão estática:

```math
\text{MaxTravelDown} \approx \frac{M \cdot g}{n \cdot k}
```

com *n* o número de rodas de apoio das duas lagartas e *k* = `SpringRate` × 1000 N/m. 2A6: 62300 · 9,81 / (14 · 450 000) = 0,097 m, então `MaxTravelDown` 0.10. Com 0.15 o casco do 2A6 ficou 5 cm alto (medido). Valores copiados da versão de rodas podem deixar o casco alto ou baixo pela diferença. Se a simulação de rodas usa o mesmo modelo de mola não foi conferido. Detalhes: [Suspensão e altura do casco](13-suspension.md).

**Confira também o centro de massa.** O `CenterOfMass` do `RigidBody` é um deslocamento a partir do ponto de centro de massa do próprio modelo, se o modelo tiver um, não uma posição absoluta ([capítulo 13](13-suspension.md#centro-de-massa)).

### 4. Escolha a roda-guia e a roda motriz e ordene as peças

Mods com rodas de mentira costumam ter uma fileira `v_wheel_l01`…`l09` sem distinção. Marque a roda certa em cada ponta:

- Leopard, T-72 e Abrams têm a roda motriz **atrás** e a roda-guia na frente.
- O M113 e vários blindados leves têm a roda motriz **na frente**.

Depois liste cada lagarta nesta ordem:

1. As rodas de apoio, da frente para trás, sem `Type` (posições 0…n−1 da lista).
2. A roda-guia (`Type Idler`).
3. A roda motriz (`Type Sprocket`).
4. Os roletes de retorno, se houver (`Type "Return Roller"`).

Use a mesma sequência nos dois lados. Exemplo para uma fileira `v_wheel_l01` (roda-guia, frente) … `v_wheel_l09` (roda motriz, trás): liste primeiro `v_wheel_l02`…`l08`, depois `v_wheel_l01` como roda-guia e `v_wheel_l09` como roda motriz. Para andar, os nomes dos ossos podem ficar como estão; o script do trem de rolamento do passo 7 espera outros nomes.

Por que a ordem importa:

- A corrente de elos nativa anda à velocidade angular da peça 1 vezes o raio da peça 0, então as duas precisam ser rodas de apoio (engenharia reversa do executável 1.8.0.13).
- A API de script e os scripts deste guia endereçam as peças como (lagarta, posição na lista). `WheelCount()` devolve as peças de uma lagarta: 13 no 2A6.
- O carregador quer a roda-guia e a roda motriz na mesma posição nas duas lagartas, e a simulação guarda uma só contagem de peças para as duas (engenharia reversa).
- A física de condução em si funcionou com as duas ordens (a versão anterior, do 2A7, listava a roda-guia primeiro).

**Roda motriz na frente.** A ordem de peças acima continua a mesma. Duas coisas não foram testadas com roda motriz na frente:

- **A corrente por script.** O `LEO_TrackChain` monta a esteira no sentido horário vista pela direita, começando pela roda motriz: em `LEO_RunningGearComponent.CreateChains` o primeiro círculo é a roda motriz (atrás), depois os roletes de retorno de trás para a frente, depois a roda-guia (frente), depois as rodas de apoio da frente para trás. Com roda motriz na frente, troque ali os círculos da roda motriz e da roda-guia (tanto a posição de repouso quanto o índice do raio). O código das rodas de apoio e dos roletes não precisa mudar.
- **A corda nativa.** A pega da roda motriz solta um ponto quando ele fica atrás ou abaixo do centro da roda motriz (engenharia reversa). Como isso funciona com roda motriz na frente não foi conferido; a corda só rodou com a roda motriz traseira do Leopard.

### 5. Limpe as sobras da versão de rodas

| Sobra | O que fazer |
| --- | --- |
| `SCR_WheelHitZone` com `m_iWheelId` | O próprio `TrackedVehicle_Base` traz quatro (IDs 0 a 3), herdados do carro. A simulação numera as rodas por lagarta, como (lagarta, posição na lista), com `WheelCount()` peças por lagarta (13 no 2A6). Como um `m_iWheelId` plano corresponde a isso não foi verificado, e reordenar a lista de `TrackPart` muda as posições |
| Parâmetros de roda do `CarProcAnimComponent` | Remova (passo 2 e passo 7) |
| A sua própria animação de esteira (rolagem de UV no material ou entidades de esteira separadas movidas por script) | Foi feita para a simulação de rodas e não foi testada na de esteira. Remova se for usar as correntes de elos do passo 7 |
| `ChimeraAIPathfindingComponent`, `ChimeraAIVehicleControlComponent` | O `ChimeraAIVehicleControlComponent` também não existe na base de esteira; o `ChimeraAIPathfindingComponent` já vem do `Vehicle_Base.et`. A IA dirigindo um tanque nativo não foi testada |

### 6. Proteção contra outros mods

O RHS-StatusQuo sobrescreve o `TrackedVehicle_Base.et` vanilla e **desliga** (`Enabled 0`) três componentes: `VehicleTrackedSimulation`, `SCR_TrackedControllerComponent` e `NwkTrackedMovementComponent`. Com esse mod carregado, o seu tanque herda os componentes desligados.

Precaução (não testada): escreva `Enabled 1` nesses três componentes, dentro do seu prefab. O valor do filho vale mais que o do pai.

### 7. Animação da suspensão, das rodas e das esteiras

Faça isto depois de o veículo andar. A simulação de esteira calcula o curso e o giro de cada peça, mas na 1.8.0.13 ela não anima o modelo por você.

**Deixe o `CarProcAnimComponent` vazio.** Num veículo de esteira ele entrega à maioria dos ossos das rodas os sinais de outra roda. No 2A6, com `wheel_suspension.pap` nas rodas de apoio, só a roda dianteira esquerda recebeu os próprios sinais (medido com a telemetria `LEOSUSP`). Uma segunda rodada com os parâmetros intercalados entre esquerda e direita mostrou o mesmo tipo de troca, mas essa rodada não tinha recarregado o mundo, então a ordem dos parâmetros não está totalmente descartada (em aberto). Não leve os `ProcAnimParams` de roda da versão de rodas.

![Lado direito do Leopard 2A6 parado: a roda de apoio da frente fica sobre os elos enquanto as rodas 2 a 7 flutuam acima deles](../images/before-wheels-floating.jpg)

*Vídeo de diagnóstico 2 (05/10/2026, trecho das 22:56), 0:09, antes da correção: os ossos das rodas movidos pelo `CarProcAnimComponent`, motor em marcha lenta. A roda de apoio da frente (à direita da imagem) fica sobre os elos. As rodas de apoio 2–7 flutuam acima deles, porque recebiam os sinais de outra peça (telemetria: 51–54 mm acima das rodas simuladas). Modelo "Leopard 2 A6" de buh-late, CC BY 4.0, modificado ([créditos](../../CREDITS.md)).*

**Mova os ossos por script.** O `LEO_RunningGearComponent` lê cada peça da simulação por (lagarta, peça) e ajusta os ossos de suspensão e de giro a cada frame ([Rodas de apoio, roda-guia, roda motriz e roletes](14-running-gear.md)). Ele precisa de:

- um osso de suspensão acima de cada osso de roda de apoio, os dois com orientação de repouso identidade ([capítulo 3](03-model-requirements.md#rig-de-suspensão-das-rodas-de-apoio)). Um tanque de rodas muitas vezes já tem um rig de três ossos do `wheel_suspension.pap` (roda, suspensão, esterçamento). Confira se o osso de suspensão é pai do osso da roda;
- os nomes de osso que ele procura: `v_suspension_<s>0<k>`, `v_wheel_<s>0<k>`, `v_idler_<s>`, `v_sprocket_<s>`, `v_roller_<s>0<k>` com *s* = `l` ou `r`. Se os seus ossos usam outro padrão (veículos de rodas vanilla usam nomes como `v_wheel_L01`), renomeie ou mude os textos de nome no script;
- a ordem de peças do passo 4;
- os atributos dele ajustados para o seu tanque: número de rodas de apoio e de roletes, raios visuais, altura de repouso e cursos ([passo 11 do passo a passo](04-from-scratch.md#11-anime-o-trem-de-rolamento)).

**Esteiras.** A versão de rodas mostra uma esteira estática ou a sua própria animação de esteira. Na base de esteira você tem duas opções ([Elos da esteira](15-track-links.md)):

| Opção | O que você precisa | Comportamento |
| --- | --- | --- |
| Corda da corrente nativa | Um modelo de elo seu (`TrackSegment`) e os atributos da corrente. O vanilla não traz modelo de elo: o executável só cita um elo de M113 de desenvolvimento que não está nos dados do jogo | Anda sozinha, mas pende onde tem folga e saiu da roda motriz depois de um capotamento e depois de uma explosão de mina no 2A6. Sem colisão com o chão, sem API de script para reiniciá-la |
| Corrente por script (2A6) | O mesmo modelo de elo e os atributos, mais o `LEO_RunningGearComponent` (`m_bScriptedChain 1`) e o `LEO_TrackBeltComponent` | Segue as rodas; por construção não sai do lugar. Ainda não testada capotando nem com mina |

Mantenha as esteiras estáticas antigas no modelo como malhas com nome: o `LEO_TrackBeltComponent` as esconde quando existem entidades de elo e as mantém se nenhuma corrente aparecer. O [passo 12 do passo a passo](04-from-scratch.md#12-acrescente-os-elos-da-esteira) lista os atributos; atenção à observação sobre roda motriz na frente no passo 4 acima.

**Multiplayer.** Os scripts de animação rodam localmente e não fazem nada num servidor dedicado. Não foram testados nas máquinas dos outros jogadores.

---

[← Exemplos em vídeo](05-video-examples.md) · [Índice](../../README.pt-BR.md) · [Referência de atributos →](07-attribute-reference.md)
