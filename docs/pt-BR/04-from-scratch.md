> 🇧🇷 Português · [🇬🇧 English](../en/04-from-scratch.md)

[← Requisitos do modelo 3D](03-model-requirements.md) · [Índice](../../README.pt-BR.md) · [Exemplos em vídeo →](05-video-examples.md)

# Passo a passo do zero

Este passo a passo leva um modelo novo até um tanque que anda, vira e dá ré, com suspensão funcionando, rodas e elos de esteira em movimento. A não ser que o passo diga outra coisa, tudo foi testado no Leopard 2A6 no Workbench 1.8.0.13, só com o jogo base. Onde um número só foi medido na versão anterior, do Leopard 2A7, o texto avisa.

Os passos 1–9 dão um tanque que anda. Os passos 10–13 acrescentam a altura do casco, a animação do trem de rolamento, os elos da esteira e as ferramentas de depuração. Os capítulos 13–16 explicam cada um a fundo.

Os blocos completos do 2A6 estão em [examples/leopard2a6](../../examples/leopard2a6/): o bloco `VehicleTrackedSimulation` inteiro e os componentes do casco com os IDs.

### 1. Crie o projeto do mod

1. No Workbench, crie um projeto novo que dependa **só** do jogo base (`ArmaReforger`, GUID `58D0FB3206B6F859`).
2. Crie as pastas `Assets/Vehicles/Tracked/<Nome>/` e `Prefabs/Vehicles/Tracked/<Nome>/`.

### 2. Importe o modelo

1. Copie o `.fbx` do casco para `Assets/...` e use *Register and import* → *as Model*.
2. Defina *Merge Meshes* (desligado), *Export Skinning* e *Export Scene Hierarchy* (ligados). Reimporte.
3. Troque o material físico de cada colisor `UCX_*` por um `armor_XXmm.gamemat`.
4. Abra o `.xob` e confira que todos os ossos de peças aparecem na lista: rodas de apoio, ossos de suspensão, rodas-guia, rodas motrizes e roletes de retorno ([Requisitos do modelo 3D](03-model-requirements.md)).
5. Importe o modelo do elo da esteira do mesmo jeito. Ele não tem colisores. Você vai precisar dele no passo 12.

### 3. Crie o prefab do casco

O prefab herda do `TrackedVehicle_Base.et`, que já inclui o controlador, a rede e uma simulação vazia. Crie um arquivo de texto `.et` e registre-o no Resource Browser (botão direito → *Register*).

```
Vehicle : "{0608D8FA71FD3433}Prefabs/Vehicles/Core/TrackedVehicle_Base.et" {
 components {
  MeshObject "{51DAA09FEFBFC0E7}" {
   Object "{XOB_GUID}Assets/Vehicles/Tracked/<Name>/<Name>_hull.xob"
  }
  RigidBody "{51DAA09FECF52BBF}" {
   Mass 62300
   CenterOfMass 0 1 0.46
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

Os valores do `RigidBody` são os do 2A6. O `CenterOfMass` depende do seu modelo: veja o passo 6.

**Regra do ID:** o texto entre chaves depois do nome da classe é o ID da instância. Para **sobrescrever** um componente herdado, repita o ID vanilla, como acima. Um ID novo cria um segundo componente.

**Não repita a herança do `.ct`.** O `TrackedVehicle_Base.et` já declara o `VehicleTrackedSimulation_Base.ct`. No filho, escreva só `VehicleTrackedSimulation "{6160DE6434513C4D}" {`.

### 4. Preencha a simulação

O `VehicleTrackedSimulation_Base.ct` vanilla está incompleto. Ele só liga motor → embreagem → câmbio e define atrito e aerodinâmica. Faltam quatro coisas, e sem elas o log mostra `Tracked - failed to load!`:

| O que falta | O que escrever | Erro se faltar |
| --- | --- | --- |
| Curvas de atrito | `FrictionCurveLongitudinal` e `FrictionCurveLateral` | `FrictionCurveLateral - configuration is missing!` |
| Modelos de roda | `RoadWheel` (com `Suspension`), `Sprocket`, `Idler` e `Roller` se houver roletes de retorno | `TrackWheel's mass / radius / brake torque is incorrect!` |
| Lagartas | `Tracks { Track Left {...} Track Right {...} }` | `Invalid number of tracks! Must be 2!` |
| Motor válido | `Inertia`, `Steepness`, `Friction` maiores que zero | `Engine's inertia is incorrect! (must be greater than zero)` |

Cole isto dentro de `Simulation Tracked "{6160DE65F2933990}" { }`. São os valores atuais do Leopard 2A6:

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
Aerodynamics Aerodynamics "{6160DE6521A7B70A}" {
 ReferenceArea 9.5
 DragCoefficient 0.9
}
```

Os modelos `RoadWheel`, `Sprocket`, `Idler` e `Roller` são **um de cada**, compartilhados por todas as peças daquele tipo. Não existe raio por roda. Deixe o `Roller` de fora se o seu tanque não tiver roletes de retorno.

O que os valores significam para o seu tanque:

| Valor | Regra | Leopard 2A6 |
| --- | --- | --- |
| `Radius` de cada modelo | Distância do osso da peça até a face de contato dos elos em volta dela, não até o aro ([capítulo 3](03-model-requirements.md#o-que-o-osso-de-uma-peça-significa-para-a-simulação)) | Roda de apoio 0.418, roda motriz 0.34, roda-guia 0.335, rolete 0.152 |
| `Radius` da `Sprocket` e `Ratio` | O raio da roda motriz define a velocidade; escolha o `Ratio` para a sua velocidade máxima ([Ajuste](10-tuning.md)) | `Ratio` 4.6 com 0,34: na teoria 72–75 km/h em 4ª a 2600–2700 rpm. Não medido no 2A6; a versão anterior, do 2A7 (`Ratio` 4.9, roda motriz 0,355), mediu 74,3 km/h |
| `Suspension` | Passo 10 | `MaxTravelDown` 0.10 define a altura do casco |
| `RetarderBaseTorque`, `RetarderThrottleSpeed` | Com os padrões, o retardador limita a velocidade ([Referência de atributos](07-attribute-reference.md)) | 0 e 20 |

### 5. Declare as lagartas

Ainda dentro do `Simulation Tracked`, liste as peças de cada lagarta nesta ordem: **primeiro as rodas de apoio, da frente para trás**, depois a roda-guia, a roda motriz e os roletes de retorno. A primeira lagarta é a esquerda (índice 0); a segunda é a direita (índice 1).

```
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
  }
 }
}
```

O exemplo mostra 2 das 7 rodas de apoio e 1 dos 4 roletes de retorno por lado, para caber na página. A lista completa do 2A6 está em [VehicleTrackedSimulation_Leopard2A6.et.txt](../../examples/leopard2a6/VehicleTrackedSimulation_Leopard2A6.et.txt): `Wheel_X01`…`X07` (peças 0–6), `Idler_X` (7), `Sprocket_X` (8), `Roller_X01`…`X04` (9–12). Cada `Pivot` precisa de um ID próprio (quaisquer 16 dígitos hexadecimais, sem repetir).

**Por que esta ordem:**

- **A corrente de elos nativa lê as peças 0 e 1.** A velocidade dela é a velocidade angular da peça 1 vezes o raio da peça 0 (engenharia reversa do executável 1.8.0.13). As duas precisam ser rodas de apoio. Com uma roda-guia na frente da lista, a corrente andaria a raio da roda-guia / raio da roda de apoio da velocidade da esteira: cerca de 63% numa configuração antiga do 2A6.
- **Os scripts usam os mesmos índices.** A API de script endereça uma peça como (lagarta, posição nesta lista), e `WheelCount()` devolve as peças de uma lagarta (13 no 2A6). O `LEO_RunningGearComponent` espera as rodas de apoio em 0…n−1, a roda-guia em n, a roda motriz em n+1 e os roletes depois dela.
- **Os dois lados precisam bater.** O carregador quer a roda-guia e a roda motriz na mesma posição nas duas lagartas, e a simulação guarda uma só contagem de peças para as duas (engenharia reversa). Use a mesma lista nos dois lados.
- A física de condução funcionou com as duas ordens: a versão anterior, do 2A7, listava a roda-guia primeiro.

**Tipos:**

- **Rodas de apoio não levam `Type`.** O padrão é `Road Wheel`, e assim você evita escrever um nome com espaço.
- **`Type Idler`** e **`Type Sprocket`** vão sem aspas.
- **Roletes de retorno: `Type "Return Roller"`**, com aspas. O valor de texto entre aspas é aceito. Acrescente o modelo `Roller` do passo 4. Testado no 2A6 com 4 roletes por lado; o efeito deles na direção não foi medido.

### 6. Colisores, massa e centro de massa

- O `RigidBody` precisa de colisores no `.xob`; sem eles o log mostra `Tracked - no physics component!`.
- `Mass` é em kg. O 2A6 usa 62300.
- **`CenterOfMass` é um deslocamento, não uma posição.** O executável o descreve como "Offset of center of mass. Relative to one defined in model (if applicable)." O modelo do 2A6 tem um ponto de centro de massa, `COM_hull`, em (0, 1,168, −0,381) no espaço do casco. Então `0 1 0.46` coloca o centro de massa em (0, 2,168, 0,079), o que foi medido pela API das rodas ([capítulo 13](13-suspension.md#centro-de-massa)). Para um modelo novo com um objeto `COM_`, comece com `0 0 0`.

**Nivele o casco com o valor de z.** Sobre as molas, a posição do centro de massa de frente para trás em relação às rodas de apoio define o caimento parado. Com *z̄* a média de z dos ossos das rodas de apoio e *k* = `SpringRate` × 1000 N/m, o caimento (nariz para cima positivo, em radianos) é:

```math
\theta = \frac{M \cdot g \cdot (\bar z - z_{\text{COM}})}{k \sum_i (z_i - \bar z)^2}
```

A soma passa por todas as rodas de apoio das duas lagartas. Na prática: ponha o z do centro de massa sobre o z médio das rodas de apoio.

| `CenterOfMass` | z do centro de massa | Caimento (modelo) | Caimento (medido) |
| --- | --- | --- | --- |
| `0 1 0` | −0,381 m | +0,90° | +0,954° (nariz para cima) |
| `0 1 0.46` (2A6) | +0,079 m | −0,047° | −0,049° |
| `0 1 0.437` | +0,056 m = *z̄* | 0° | não testado |

Isso dá cerca de 2,1° por metro de z no 2A6 (2,06°/m pelo modelo, 2,18°/m pelos dois pontos medidos). Para medir, acrescente o `LEO_TelemetryComponent` (passo 9): ele registra `LEOTEL pose pitch=… roll=…` a cada 0,5 s, mesmo com o motor desligado, caimento positivo com o nariz para cima. Estacione em chão plano, leia o caimento, vire o tanque 180° e leia de novo. O 2A6 leu +0,954° nos dois sentidos, o que descarta o terreno.

**Altura: em aberto.** O 2A6 mantém y = 1 de versões anteriores. Com o `COM_hull` dele, isso põe o centro de massa a 2,17 m do chão, cerca de 0,47 m acima do teto do casco. Metade da bitola dividida por essa altura dá cerca de 0,61 g, contra cerca de 1,14 g com y = 0 (calculado). O 2A6 capotou uma vez a 33–41 km/h; o centro de massa alto é um fator de risco calculado, não uma causa provada. Um valor mais baixo ainda não foi testado.

O 2A6 também define `LinearSleepingThreashold 0.4` e `AngularSleepingThreashold 0.5` no `RigidBody` (escritos como na engine). Um tanque parado entra em repouso; a telemetria então repete os mesmos valores.

### 7. Banco do motorista

Para testar, alguém precisa dirigir. O `PilotCompartmentSlot` já vem do `TrackedVehicle_Base.et`; configure a entrada do motorista como em qualquer outro veículo do jogo.

### 8. Teste

1. Crie um mundo de teste como subcena de um mapa vanilla (por exemplo `GM_Arland`) e coloque o prefab.
2. **Depois de cada edição do prefab, recarregue o mundo.** Um veículo num mundo que já está aberto mantém os valores de simulação com que foi carregado. No 2A6, uma rodada ainda se comportou como `MaxTravelDown 0.15` depois de o prefab ter mudado para 0.10. Faça assim:
   1. Salve o prefab e feche o mundo no World Editor.
   2. Recarregue os scripts sem mundo aberto. A versão Diag disparou um assert `Resources are leaking! Check log!` depois de cada recarga de scripts no log de 05/10/2026 (7 de 7), também sem mundo aberto. O Workbench seguiu depois de *Ignore*.
   3. Abra o mundo de novo: dê um duplo clique nele no Resource Browser.
3. Aperte *Play*, entre como motorista e ligue o motor.
4. Confira cada função:

| Comando | Resultado esperado |
| --- | --- |
| W | Acelera com 80% do acelerador (`ThrottleTurbo` 0.2 reserva os outros 20%). 2A6: 20 km/h em 4,1 s, 30 km/h em 9,7 s (uma arrancada registrada) |
| W + Shift (turbo) | 100% do acelerador |
| S andando | Freia |
| S parado | Engata a ré sozinho (não existe tecla de ré). A ré nativa não anda: veja a [seção de bugs](09-bugs.md) e o passo 9 |
| A / D andando | Vira, com a lagarta de fora mais rápida |
| A / D parado | Gira no lugar (giro neutro / pivô). O casco do 2A6 recuou a 0,2–3 km/h durante o giro (medido; causa em aberto) |
| Sair do banco do motorista com o motor ligado | O controlador puxa o freio de mão e seleciona a ré; o bug da ré então segura o motor acima de `RpmMax`. O passo 9 resolve |

5. Abra o *Log Console* e procure linhas com `Tracked`, `Wheels`, `Engine` ou `TrackWheel`. A seção [Solução de problemas](12-troubleshooting.md) explica cada erro.
6. Compare com o resultado esperado em [Exemplos em vídeo](05-video-examples.md).

### 9. Faça a ré andar (e instale os scripts)

A ré nativa não anda na 1.8.0.13 ([seção de bugs](09-bugs.md)). O `LEO_TrackedReverseComponent` empurra o casco para trás enquanto o motorista segura S e, com `m_bStopEngineOnExit` (padrão 1), desliga o motor quando o banco do motorista fica vazio. Mantenha `ThrottleReverseTarget 1` no controlador.

Todos os scripts deste guia se instalam do mesmo jeito ([Scripts prontos](11-scripts.md)):

1. Salve os arquivos de [scripts/](../../scripts/) em `scripts/Game/Vehicle/` dentro do seu mod. O `LEO_TrackChain.c` é uma classe simples que o `LEO_RunningGearComponent` usa; instale junto com ele.
2. Recompile os scripts com o *Play* parado e sem mundo aberto.
3. Acrescente os componentes no bloco `components { }` do casco. Os IDs abaixo são os do 2A6 ([HullComponents_Leopard2A6.et.txt](../../examples/leopard2a6/HullComponents_Leopard2A6.et.txt)):

```
CarProcAnimComponent "{5D6501223785D009}" {
}
LEO_RunningGearComponent "{5292A83C2D6FC873}" {
}
LEO_TrackedReverseComponent "{7A1D4C9E2B6F3085}" {
}
LEO_TelemetryComponent "{3C5E7A9B1D2F4608}" {
}
LEO_TrackBeltComponent "{8E3A61C04F2D9B57}" {
}
```

| Componente | Passo | Faz |
| --- | --- | --- |
| `LEO_TrackedReverseComponent` | 9 | Contorno da ré; desliga o motor quando o motorista sai |
| `LEO_TelemetryComponent` | 6, 10, 13 | Linhas `LEOTEL` e `LEOSUSP` no log |
| `CarProcAnimComponent` (vazio) | 11 | Mantém o componente herdado sem parâmetros de roda |
| `LEO_RunningGearComponent` | 11, 12 | Suspensão e giro dos ossos das peças, corrente de elos por script |
| `LEO_TrackBeltComponent` | 12 | Esconde as malhas estáticas das esteiras quando já existem elos |

Troque o prefixo `LEO_` se for usar em outro mod, para não colidir com o Leopard. Os padrões dos atributos são os do 2A6; os próximos passos dizem quais mudar.

### 10. Dimensione a suspensão e confira a altura do casco

A mola fica livre (força zero) no curso todo para baixo, e o `SpringRate` está em N/mm, ou seja, kN/m (descrição no executável, confirmada por duas rodadas no 2A6). Parada, cada roda de apoio fica comprimida a partir do curso todo para baixo em:

```math
s = \frac{M \cdot g}{n \cdot k}
```

com *n* o número de rodas de apoio das duas lagartas e *k* em N/m. 2A6: 62300 · 9,81 / (14 · 450 000) = 0,097 m. **Use `MaxTravelDown` ≈ *s*** e o tanque fica na altura do modelo:

| `MaxTravelDown` | Curso parado previsto | Curso parado medido no 2A6 |
| --- | --- | --- |
| 0.15 | −53 mm (casco 5 cm alto) | −53,0 mm (média das 7 rodas, 13 de 14 amostras paradas) |
| 0.10 | −3 mm | −3,0 mm (mediana); −1 a −5 mm por roda, da frente para trás |

Curso negativo quer dizer que a roda fica pendurada abaixo da posição do modelo, então o casco fica alto. O `MaxTravelUp` (0.30 no 2A6) é a folga acima da posição do modelo. Os amortecedores são em N·s/m; o [capítulo 13](13-suspension.md#dimensionando-a-suspensão) mostra como escolhê-los.

Confira no Play: com o motor ligado, o `LEOSUSP` imprime a cada 2 s o `travel_mm` de cada roda de apoio, medido a partir do `m_fWheelRestHeight` do `LEO_TelemetryComponent` (0.4186 no 2A6; ajuste para a altura do centro das suas rodas de apoio). Parado em chão plano, a média de uma lagarta deve ficar perto de 0. Lembre-se da recarga do mundo do passo 8: a rodada que ainda mostrou −51 a −54 mm depois da mudança não tinha recarregado o mundo.

Detalhes: [Suspensão e altura do casco](13-suspension.md).

### 11. Anime o trem de rolamento

A simulação calcula o curso e o giro de cada peça, mas o `CarProcAnimComponent` não os entrega aos ossos certos num veículo de esteira na 1.8.0.13. No 2A6, a maioria dos ossos das rodas recebeu os sinais de outra roda (medido com a telemetria `LEOSUSP` em duas rodadas, e visto em vídeo). Uma ordem intercalada dos parâmetros não resolveu na segunda rodada, mas essa rodada não tinha recarregado o mundo, então não foi um teste limpo. Por isso:

- **Não acrescente `ProcAnimParams` de roda.** O `TrackedVehicle_Base` traz um `CarProcAnimComponent "{5D6501223785D009}"` vazio. Deixe-o vazio. O 2A6 escreve uma sobrescrita vazia porque antes tinha parâmetros.
- **Acrescente o `LEO_RunningGearComponent`** (passo 9). A cada frame ele lê cada peça da simulação por (lagarta, peça) e ajusta os ossos com `Animation.SetBoneMatrix`: o curso no `v_suspension_<s>0<k>`, o giro nas rodas de apoio, roda-guia, roda motriz e roletes.

Ele precisa do rig de ossos e dos nomes do [capítulo 3](03-model-requirements.md#rig-de-suspensão-das-rodas-de-apoio), da ordem de peças do passo 5, da roda-guia na frente e da roda motriz atrás. Ajuste estes atributos para o seu tanque:

| Atributo | 2A6 | Defina como |
| --- | --- | --- |
| `m_iRoadWheels` | 7 | Rodas de apoio por lagarta |
| `m_iRollers` | 4 | Roletes de retorno por lagarta |
| `m_fRoadWheelRadius` | 0.345 | Raio visual do aro de uma roda de apoio, onde ela rola na face interna da esteira |
| `m_fRollerRadius` | 0.0847 | Raio visual do aro de um rolete de retorno |
| `m_fRestHeight` | 0.4186 | Altura do centro da roda de apoio em repouso |
| `m_fMaxTravelDown` / `m_fMaxTravelUp` | 0.10 / 0.30 | Os mesmos valores do modelo `Suspension` |
| `m_fDropRate` | 8 | Rapidez com que uma roda sem contato com o chão cai até o curso todo e volta (1/s) |
| `m_fMaxDistance` | 300 | Além desta distância da câmera os ossos não são atualizados (m) |

Confira:

- O log mostra `LEOGEAR simulation: 2 tracks, WheelCount 13` quando a simulação carrega (a contagem é por lagarta).
- No `LEOSUSP`, o `diff_mm` (osso menos simulação) fica perto de 0 e as 7 rodas de apoio de uma lagarta giram juntas. 2A6, duas sessões com o script publicado: `worst_mm` com mediana de 0 mm, 90–95% das amostras dentro de 3 mm; valores maiores só enquanto uma roda volta depois de perder o contato. Uma versão anterior, que suavizava a cada frame, tinha só 70% dentro de 3 mm.

Confirmado no jogo no 2A6. O componente não faz nada num servidor dedicado e não foi testado nas máquinas dos outros jogadores (proxies do multiplayer). Detalhes: [Rodas de apoio, roda-guia, roda motriz e roletes](14-running-gear.md).

### 12. Acrescente os elos da esteira

Há dois jeitos de mostrar elos em movimento. Os dois usam o modelo de elo do [capítulo 3](03-model-requirements.md#modelo-do-elo-da-esteira) e os raios da simulação como caminho:

|  | Corda da corrente nativa | Corrente por script (2A6) |
| --- | --- | --- |
| O que move os elos | A engine: uma corda 2D com gravidade, os círculos das rodas e a tração pela roda motriz | O `LEO_TrackChain`: um caminho refeito a cada frame em volta dos círculos das rodas no curso atual |
| Colisão com o chão | Nenhuma | Não precisa: os elos seguem as rodas |
| Problemas conhecidos | Pende onde tem folga; saiu da roda motriz depois de capotar e depois de uma explosão de mina; sem API de script para reiniciá-la | Por construção não sai do lugar; ainda não testada capotando nem com mina |
| Precisa de | Os atributos abaixo | Os atributos abaixo, o `LEO_RunningGearComponent` com `m_bScriptedChain 1` e o `LEO_TrackBeltComponent` |

O 2A6 mantém os atributos nativos junto com a corrente por script. A corda continua rodando debaixo dos elos escondidos dela, e o diagnóstico do veículo (passo 13) precisa de uma corrente. Rodar a corrente por script sem os atributos nativos não foi testado.

**Atributos da corrente nativa.** Escreva-os dentro da `VehicleTrackedSimulation "{6160DE6434513C4D}"`, depois da chave que fecha o `Simulation Tracked`. Eles são irmãos do `Simulation`, não fazem parte dele:

```
TrackSegments 73
TrackPositions {
 <73 "z y" pairs from tools/trackpositions.py>
}
TrackLength 0.2110
TrackThickness 0.076
TrackOffset1 -1.3281 0 0
TrackOffset2 1.3281 0 0
TrackSegment "{B50AEDC702735F50}Assets/Vehicles/Tracked/Leopard2A6/Leopard2A6_track_link.xob"
```

| Atributo | O que pôr | Leopard 2A6 |
| --- | --- | --- |
| `TrackSegments` | Elos por lagarta. Só o gerador do editor lê; em tempo de jogo o número de elos é o número de entradas de `TrackPositions`. Mantenha os dois iguais | 73 |
| `TrackPositions` | Uma lista plana de pares `z y` no espaço do casco: pontos da corrente em volta dos círculos das rodas, índice 0 no ramo de cima, crescendo para a frente | 73 pares |
| `TrackLength` | Distância de repouso entre pontos vizinhos, mais ou menos o passo do elo. Comprimento da corrente = N × `TrackLength` | 0.2110 (passo 0,2101 + 0,9 mm) |
| `TrackThickness` | Guardado, mas nunca lido na 1.8.0.13 (engenharia reversa) | 0.076 |
| `TrackOffset1` / `TrackOffset2` | Posição lateral do centro da esteira da lagarta 0 (esquerda) e da lagarta 1 (direita) | −1,3281 / 1,3281 |
| `TrackSegment` | O `.xob` do elo | `Leopard2A6_track_link.xob` |

A engine só monta a corrente com pelo menos 2 `TrackPositions` e `TrackLength` > 0 (engenharia reversa).

- **Posições.** A ação do editor *Generate track positions* nunca foi achada na interface do Workbench durante o trabalho no 2A6. Use a [tools/trackpositions.py](../../tools/trackpositions.py). Edite a lista de círculos (z, y e `Radius` da simulação de cada peça de um lado), rode `python trackpositions.py 73 0.2101` e cole o bloco `TrackPositions { }` que ela imprime (as linhas com `#` acima dele são só informação). Ela distribui N pontos por igual no contorno convexo dos círculos.
- **Número de elos.** Comprimento do caminho em volta dos círculos da simulação / passo do elo, arredondado para cima. 2A6: 15,284 m / 0,2101 m = 72,7 → 73. A esteira original do modelo tem 71 elos, porque a linha dos pinos dela é uma volta menor.
- **Folga.** A corda não colide com o chão, então a folga pende; com folga de menos, as rodas que caem provavelmente a arrancam (deduzido). Com 73 × 0,2101 (5,3 cm a mais que o caminho de repouso, contados pelos arcos) a corda do 2A6 saiu da roda motriz depois de capotar; com 73 × 0,2110 (11,9 cm a mais) ela pendeu abaixo das rodas. O [capítulo 15](15-track-links.md) tem os números.

**Corrente por script.** Mantenha `m_bScriptedChain 1` no `LEO_RunningGearComponent` e acrescente o `LEO_TrackBeltComponent` (passo 9). Ajuste:

| Atributo | Componente | 2A6 | Defina como |
| --- | --- | --- | --- |
| `m_bScriptedChain` | `LEO_RunningGearComponent` | 1 | 1: elos por script, e os elos nativos ficam escondidos |
| `m_sLinkModel` | `LEO_RunningGearComponent` | `Leopard2A6_track_link.xob` | O `.xob` do seu elo |
| `m_iLinks` | `LEO_RunningGearComponent` | 73 | Elos por lagarta |
| `m_sNativeLinkName` | `LEO_RunningGearComponent` | `Leopard2A6_track_link` | Parte do nome do seu modelo de elo |
| `m_sStaticBeltMeshes` | `LEO_TrackBeltComponent` | `track_l track_r` | Nomes das suas malhas estáticas de esteira |
| `m_sLinkModelName` | `LEO_TrackBeltComponent` | `Leopard2A6_track_link` | Parte do nome do seu modelo de elo |
| `m_fTimeout` | `LEO_TrackBeltComponent` | 5 | Segundos de espera pelos elos antes de manter as esteiras estáticas |

Confira: o log mostra `LEOTRACK link chain found (292 links): 2 static belt meshes hidden` (146 elos nativos + 146 por script no 2A6). O caminho da esteira supõe a roda-guia na frente e a roda motriz atrás; para roda motriz na frente, veja [Convertendo um veículo existente](06-converting.md#4-escolha-a-roda-guia-e-a-roda-motriz-e-ordene-as-peças).

Detalhes: [Elos da esteira: a corda nativa e a corrente por script](15-track-links.md).

### 13. Confira com o diagnóstico do veículo e a telemetria

O executável Diag (`ArmaReforgerWorkbenchSteamDiag.exe`) tem uma sobreposição de depuração nativa do veículo:

1. No *Play*, abra o menu de diagnóstico com **Win esquerdo + Alt**.
2. Em *Vehicles*, ligue *Show vehicle debug* e *Player vehicle only*. Eles ficam na própria lista Vehicles, não no submenu *Tracked* (observado no vídeo de 06/10/2026). Nos vídeos do 2A6 também estavam ligados *Show engine*, *Show suspension*, *Wheel engine brake* e *MT simulation*.
3. A sobreposição desenha um círculo branco no `Radius` da simulação de cada peça, a corda nativa como uma linha poligonal, uma caixa de estatísticas por lagarta e os painéis *Debug* e *Engine*.

**Monte a corrente de elos primeiro.** Em 02/10/2026, sem atributos de corrente no prefab, ligar *Show vehicle debug* / *Show Controller Diags* (o diário cita os dois) derrubou o Workbench; a sobreposição lê a corrente sem conferir se ela existe (engenharia reversa). Com a corrente presente, ela rodou o vídeo de teste inteiro de 5 minutos de 06/10/2026 sem travar.

Conferências rápidas:

- Os círculos das rodas de apoio chegam à face de contato dos elos. Se não chegarem, o `Radius` está errado.
- Na caixa de estatísticas de cada lagarta, `Speed` / `AngVel` é igual ao `Radius` da roda motriz (0,3400 no 2A6).
- No `console.log` (`Documents\My Games\ArmaReforgerWorkbench\logs\logs_<data>\`): `LEOTEL` (velocidade, rpm, marcha, embreagem, freio, postura), `LEOSUSP` (suspensão e giro), `LEOGEAR` e `LEOTRACK` (mensagens de configuração).

Detalhes: [Diagnóstico do veículo e telemetria](16-debugging.md).

---

[← Requisitos do modelo 3D](03-model-requirements.md) · [Índice](../../README.pt-BR.md) · [Exemplos em vídeo →](05-video-examples.md)
