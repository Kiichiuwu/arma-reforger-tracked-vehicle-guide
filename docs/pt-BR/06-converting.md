> 🇧🇷 Português · [🇬🇧 English](../en/06-converting.md)

[← Exemplos em vídeo](05-video-examples.md) · [Índice](../../README.pt-BR.md) · [Referência de atributos →](07-attribute-reference.md)

# Converter um veículo existente

A maioria dos tanques de mods usa a simulação de rodas (`VehicleWheeledSimulation`) com muitos eixos. Converter para a esteira nativa é trocar a base e traduzir os blocos.

### 1. Trocar a herança

Faça uma cópia de segurança do prefab. Depois troque a primeira linha para herdar de `TrackedVehicle_Base.et`:

```
Vehicle : "{0608D8FA71FD3433}Prefabs/Vehicles/Core/TrackedVehicle_Base.et" {
```

Se o prefab herdava de um veículo vanilla (por exemplo `LAV25_base.et`), ele perde tudo o que vinha de lá. Copie para o seu prefab os componentes que você ainda quer.

### 2. Remover os componentes de carro

Os dois bases usam ids diferentes para o nó de controladores. Se você só trocar a herança, o prefab fica com dois nós e dois controladores.

| Remova (rodas) | Fica no lugar (esteira, herdado) |
| --- | --- |
| `VehicleWheeledSimulation "{731B26FCA2F19855}"` | `VehicleTrackedSimulation "{6160DE6434513C4D}"` |
| `BaseVehicleNodeComponent "{20FB66C5B2237133}"` | `BaseVehicleNodeComponent "{5D6501223785D0E7}"` |
| `SCR_CarControllerComponent` (dentro do nó) | `SCR_TrackedControllerComponent "{60F8D3F71B2F1D79}"` |
| `NwkCarMovementComponent "{5D6CA5AFEC980F35}"` | `NwkTrackedMovementComponent "{5D6CA5AFEC980F35}"` |

O movimento em rede usa o **mesmo id** nos dois bases, mas a classe é outra. Apague o bloco do carro para não sobrescrever o da esteira.

Se o seu nó antigo tinha outros componentes personalizados, mova-os para dentro do nó `{5D6501223785D0E7}`.

### 3. Traduzir os atributos

| Simulação de rodas | Esteira nativa | Observação |
| --- | --- | --- |
| `Engine`, `Clutch`, `Gearbox` | Os mesmos blocos | Mesmos nomes. Mas veja a [seção de bugs](09-bugs.md): a saída do câmbio e a embreagem funcionam diferente |
| `Axle` → `Wheel { Radius, BrakeTorque }` | `RoadWheel { Radius, BrakeTorque, Mass }` | Um modelo só para todas as rodas de apoio |
| `Axle` → `Suspension { ... }` | `RoadWheel` → `Suspension { ... }` | Mesma classe e mesmos atributos |
| `MaxSteeringAngle 25` | `MaxSteeringAngle 0` | A esteira vira por diferença de velocidade |
| `WheelPosition Wheel_L01 { PivotID "v_wheel_l01" }` | `TrackPart Wheel_L01 { Pivot Pivot "{id}" { Bone "v_wheel_l01" } }` | Os ossos antigos servem sem mudança |
| Diferencial por eixo | `Differential TripleDifferential` | `Ratio`, `SteeringRatio`, `NeutralSteeringRatio` |
| Velocidade pelo raio da roda | Velocidade pelo raio da **roda motriz** | Recalcule o `Ratio` ([seção de afinação](10-tuning.md)) |

### 4. Escolher idler e roda motriz

Mods com rodas falsas costumam ter uma fileira `v_wheel_l01`…`l09` sem distinção. Marque a roda certa de cada ponta:

- Leopard, T-72 e Abrams têm a roda motriz **atrás** e o idler na frente.
- M113 e vários blindados leves têm a roda motriz **na frente**.
- A ordem na lista continua sendo da frente para trás. Só o `Type` muda.

### 5. Limpar sobras da versão de rodas

| Sobra | O que fazer |
| --- | --- |
| `SCR_WheelHitZone` com `m_iWheelId` | O próprio `TrackedVehicle_Base` traz quatro (ids 0 a 3), herdados do carro. A numeração de rodas da esteira não foi verificada |
| `ChimeraAIPathfindingComponent`, `ChimeraAIVehicleControlComponent` | O `ChimeraAIVehicleControlComponent` também não existe no base de esteira; o `ChimeraAIPathfindingComponent` já vem do `Vehicle_Base.et`. A IA dirigindo tanque nativo não foi testada |

### 6. Proteção contra outros mods

O RHS-StatusQuo sobrescreve o `TrackedVehicle_Base.et` vanilla e **desliga** (`Enabled 0`) três componentes: `VehicleTrackedSimulation`, `SCR_TrackedControllerComponent` e `NwkTrackedMovementComponent`. Com esse mod carregado, o seu tanque herda os componentes desligados.

Precaução (não testada): escreva `Enabled 1` nesses três componentes, dentro do seu prefab. O valor do filho vence o do pai.

### 7. Animação das rodas

No nosso teste, a esteira nativa não girou as rodas do modelo sozinha. O sistema visual de elos (`TrackSegments`) depende de um asset que não vem no jogo. Trate a animação como trabalho separado, depois que o veículo já anda.

---

[← Exemplos em vídeo](05-video-examples.md) · [Índice](../../README.pt-BR.md) · [Referência de atributos →](07-attribute-reference.md)
