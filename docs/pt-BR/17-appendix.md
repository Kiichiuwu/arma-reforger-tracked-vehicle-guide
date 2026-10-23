> 🇧🇷 Português · [🇬🇧 English](../en/17-appendix.md)

[← Diagnóstico do veículo e telemetria](16-debugging.md) · [Índice](../../README.pt-BR.md) · [Multiplayer →](18-multiplayer.md)

# Apêndice

### A. Bloco completo do Leopard 2A6

Este é o bloco `VehicleTrackedSimulation` do `Leopard2A6_base.et` do mod, sem comentários. Ele vai dentro de `components { }` do prefab do casco. O mesmo texto está em [`examples/leopard2a6/VehicleTrackedSimulation_Leopard2A6.et.txt`](../../examples/leopard2a6/VehicleTrackedSimulation_Leopard2A6.et.txt).

```
  VehicleTrackedSimulation "{6160DE6434513C4D}" {
   Simulation Tracked "{6160DE65F2933990}" {
    SolverSubsteps 4
    Engine Engine Engine {
     Inertia 3
     Steepness 50
     Friction 470
     MaxPower 1100
     MaxTorque 4700
     RpmMaxPower 2600
     RpmMaxTorque 1600
     BrakingTorqueRatio 0.15
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
     Ratio 4.6
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
     Radius 0.418
     BrakeTorque 6000
     Suspension Suspension "{DBF219524735553D}" {
      MaxSteeringAngle 0
      SpringRate 450
      CompressionDamper 25000
      RelaxationDamper 35000
      MaxTravelUp 0.30
      MaxTravelDown 0.10
      RayStartOffsetUp 0.5
     }
    }
    Sprocket Sprocket "{68A5D8FC81D86ECE}" {
     Mass 250
     Radius 0.34
     BrakeTorque 30000
    }
    Idler Idler "{C8F9E465BBF2B04F}" {
     Mass 150
     Radius 0.335
     BrakeTorque 0
    }
    Roller Roller "{EC9CBBD192126CCB}" {
     Mass 20
     Radius 0.152
     BrakeTorque 0
    }
    RegenerativeSteering 1
    NeutralSteering 1
    PivotSteering 1
    PivotSteeringThreshold 0.5
    BrakeSteering 0
    RetarderBaseTorque 0
    RetarderThrottleSpeed 20
    Tracks {
     Track Left {
      Wheels {
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
       TrackPart Wheel_L03 {
        Pivot Pivot "{FF507AFB45B12875}" {
         Bone "v_wheel_l03"
        }
       }
       TrackPart Wheel_L04 {
        Pivot Pivot "{4B7AD08C0BDD71B4}" {
         Bone "v_wheel_l04"
        }
       }
       TrackPart Wheel_L05 {
        Pivot Pivot "{ED4C344B4513B0A9}" {
         Bone "v_wheel_l05"
        }
       }
       TrackPart Wheel_L06 {
        Pivot Pivot "{D7F1EB919261FBB6}" {
         Bone "v_wheel_l06"
        }
       }
       TrackPart Wheel_L07 {
        Pivot Pivot "{498B3EE52377E3C9}" {
         Bone "v_wheel_l07"
        }
       }
       TrackPart Idler_L {
        Type Idler
        Pivot Pivot "{C7D9DF8DAAE72E34}" {
         Bone "v_idler_l"
        }
       }
       TrackPart Sprocket_L {
        Type Sprocket
        Pivot Pivot "{F67E7A2BDE42902D}" {
         Bone "v_sprocket_l"
        }
       }
       TrackPart Roller_L01 {
        Type "Return Roller"
        Pivot Pivot "{495CE3DBFC7DD5D5}" {
         Bone "v_roller_l01"
        }
       }
       TrackPart Roller_L02 {
        Type "Return Roller"
        Pivot Pivot "{068B4C4818A9ABAB}" {
         Bone "v_roller_l02"
        }
       }
       TrackPart Roller_L03 {
        Type "Return Roller"
        Pivot Pivot "{857B6B1E4C45E6F7}" {
         Bone "v_roller_l03"
        }
       }
       TrackPart Roller_L04 {
        Type "Return Roller"
        Pivot Pivot "{92F7DC37B619974C}" {
         Bone "v_roller_l04"
        }
       }
      }
     }
     Track Right {
      Wheels {
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
       TrackPart Wheel_R03 {
        Pivot Pivot "{C45AA21B5FB1DC10}" {
         Bone "v_wheel_r03"
        }
       }
       TrackPart Wheel_R04 {
        Pivot Pivot "{1B31E132532A4DC9}" {
         Bone "v_wheel_r04"
        }
       }
       TrackPart Wheel_R05 {
        Pivot Pivot "{73214A0BD83CBA54}" {
         Bone "v_wheel_r05"
        }
       }
       TrackPart Wheel_R06 {
        Pivot Pivot "{1135D63A6B8A7A7F}" {
         Bone "v_wheel_r06"
        }
       }
       TrackPart Wheel_R07 {
        Pivot Pivot "{078A016009810BDF}" {
         Bone "v_wheel_r07"
        }
       }
       TrackPart Idler_R {
        Type Idler
        Pivot Pivot "{78491E57BF71F53D}" {
         Bone "v_idler_r"
        }
       }
       TrackPart Sprocket_R {
        Type Sprocket
        Pivot Pivot "{56A9D7F8C0E3E63F}" {
         Bone "v_sprocket_r"
        }
       }
       TrackPart Roller_R01 {
        Type "Return Roller"
        Pivot Pivot "{EB15271F764DB1D9}" {
         Bone "v_roller_r01"
        }
       }
       TrackPart Roller_R02 {
        Type "Return Roller"
        Pivot Pivot "{0DE3390B35CF1172}" {
         Bone "v_roller_r02"
        }
       }
       TrackPart Roller_R03 {
        Type "Return Roller"
        Pivot Pivot "{61C42E2BEBDF2121}" {
         Bone "v_roller_r03"
        }
       }
       TrackPart Roller_R04 {
        Type "Return Roller"
        Pivot Pivot "{CCE7F4BD417F618E}" {
         Bone "v_roller_r04"
        }
       }
      }
     }
    }
    Aerodynamics Aerodynamics "{6160DE6521A7B70A}" {
     ReferenceArea 9.5
     DragCoefficient 0.9
    }
   }
   TrackSegments 73
   TrackPositions {
    -2.0577 1.2299 -1.8483 1.2299 -1.6390 1.2299 -1.4296 1.2299 -1.2202 1.2299 -1.0108 1.2299 -0.8015 1.2299 -0.5921 1.2299 -0.3827 1.2299 -0.1734 1.2299 0.0360 1.2299 0.2454 1.2299 0.4548 1.2299 0.6641 1.2299 0.8735 1.2299 1.0829 1.2299 1.2922 1.2299 1.5016 1.2299 1.7110 1.2299 1.9204 1.2299 2.1297 1.2299 2.3388 1.2188 2.5479 1.2074 2.7569 1.1960 2.9660 1.1846 3.1749 1.1720 3.3601 1.0817 3.4574 0.9002 3.4301 0.6960 3.2918 0.5419 3.1294 0.4097 2.9669 0.2776 2.8045 0.1455 2.6313 0.0298 2.4256 0.0006 2.2163 0.0006 2.0069 0.0006 1.7975 0.0006 1.5881 0.0006 1.3788 0.0006 1.1694 0.0006 0.9600 0.0006 0.7507 0.0006 0.5413 0.0006 0.3319 0.0006 0.1225 0.0006 -0.0868 0.0006 -0.2962 0.0006 -0.5056 0.0006 -0.7149 0.0006 -0.9243 0.0006 -1.1337 0.0006 -1.3431 0.0006 -1.5524 0.0006 -1.7618 0.0006 -1.9712 0.0006 -2.1805 0.0006 -2.3899 0.0006 -2.5976 0.0181 -2.7826 0.1146 -2.9616 0.2232 -3.1406 0.3317 -3.3197 0.4403 -3.4980 0.5500 -3.6193 0.7165 -3.6222 0.9226 -3.5055 1.0925 -3.3122 1.1638 -3.1031 1.1748 -2.8940 1.1859 -2.6849 1.1969 -2.4759 1.2080 -2.2668 1.2191
   }
   TrackLength 0.2110
   TrackThickness 0.076
   TrackOffset1 -1.3281 0 0
   TrackOffset2 1.3281 0 0
   TrackSegment "{B50AEDC702735F50}Assets/Vehicles/Tracked/Leopard2A6/Leopard2A6_track_link.xob"
  }
```

Notas sobre o bloco:

- Os sete atributos da corrente de elos (de `TrackSegments` a `TrackSegment`) pertencem à própria `VehicleTrackedSimulation`. Eles vêm **depois** da chave que fecha o `Simulation Tracked`, não dentro dele.
- O `TrackPositions` é uma lista plana de 146 números: 73 pares (z, y) no espaço do modelo do casco. A [`tools/trackpositions.py`](../../tools/trackpositions.py) a reproduz exatamente ([Scripts prontos](11-scripts.md)).
- O `TrackSegment` aponta para o modelo de elo do próprio mod do Leopard. Troque pelo seu ([capítulo 15](15-track-links.md)).
- A ordem dos `TrackPart` é a mesma nas duas lagartas: rodas de apoio primeiro (índice 0–6), depois a roda-guia (7), a roda motriz (8) e os roletes de retorno (9–12). O `WheelCount()` informa 13 por lagarta. A corrente nativa tira a velocidade das peças 0 e 1, então as duas precisam ser rodas de apoio (engenharia reversa, [capítulo 15](15-track-links.md)). O `LEO_RunningGearComponent` espera essa mesma ordem ([Scripts prontos](11-scripts.md)).
- O `Type "Return Roller"` é escrito como texto entre aspas; o motor aceita, e o modelo `Roller` define os roletes de retorno.
- A edição anterior deste guia usava um bloco do Leopard 2A7 (roda-guia primeiro, `Ratio` 4.9, sem roletes de retorno, sem corrente de elos). Ele continua no histórico do git, na tag `v1-leopard2a7`.

Os atributos da corrente, como o executável 1.8.0.13 os registra (engenharia reversa):

| Atributo | Tipo | Leopard 2A6 | O que o motor faz com ele |
| --- | --- | --- | --- |
| `TrackSegments` | int | 73 | Lido só pela ação de editor *Generate track positions*, como o número de pontos a gerar. Em execução, o número de elos é o número de entradas do `TrackPositions` |
| `TrackPositions` | array de Vector2 | 73 pares | Os pontos de repouso da corrente (z, y), compartilhados pelas duas lagartas. O índice 0 fica no trecho de cima e o índice cresce para a frente ao longo dele. A corrente só é montada com 2 ou mais entradas |
| `TrackLength` | float | 0.2110 | Distância de repouso entre pontos vizinhos. O comprimento da corrente é N × `TrackLength`. A corrente só é montada se for acima de 0 |
| `TrackThickness` | float | 0.076 | Guardado, nunca lido |
| `TrackOffset1` | Vector3 | −1.3281 0 0 | Deslocamento do primeiro `Track` (esquerdo); x é a posição lateral da esteira |
| `TrackOffset2` | Vector3 | 1.3281 0 0 | Deslocamento do segundo `Track` (direito) |
| `TrackSegment` | ResourceName | `.xob` do elo do Leopard | Modelo de cada entidade de elo (2 × N entidades, sem física) |

### B. Overrides de componentes no casco do Leopard 2A6

O resto da configuração de veículo de esteira do prefab do casco. A primeira parte está em [`examples/leopard2a6/HullComponents_Leopard2A6.et.txt`](../../examples/leopard2a6/HullComponents_Leopard2A6.et.txt):

```
  RigidBody "{51DAA09FECF52BBF}" {
   Mass 62300
   CenterOfMass 0 1 0.46
   LinearSleepingThreashold 0.4
   AngularSleepingThreashold 0.5
  }
  CarProcAnimComponent "{5D6501223785D009}" {
  }
  LEO_RunningGearComponent "{5292A83C2D6FC873}" {
  }
  LEO_TrackedDriveComponent "{7A1D4C9E2B6F3086}" {
  }
  LEO_TelemetryComponent "{3C5E7A9B1D2F4608}" {
  }
  LEO_TrackBeltComponent "{8E3A61C04F2D9B57}" {
  }
```

No nó dos controladores, o Leopard só muda o `ThrottleReverseTarget`:

```
  BaseVehicleNodeComponent "{5D6501223785D0E7}" {
   components {
    SCR_TrackedControllerComponent "{60F8D3F71B2F1D79}" {
     ThrottleReverseTarget 1
    }
   }
  }
```

- O `CarProcAnimComponent` fica vazio; os componentes `LEO_` mantêm os atributos padrão, que são os valores do Leopard ([Scripts prontos](11-scripts.md)).
- O `CenterOfMass` é um **deslocamento** a partir do centro de massa definido no modelo, não uma posição absoluta. O executável o descreve como "Offset of center of mass. Relative to one defined in model (if applicable)." O modelo do casco do 2A6 tem um empty `COM_hull` em (0,000176; 1,16777; −0,38093), então `0 1 0.46` põe o centro de massa da física em (0,0002; 2,1678; 0,0791), 2,17 m acima do chão (medido pelo `WheelGetPosition`). O z 0.46 nivela o casco (inclinação de −0,049° parado). O deslocamento Y ainda não foi reajustado (em aberto, [capítulo 13](13-suspension.md)).
- Os limites de sono deixam um tanque parado dormir. Enquanto ele dorme, as linhas da telemetria se repetem bit a bit.

### C. Geometria do trem de rolamento do Leopard 2A6

Posições de repouso dos ossos no espaço do modelo do casco, em metros (eixos do Enfusion: x para a direita, y para cima, z para a frente; chão em y = 0). Mostrado o lado esquerdo; o lado direito tem o x oposto. Medido no `.fbx` do casco instalado.

| Peça | Ossos | x (esquerda) | y | z (frente para trás) |
| --- | --- | --- | --- | --- |
| Rodas de apoio | `v_wheel_l01` … `v_wheel_l07` | −1,3288 | 0,41858 | 2,47784, 1,66161, 0,88941, 0,16036, −0,72156, −1,59587, −2,47779 |
| Roda-guia | `v_idler_l` | −1,3298 | 0,84028 | 3,12783 |
| Roda motriz | `v_sprocket_l` | −1,3307 | 0,82407 | −3,29679 |
| Roletes de retorno 01 e 03 | `v_roller_l01`, `v_roller_l03` | −1,4193 | 1,07793 | 2,13063, −0,40004 |
| Roletes de retorno 02 e 04 | `v_roller_l02`, `v_roller_l04` | −1,1593 | 1,07793 | 1,34050, −2,05767 |

Os ossos de suspensão `v_suspension_l0<k>` e os rotators `v_wheel_l0<k>_rotator` ficam no mesmo ponto da roda deles.

Raio da simulação contra raio visual. Cada raio da simulação é cerca de uma espessura de elo maior que o aro, porque a simulação mede até a face da esteira em contato com o chão:

| Peça | `Radius` da simulação | Raio visual do aro | Massa | `BrakeTorque` |
| --- | --- | --- | --- | --- |
| Roda de apoio | 0,418 m | 0,345 m | 110 kg | 6000 |
| Roda-guia | 0,335 m | 0,264 m | 150 kg | 0 |
| Roda motriz | 0,34 m | cerca de 0,29 m (as notas variam até 0,32; nenhum script usa) | 250 kg | 30000 |
| Rolete de retorno | 0,152 m | 0,0847 m | 20 kg | 0 |

Esteira e elos:

| Valor | Leopard 2A6 | Como se sabe |
| --- | --- | --- |
| Elos na esteira do próprio modelo | 71 por lado | Medido na malha |
| Passo do elo no modelo | 0,2032–0,2127 m, média 0,2101 m | Medido na malha |
| Espessura da sapata | 0,076 m | Medido na malha |
| Largura e centro da esteira | 0,6355 m, centrada em x ∓1,3281 | Medido na malha |
| Modelo do elo `Leopard2A6_track_link.xob` | 118 vértices (LOD0), 47 (LOD1); origem no centro do par de sapatas, face do chão em y = 0; sem colisores | Medido |
| Caminho da esteira em volta dos círculos da simulação, em repouso | 15,284 m | Calculado pelos ossos e raios |
| Comprimento da corrente | 73 × 0,2110 = 15,403 m (11,9 cm acima do caminho em repouso) | Calculado |

Esses valores vêm do mod feito sobre o modelo ["Leopard 2 A6"](https://sketchfab.com/3d-models/leopard-2-a6-7cb23d5322df4b409a880de635826067) de [buh-late](https://sketchfab.com/buh-late), licenciado sob [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/), publicado em 17/01/2024. Ele foi modificado para o Enfusion (dividido em partes, novos pais, ossos e rig de suspensão, colisores, LODs, texturas refeitas). O autor não endossa este projeto. Veja [CREDITS.md](../../CREDITS.md).

### D. GUIDs de recursos vanilla

| Recurso | GUID |
| --- | --- |
| Jogo base (`ArmaReforger.gproj`) | `58D0FB3206B6F859` |
| `Prefabs/Vehicles/Core/TrackedVehicle_Base.et` | `{0608D8FA71FD3433}` |
| `Prefabs/Vehicles/Core/Components/VehicleTrackedSimulation_Base.ct` | `{BBF0C4D0AF8761DC}` |
| `Prefabs/Vehicles/Core/Vehicle_Base.et` (pai do `TrackedVehicle_Base.et`) | `{4085446E2B406849}` |
| `anims/proc/wheel_rotation.pap` | `{CCE4D4889B1BB408}` |
| `anims/proc/wheel_suspension.pap` | `{FEFBB47B33FB500F}` |
| `armor_100mm.gamemat` | `{5824DB4DA1A28E22}` |
| `armor_80mm.gamemat` | `{B6F2475AE2D7E8FD}` |
| `armor_40mm.gamemat` | `{8E8A10341B3BF250}` |
| `armor_20mm.gamemat` | `{33CE4B76B338E44F}` |
| Elo de esteira do M113 (não vem com o jogo) | `{32FF46BA61EE0DCA}` |

O `ID "BBCBA43A9778AE21"` dentro do `TrackedVehicle_Base.et` é o ID da entidade, **não** o GUID do arquivo.

Os dois arquivos `.pap` são as animações procedurais vanilla das rodas dos veículos de rodas:

| Arquivo | Vagas de osso | Sinal |
| --- | --- | --- |
| `wheel_rotation.pap` | `wheel` | Rotação em volta do X a partir de `rotation` (faixa do editor −360..360, graus) |
| `wheel_suspension.pap` | `wheel`, `suspension`, `rotator` | `wheel`: rotação em volta do X a partir de `rotation`. `suspension`: translação ao longo do Y a partir de `suspension`. `rotator`: esterçamento. A faixa de prévia da entrada de suspensão no editor é −1..1; na simulação de esteira o sinal está em metros, de −`MaxTravelDown` a +`MaxTravelUp` (medido) |

Num veículo de esteira no 1.8.0.13, o `CarProcAnimComponent` liga esses sinais às rodas erradas. Não os use no trem de rolamento ([capítulo 14](14-running-gear.md)).

O elo do M113 só é citado por uma rotina de teste do motor e não está em nenhum pak de dados. Use o seu próprio modelo de elo.

### E. IDs de instância da base de esteira

Use estes IDs para sobrescrever os componentes herdados:

| Componente | ID |
| --- | --- |
| `VehicleTrackedSimulation` | `{6160DE6434513C4D}` |
| `Simulation Tracked` | `{6160DE65F2933990}` |
| `Differential TripleDifferential` | `{663757C64F4BFDDD}` |
| `Aerodynamics` | `{6160DE6521A7B70A}` |
| `BaseVehicleNodeComponent` | `{5D6501223785D0E7}` |
| `SCR_TrackedControllerComponent` | `{60F8D3F71B2F1D79}` |
| `NwkTrackedMovementComponent` | `{5D6CA5AFEC980F35}` |
| `CarProcAnimComponent` (vazio na base) | `{5D6501223785D009}` |
| `RigidBody` | `{51DAA09FECF52BBF}` |
| `MeshObject` | `{51DAA09FEFBFC0E7}` |
| `SCR_WheeledDamageManagerComponent` | `{141326E9FD94FE40}` |

Os modelos de roda (`RoadWheel`, `Sprocket`, `Idler`, `Roller`), as curvas de atrito e os IDs de `Pivot` do bloco A são do próprio Leopard. A base vanilla não define nenhum deles.

### F. API de script da simulação de esteira

O `TrackedControllerComponent.GetSimulation()` devolve a `VehicleTrackedSimulation`. Os métodos dela no 1.8.0.13, dos arquivos gerados `VehicleTrackedSimulation.c` e `VehicleBaseSimulation.c`, com o que este trabalho descobriu. O [capítulo 14](14-running-gear.md) explica os getters das rodas em uso.

Rodas e lagartas (os índices são a lagarta, depois o índice do `TrackPart` na ordem do prefab):

| Método | Comportamento | Como se sabe |
| --- | --- | --- |
| `TrackCount()` | 0 até a simulação carregar, depois sempre 2 (não lê o prefab) | Engenharia reversa |
| `WheelCount()` | `TrackPart` de **uma** lagarta: 13 no Leopard | Medido (`LEOGEAR`) |
| `WheelGetPosition(track, wheel, displacement = 0)` | Centro da roda em relação ao centro de massa da física. Displacement 0 = curso todo para baixo. O único getter que confere o índice da roda | Medido; engenharia reversa |
| `WheelHasContact(track, wheel)` | Verdadeiro enquanto o raio da suspensão acerta algo | Medido |
| `WheelGetContactPosition(track, wheel)` | Ponto de contato no espaço do mundo. Um valor guardado, devolvido também sem contato | Medido; engenharia reversa |
| `WheelGetContactNormal`, `WheelGetContactMaterial`, `WheelGetContactEntity` | Normal, material e entidade no contato | Documentação da API; não usados |
| `WheelGetContactLiquidState`, `WheelGetContactLiquidMaterial`, `WheelGetContactLiquidPosition` | Contato com água e outros líquidos | Documentação da API; não usados |
| `WheelGetRPM(track, wheel)` | Velocidade angular da roda × 60/2π. Sinal da velocidade para a frente. Uma roda de apoio lê a velocidade da esteira / o seu `Radius`; uma roda sem contato pode ler valores bem diferentes | Medido; engenharia reversa |
| `WheelGetRadius(track, wheel)` | O `Radius` do modelo | Medido |
| `WheelGetRollingDrag`, `WheelSetRollingDrag` | Porcentagem de resistência ao rolamento | Documentação da API; não testado |

Os getters conferidos (`TrackCount`, `WheelCount`, `WheelGetPosition`, `WheelHasContact`, `WheelGetContactPosition`, `WheelGetContactNormal`, `WheelGetRadius`, `WheelGetRPM`, `WheelGetRollingDrag`) testam primeiro se a simulação carregou, e devolvem 0, false ou um vetor zero se não. Nenhuma ligação confere o índice da lagarta, e só o `WheelGetPosition` confere o índice da roda: um índice fora da faixa lê memória alheia (engenharia reversa).

Transmissão e estado:

| Método | Notas |
| --- | --- |
| `GetGear()`, `SetGear(int)` | 0 = R, 1 = N, 2.. = marchas à frente. O `SetGear` confere a faixa |
| `GetClutch()`, `SetClutch(float)` | 0 = desacoplada, 1 = acoplada |
| `GetBrake()`, `SetBreak(float, bool hb)` | `SetBreak(x, true)` guarda 6,0, então o `GetBrake()` lê 6,0 para o freio de mão |
| `GetThrottle()`, `SetThrottle(float)`, `IsHandbrakeOn()` | Vazias na simulação de esteira: sempre 0, sem efeito, sempre false |
| `GetInputs(out, out)` | **Derruba**: a ligação escreve através de um ponteiro nulo. Nunca chame |
| `SetInputs(steer, throttle)` | Não testado |
| `EngineStart()`, `EngineStop()`, `EngineIsOn()` | Estado do motor |
| `EngineGetRPM()`, `EngineGetRPMFeedback()`, `EngineGetLoad()` | O Feedback é o rpm devolvido pelo lado do câmbio (rpm da roda motriz × marcha × `Ratio`, visto no diagnóstico) |
| `EngineGetRPMIdle()`, `EngineGetRPMPeakTorque()`, `EngineGetRPMPeakPower()`, `EngineGetRPMMax()` | Valores do prefab |
| `EngineGetPeakTorque()`, `EngineGetPeakTorqueState()`, `EngineSetPeakTorqueState(float)`, e o mesmo para `PeakPower` | Pico de torque (N·m) e de potência (kW), inicial e atual |
| `GearboxGetEfficiency()`, `GearboxGetEfficiencyState()`, `GearboxSetEfficiencyState(float)`, `GearboxGearsCount()`, `GearboxForwardGearsCount()` | Valores do câmbio |
| `ForceEnableSimulation()` | Só para cinemáticas, segundo a documentação da API |

Da classe base `VehicleBaseSimulation`: `IsValid()`, `GetWorldTransform()` (espaço interno da física), `GetVelocity()`, `GetVelocityLocal()`, `GetAngularVelocity()`, `GetAngularVelocityLocal()`, `GetAcceleration()`, `GetAccelerationLocal()`, `GetSpeedKmh()`, `GetGForce()`, `GetGForceMagnitude()`, `HasAnyGroundContact()`, `SetSurfaceOverride()`.

A API de script **não** tem acesso à corrente de elos: nenhum getter, nenhum setter, nenhum reinício (a `VehicleTrackedSimulationClass` é vazia).

### G. Sinais da simulação para animação procedural

A simulação de esteira oferece estes sinais ao `CarProcAnimComponent` (engenharia reversa da tabela de nomes de sinais do executável):

| Por | Sinais |
| --- | --- |
| Roda | `steering`, `suspension`, `rotation`, `damper`, `longslip`, `latslip` |
| Lagarta | `trackSpeed` (km/h), `trackLongSlip`, `trackLatSlip` |

O `rotation` está em graus (menos o ângulo visual da roda) e fica em 0 a menos que a corrente de elos exista. O `suspension` está em metros ao longo do Y do osso. A simulação nunca move ossos sozinha. No 1.8.0.13 a ligação desses sinais às rodas de esteira está errada ([capítulo 14](14-running-gear.md)).

### H. Entradas do menu de diagnóstico do veículo

No executável Diag, abra o menu de diagnóstico com `Win` esquerdo + `Alt` no *Play*. As entradas abaixo são as que aparecem no vídeo de teste A (06/10/2026). O [capítulo 16](16-debugging.md) explica como usá-las e como ler a sobreposição.

O menu de veículos tem os submenus *Helicopter*, *Tracked*, *Portals*, *Vehicle Networking and Reconciliation* e *Compartments*, e depois estas entradas:

| Entrada | Visto no teste de 06/10/2026 |
| --- | --- |
| *Show stats* | Não testada |
| *Show vehicle debug* | Chave geral da sobreposição. Com ela ligada, o painel *Debug* (Steering, Clutch, Brake, Throttle, Gearbox Gear, Speed, Slope, G-Force, Y/P/R) aparecia. Com uma corrente de elos não travou |
| *Player vehicle only* | Ligada durante o teste |
| *Show CoM* | Não testada |
| *Show inertia* | Mostra o tensor de inércia (327602.7, 395692.4, 94730.0 no 2A6) |
| *Show forces* | Linhas vermelhas de força; com 62 t saem da tela |
| *Show engine* | Ligada durante o teste, com o painel *Engine* à vista (Running, Load, Torque, Power, RPM, Feedback, gráfico) |
| *Show contacts* | Alternada sem travar |
| *Show raycast* | Alternada sem travar |
| *Show suspension* | Ligada durante o teste; marcadores de suspensão desenhados em cada roda |
| *Show swaybar*, *Show wheels*, *Show bones*, *Show slope* | *Show wheels* alternada sem travar; as outras não testadas |
| *Test raycast* | Alternada sem travar |
| *Toggle signals* | Não testada |
| *Reset vehicle* (lctrl+lalt+r), *Teleport up* (lctrl+lalt+t) | Atalhos mostrados no menu; não testadas |
| *Wheel drag set*, *Wheel drag value* | Não testadas |
| *Disable freelook*, *Enable debugger trace*, *Force vehicle active*, *Show hidden properties* | *Enable debugger trace* e *Show hidden properties* alternadas sem travar |
| *Wheel engine brake*, *MT simulation* | Ligadas durante o teste |
| *Show Controller Diags* | Acrescenta um painel *Inputs*. Alternada sem travar com a corrente presente |
| *Show Compartment Events* | Não testada |
| *Show dust materials*, *Fuel consumption*, *Visualize push force*, *Collisions debug prints* | Não ligadas à simulação de esteira; não testadas |

O submenu *Tracked* tem as entradas da corrente de elos. *Draw tracks*, *Track dynamic*, *Track gravity* e *Reset tracks* só existem nas versões Diag (busca de textos nos executáveis):

| Entrada | Valor mostrado | Visto no teste de 06/10/2026 |
| --- | --- | --- |
| *Steering mode* | wheeled | Selecionada; o valor ficou *wheeled* |
| *TrackSim* | true | Desligada por alguns segundos: nada visível mudou |
| *Draw tracks* | true | Desligada tira os círculos das rodas, a linha da corda e as estatísticas da corda por lagarta |
| *Track dynamic* | true | Não testada |
| *Track gravity* | true | Desligada: nenhum efeito visível pela câmera traseira |
| *Reset tracks* | disabled | Não testada |

### I. Endereços no executável

Para quem quiser conferir a engenharia reversa. Arquivo `ArmaReforgerWorkbenchSteamDiag.exe` 1.8.0.13, base `0x140000000`.

Transmissão, controlador e rede:

| Endereço | Função |
| --- | --- |
| `0x140ea7050` | Passo da simulação de esteira |
| `0x140e9feb0` | Relação por lado (divide a carga) |
| `0x140ea7ac0` / `0x140ea7aec` | Multiplica e divide a reação pela relação |
| `0x140e85830` | Sincronização do motor (aplica a reação ao rpm) |
| `0x140e85060` | Curva de torque do motor |
| `0x140e86070` | Tabela do câmbio `[−\|Reverse\|, 0, Forward...]` |
| `0x140fe8320` | Máquina de estados da escolha de sentido |
| `0x140fe7cf0` / `0x140fe8880` | Pedido de marcha e máquina de estados da troca |
| `0x140fe85c0` | Embreagem na arrancada |
| `0x140fe76c0` / `0x140fee110` | Leitura dos comandos e porta dos comandos |
| `0x140fe4940` | Repetição dos comandos no servidor (`NwkTrackedMovementComponent`) |

Carregamento, rodas e ligações de script:

| Endereço | Função |
| --- | --- |
| `0x140ea4b10` | Carregador da esteira: exatamente 2 lagartas, pelo menos 3 peças cada, roda-guia e roda motriz no mesmo índice nos dois lados; escolha pelo `Type` (Road Wheel 0, Sprocket 1, Idler 2, Return Roller 3) |
| `0x140ea4b00` | Teste de validade chamado primeiro pelas ligações dos getters conferidos (simulação + `0x1480` == 2) |
| `0x140fc49e0` | Ligação do `TrackCount()` (devolve a constante 2 quando válida) |
| `0x140fc5490` | Ligação do `WheelCount()` |
| `0x140fc5bf0` | Ligação do `WheelGetPosition()`, a única que confere o índice da roda |
| `0x140fc5d10` | Ligação do `WheelGetRPM()`; o valor é a velocidade angular × 9,549296 |
| `0x140fc10a0` | Ligação defeituosa do `GetInputs()` (travamento 1) |
| `0x140fc1270` / `0x140fc1370` | Fornecedor de sinais de animação procedural (busca da roda pelo nome com uma comparação simples de texto) e preenchedor de valores |
| `0x142fa44e0` | Tabela de nomes de sinais (`steering`, `suspension`, `rotation`, ... `trackSpeed`) |

Corrente de elos (corda):

| Endereço | Função |
| --- | --- |
| `0x140fc18d0` | Registra os atributos da corrente depois de `Simulation` |
| `0x140fbc1f0` | Construtor do componente: monta as duas cordas e cria 2 × N entidades de elo quando há 2 ou mais `TrackPositions` e o `TrackLength` é acima de 0 |
| `0x140fc0520` | *Generate track positions* (ação de editor 1; despachante `0x140fc5390`). Semeia uma elipse, roda 1000 passos de corda de 1/60 s, escreve o índice 0 em cima com o índice crescendo para a frente ao longo do trecho de cima |
| `0x140fc4090` | Atualização da corrente por passo; sai cedo a menos que as duas cordas existam |
| `0x140fc4430` | Velocidade angular a partir da variação de ângulo do `TrackPart` 1; move os ângulos visuais das rodas e a corrente |
| `0x140fc13d0` | `trackSpeed` = raio do `TrackPart` 0 × ω × 3,6 |
| `0x140fbfed0` | Junta um círculo por `TrackPart` (raio da simulação, posição atual) |
| `0x140a478c0` | Passo da corda: subpasso fixo de 0,0083 s, 25 iterações de restrição em cada |
| `0x140a43a60` | Integração de Verlet de cada ponto; gira o ponto preso pela roda motriz |
| `0x140a40e80` | Relaxamento das restrições de distância |
| `0x140a41760` | Colisão com os círculos das rodas, e o encaixe dos pontos sob rodas que têm contato com o chão |
| `0x140fbe7f0` | Põe cada entidade de elo no ponto médio de dois pontos da corrente (`EOnPostFrame`) |
| `0x140fbdf30` | Desenho do diagnóstico. Passa os ponteiros da corda em componente + `0x1638` / `0x1640` para `0x140a41ce0` sem conferir nulo: travamento 2, só quando não existe corrente |

Rede e atrito (acrescentado em 17/10/2026, veja o [capítulo 18](18-multiplayer.md)):

| Endereço | Função |
| --- | --- |
| `0x14106c700` | Construtor do `NwkTrackedMovementComponent`: tolerâncias de reconciliação (distância |v|/300 limitada a 0,001–1,5 m, ângulo 0,003, velocidade 0,3, velocidade angular 0,25) |
| `0x1410743d0` | Despachante de rede por passo: o cliente do motorista produz (`0x141075190`) e é aprovado (`0x1410745f0`) ou corrigido (`0x141074770`); a autoridade processa (`0x141074940`) e consome (`0x14106e4d0`) |
| `0x140fe5d10` | Gravador do move: direção, acelerador, freio, freio de mão (do stub `IsHandbrakeOn` `0x140fc23c0`), marcha, embreagem, estado da troca |
| `0x140fe7c70` / `0x140fe4940` | Repetição do move no servidor, no controlador e na simulação; a ligação do `SetBreak` `0x140fc3c10` corta o freio em [0, 1] |
| `0x14104d8f0` | Comparação, no servidor, do estado dele com o relato do cliente |
| `0x141076250` / `0x141075cc0` | Correção no cliente e ressimulação nativa dos moves pendentes |
| `0x141054b50` | Produtor de moves do `NwkCarMovementComponent`, para comparação (36 chamadas contra 31 no de esteira) |
| `0x1410740b0` / `0x141053790` | Virtual de ligar a simulação, esteira / carro (bit 0x400 de +0xf4; ligar só seta o bit) |
| `0x140a448a0` / `0x140a410a0` | Força de contato da roda de apoio / pneu Magic Formula fixo; tabela de parâmetros `0x142e57a00`; caminho antigo de atrito desligado pelo byte `0x143b38fd1` |
| `0x1409dba90` / `0x140ba8310` | Atualização do gerenciador de câmera (câmeras por script só em keyframes) / condutor de keyframes da animação do personagem |

---

[← Diagnóstico do veículo e telemetria](16-debugging.md) · [Índice](../../README.pt-BR.md) · [Multiplayer →](18-multiplayer.md)
