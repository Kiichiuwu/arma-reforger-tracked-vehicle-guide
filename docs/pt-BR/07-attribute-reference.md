> 🇧🇷 Português · [🇬🇧 English](../en/07-attribute-reference.md)

[← Convertendo um veículo existente](06-converting.md) · [Índice](../../README.pt-BR.md) · [Como funciona por dentro →](08-internals.md)

# Referência de atributos

Os nomes, padrões e faixas abaixo foram lidos do código de registro de atributos do executável 1.8.0.13. "—" quer dizer que o padrão não aparece no binário. As descrições entre aspas são os textos de ajuda guardados no executável.

A coluna *Leopard* mostra os valores do exemplo prático, o **Leopard 2A6** (bloco completo em [`examples/leopard2a6`](../../examples/leopard2a6/VehicleTrackedSimulation_Leopard2A6.et.txt)). A coluna *Efeito* diz o que foi medido no jogo, observado nos vídeos de teste ou lido no código. Alguns resultados foram medidos na versão anterior deste guia, do Leopard 2A7, e isso está indicado.

> Quando mudar qualquer um destes valores, feche o mundo no World Editor e abra de novo. Um veículo que já está num mundo aberto mantém os valores antigos da simulação: com `MaxTravelDown` mudado de 0.15 para 0.10 no prefab, o tanque no mundo aberto continuou parado na altura antiga (medido).

### Simulation Tracked (nível de cima)

| Atributo | Padrão | Leopard | Efeito |
| --- | --- | --- | --- |
| `SolverSubsteps` | — (faixa 1–20) | 4 | Subpassos por frame. Com 60 Hz e 4 subpassos, dt = 1/240 s |
| `FrictionCurveLongitudinal` | — | curva plana 1,0 | **Inerte.** Nunca é lida no 1.8.0.13 (o caminho antigo de atrito está desligado por uma constante). Mantenha para o veículo carregar (o [capítulo 18](18-multiplayer.md) tem as medições) |
| `FrictionCurveLateral` | — | curva plana 1,0 | **Obrigatória para carregar, fora isso inerte.** O carregador só confere se ela tem nós (`FrictionCurveLateral - configuration is missing!`) |
| `FrictionBaseLongitudinal` / `FrictionBaseLateral` | — (faixa 0–inf) | herdado | Base de atrito do modelo antigo |
| `V2FrictionBaseLng` | — | 0.95 (vanilla) | Atrito longitudinal do modelo V2 |
| `V2FrictionBaseLat` | — | **1.6** (vanilla 2) | Multiplicador do atrito lateral do modelo V2, o que roda de verdade. A fórmula de pneu do V2 é um pneu de carro fixo (carga nominal 5 kN) usado com 43,7 kN por roda de apoio, então a aderência lateral é baixa; 3 fez o 2A6 capotar em curvas fortes, 1.6 o mantém de pé ([capítulo 10](10-tuning.md)) |
| `V2FrictionSurfaceLng` / `V2FrictionSurfaceLat` | `Tread` | Lat = `NonTread` (vanilla) | Enum `Tread` ou `NonTread` |
| `V2FrictionSteerEnabled` | true | herdado | Reduz o atrito lateral ao virar |
| `V2FrictionSteerFactorCurve` | — | **1 / 0,35 / 0,08** em 0 / 0,5 / 1 (vanilla 1 → 0,2) | Fator de aderência lateral em função do comando de direção. O último valor define o quanto o giro no lugar é pesado |
| `V2FrictionSteerFactorLngCurve` | — | **1 / 0,5 / 1 / 1** em 0 / 0,25 / 0,5 / 1 (vanilla 1 / 0 / 1) | Fator de aderência lateral por índice de `TrackPart`, amostrado em índice / (n − 1) com n = 13 peças: as 7 rodas de apoio cobrem 0–0,5. Com a curva vanilla a roda de apoio traseira **não** tem aderência lateral (engenharia reversa, mapeamento deduzido) |
| `RegenerativeSteering` | — | 1 | O torque vai para a lagarta de fora na curva |
| `NeutralSteering` | — | 1 | No giro no lugar, a lagarta de dentro gira para trás. Observado no 2A6: as duas lagartas giram em sentidos opostos |
| `PivotSteering` | — | 1 | No giro no lugar, a lagarta de dentro trava. **Com 0, a versão anterior, do 2A7, não girava no lugar** |
| `PivotSteeringThreshold` | 0.5 (0–1) | 0.5 | Entrada mínima de esterçamento para o giro no lugar |
| `BrakeSteering` | — | 0 | Freia a lagarta de dentro na curva |
| `BrakeSteeringThreshold` | — | herdado | Entrada mínima para virar no freio |
| `RetarderBaseTorque` | 10 | 0 | Freio que cresce com a velocidade. **Com 10, a versão anterior, do 2A7, não passava de cerca de 54 km/h** |
| `RetarderThrottleSpeed` | 14,5 m/s | 20 | Com 14,5, a versão anterior, do 2A7, travava em cerca de 51 km/h |
| `InertiaOverrideEnabled` / `InertiaOverride` | false / 1 1 1 | herdado | Sobrescreve o tensor de inércia |
| `RaycastLayer` / `LiquidsLayers` | — | `VehicleCast` / `Liquids` (vanilla) | Camadas para os raios das rodas e para a água |

### Motor, embreagem, câmbio e diferencial

| Bloco | Atributo | Padrão | Leopard | Efeito |
| --- | --- | --- | --- | --- |
| `Engine` | `Inertia` | — (>0) | 3 | Inércia do motor. Precisa ser maior que zero |
| `Engine` | `InertiaCoupled` | — | não usado | Inércia extra com a embreagem acoplada |
| `Engine` | `MaxPower` | 100 kW | 1100 | Potência máxima |
| `Engine` | `MaxTorque` | 186 N·m | 4700 | Torque máximo. O painel de depuração mostrou até 4682 N·m no 2A6 |
| `Engine` | `RpmMaxPower` | 5800 | 2600 | RPM da potência máxima |
| `Engine` | `RpmMaxTorque` | 4400 | 1600 | RPM do torque máximo |
| `Engine` | `Steepness` | — (>0) | 50 | Forma da curva de torque. Os motores vanilla usam de 4 a 50 |
| `Engine` | `Friction` | — (>0) | 470 | Atrito interno. **1400 comeu um terço do torque** (versão anterior, do 2A7). Com acelerador em 0, o torque de freio-motor no painel de depuração bateu com −`Friction` · (rpm − `RpmIdle`) / (`RpmMax` − `RpmIdle`), até −471 N·m (observado no 2A6) |
| `Engine` | `BrakingTorqueRatio` | 0.15 | 0.15 | Freio-motor como fração do `MaxTorque`, pelo nome. O efeito dele não foi isolado: o freio-motor visto no 2A6 bateu com o `Friction` |
| `Engine` | `RpmIdle` / `RpmRedline` / `RpmMax` | 1250 / 6250 / 7000 | 700 / 2700 / 3000 | Marcha lenta, corte e limite. A ordem precisa ser crescente |
| `Engine` | `HasGovernor` | — | 1 | Limitador de RPM. Acima de `RpmRedline` o torque despenca: 2462 N·m a 2719 rpm, 86 N·m a 2935 rpm (observado, 2A6, 1ª marcha) |
| `Clutch` | `MaxClutchTorque` | 225 | 9000 | **Ignorado** pela simulação de esteira: a embreagem é só um multiplicador de 0–1 |
| `Gearbox` | `Forward { ... }` | — | 6.0 3.3 1.8 1.0 | Relações das marchas para a frente |
| `Gearbox` | `Reverse` | — | 2.3 | **Uma** relação de ré. O sinal é forçado para negativo (veja [Bugs](09-bugs.md)) |
| `Gearbox` | `Efficiency` | — | 0.9 | Eficiência da transmissão |
| `TripleDifferential` | `Ratio` | — (>0) | 4.6 | Redução final. Com a roda motriz de 0,34 m, a 4ª dá cerca de 72–75 km/h na teoria; a velocidade máxima do 2A6 não foi medida (veja [Ajuste](10-tuning.md)) |
| `TripleDifferential` | `SteeringRatio` | — | 6 | Relação máxima ao virar andando. **Sem ela, a versão anterior, do 2A7, não virava.** Observado no 2A6 com esterçamento total em 1ª: a lagarta de dentro girou a 1/6 da de fora (99,08 contra 16,51 rpm) |
| `TripleDifferential` | `SteeringThreshold` | 0.01 | herdado | Entrada mínima para virar |
| `TripleDifferential` | `NeutralSteeringRatio` | — | 12 | Relação do giro no lugar. Observado no 2A6: num giro no lugar, o rpm *Feedback* do lado da transmissão foi a média dos rpm absolutos das duas lagartas × 12 |

O atributo `Output` dos blocos só liga a cadeia motor → embreagem → câmbio. Não defina `Output` no câmbio; o vanilla também não define.

### Rodas e suspensão

A simulação tem um modelo por tipo de roda: `RoadWheel`, `Sprocket`, `Idler` e `Roller` (o rolete de retorno). Cada `TrackPart` de uma lagarta usa o modelo do seu `Type`. O modelo `Roller` funciona: o 2A6 carrega com 4 roletes de retorno por lado e informa 13 rodas por lagarta. Escreva-o como os outros:

```
    Roller Roller "{EC9CBBD192126CCB}" {
     Mass 20
     Radius 0.152
     BrakeTorque 0
    }
```

| Bloco | Atributo | Padrão | Leopard (roda de apoio / roda motriz / roda-guia / rolete) | Efeito |
| --- | --- | --- | --- | --- |
| os quatro | `Mass` | 12 kg | 110 / 250 / 150 / 20 | Massa da roda. Precisa ser maior que zero (`TrackWheel's mass is incorrect!`) |
| os quatro | `Radius` | 0,6 m | 0.418 / 0.34 / 0.335 / 0.152 | Distância do centro da roda até a **face de contato da esteira**, não até o aro. Roda de apoio: o centro da roda fica `Radius` acima do ponto de contato, então define a altura do casco. Roda motriz: define a velocidade da esteira (velocidade = ω da roda motriz × `Radius`). O raio de cada peça também é um círculo que a corrente de elos nativa envolve. Precisa ser maior que zero |
| os quatro | `BrakeTorque` | 4000 N·m | 6000 / 30000 / 0 / 0 | Torque de freio por roda. Precisa ser zero ou mais |

Como os raios do 2A6 foram escolhidos (medido no modelo): cada raio da simulação é o raio visual do aro mais cerca de uma espessura de elo. Aro da roda de apoio 0,345 + 0,073 = 0,418; roda-guia 0,264 + 0,071 = 0,335; rolete de retorno 0,0847 + 0,067 = 0,152. Na roda motriz, a face interna da esteira fica a 0,262 m do centro; com a sapata de 0,076 m isso dá 0,338, arredondado para 0,34.

#### Suspensão (só dentro do `RoadWheel`)

| Atributo | Padrão | Leopard | Descrição no executável | O que medimos |
| --- | --- | --- | --- | --- |
| `MaxSteeringAngle` | — | 0 | "Unit: degrees. Maximum wheel rotation angle (positive number). For non steering wheels just leave it at 0." | Sempre 0 num veículo de esteira |
| `SpringRate` | 20 | 450 | "Unit: N/mm. Spring force per mm. Tells how much force is needed to compress the spring by one mm." | 450 = 450 kN/m. A mola fica **livre (força zero) no curso todo para baixo** (−`MaxTravelDown`). A compressão estática a partir do curso todo para baixo é m·g/(n·k): 62300 · 9,81 / (14 · 450 000) = 0,097 m. Confirmado por duas rodadas |
| `CompressionDamper` | 2500 (provavelmente; o texto fica ao lado do nome) | 25000 | "Unit: N.s/m. Compression damper force per 1 m/s. Used when suspension is compressing." | Taxa de amortecimento de cerca de 0,28 por roda (calculada, não medida) |
| `RelaxationDamper` | — | 35000 | "Unit: N.s/m. Relaxation damper force per 1 m/s. Used when suspension is decompressing." | Taxa de amortecimento de cerca de 0,39 por roda (calculada, não medida) |
| `MaxTravelUp` | 0.15 | 0.30 | "Unit: m. How much can spring compress its natural length. It's the distance that the suspension can be compressed from modeled position." | Batente acima da posição da roda no modelo. O sinal de suspensão do próprio motor para em exatamente +0,30 |
| `MaxTravelDown` | — | 0.10 | "Unit: m. How much can spring stretch its natural length. It's the distance that the suspension can be expanded from modeled position." | Curso todo para baixo abaixo da posição do modelo, onde a força da mola é zero. Define a altura do casco (veja abaixo). Uma roda sem contato com o chão fica pendurada aqui |
| `RayStartOffsetUp` | — | 0.5 | "Unit: m. Extension of raycast start above top position of suspension. Suspension total length is not affected." | O raio corre ao longo do eixo para baixo do casco. Como começa acima do batente, ele pode achar chão acima de `MaxTravelUp`: o curso tirado do contato chegou a +0,382 m em pedras e minas, enquanto o sinal do motor parou em +0,30 |

O `MaxTravelDown` define onde o tanque fica parado. A força da mola é zero no curso todo para baixo, então a roda para a esta distância da posição do modelo (+ = para cima):

```math
x_{\text{rest}} = \frac{m \cdot g}{n \cdot k} - \text{MaxTravelDown}
```

| `MaxTravelDown` | Curso parado previsto | Curso parado medido (2A6, parado) | Resultado |
| --- | --- | --- | --- |
| 0.15 | −53,0 mm | −53,0 mm (média por quadro das 7 rodas de uma lagarta, 13 de 14 quadros parados) | Casco 5,3 cm mais alto que no modelo |
| 0.10 | −3,0 mm | −3,0 mm (mediana, 256 amostras em chão plano); −1 a −5 mm da frente para trás | Casco na altura do modelo |

Regra: use `MaxTravelDown` ≈ m·g / (n·k), com n o número de rodas de apoio dos **dois** lados e k = `SpringRate` × 1000 N/m. Assim o tanque fica na altura que você modelou. Detalhes e o caimento do casco em [Suspensão e altura do casco](13-suspension.md).

#### Lagartas e TrackParts

| Bloco | Atributo | Padrão | Leopard | Efeito |
| --- | --- | --- | --- | --- |
| `Tracks` | `Track Left`, `Track Right` | — | as duas | Exatamente duas lagartas (`Invalid number of tracks! Must be 2!`). A primeira é a lagarta 0 na API de script e recebe o `TrackOffset1` |
| `Track` → `Wheels` | lista de `TrackPart` | — | 13 por lagarta | Pelo menos 3 por lagarta. A posição nesta lista é o índice da roda na API de script |
| `TrackPart` | `Type` | Road Wheel | veja abaixo | `Road Wheel` (padrão: não escreva `Type`), `Sprocket`, `Idler`, `Return Roller`. Escreva o último entre aspas: `Type "Return Roller"` (aceito) |
| `TrackPart` → `Pivot` | `Bone`, `Position`, `Rotation` | — | `Bone "v_wheel_l01"` … | O osso da roda e um deslocamento opcional. A posição de repouso dele é o centro da roda no modelo: o curso da suspensão é medido a partir dele |

A ordem da lista de `TrackPart` importa (engenharia reversa do executável 1.8.0.13, coerente com todos os testes):

- A roda motriz e a roda-guia precisam estar no mesmo índice nas duas lagartas (`Wheels - sprocket mismatch between tracks`, `... idler ...`).
- Os índices 0 e 1 precisam ser rodas de apoio. A corrente de elos tira a velocidade do raio da peça 0 e a rotação da peça 1. Com a roda-guia primeiro (a ordem antiga do 2A6, quando o `Radius` da roda-guia ainda era 0.264), a corrente andaria a 0,264 / 0,418 = 63% da velocidade das rodas de apoio.
- A ordem do Leopard, a mesma nos dois lados: rodas de apoio `Wheel_X01`…`Wheel_X07` (índice 0–6, da frente para trás), `Idler_X` (7), `Sprocket_X` (8), `Roller_X01`…`Roller_X04` (9–12). O [`LEO_RunningGearComponent`](../../scripts/LEO_RunningGearComponent.c) espera essa ordem.

### Aerodinâmica

| Atributo | Padrão | Leopard | Efeito |
| --- | --- | --- | --- |
| `ReferenceArea` | 3 (vanilla) | 9.5 | Área frontal em m² |
| `DragCoefficient` | 0.5 | 0.9 | Coeficiente de arrasto |

### RigidBody (casco)

| Atributo | Leopard | Descrição no executável | Efeito |
| --- | --- | --- | --- |
| `Mass` | 62300 | — | Massa total em kg |
| `CenterOfMass` | `0 0.2 0.46` | "Offset of center of mass. Relative to one defined in model (if applicable)." | Um **deslocamento**, somado ao ponto `COM_hull` do modelo (1,168 m). Y 0.2 põe o centro de massa a cerca de 1,37 m do chão; com o Y 1 anterior (2,17 m) o 2A6 capotava em curvas fortes depois que as rodas de apoio traseiras passaram a ter aderência lateral. z 0.46 nivela o casco ([capítulo 13](13-suspension.md)) |
| `LinearSleepingThreashold` / `AngularSleepingThreashold` | 0.4 / 0.5 | "Below this linear velocity the body goes to sleep" / "Below this angular velocity the body goes to sleep" | Um tanque parado entra em repouso. A telemetria então repete amostras idênticas; é o repouso, não um registro quebrado. A grafia é a da engine |

### SCR\_TrackedControllerComponent

Fica dentro do `BaseVehicleNodeComponent`. Os padrões vêm do executável. A coluna *Vanilla* mostra o que o `TrackedVehicle_Base.et` muda. O Leopard só muda `ThrottleReverseTarget` (1).

| Atributo | Padrão | Vanilla | Efeito |
| --- | --- | --- | --- |
| `Type` | 0 | — | O código nunca lê |
| `TransmissionRND` | false | — | Câmbio de três posições (R, N, D). Não libera nem bloqueia a ré |
| `SteeringForwardSpeed` / `SteeringCenterSpeed` | — | `19 1.1 100 0.02` / `19 1.1 60 1.8` | Velocidade do esterçamento ao virar e ao centralizar. Formato não decifrado |
| `ThrottleTurbo` | 0.2 | — | Parte do acelerador reservada ao turbo. W sozinho dá 80% (`engineThrust` 0,80, também no 2A6) |
| `ThrottleTurboTime` | 0,15 s | — | Tempo para o turbo entrar |
| `ThrottleReverseTarget` | 0.2 | — | Acelerador usado na ré. **Use 1** (veja [Bugs](09-bugs.md)) |
| `ClutchUncoupleTime` / `ClutchCoupleTime` | 0,25 / 0,35 s | — | Tempo para soltar e para acoplar a embreagem |
| `ClutchCoupleForwardRpm` / `ClutchCoupleReverseRpm` | 0 | — | RPM de acoplamento na saída. Só importa na arrancada |
| `ClutchCoupleUphillFactor` | 1 | — | Ajuste do acoplamento em subida |
| `BrakingCurve` | — | `0.3 0.7 0.6 1.4 1 2.1` | Pares \[fração, segundos\]: 30% em 0,7 s, 60% em 1,4 s, 100% em 2,1 s |
| `BrakeTurboTime` | — | — | Tempo de freio com turbo |
| `RpmSmoothing` | 0.7 | 0.95 | Filtro de RPM para as trocas |
| `SlopeSmoothing` | 0.75 | — | Filtro de inclinação |
| `Latency` | 0 | 0.1 | Atraso da troca |
| `UpShiftFactor` / `UpShiftFactorDownhill` | — | — | Fator de RPM para subir marcha (subida / descida) |
| `DownShiftFactor` / `DownShiftFactorDownhill` | 0.4 / — | — | Fator de RPM para reduzir |
| `PeakTorqueUpshiftingHysteresis` / `...Downshifting...` | -0.14 / -0.2 | — | Histerese em volta do torque máximo |
| `SteeringFactorUpshift` / `SteeringFactorDownshift` | 0.1 / 0.05 | 0.1 / 0.05 | Correção da troca ao virar |
| `TurboShiftFactor` | 1.1 | — | Pontos de troca mais altos com turbo |
| `PeakPowerShiftingModeTurbo` / `...Normal` | true / — | — | Troca na potência máxima |
| `PeakPowerUpshiftingHysteresis` / `...Downshifting...` | 0 / -0.1 | — | Histerese em volta da potência máxima |
| `PeakPowerUpShiftFactorUphill` e afins | 0.7 | — | Fatores do modo de potência máxima |
| `MaxStartupTime`, `MaxStartupAttempts`, `EngineStartupChance`, `ShutdownTime` | — | 0.6 / 10 / 100 / 600 | Partida do motor (comum a todos os veículos) |

### Componente VehicleTrackedSimulation: a corrente de elos (fora do Simulation)

Estes sete atributos montam a corrente de elos da própria engine, uma corda 2D de entidades de elo em volta das rodas (como ela se move: [Como funciona por dentro](08-internals.md#a-corrente-de-elos-o-passo-da-corda)). Eles ficam no próprio componente `VehicleTrackedSimulation`, ao lado do objeto `Simulation Tracked`, não dentro dele. Nenhum tem descrição no executável.

| Atributo | Padrão | Leopard | Efeito |
| --- | --- | --- | --- |
| `TrackSegments` | 0 | 73 | Lido só pelo *Generate track positions*, como o número de pontos a gerar. Em tempo de jogo o número de elos por lagarta é o número de entradas de `TrackPositions` (engenharia reversa). Mantenha os dois iguais |
| `TrackPositions` | — | 73 pares `z y` | Uma lista de pontos (z, y) no espaço do modelo do casco (z para a frente, y para cima), compartilhada pelas duas lagartas. Escreva como uma lista plana `{ z y z y ... }`: os 146 números do 2A6 carregam como 73 pontos. O índice 0 fica no ramo de cima, e o índice precisa crescer **para a frente** ao longo do ramo de cima (o sentido da própria engine). Coloque os pontos no caminho externo em volta dos círculos de raio da simulação |
| `TrackLength` | — | 0.2110 | Distância de repouso entre pontos vizinhos, então a corrente tem N × `TrackLength` de comprimento. Precisa ser um pouco maior que o caminho em volta das rodas. Caminho do 2A6 em repouso: 15,284 m. Com 73 × 0,2101 (o passo real do elo) sobravam só 5 cm, e a corda saiu da roda motriz depois de um capotamento. Com 73 × 0,2110 sobram 12 cm, e a folga pende (sem colisão com o chão). Veja [Elos da esteira](15-track-links.md) |
| `TrackThickness` | — | 0.076 | Guardado, mas nunca lido na 1.8.0.13 (engenharia reversa). O 2A6 registra aqui a espessura da sapata; não tem efeito |
| `TrackOffset1` | — | `-1.3281 0 0` | Deslocamento da corrente da primeira lagarta (`Tracks[0]`, Left no Leopard). O x é a posição lateral do centro da esteira. As duas correntes são o mesmo laço 2D, deslocado para o lado |
| `TrackOffset2` | — | `1.3281 0 0` | O mesmo para a segunda lagarta (Right) |
| `TrackSegment` | nenhum | `{B50AEDC702735F50}Assets/Vehicles/Tracked/Leopard2A6/Leopard2A6_track_link.xob` | O modelo do elo. A engine cria uma entidade filha por ponto por lagarta (146 no 2A6), sem física. Modele o elo como ele fica no ramo de baixo: origem no centro do par de sapatas, face de contato em y = 0, +Y para cima, +Z para a frente. Nenhum elo vem com o jogo: faça o seu ([Elos da esteira](15-track-links.md)) |

A corrente só é criada quando `TrackPositions` tem pelo menos 2 entradas e `TrackLength` é maior que 0. Deixe estes atributos de fora no primeiro teste de direção: a simulação anda sem corrente. Sem corrente, não abra o diagnóstico *Show vehicle debug*: ele derrubou o Workbench (veja [Bugs](09-bugs.md)). Com uma corrente presente, ele rodou sem travar.

A corrente nativa tem gravidade mas não colide com o chão, e nenhuma API de script consegue reiniciá-la. O Leopard a mantém rodando, escondida, e desenha os próprios elos por script ([Elos da esteira](15-track-links.md)).

#### A ação *Generate track positions*

O executável registra *Generate track positions* como a ação de editor 1 da `VehicleTrackedSimulation` (a ação 2 é *Compute local inertia*). O que ela faz (engenharia reversa, não executado):

1. Lê os círculos reais das rodas da lagarta 0, então precisa de um veículo colocado e inicializado, com `TrackSegments` e `TrackLength` já definidos.
2. Coloca `TrackSegments` pontos numa elipse em volta da origem do modelo.
3. Roda 1000 passos de corda de 1/60 s com gravidade e os círculos das rodas.
4. Escreve os pontos em `TrackPositions`, índice 0 no topo, crescendo para a frente.

A ação não foi achada na interface do Workbench durante este trabalho. Talvez esteja no menu de contexto do componente no World Editor: o texto de ajuda do executável para o atributo `MasslessInertia` do componente de física diz que ele "Can be computed via context menu on component." É um palpite e não foi verificado. A lista do 2A6 foi feita offline com a [`tools/trackpositions.py`](../../tools/trackpositions.py) (MIT). O script amostra o círculo de cada `TrackPart` no raio da simulação, tira a envoltória convexa, ordena para o ramo de cima andar para a frente e reamostra em comprimentos de arco iguais. Edite a lista de círculos no topo do script para o seu tanque e rode `python trackpositions.py 73 0.2101` (número de elos, passo do elo).

### API de script: os getters das rodas

A `VehicleTrackedSimulation` expõe o estado de cada roda. Os índices das rodas são (lagarta, peça): a lagarta 0 é o primeiro `Track` em `Tracks`; a peça é a posição na lista `Wheels` dela. O 2A6 anima todo o trem de rolamento a partir destes getters ([Rodas de apoio, roda-guia, roda motriz e roletes](14-running-gear.md)).

| Método | Devolve | O que se sabe |
| --- | --- | --- |
| `TrackedControllerComponent.GetSimulation()` | `VehicleTrackedSimulation` | O jeito recomendado de pegar a simulação |
| `IsValid()` | `bool` | Vem da `VehicleBaseSimulation`: "Returns true if this component was properly initialized." |
| `TrackCount()` | `int` | 2 depois que a simulação carrega, 0 antes. É uma constante, não lida do prefab (engenharia reversa). Ficou válido 0,8–1,3 s depois de o veículo ser criado no Play, antes de o motor ser ligado (medido) |
| `WheelCount()` | `int` | Peças **por lagarta**: 13 no 2A6 (medido) |
| `WheelHasContact(track, wheel)` | `bool` | Contato com o chão do raio da roda. False no ar ou capotado |
| `WheelGetContactPosition(track, wheel)` | `vector`, espaço do mundo | Devolve um valor guardado sem conferir o contato (engenharia reversa): leia só quando `WheelHasContact` for true. Converta com `owner.CoordToLocal()`. O y local + `WheelGetRadius()` dá a altura do centro da roda; bateu com o sinal de suspensão do próprio motor com 1,25 mm de diferença média (medido) |
| `WheelGetContactNormal` / `WheelGetContactMaterial` / `WheelGetContactEntity` | `vector` / `GameMaterial` / `IEntity` | Existem; não usados nos testes |
| `WheelGetContactLiquidState` / `...LiquidMaterial` / `...LiquidPosition` | contato com líquido | Existem; não testados |
| `WheelGetRadius(track, wheel)` | `float` | O `Radius` do modelo |
| `WheelGetRPM(track, wheel)` | `float` | Velocidade angular interna (rad/s) × 60/2π (engenharia reversa). O sinal é o sinal da velocidade para a frente (87 de 87 amostras com todas as rodas em contato acima de 2 km/h, medido). Para uma roda de apoio é a velocidade da esteira / o seu `Radius`: rpm da roda de apoio = rpm da roda motriz × 0,34/0,418 (medido). Se a roda-guia, a roda motriz e os roletes informam velocidade da esteira / o próprio `Radius` não foi testado. Uma roda sem contato pode informar um valor bem diferente (−230,6 rpm em ré, contra 0–20 no chão) |
| `WheelGetPosition(track, wheel, displacement)` | `vector` | O centro da roda no referencial interno da física, cuja origem é o centro de massa do corpo. `displacement` 0 = curso todo para baixo. No 2A6 devolveu y = −1,899 = 0,4186 − 0,15 − 2,1678 para toda roda (medido com `MaxTravelDown` 0.15). Some o centro de massa para ter coordenadas do casco. A unidade de `displacement` não foi testada |
| `WheelGetRollingDrag` / `WheelSetRollingDrag` | `float` | Existem; não testados |

Regras para todo script que os lê:

- Proteja cada frame. Até a simulação carregar, os getters devolvem 0, false ou um vetor zero:

```c
VehicleTrackedSimulation simulation = controller.GetSimulation();
if (!simulation || !simulation.IsValid() || simulation.TrackCount() < 2)
	return;
```

- Mantenha você mesmo `track < TrackCount()` e `wheel < WheelCount()`. Só o `WheelGetPosition` confere o índice da roda, e nenhum getter confere o índice da lagarta; com um índice errado os outros leem fora dos seus vetores (engenharia reversa).
- Nunca chame `GetInputs()`: derruba o Workbench. Na simulação de esteira, `GetThrottle()` sempre devolve 0, `SetThrottle()` não faz nada e `IsHandbrakeOn()` sempre devolve false. `SetBreak(x, true)` guarda 6,0, e é por isso que `GetBrake()` pode ler 6,0.

### Sinais disponíveis

| Sinal | Existe? | Valor |
| --- | --- | --- |
| `engineRPM` | sim | RPM do motor |
| `engineThrust` | sim | Acelerador aplicado (0,8 com W, 1,0 com turbo) |
| `engineOn` | sim | Motor ligado |
| `brake` | sim | Entrada de freio. Lê 6 quando o controlador segura o tanque parado |
| `gear`, `throttle`, `clutch`, `handBrake` | **não** | — |

Sobre o 6: o campo *Brake* do painel de depuração mostra 6,0 parado, nos giros no lugar e no fim de uma freada segurada (0,7 → 1,0 → 6,0), observado no 2A6. `SetBreak(x, true)` guarda 6,0 (engenharia reversa); se todo 6,0 vem desse caminho do freio de mão está em aberto.

O executável guarda mais nomes de sinal no mesmo bloco de textos dos atributos da simulação de esteira: `speed`, `steeringInput`, `suspensionMove`, `drivetrainRPM` (usado pelo LAV-25 vanilla), `engineLoad` e `handbrake` (com b minúsculo). Eles foram achados numa busca de textos e **não foram testados** na simulação de esteira.

Nenhum sinal por roda foi achado. O `CarProcAnimComponent` recebe entradas de roda da simulação de esteira (a entrada "rotation" do `.pap`, faixa do editor −360..360, e "suspension", em metros ao longo do Y do osso de −`MaxTravelDown` a +`MaxTravelUp`), mas na 1.8.0.13 elas caem nas rodas erradas. Leia o estado das rodas com os getters acima ([Rodas de apoio, roda-guia, roda motriz e roletes](14-running-gear.md)).

---

[← Convertendo um veículo existente](06-converting.md) · [Índice](../../README.pt-BR.md) · [Como funciona por dentro →](08-internals.md)
