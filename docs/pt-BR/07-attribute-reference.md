> 🇧🇷 Português · [🇬🇧 English](../en/07-attribute-reference.md)

[← Converter um veículo existente](06-converting.md) · [Índice](../../README.pt-BR.md) · [Como funciona por dentro →](08-internals.md)

# Referência de atributos

Os nomes, padrões e faixas abaixo foram lidos do código de registro de atributos do executável 1.8.0.13. "—" significa que o padrão não aparece no binário. A coluna *Efeito* diz o que observamos em jogo ou no código.

### Simulation Tracked (nível de cima)

| Atributo | Padrão | Leopard | Efeito |
| --- | --- | --- | --- |
| `SolverSubsteps` | — (faixa 1–20) | 4 | Subpassos por frame. Com 60 Hz e 4 subpassos, dt = 1/240 s |
| `FrictionCurveLongitudinal` | — | curva plana 1.0 | **Obrigatória.** Atrito no sentido da marcha |
| `FrictionCurveLateral` | — | curva plana 1.0 | **Obrigatória.** Atrito lateral |
| `FrictionBaseLongitudinal` / `FrictionBaseLateral` | — (faixa 0–inf) | herdado | Base do atrito do modelo antigo |
| `V2FrictionBaseLng` | — | 0.95 (vanilla) | Atrito longitudinal do modelo V2 |
| `V2FrictionBaseLat` | — | 2 (vanilla) | Atrito lateral do modelo V2 |
| `V2FrictionSurfaceLng` / `V2FrictionSurfaceLat` | `Tread` | Lat = `NonTread` (vanilla) | Enum `Tread` ou `NonTread` |
| `V2FrictionSteerEnabled` | true | herdado | Reduz o atrito lateral ao virar |
| `V2FrictionSteerFactorCurve` | — | vanilla (1 → 0.2) | Fator de atrito em função da curva |
| `V2FrictionSteerFactorLngCurve` | — | vanilla (1, 0, 1) | Fator por posição da roda, da frente para trás |
| `RegenerativeSteering` | — | 1 | Torque vai para a esteira de fora na curva |
| `NeutralSteering` | — | 1 | No giro no lugar, a esteira de dentro gira para trás |
| `PivotSteering` | — | 1 | No giro no lugar, a esteira de dentro trava. **Com 0, o tanque não girou parado** |
| `PivotSteeringThreshold` | 0.5 (0–1) | 0.5 | Entrada mínima de direção para o giro no lugar |
| `BrakeSteering` | — | 0 | Freia a esteira de dentro na curva |
| `BrakeSteeringThreshold` | — | herdado | Entrada mínima para o freio de direção |
| `RetarderBaseTorque` | 10 | 0 | Freio que cresce com a velocidade. **Com 10, o Leopard não passava de \~54 km/h** |
| `RetarderThrottleSpeed` | 14.5 m/s | 20 | Com 14.5, a velocidade travava em \~51 km/h |
| `InertiaOverrideEnabled` / `InertiaOverride` | false / 1 1 1 | herdado | Sobrescreve o tensor de inércia |
| `RaycastLayer` / `LiquidsLayers` | — | `VehicleCast` / `Liquids` (vanilla) | Camadas dos raios das rodas e da água |

### Motor, embreagem, câmbio e diferencial

| Bloco | Atributo | Padrão | Leopard | Efeito |
| --- | --- | --- | --- | --- |
| `Engine` | `Inertia` | — (>0) | 3 | Inércia do motor. Precisa ser maior que zero |
| `Engine` | `InertiaCoupled` | — | não usado | Inércia extra com a embreagem acoplada |
| `Engine` | `MaxPower` | 100 kW | 1100 | Potência máxima |
| `Engine` | `MaxTorque` | 186 N·m | 4700 | Torque máximo |
| `Engine` | `RpmMaxPower` | 5800 | 2600 | Rotação de potência máxima |
| `Engine` | `RpmMaxTorque` | 4400 | 1600 | Rotação de torque máximo |
| `Engine` | `Steepness` | — (>0) | 50 | Forma da curva de torque. Os motores vanilla usam de 4 a 50 |
| `Engine` | `Friction` | — (>0) | 470 | Atrito interno. **1400 comia um terço do torque** |
| `Engine` | `BrakingTorqueRatio` | 0.15 | 0.15 | Freio-motor, fração do `MaxTorque` |
| `Engine` | `RpmIdle` / `RpmRedline` / `RpmMax` | 1250 / 6250 / 7000 | 700 / 2700 / 3000 | Marcha lenta, corte e limite. A ordem precisa ser crescente |
| `Engine` | `HasGovernor` | — | 1 | Limitador de rotação |
| `Clutch` | `MaxClutchTorque` | 225 | 9000 | **Ignorado** pela esteira: a embreagem é só um multiplicador 0–1 |
| `Gearbox` | `Forward { ... }` | — | 6.0 3.3 1.8 1.0 | Relações das marchas à frente |
| `Gearbox` | `Reverse` | — | 2.3 | **Uma** relação de ré. O sinal é forçado a negativo (ver [bugs](09-bugs.md)) |
| `Gearbox` | `Efficiency` | — | 0.9 | Eficiência da transmissão |
| `TripleDifferential` | `Ratio` | — (>0) | 4.9 | Redução final |
| `TripleDifferential` | `SteeringRatio` | — | 6 | Relação máxima ao virar andando. **Sem ela, o tanque não virou** |
| `TripleDifferential` | `SteeringThreshold` | 0.01 | herdado | Entrada mínima para virar |
| `TripleDifferential` | `NeutralSteeringRatio` | — | 12 | Relação do giro no lugar |

O atributo `Output` dos blocos só liga a cadeia motor → embreagem → câmbio. Não defina `Output` no câmbio; o vanilla também não define.

### Rodas e suspensão

| Bloco | Atributo | Padrão | Leopard | Efeito |
| --- | --- | --- | --- | --- |
| `RoadWheel` / `Sprocket` / `Idler` / `Roller` | `Mass` | 12 kg | 110 / 250 / 150 / — | Massa da roda |
| todos | `Radius` | 0.6 m | 0.438 / 0.355 / 0.265 / — | O da roda motriz define a velocidade da esteira |
| todos | `BrakeTorque` | 4000 N·m | 6000 / 30000 / 0 / — | Torque de freio por roda |
| `Suspension` (só `RoadWheel`) | `MaxSteeringAngle` | — | 0 | Sempre 0 na esteira |
| `Suspension` | `SpringRate` | 20 | 450 | Rigidez da mola (unidade provável: N/mm) |
| `Suspension` | `CompressionDamper` | 2500 | 25000 | Amortecimento na compressão |
| `Suspension` | `RelaxationDamper` | — | 35000 | Amortecimento no retorno |
| `Suspension` | `MaxTravelUp` / `MaxTravelDown` | 0.15 / — | 0.30 / 0.15 | Curso para cima e para baixo, em metros |
| `Suspension` | `RayStartOffsetUp` | — | 0.5 | Quanto acima do osso o raio começa |
| `TrackPart` | `Type` | Road Wheel | — | `Road Wheel`, `Sprocket`, `Idler`, `Return Roller` |
| `TrackPart` → `Pivot` | `Bone`, `Position`, `Rotation` | — | `Bone "v_..."` | Osso da roda e deslocamento opcional |

### Aerodinâmica

| Atributo | Padrão | Leopard | Efeito |
| --- | --- | --- | --- |
| `ReferenceArea` | 3 (vanilla) | 9.5 | Área frontal em m² |
| `DragCoefficient` | 0.5 | 0.9 | Coeficiente de arrasto |

### SCR\_TrackedControllerComponent

Fica dentro de `BaseVehicleNodeComponent`. Os padrões são do executável; a coluna *Vanilla* mostra o que o `TrackedVehicle_Base.et` muda.

| Atributo | Padrão | Vanilla | Efeito |
| --- | --- | --- | --- |
| `Type` | 0 | — | Nunca é lido pelo código |
| `TransmissionRND` | false | — | Câmbio de três posições (R, N, D). Não libera nem bloqueia a ré |
| `SteeringForwardSpeed` / `SteeringCenterSpeed` | — | `19 1.1 100 0.02` / `19 1.1 60 1.8` | Velocidade da direção ao virar e ao centralizar. Formato não decodificado |
| `ThrottleTurbo` | 0.2 | — | Parte do acelerador reservada ao turbo. W sozinho dá 80% |
| `ThrottleTurboTime` | 0.15 s | — | Tempo para entrar o turbo |
| `ThrottleReverseTarget` | 0.2 | — | Acelerador usado em ré. **Use 1** (ver [bugs](09-bugs.md)) |
| `ClutchUncoupleTime` / `ClutchCoupleTime` | 0.25 / 0.35 s | — | Tempo para soltar e acoplar a embreagem |
| `ClutchCoupleForwardRpm` / `ClutchCoupleReverseRpm` | 0 | — | Rotação de acoplamento ao arrancar. Só importa na saída |
| `ClutchCoupleUphillFactor` | 1 | — | Ajuste do acoplamento em subida |
| `BrakingCurve` | — | `0.3 0.7 0.6 1.4 1 2.1` | Pares \[fração, segundos\]: 30% em 0.7 s, 60% em 1.4 s, 100% em 2.1 s |
| `BrakeTurboTime` | — | — | Tempo do freio com turbo |
| `RpmSmoothing` | 0.7 | 0.95 | Filtro da rotação para trocar marchas |
| `SlopeSmoothing` | 0.75 | — | Filtro da inclinação |
| `Latency` | 0 | 0.1 | Atraso das trocas |
| `UpShiftFactor` / `UpShiftFactorDownhill` | — | — | Fator de rotação para subir marcha (subida / descida) |
| `DownShiftFactor` / `DownShiftFactorDownhill` | 0.4 / — | — | Fator de rotação para reduzir |
| `PeakTorqueUpshiftingHysteresis` / `...Downshifting...` | -0.14 / -0.2 | — | Histerese em torno do torque máximo |
| `SteeringFactorUpshift` / `SteeringFactorDownshift` | 0.1 / 0.05 | 0.1 / 0.05 | Correção das trocas ao virar |
| `TurboShiftFactor` | 1.1 | — | Trocas mais altas com turbo |
| `PeakPowerShiftingModeTurbo` / `...Normal` | true / — | — | Troca pela potência máxima |
| `PeakPowerUpshiftingHysteresis` / `...Downshifting...` | 0 / -0.1 | — | Histerese em torno da potência máxima |
| `PeakPowerUpShiftFactorUphill` e afins | 0.7 | — | Fatores do modo de potência máxima |
| `MaxStartupTime`, `MaxStartupAttempts`, `EngineStartupChance`, `ShutdownTime` | — | 0.6 / 10 / 100 / 600 | Partida do motor (comuns a todos os veículos) |

### Componente VehicleTrackedSimulation (fora do Simulation)

| Atributo | Efeito |
| --- | --- |
| `TrackSegments`, `TrackSegment`, `TrackPositions`, `TrackLength`, `TrackThickness`, `TrackOffset1`, `TrackOffset2` | Corrente visual de elos. O asset padrão (elo do M113) não vem no jogo |
| Botão *Generate track positions* | Gera as posições da corrente no Workbench |

Deixe esses vazios para o primeiro teste. A corrente só é criada com `TrackLength > 0` e pelo menos 2 `TrackPositions`.

### Sinais disponíveis

| Sinal | Existe? | Valor |
| --- | --- | --- |
| `engineRPM` | sim | Rotação do motor |
| `engineThrust` | sim | Acelerador aplicado (0.8 com W, 1.0 com turbo) |
| `engineOn` | sim | Motor ligado |
| `brake` | sim | Freio. Vale 6 com o freio de estacionamento aplicado pelo controlador |
| `gear`, `throttle`, `clutch`, `handBrake` | **não** | — |

---

[← Converter um veículo existente](06-converting.md) · [Índice](../../README.pt-BR.md) · [Como funciona por dentro →](08-internals.md)
