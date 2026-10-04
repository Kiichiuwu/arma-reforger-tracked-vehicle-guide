> 🇧🇷 Português · [🇬🇧 English](../en/04-from-scratch.md)

[← Requisitos do modelo 3D](03-model-requirements.md) · [Índice](../../README.pt-BR.md) · [Exemplos em vídeo →](05-video-examples.md)

# Passo a passo: veículo de esteira do zero

Este roteiro leva um modelo novo até um tanque que anda, vira e dá ré. Tudo foi testado no Leopard 2A7, Workbench 1.8.0.13.

### 1. Criar o projeto do mod

1. No Workbench, crie um projeto novo com dependência **apenas** do jogo base (`ArmaReforger`, GUID `58D0FB3206B6F859`).
2. Se aparecer a janela *Missing Addon*, abra o Workbench pela linha de comando com `-addonsDir`. Liste a pasta `addons` do jogo, a do Tools e a pasta de addons do Workbench em Documentos.
3. Crie as pastas `Assets/Vehicles/Tracked/<Nome>/` e `Prefabs/Vehicles/Tracked/<Nome>/`.

### 2. Importar o modelo

1. Copie o `.fbx` para `Assets/...` e use *Register and import* → *as Model*.
2. Ajuste *Merge Meshes* (desligado), *Export Skinning* e *Export Scene Hierarchy* (ligados). Reimporte.
3. Troque o material físico de cada colisor `UCX_*` por um `armor_XXmm.gamemat`.
4. Abra o `.xob` e confira se todos os ossos das rodas aparecem na lista de ossos.

### 3. Criar o prefab do casco

O prefab herda de `TrackedVehicle_Base.et`, que já traz o controlador, a rede e uma simulação vazia. Crie um arquivo de texto `.et` e registre-o no Resource Browser (botão direito → *Register*).

```
Vehicle : "{0608D8FA71FD3433}Prefabs/Vehicles/Core/TrackedVehicle_Base.et" {
 components {
  MeshObject "{51DAA09FEFBFC0E7}" {
   Object "{GUID_DO_XOB}Assets/Vehicles/Tracked/Nome/Nome_hull.xob"
  }
  RigidBody "{51DAA09FECF52BBF}" {
   Mass 64500
   CenterOfMass 0 1 -0.3
  }
  VehicleTrackedSimulation "{6160DE6434513C4D}" {
   Simulation Tracked "{6160DE65F2933990}" {
   }
  }
  BaseVehicleNodeComponent "{5D6501223785D0E7}" {
   components {
    SCR_TrackedControllerComponent "{60F8D3F71B2F1D79}" {
     ThrottleReverseTarget 1
    }
   }
  }
 }
}
```

**Regra dos ids:** o texto entre chaves depois do nome da classe é o id da instância. Para **sobrescrever** um componente herdado, repita o id do vanilla, como acima. Um id novo cria um segundo componente.

**Não repita a herança do `.ct`.** O `TrackedVehicle_Base.et` já declara `VehicleTrackedSimulation_Base.ct`. No filho, escreva só `VehicleTrackedSimulation "{6160DE6434513C4D}" {`.

### 4. Preencher a simulação

O `VehicleTrackedSimulation_Base.ct` vanilla está incompleto. Ele só liga motor → embreagem → câmbio e define atrito e aerodinâmica. Faltam quatro coisas, e sem elas o log mostra `Tracked - failed to load!`:

| Falta | O que escrever | Erro se faltar |
| --- | --- | --- |
| Curvas de atrito | `FrictionCurveLongitudinal` e `FrictionCurveLateral` | `FrictionCurveLateral - configuration is missing!` |
| Modelos de roda | `RoadWheel` (com `Suspension`), `Sprocket`, `Idler` | `TrackWheel's mass / radius / brake torque is incorrect!` |
| Esteiras | `Tracks { Track Left {...} Track Right {...} }` | `Invalid number of tracks! Must be 2!` |
| Motor válido | `Inertia`, `Steepness`, `Friction` maiores que zero | `Engine's inertia is incorrect! (must be greater than zero)` |

Cole dentro de `Simulation Tracked "{6160DE65F2933990}" { }`. Os valores são os do Leopard, já testados:

```
SolverSubsteps 4
Engine Engine Engine {
 Inertia 3
 Steepness 50
 Friction 470
 MaxPower 1100
 MaxTorque 4700
 RpmMaxPower 2600
 RpmMaxTorque 1600
 RpmIdle 700
 RpmRedline 2700
 RpmMax 3000
 HasGovernor 1
 Output "Clutch"
}
Clutch Clutch Clutch {
 MaxClutchTorque 9000
 Output "Gearbox"
}
Gearbox Gearbox Gearbox {
 Forward {
  6.0 3.3 1.8 1.0
 }
 Reverse 2.3
 Efficiency 0.9
}
Differential TripleDifferential "{663757C64F4BFDDD}" {
 Ratio 4.9
 SteeringRatio 6
 NeutralSteeringRatio 12
}
FrictionCurveLongitudinal CurveCubicSplineFloat "{8C1E5A3B7D924F60}" {
 SplineType Linear
 EndCondition OpenFlat
 Knots {
  0
  1
 }
 ParamMin 0
 ParamMax 1
 ValueMin 0
 ValueMax 1
 UnrestrictedFlags 0
 Values {
  1
  1
 }
}
FrictionCurveLateral CurveCubicSplineFloat "{2F7B9D41C6A3E858}" {
 SplineType Linear
 EndCondition OpenFlat
 Knots {
  0
  1
 }
 ParamMin 0
 ParamMax 1
 ValueMin 0
 ValueMax 1
 UnrestrictedFlags 0
 Values {
  1
  1
 }
}
RoadWheel RoadWheel "{379D05FA6C0BDB8F}" {
 Mass 110
 Radius 0.438
 BrakeTorque 6000
 Suspension Suspension "{DBF219524735553D}" {
  MaxSteeringAngle 0
  SpringRate 450
  CompressionDamper 25000
  RelaxationDamper 35000
  MaxTravelUp 0.30
  MaxTravelDown 0.15
  RayStartOffsetUp 0.5
 }
}
Sprocket Sprocket "{68A5D8FC81D86ECE}" {
 Mass 250
 Radius 0.355
 BrakeTorque 30000
}
Idler Idler "{C8F9E465BBF2B04F}" {
 Mass 150
 Radius 0.265
 BrakeTorque 0
}
RegenerativeSteering 1
NeutralSteering 1
PivotSteering 1
PivotSteeringThreshold 0.5
BrakeSteering 0
RetarderBaseTorque 0
RetarderThrottleSpeed 20
Aerodynamics Aerodynamics "{6160DE6521A7B70A}" {
 ReferenceArea 9.5
 DragCoefficient 0.9
}
```

Os modelos `RoadWheel`, `Sprocket` e `Idler` são **um de cada**, compartilhados por todas as rodas daquele tipo. Não existe raio por roda.

### 5. Declarar as esteiras

Ainda dentro de `Simulation Tracked`, liste as peças de cada esteira **da frente para trás**. A primeira esteira é a esquerda (índice 0); a segunda, a direita (índice 1).

```
Tracks {
 Track Left {
  Wheels {
   TrackPart Idler_L {
    Type Idler
    Pivot Pivot "{C7D9DF8DAAE72E34}" {
     Bone "v_idler_l"
    }
   }
   TrackPart Wheel_L01 {
    Pivot Pivot "{3542EB258D51A1DB}" {
     Bone "v_wheel_l01"
    }
   }
   TrackPart Wheel_L02 {
    Pivot Pivot "{74F1DDA5267BB96C}" {
     Bone "v_wheel_l02"
    }
   }
   TrackPart Sprocket_L {
    Type Sprocket
    Pivot Pivot "{F67E7A2BDE42902D}" {
     Bone "v_sprocket_l"
    }
   }
  }
 }
 Track Right {
  Wheels {
   TrackPart Idler_R {
    Type Idler
    Pivot Pivot "{78491E57BF71F53D}" {
     Bone "v_idler_r"
    }
   }
   TrackPart Wheel_R01 {
    Pivot Pivot "{A57984BE1E4ABF5B}" {
     Bone "v_wheel_r01"
    }
   }
   TrackPart Wheel_R02 {
    Pivot Pivot "{6BFC478567B41E45}" {
     Bone "v_wheel_r02"
    }
   }
   TrackPart Sprocket_R {
    Type Sprocket
    Pivot Pivot "{56A9D7F8C0E3E63F}" {
     Bone "v_sprocket_r"
    }
   }
  }
 }
}
```

O exemplo tem 2 rodas de apoio por lado para caber na página; acrescente as outras no mesmo formato. Cada `Pivot` precisa de um id próprio (16 dígitos hexadecimais quaisquer, sem repetir).

- **Rodas de apoio não levam `Type`.** O padrão do enum é `Road Wheel`, e assim se evita escrever um nome com espaço.
- **`Type Idler`** e **`Type Sprocket`** vão sem aspas.
- **Rolete de retorno:** `Type` com valor `Return Roller` e um modelo `Roller Roller "{id}" { ... }`. Não testado.

### 6. Colisores e massa

- O `RigidBody` precisa de colisores no `.xob`; sem eles o log mostra `Tracked - no physics component!`.
- `Mass` em kg. O Leopard usa 64500.
- `CenterOfMass` deve ficar baixo e um pouco atrás do centro. Um centro alto faz o tanque capotar nas curvas.

### 7. Tripulação e entrada

O `PilotCompartmentSlot` já vem do vanilla, mas sem porta. Dê ao motorista uma `CompartmentDoorInfo` no `SCR_BaseCompartmentManagerComponent`. Sem porta, a ação de entrar aparece como *obstruída* (`#AR-UserAction_SeatObstructed`). Portas com `GetInTeleport 1` e `GetOutTeleport 1` dispensam animação.

### 8. Testar

1. Crie um mundo de teste como sub-cena de um mapa vanilla (por exemplo `GM_Arland`) e coloque o prefab.
2. Aperte *Play*, entre como motorista e ligue o motor.
3. Confira cada função:

| Comando | Resultado esperado |
| --- | --- |
| W | Acelera com 80% do acelerador (`ThrottleTurbo` 0.2 reserva os outros 20%) |
| W + Shift (turbo) | 100% do acelerador |
| S em movimento | Freia |
| S parado | Engata a ré sozinho (não existe tecla de ré). Veja a [seção de bugs](09-bugs.md): a ré nativa não anda |
| A / D andando | Vira, com a esteira de fora mais rápida |
| A / D parado | Gira no lugar (pivot / neutral steering) |

4. Abra o *Log Console* e procure linhas com `Tracked`, `Wheels`, `Engine` ou `TrackWheel`. A seção [Solução de problemas](12-troubleshooting.md) traduz cada erro.
5. Para a ré funcionar de verdade, adicione o componente da seção [Scripts prontos](11-scripts.md).
6. Compare com o resultado esperado em [Exemplos em vídeo](05-video-examples.md).

---

[← Requisitos do modelo 3D](03-model-requirements.md) · [Índice](../../README.pt-BR.md) · [Exemplos em vídeo →](05-video-examples.md)
