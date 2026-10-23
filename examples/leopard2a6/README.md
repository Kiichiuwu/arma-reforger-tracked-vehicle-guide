# Leopard 2A6 example / Exemplo do Leopard 2A6

**EN** — This folder holds the tested prefab blocks of the Leopard 2A6 hull, the worked example of the
guide. They come from the mod's `Leopard2A6_base.et`, which inherits `TrackedVehicle_Base.et`. The
mod's models and textures are not in this repository. The previous example (Leopard 2A7) is in the
git history under the tag
[`v1-leopard2a7`](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/tree/v1-leopard2a7).

**PT** — Esta pasta tem os blocos de prefab testados do casco do Leopard 2A6, o exemplo prático do
guia. Eles vêm do `Leopard2A6_base.et` do mod, que herda de `TrackedVehicle_Base.et`. Os modelos e
as texturas do mod não estão neste repositório. O exemplo anterior (Leopard 2A7) está no histórico
do git, na tag
[`v1-leopard2a7`](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/tree/v1-leopard2a7).

| File / Arquivo | What it is / O que é |
| --- | --- |
| [`VehicleTrackedSimulation_Leopard2A6.et.txt`](VehicleTrackedSimulation_Leopard2A6.et.txt) | The full `VehicleTrackedSimulation` block: the `Simulation Tracked` values (engine, clutch, gearbox, differential, friction curves, wheel templates, two tracks with 13 `TrackPart` entries each) and, after it, the link chain attributes with the 73 `TrackPositions`. / O bloco `VehicleTrackedSimulation` completo: os valores do `Simulation Tracked` (motor, embreagem, câmbio, diferencial, curvas de atrito, modelos de roda, duas lagartas com 13 `TrackPart` cada) e, depois dele, os atributos da corrente de elos com as 73 `TrackPositions`. |
| [`HullComponents_Leopard2A6.et.txt`](HullComponents_Leopard2A6.et.txt) | The other hull component overrides: `RigidBody` (mass, centre of mass offset, sleeping thresholds), the empty `CarProcAnimComponent` and the `LEO_*` running gear components with their IDs. / Os outros componentes sobrescritos no casco: `RigidBody` (massa, deslocamento do centro de massa, limites de repouso), o `CarProcAnimComponent` vazio e os componentes `LEO_*` do trem de rolamento com os IDs. |
| [`Prefabs_Leopard2A6_variants.et.txt`](Prefabs_Leopard2A6_variants.et.txt) | The three Workshop prefabs, children of `Leopard2A6_base.et`: vanilla simulation + `LEO_TrackedDriveComponent`, and Kiichi Tracked System (`LEO_TrackSimComponent`, no native simulation block) for 1.8 and 1.9 ([chapter 19](../../docs/en/19-kiichi-tracked-system.md)). / Os três prefabs do Workshop, filhos do `Leopard2A6_base.et`: simulação vanilla + `LEO_TrackedDriveComponent`, e Kiichi Tracked System (`LEO_TrackSimComponent`, sem bloco de simulação nativa) para 1.8 e 1.9 ([capítulo 19](../../docs/pt-BR/19-kiichi-tracked-system.md)). |

## How to use / Como usar

**EN**

1. Paste both blocks inside `components { }` of a hull prefab that inherits
   `{0608D8FA71FD3433}Prefabs/Vehicles/Core/TrackedVehicle_Base.et`.
2. Keep the inherited IDs, so the blocks override the base components: `VehicleTrackedSimulation`
   `{6160DE6434513C4D}`, `Simulation Tracked` `{6160DE65F2933990}`, `TripleDifferential`
   `{663757C64F4BFDDD}`, `Aerodynamics` `{6160DE6521A7B70A}`, `RigidBody` `{51DAA09FECF52BBF}` and
   `CarProcAnimComponent` `{5D6501223785D009}`. The other IDs belong to the Leopard; Workbench makes
   new ones when you add components yourself.
3. Replace what belongs to the Leopard: the bone names (`v_wheel_l01` ...), the masses, radii and
   drivetrain values, `TrackPositions` (rebuild them with [`tools/trackpositions.py`](../../tools/trackpositions.py)),
   `TrackOffset1/2` and the link model in `TrackSegment`. The link model
   `{B50AEDC702735F50}Assets/Vehicles/Tracked/Leopard2A6/Leopard2A6_track_link.xob` is not in this
   repository.
4. Add the controller override, which is not in the files:

   ```
   BaseVehicleNodeComponent "{5D6501223785D0E7}" {
    components {
     SCR_TrackedControllerComponent "{60F8D3F71B2F1D79}" {
      ThrottleReverseTarget 1
     }
    }
   }
   ```

5. Copy the scripts from [`scripts/`](../../scripts/) into your addon's `scripts/Game/` folder (the
   Leopard mod uses `scripts/Game/Vehicle/`). Copy them all: `LEO_RunningGearComponent.c` needs
   `LEO_TrackChain.c`, `LEO_TrackSimComponent.c` and `LEO_TrackedDriveComponent.c`, and both drive components
   need `LEO_PlayerController.c`.
   The `LEO_*` components are empty in the block because their attribute defaults are the Leopard's
   values; set them for your tank ([chapter 11](../../docs/en/11-scripts.md)).

**PT**

1. Cole os dois blocos dentro de `components { }` de um prefab de casco que herda
   `{0608D8FA71FD3433}Prefabs/Vehicles/Core/TrackedVehicle_Base.et`.
2. Mantenha os IDs herdados, para os blocos sobrescreverem os componentes da base:
   `VehicleTrackedSimulation` `{6160DE6434513C4D}`, `Simulation Tracked` `{6160DE65F2933990}`,
   `TripleDifferential` `{663757C64F4BFDDD}`, `Aerodynamics` `{6160DE6521A7B70A}`, `RigidBody`
   `{51DAA09FECF52BBF}` e `CarProcAnimComponent` `{5D6501223785D009}`. Os outros IDs são do Leopard;
   o Workbench cria novos quando você adiciona os componentes.
3. Troque o que é do Leopard: os nomes dos ossos (`v_wheel_l01` ...), as massas, os raios e os
   valores do trem de força, as `TrackPositions` (refaça com
   [`tools/trackpositions.py`](../../tools/trackpositions.py)), `TrackOffset1/2` e o modelo do elo em
   `TrackSegment`. O modelo do elo
   `{B50AEDC702735F50}Assets/Vehicles/Tracked/Leopard2A6/Leopard2A6_track_link.xob` não está neste
   repositório.
4. Acrescente a sobrescrita do controlador, que não está nos arquivos (bloco acima, em **EN**, passo 4).
5. Copie os scripts de [`scripts/`](../../scripts/) para a pasta `scripts/Game/` do seu addon (o mod
   do Leopard usa `scripts/Game/Vehicle/`). O `LEO_RunningGearComponent.c` precisa do
   `LEO_TrackChain.c`, do `LEO_TrackSimComponent.c` e do `LEO_TrackedDriveComponent.c`, e os dois componentes de
   direção precisam do `LEO_PlayerController.c`. Copie todos. Os componentes `LEO_*` estão vazios no bloco porque os valores padrão dos
   atributos são os do Leopard; ajuste-os para o seu tanque ([capítulo 11](../../docs/pt-BR/11-scripts.md)).

## Key values / Valores principais

| Value / Valor | Leopard 2A6 | Note / Nota |
| --- | --- | --- |
| Mass / Massa | 62300 kg | `RigidBody` |
| `CenterOfMass` | 0 1 0.46 | An offset from the hull model's own centre of mass (0, 1.168, -0.381), so the physical centre of mass is at (0, 2.168, 0.079), 2.17 m above the ground. It levels the hull at rest (pitch -0.05°, measured). It is probably too high and was not retuned (open). / Deslocamento a partir do centro de massa do próprio modelo do casco (0; 1,168; -0,381), então o centro de massa físico fica em (0; 2,168; 0,079), 2,17 m acima do chão. Deixa o casco nivelado parado (arfagem -0,05°, medida). Provavelmente alto demais; não foi reajustado (em aberto). |
| Engine / Motor | 1100 kW @ 2600 rpm, 4700 N·m @ 1600 rpm; idle 700, redline 2700, max 3000 rpm | MTU MB 873; `Inertia 3`, `Friction 470`, `HasGovernor 1` |
| Clutch / Embreagem | `MaxClutchTorque 9000` | |
| Gearbox / Câmbio | 6.0 / 3.3 / 1.8 / 1.0, reverse 2.3, efficiency 0.9 | |
| `TripleDifferential` | `Ratio 4.6`, `SteeringRatio 6`, `NeutralSteeringRatio 12` | Final drive / redução final |
| Road wheels / Rodas de apoio | 7 per side, `Radius 0.418`, `Mass 110`, `BrakeTorque 6000` | The radius reaches the ground face of the links; the visible rim is 0.345 m. / O raio vai até a face de contato dos elos; o aro visível tem 0,345 m. |
| Suspension / Suspensão | `SpringRate 450` (kN/m), `CompressionDamper 25000`, `RelaxationDamper 35000` (N·s/m), `MaxTravelUp 0.30`, `MaxTravelDown 0.10`, `RayStartOffsetUp 0.5` (m) | 62300 × 9.81 / (14 × 450000) = 0.097 m, so `MaxTravelDown 0.10` puts the hull at its modelled height (rest travel -3 mm, measured). / 62300 × 9,81 / (14 × 450000) = 0,097 m, então `MaxTravelDown 0.10` põe o casco na altura modelada (curso parado -3 mm, medido). |
| Idler / Roda-guia | `Radius 0.335`, `Mass 150`, `BrakeTorque 0` | |
| Sprocket / Roda motriz | `Radius 0.34`, `Mass 250`, `BrakeTorque 30000` | Sets the vehicle speed. / Define a velocidade do veículo. |
| Return rollers / Roletes de retorno | 4 per side, `Radius 0.152`, `Mass 20`, `BrakeTorque 0` | `Type "Return Roller"` |
| `TrackPart` order / Ordem | `Wheel_X01`..`Wheel_X07` (0-6), `Idler_X` (7), `Sprocket_X` (8), `Roller_X01`..`Roller_X04` (9-12) | Road wheels first, same order on both sides. / Rodas de apoio primeiro, mesma ordem dos dois lados. |
| Link chain / Corrente de elos | `TrackSegments 73`, 73 `TrackPositions`, `TrackLength 0.2110`, `TrackThickness 0.076`, `TrackOffset1 -1.3281 0 0`, `TrackOffset2 1.3281 0 0` | Link pitch 0.2101 m + 0.9 mm: the chain is 11.9 cm longer than the path around the wheels at rest. `TrackThickness` is stored but never read in 1.8.0.13 (reverse-engineered). / Passo do elo 0,2101 m + 0,9 mm: a corrente fica 11,9 cm mais longa que o caminho em volta das rodas parado. `TrackThickness` é guardado mas nunca lido na 1.8.0.13 (engenharia reversa). |
| Other / Outros | `RegenerativeSteering 1`, `NeutralSteering 1`, `PivotSteering 1`, `PivotSteeringThreshold 0.5`, `BrakeSteering 0`, `RetarderBaseTorque 0`, `RetarderThrottleSpeed 20`, `ReferenceArea 9.5`, `DragCoefficient 0.9`, `SolverSubsteps 4`; flat friction curves at 1.0 | |
| `ThrottleReverseTarget` | 1 | On the controller / No controlador |

**EN** — Speeds. With final drive 4.6 and the 0.34 m sprocket, theory gives 72.4 km/h in 4th gear at
2600 rpm and 75.2 km/h at 2700 rpm. The top speed was not measured on the 2A6; the highest logged
was 47.6 km/h. The earlier 2A7 build (final drive 4.9, sprocket 0.355 m) measured 74.3 km/h. In
reverse the native track speed is capped at 36.3 km/h (computed from `RpmMax` 3000); the push of `LEO_TrackedReverseComponent`
fades to zero at 31 km/h, and the tank reached 27.8 km/h (measured).

**PT** — Velocidades. Com redução final 4,6 e roda motriz de 0,34 m, a teoria dá 72,4 km/h em 4ª a
2600 rpm e 75,2 km/h a 2700 rpm. A velocidade máxima não foi medida no 2A6; a maior registrada foi
47,6 km/h. A versão anterior do 2A7 (redução 4,9, roda motriz 0,355 m) mediu 74,3 km/h. Em ré, a
velocidade nativa da esteira tem teto de 36,3 km/h (calculado a partir de `RpmMax` 3000); o empurrão do `LEO_TrackedReverseComponent`
cai a zero em 31 km/h, e o tanque chegou a 27,8 km/h (medido).

Explained in / Explicado em: [10-tuning](../../docs/en/10-tuning.md),
[13-suspension](../../docs/en/13-suspension.md), [14-running-gear](../../docs/en/14-running-gear.md),
[15-track-links](../../docs/en/15-track-links.md) ·
[pt-BR: 10](../../docs/pt-BR/10-tuning.md), [13](../../docs/pt-BR/13-suspension.md),
[14](../../docs/pt-BR/14-running-gear.md), [15](../../docs/pt-BR/15-track-links.md).

## Model credit / Crédito do modelo

**EN** — "Leopard 2 A6" by [buh-late](https://sketchfab.com/buh-late)
([Sketchfab](https://sketchfab.com/3d-models/leopard-2-a6-7cb23d5322df4b409a880de635826067)),
licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/), published 2024-01-17.
Modified for Enfusion: split into parts, re-parented, bones and suspension rig, colliders, LODs,
repacked textures. The link model is cut out of the same model. The author does not endorse this
project. Full list of changes in [CREDITS.md](../../CREDITS.md).

**PT** — "Leopard 2 A6" de [buh-late](https://sketchfab.com/buh-late)
([Sketchfab](https://sketchfab.com/3d-models/leopard-2-a6-7cb23d5322df4b409a880de635826067)),
licenciado sob [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/), publicado em 17/01/2024.
Modificado para a Enfusion: separado em peças, hierarquia nova, ossos e rig de suspensão, colisores,
LODs, texturas reempacotadas. O modelo do elo foi recortado do mesmo modelo. O autor não endossa este
projeto. Lista completa das alterações em [CREDITS.md](../../CREDITS.md).

The prefab blocks in this folder are code and use the [MIT](../../LICENSE) license. / Os blocos de
prefab desta pasta são código e usam a licença [MIT](../../LICENSE).
