# Leopard 2A7 example / Exemplo do Leopard 2A7

**EN** — This folder will hold the complete Leopard 2A7 mod (Workbench project, prefabs, model,
scripts) as a worked example of the native tracked vehicle. For now it contains the tested
`VehicleTrackedSimulation` block from the hull prefab.

**PT** — Esta pasta vai receber o mod completo do Leopard 2A7 (projeto do Workbench, prefabs,
modelo, scripts) como exemplo prático da esteira nativa. Por enquanto, ela tem o bloco
`VehicleTrackedSimulation` testado, tirado do prefab do casco.

| File / Arquivo | What it is / O que é |
| --- | --- |
| [`VehicleTrackedSimulation_Leopard2A7.et.txt`](VehicleTrackedSimulation_Leopard2A7.et.txt) | The block that goes inside `components { }` of a hull prefab inheriting `TrackedVehicle_Base.et` / O bloco que vai dentro de `components { }` de um prefab de casco herdado de `TrackedVehicle_Base.et` |

Values / Valores: 64.5 t, MTU MB 873 (1100 kW @ 2600 rpm, 4700 N·m), gears 6.0 / 3.3 / 1.8 / 1.0,
reverse 2.3, final drive 4.9, sprocket radius 0.355 m, 7 road wheels per side. Explained in
[docs/en/10-tuning.md](../../docs/en/10-tuning.md) / [docs/pt-BR/10-tuning.md](../../docs/pt-BR/10-tuning.md).

The hull prefab also sets `RigidBody { Mass 64500 CenterOfMass 0 1 -0.3 }` and, on the controller,
`ThrottleReverseTarget 1`. / O prefab do casco também define `RigidBody { Mass 64500 CenterOfMass 0 1 -0.3 }`
e, no controlador, `ThrottleReverseTarget 1`.

## Model credit / Crédito do modelo

"Leopard 2 A7" by [king_st0ne](https://sketchfab.com/kingstonlee96)
([Sketchfab](https://sketchfab.com/3d-models/leopard-2-a7-b29d1cb2e65e4fa8a6e99f88e11b013b)),
[CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/). When the model is added here, it
will keep that license. / Quando o modelo for adicionado aqui, ele mantém essa licença.
