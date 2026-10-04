> 🇧🇷 Português · [🇬🇧 English](../en/13-appendix.md)

[← Solução de problemas](12-troubleshooting.md) · [Índice](../../README.pt-BR.md)

# Apêndice

### A. Bloco completo do Leopard 2A7

Este é o bloco que está no `Leopard2A7_base.et` do mod, sem comentários. Ele vai dentro de `components { }` do prefab do casco.

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
       TrackPart Sprocket_R {
        Type Sprocket
        Pivot Pivot "{56A9D7F8C0E3E63F}" {
         Bone "v_sprocket_r"
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
  }
```

No nó de controladores, o Leopard só muda `ThrottleReverseTarget 1`. O `RigidBody` tem `Mass 64500` e `CenterOfMass 0 1 -0.3`.

### B. GUIDs de recursos vanilla

| Recurso | GUID |
| --- | --- |
| Jogo base (`ArmaReforger.gproj`) | `58D0FB3206B6F859` |
| `Prefabs/Vehicles/Core/TrackedVehicle_Base.et` | `{0608D8FA71FD3433}` |
| `Prefabs/Vehicles/Core/Components/VehicleTrackedSimulation_Base.ct` | `{BBF0C4D0AF8761DC}` |
| `Prefabs/Vehicles/Core/Vehicle_Base.et` (pai do `TrackedVehicle_Base.et`) | `{4085446E2B406849}` |
| `Prefabs/Vehicles/Core/Components/VehicleWheeledSimulation_Base.ct` | `{A25CB790B888B786}` |
| `armor_100mm.gamemat` | `{5824DB4DA1A28E22}` |
| `armor_80mm.gamemat` | `{B6F2475AE2D7E8FD}` |
| `armor_40mm.gamemat` | `{8E8A10341B3BF250}` |
| `armor_20mm.gamemat` | `{33CE4B76B338E44F}` |
| Elo de esteira do M113 (não vem no jogo) | `{32FF46BA61EE0DCA}` |

O `ID "BBCBA43A9778AE21"` dentro do `TrackedVehicle_Base.et` é o id da entidade, **não** o GUID do arquivo.

### C. Ids de instância do base de esteira

Use estes ids para sobrescrever os componentes herdados:

| Componente | Id |
| --- | --- |
| `VehicleTrackedSimulation` | `{6160DE6434513C4D}` |
| `Simulation Tracked` | `{6160DE65F2933990}` |
| `Differential TripleDifferential` | `{663757C64F4BFDDD}` |
| `Aerodynamics` | `{6160DE6521A7B70A}` |
| `BaseVehicleNodeComponent` | `{5D6501223785D0E7}` |
| `SCR_TrackedControllerComponent` | `{60F8D3F71B2F1D79}` |
| `NwkTrackedMovementComponent` | `{5D6CA5AFEC980F35}` |
| `RigidBody` | `{51DAA09FECF52BBF}` |
| `MeshObject` | `{51DAA09FEFBFC0E7}` |
| `SCR_BaseCompartmentManagerComponent` | `{20FB66C5DCB8DF72}` |
| `SCR_WheeledDamageManagerComponent` | `{141326E9FD94FE40}` |

### D. Endereços no executável

Para quem quiser conferir a engenharia reversa. Arquivo `ArmaReforgerWorkbenchSteamDiag.exe` 1.8.0.13, base `0x140000000`.

| Endereço | Função |
| --- | --- |
| `0x140ea7050` | Passo da simulação de esteira |
| `0x140e9feb0` | Relação por lado (divide a carga) |
| `0x140ea7ac0` / `0x140ea7aec` | Multiplicação e divisão da reação pela relação |
| `0x140e85830` | Sincronização do motor (aplica a reação na rotação) |
| `0x140e85060` | Curva de torque do motor |
| `0x140e86070` | Tabela do câmbio `[−\|Reverse\|, 0, Forward...]` |
| `0x140fe8320` | Máquina de estados da seleção de sentido |
| `0x140fe7cf0` / `0x140fe8880` | Pedido de marcha e máquina de troca |
| `0x140fe85c0` | Embreagem na arrancada |
| `0x140fe76c0` / `0x140fee110` | Leitura das entradas e portão de entradas |
| `0x140fc10a0` | Ligação defeituosa de `GetInputs()` (crash 1) |
| componente + `0x1638` | Corrente visual de elos, lida sem checagem pelo diag (crash 2) |
| `0x140fe4940` | Reexecução dos comandos no servidor (`NwkTrackedMovementComponent`) |

### E. Ferramentas usadas neste projeto

| Ferramenta | Para quê |
| --- | --- |
| `wbclient.py` | Cliente da API de rede do Workbench (JSON-RPC em `127.0.0.1:5775`) |
| `pakx.py` / `pakgrep.py` | Ler e buscar arquivos dentro dos `.pak` do jogo |
| `rdbguid.py` | Achar o GUID de um recurso vanilla no `resourceDatabase.rdb` |
| `prep_hier.py` / `export.py` | Converter o `.blend` hierarquizado em esqueleto e exportar o FBX |
| `assemble.py` | Montar os prefabs do mod a partir dos rascunhos |
| `LEO_TelemetryComponent` | Medir o comportamento em jogo |

---

[← Solução de problemas](12-troubleshooting.md) · [Índice](../../README.pt-BR.md)
