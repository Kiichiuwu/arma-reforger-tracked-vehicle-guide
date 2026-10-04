> 🇧🇷 Português · [🇬🇧 English](../en/12-troubleshooting.md)

[← Scripts prontos](11-scripts.md) · [Índice](../../README.pt-BR.md) · [Apêndice →](13-appendix.md)

# Solução de problemas

Procure a mensagem ou o sintoma na tabela certa. As mensagens aparecem no *Log Console* do Workbench e no `console.log` da sessão.

### Erros ao carregar o veículo

| Mensagem | Causa | Correção |
| --- | --- | --- |
| `Tracked - failed to load!` | Algum dos erros abaixo | Leia a linha logo acima dela no log |
| `FrictionCurveLateral - configuration is missing!` | O `.ct` vanilla não tem as curvas | Adicione `FrictionCurveLongitudinal` e `FrictionCurveLateral` |
| `Invalid number of tracks! Must be 2!` | Falta `Tracks` ou há mais de duas | Exatamente `Track Left` e `Track Right` |
| `Wheels - configuration is missing! Every track should have at least 3 wheels!` | Menos de 3 peças numa esteira | Pelo menos 3 `TrackPart` por esteira |
| `Wheels - ... mismatch between tracks` | Contagem diferente de algum tipo entre os lados | Mesmo número de idlers, rodas e rodas motrizes nos dois lados |
| `TrackWheel's mass / radius / brake torque is incorrect!` | Modelo de roda ausente ou com valor fora da faixa | Defina `RoadWheel`, `Sprocket` e `Idler` com massa e raio positivos |
| `Engine's inertia is incorrect! (must be greater than zero)` | `Inertia` 0 | `Inertia` maior que zero. O mesmo vale para `Steepness` e `Friction` |
| `Tracked - no physics component!` | Sem colisores ou sem `RigidBody` | Colisores `UCX_*` no `.xob` |
| `Aerodynamics - configuration is missing!` | Bloco `Aerodynamics` apagado | Mantenha o herdado ou escreva um novo |
| Aviso: falta `CarControllerComponent` | `AICarMovementComponent` do base vanilla | Inofensivo |
| Aviso do som pedindo o osso `v_axle_01` | Componente de som herdado do carro | Inofensivo; ou crie o osso |

### Problemas ao dirigir

| Sintoma | Causa | Correção |
| --- | --- | --- |
| W + A/D só reduz a velocidade, não vira | `SteeringRatio` sem valor | `SteeringRatio 6` |
| A/D parado não gira no lugar | Faltam `NeutralSteeringRatio` e `PivotSteering` | `NeutralSteeringRatio 12`, `PivotSteering 1`, `NeutralSteering 1` |
| Velocidade trava em \~51–54 km/h | Retardador padrão | `RetarderBaseTorque 0`, `RetarderThrottleSpeed 20` |
| Velocidade máxima bem diferente da conta | Conta feita com o raio da roda de apoio | Refaça com o raio da roda motriz |
| Aceleração muito lenta | `Friction` alta, `Inertia` alta ou primeira marcha longa | `Friction` \~10% do torque; encurte a 1ª |
| W nunca chega a 100% | `ThrottleTurbo` 0.2 | Normal; use o turbo |
| S parado: motor sobe acima de `RpmMax` e nada acontece | Bug da ré nativa | Script de ré |
| S parado: o tanque anda para a frente | `ThrottleReverseTarget` abaixo de 1 | `ThrottleReverseTarget 1` |
| Na descida, mantém a velocidade de ré, mas no plano não sai | Bug da ré: só a gravidade move | Script de ré |
| Casco afunda ou flutua | `RoadWheel.Radius` não bate com a altura dos ossos, ou mola fraca | Raio = altura do osso acima do chão; recalcule `SpringRate` |
| Tanque escorrega devagar numa rampa parado | Não resolvido | Em aberto |

### Scripts e crashes

| Sintoma | Causa | Correção |
| --- | --- | --- |
| Workbench fecha ao ligar o motor | Script chamou `GetInputs()` | Remova a chamada |
| Workbench fecha ao abrir um diag de veículo | *Show vehicle debug* ou *Show Controller Diags* | Não use esses diags |
| Workbench fecha ao recompilar | Recompilação com o *Play* rodando | Pare o *Play* antes |
| `Pointer type Physics can only be used with local variables` | `Physics` como membro da classe | Use variável local em cada função |
| `LEOREV no pilot compartment found` | Compartimentos registrados depois do `OnPostInit` | Pegue o slot do piloto sob demanda (já feito na v4) |
| `LEOREV native gearbox still in forward gear` | O controlador ainda não trocou para ré | Normal por até 0.5 s |
| `GetThrottle()` sempre 0 | Função vazia na esteira | Leia o sinal `engineThrust` |

### Modelo e importação

| Sintoma | Causa | Correção |
| --- | --- | --- |
| Janela *Missing Addon* (`Game addon '58D0FB3206B6F859' not found`) | Workbench aberto sem as pastas de addons | Abra com `-addonsDir` |
| Ossos não aparecem no `.xob` | Importação sem esqueleto | *Export Skinning* e *Export Scene Hierarchy* ligados, *Merge Meshes* desligado |
| `Scene_Root` duplicado | Osso raiz do Blender com esse nome | Renomeie para `v_body` |
| Erro de material nos colisores | O padrão `material/metal.gamemat` não existe | Use `armor_XXmm.gamemat` |
| Torre gira em torno da traseira | `v_turret_slot` fora do centro do anel | Osso no centro do anel; origem do `.xob` da torre no mesmo ponto |
| Atirador aparece como *obstruído* | Torre sem `DoorInfoList` | Portas com teleporte no compartimento da torre |

---

[← Scripts prontos](11-scripts.md) · [Índice](../../README.pt-BR.md) · [Apêndice →](13-appendix.md)
