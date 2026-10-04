> 🇧🇷 Português · [🇬🇧 English](../en/08-internals.md)

[← Referência de atributos](07-attribute-reference.md) · [Índice](../../README.pt-BR.md) · [Bugs e limitações (1.8.0.13) →](09-bugs.md)

# Como funciona por dentro

Esta seção resume a engenharia reversa do executável 1.8.0.13. Saber o caminho do sinal ajuda a entender por que cada valor importa.

### Entradas do motorista

A esteira tem um conjunto próprio de ações de entrada, separado do carro:

| Ação | Função |
| --- | --- |
| `TrackedThrottle` | Acelerar |
| `TrackedBrake` | Frear; parado, vira ré |
| `TrackedSteeringLeft` / `TrackedSteeringRight` | Virar |
| `TurboToggle` / `TurboHold` | Turbo (100% do acelerador) |
| `HandBrake` / `HandBrakePersistent` | Freio de mão |
| `EngineStart` / `EngineStop` | Ligar e desligar |

**Não existe** ação de trocar marcha, de ré ou de embreagem. O carro tem `CarShift` e `CarShiftReverse`; a esteira, não.

As entradas só passam se o motorista estiver vivo, sem usar item e sem estar entrando, saindo ou trocando de banco.

### O controlador (SCR\_TrackedControllerComponent)

O controlador transforma as entradas em acelerador, freio, marcha e embreagem.

**Acelerador.** W entrega `1 − ThrottleTurbo`. Com o padrão 0.2, isso dá 80%. O turbo libera os 20% restantes.

**Seleção de sentido (automática):**

- De **neutro** para **ré**: freio acima de 1/32 e velocidade abaixo de 1.5 m/s. A troca é imediata.
- De **frente** para **ré**: freio acima de 1/32, acelerador abaixo de 1/32 e velocidade abaixo de 1.5 m/s por 0.5 s.
- Em ré, os pedais são trocados a cada frame. `TrackedBrake` vira acelerador, com valor `ThrottleReverseTarget`.

**Marchas.** O índice 0 é a ré, o 1 é o neutro e do 2 em diante vêm as marchas à frente. O primeiro valor de `Forward { }` é o índice 2.

**Embreagem.** É um multiplicador de 0 a 1, controlado pelo tempo (`ClutchCoupleTime`, `ClutchUncoupleTime`). `ClutchCouple*Rpm` só vale na arrancada. `MaxClutchTorque` não é usado.

**Trocas.** As trocas automáticas usam a rotação filtrada (`RpmSmoothing`), a inclinação (`SlopeSmoothing`) e os fatores de subida e redução.

### A simulação (VehicleTrackedSimulation)

A cada subpasso, a simulação calcula motor → embreagem → câmbio → diferencial → esteiras → rodas. Depois aplica as forças no `RigidBody`.

**Passo de tempo:**

```math
\Delta t = \frac{1}{60 \cdot \text{SolverSubsteps}} \quad (\text{4 subpassos} \Rightarrow 1/240\ \text{s})
```

**Tabela do câmbio.** Internamente, a lista vira `[−|Reverse|, 0, Forward...]`. A relação de ré é **sempre negativa**, mesmo se você escrever um valor negativo.

**Velocidade da esteira.** Ela usa o raio da **roda motriz** (`Sprocket.Radius`), não o da roda de apoio:

```math
v\,[\text{km/h}] = \frac{\text{rpm}}{g \cdot R_d} \cdot \frac{2\pi}{60} \cdot r_{\text{sprocket}} \cdot 3.6
```

Aqui, *g* é a relação da marcha e *R\_d* é o `Ratio` do `TripleDifferential`. Conferência no Leopard: com *R\_d* = 6.0, a telemetria mostrou 45 km/h a \~2000 rpm em 4ª, e a fórmula dá 44.6 km/h.

**Reação da carga no motor.** A carga das rodas volta para o motor pela mesma cadeia de relações:

```math
\text{reação} = \frac{\sum \text{carga}_{\text{rodas}}}{g \cdot R_d}
```

```math
\text{rpm} \mathrel{-}= \frac{\text{embreagem} \cdot \text{reação}}{\text{Inertia} + \text{embreagem} \cdot \text{InertiaCoupled}} \cdot \Delta t \cdot 9.5493
```

A carga é sempre positiva; parado, ela é o peso do veículo. Por isso o sinal de *g* decide se a carga freia ou acelera o motor. Na ré, *g* é negativo, e a carga **acelera** o motor. A [seção seguinte](09-bugs.md) explica o efeito.

**Torque do motor.** A curva vai de `RpmIdle` até `RpmMax`, com pico em `RpmMaxTorque`. Em `RpmMax`, o torque é zero.

**Direção.** Andando, o diferencial interpola até `SteeringRatio` conforme a entrada de direção. Parado, usa `NeutralSteeringRatio`. `PivotSteering` trava a esteira de dentro; `NeutralSteering` a faz girar para trás.

**Rodas.** Cada roda lança um raio a partir do osso, começando `RayStartOffsetUp` acima. A suspensão calcula mola e amortecedor. O atrito usa as curvas `FrictionCurve*` e os valores `V2Friction*`.

### Rede (multiplayer)

O `NwkTrackedMovementComponent` envia os comandos do motorista ao servidor, que os reexecuta. Uma força aplicada por script só na máquina do motorista **não** faz parte desses comandos. Em multiplayer, ela também precisa rodar no servidor.

---

[← Referência de atributos](07-attribute-reference.md) · [Índice](../../README.pt-BR.md) · [Bugs e limitações (1.8.0.13) →](09-bugs.md)
