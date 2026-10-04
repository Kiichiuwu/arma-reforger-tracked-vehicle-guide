> 🇧🇷 Português · [🇬🇧 English](../en/02-architecture.md)

[← Visão geral](01-overview.md) · [Índice](../../README.pt-BR.md) · [Requisitos do modelo 3D →](03-model-requirements.md)

# Arquitetura

```mermaid
flowchart TD
  IN["Entradas do motorista<br/>TrackedThrottle, TrackedBrake, Steering L/R<br/>Turbo, HandBrake, EngineStart/Stop"]
  NWK["NwkTrackedMovementComponent<br/>envia os comandos ao servidor,<br/>que os reexecuta (multiplayer)"]
  CTL["SCR_TrackedControllerComponent<br/>W = 80% do acelerador; turbo = 100%<br/>S parado engata a ré sozinho<br/>escolhe marcha, embreagem e trocas"]
  REV["LEO_TrackedReverseComponent (script)<br/>contorno da ré quebrada:<br/>S parado empurra o casco para trás"]
  subgraph SIM["VehicleTrackedSimulation › Simulation Tracked"]
    ENG["Engine<br/>curva de torque, zero em RpmMax"] --> CLU["Clutch<br/>fator de 0 a 1"]
    CLU --> GBX["Gearbox<br/>Forward + uma ré<br/>ré sempre negativa (bug)"]
    GBX --> DIF["TripleDifferential<br/>Ratio e relações de curva"]
    DIF --> TRK["Tracks<br/>Left (0) e Right (1)"]
    TRK --> WHL["RoadWheel · Sprocket · Idler<br/>raio do osso até o chão + suspensão"]
  end
  RB["RigidBody<br/>o casco recebe as forças das rodas"]
  IN --> CTL
  CTL --> ENG
  IN --> NWK
  IN --> REV
  WHL --> RB
  REV --> RB
  classDef bug stroke:#d33,stroke-width:3px
  classDef fix stroke:#36c,stroke-width:3px
  class GBX bug
  class REV fix
```

*Borda vermelha: bug da ré nativa. Borda azul: contorno por script.*

O motorista nunca fala direto com a simulação. O controlador lê as teclas e decide acelerador, freio, marcha e embreagem. A simulação calcula motor, transmissão, esteiras e rodas, e aplica as forças no casco.

Onde cada peça mora no prefab:

| Componente | Onde fica | Papel |
| --- | --- | --- |
| `VehicleTrackedSimulation` | `components` do casco | Física da esteira: motor, transmissão, esteiras, rodas |
| `SCR_TrackedControllerComponent` | Dentro de `BaseVehicleNodeComponent` | Traduz as teclas em pedais, marcha e embreagem |
| `NwkTrackedMovementComponent` | `components` do casco | Replica o movimento em multiplayer |
| `RigidBody` | `components` do casco | Massa, centro de massa e colisão |
| `SCR_WheeledDamageManagerComponent` | `components` do casco | Dano: casco, motor, câmbio, rodas |
| `SignalsSourceAccess` | Dentro do `VehicleTrackedSimulation` (vem do `.ct`) | Liga a simulação aos sinais `engineRPM`, `engineThrust` etc. |

Todos vêm prontos do `TrackedVehicle_Base.et`. O que falta é preencher a simulação, como mostra o [passo a passo](04-from-scratch.md).

---

[← Visão geral](01-overview.md) · [Índice](../../README.pt-BR.md) · [Requisitos do modelo 3D →](03-model-requirements.md)
