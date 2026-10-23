> 🇧🇷 Português · [🇬🇧 English](../en/12-troubleshooting.md)

[← Scripts prontos](11-scripts.md) · [Índice](../../README.pt-BR.md) · [Suspensão e altura do casco →](13-suspension.md)

# Solução de problemas

Procure a mensagem ou o sintoma na tabela certa. As mensagens aparecem no *Log Console* do Workbench e no `console.log` da sessão (`Documents\My Games\ArmaReforgerWorkbench\logs\logs_<data>\`). As linhas que começam com `LEOREV`, `LEOTEL`, `LEOSUSP`, `LEOGEAR` ou `LEOTRACK` vêm dos [scripts prontos](11-scripts.md).

As linhas vêm do trabalho com o Leopard no Workbench 1.8.0.13: a versão anterior, do 2A7, e o 2A6 atual. As causas marcadas como deduzidas, de engenharia reversa ou em aberto não foram confirmadas no jogo.

### Erros ao carregar o veículo

| Mensagem | Causa | Correção |
| --- | --- | --- |
| `Tracked - failed to load!` | Um dos erros abaixo | Leia a linha logo acima no log |
| `FrictionCurveLateral - configuration is missing!` | O `.ct` vanilla não tem curvas | Acrescente `FrictionCurveLongitudinal` e `FrictionCurveLateral` |
| `Invalid number of tracks! Must be 2!` | `Tracks` falta ou tem mais de duas | Exatamente `Track Left` e `Track Right` |
| `Wheels - configuration is missing! Every track should have at least 3 wheels!` | Menos de 3 peças numa lagarta | Pelo menos 3 `TrackPart` por lagarta |
| `Wheels - ... mismatch between tracks` | A roda-guia ou a roda motriz não está no mesmo índice de `TrackPart` nas duas lagartas (engenharia reversa) | Use a mesma ordem de `TrackPart` nos dois lados |
| `TrackWheel's mass / radius / brake torque is incorrect!` | Falta um modelo de roda, ou um valor está fora da faixa: massa e raio precisam ser maiores que zero, torque de freio zero ou mais | Defina `RoadWheel`, `Sprocket` e `Idler`, e também `Roller` se uma lagarta tiver peças `Type "Return Roller"` |
| `Engine's inertia is incorrect! (must be greater than zero)` | `Inertia` 0 | `Inertia` maior que zero. O mesmo vale para `Steepness` e `Friction` |
| `Tracked - no physics component!` | Sem colisores ou sem `RigidBody` | Colisores `UCX_*` no `.xob` |
| `Aerodynamics - configuration is missing!` | Bloco `Aerodynamics` apagado | Mantenha o herdado ou escreva um novo |
| Aviso: falta o `CarControllerComponent` | `AICarMovementComponent` da base vanilla | Inofensivo |
| Aviso de som pedindo o osso `v_axle_01` | Componente de som herdado do carro | Inofensivo; ou crie o osso |

### Problemas ao dirigir

| Sintoma | Causa | Correção |
| --- | --- | --- |
| W + A/D só freia e não vira | `SteeringRatio` sem valor | `SteeringRatio 6` |
| A/D parado não gira no lugar | Faltam `NeutralSteeringRatio` e `PivotSteering` | `NeutralSteeringRatio 12`, `PivotSteering 1`, `NeutralSteering 1` |
| Velocidade travada em \~51–54 km/h | Retardador padrão | `RetarderBaseTorque 0`, `RetarderThrottleSpeed 20` |
| Velocidade máxima muito longe do cálculo | O cálculo usou o raio da roda de apoio | Refaça com o raio da roda motriz. No 2A6, velocidade ÷ velocidade angular da roda motriz deu 0,344 m contra `Sprocket.Radius` 0.34 (medido) |
| Aceleração muito lenta | `Friction` alto, `Inertia` alta ou 1ª marcha longa | `Friction` \~10% do torque; encurte a 1ª |
| W nunca chega a 100% | `ThrottleTurbo` 0.2 | Normal; use o turbo |
| S parado: o motor sobe acima do `RpmMax` e nada acontece | Bug da ré nativa (no 2A6 o motor fica preso em 3766–3768 rpm) | [`LEO_TrackedDriveComponent`](11-scripts.md) |
| S parado: o tanque anda para a frente | `ThrottleReverseTarget` abaixo de 1 | `ThrottleReverseTarget 1` |
| Na descida mantém velocidade de ré, mas no plano não anda | Bug da ré: só a gravidade o move | Script de ré |
| O motor fica em cerca de 4500 rpm depois que o motorista sai | Sem motorista o controlador puxa o freio de mão e, parado, seleciona a ré; o bug da ré então prende o motor (observado no 2A6) | `m_bStopEngineOnExit 1` no script de ré (v4) |
| Num giro no lugar o motor cai para o `RpmIdle` (700 rpm) | Com a embreagem acoplada o controlador segura o freio em 6,0 durante o giro (medido). O motivo está em aberto | Não precisa corrigir: o casco ainda gira. Com a embreagem em 0, o motor fica no governador (2700 rpm) (observado no vídeo) |
| O casco recua num giro no lugar | A lagarta que anda para trás gira mais rápido que a outra (medido de −0,1 a −3,8 km/h de recuo). Causa em aberto | Em aberto |
| Acelerador a fundo, o motor fica em 700 rpm e o tanque mal se mexe | O tanque está travado ou encalhado numa pedra, com só 1–5 rodas de apoio por lado tocando o chão (medido) | Saia de ré. Comportamento esperado |
| O tanque capota fácil em velocidade | Talvez o centro de massa alto: o `CenterOfMass` é um deslocamento somado ao ponto COM do próprio modelo. No 2A6, `0 1 0.46` põe o COM 2,17 m acima do chão, com um fator de estabilidade estática de cerca de 0,61 g (calculado, não provado) | Baixe o deslocamento Y (Y 0 daria cerca de 1,14 g). Ainda não testado ([capítulo 13](13-suspension.md)) |
| O tanque parado escorrega devagar numa ladeira | Não resolvido. Um caso registrado: 0,3–0,4 km/h por 2,6 s numa rampa de \~6° com o freio em 6,0 | Em aberto |
| Num servidor dedicado, virar parado não faz nada; o motor acelera e o casco faz rubberband | O move de rede descarta o freio de mão (`IsHandbrakeOn` é um stub), então o servidor nunca gira (medido) | `LEO_TrackedDriveComponent` (o próprio servidor aplica `SetBreak(x, true)`), ou o Kiichi Tracked System ([capítulo 18](18-multiplayer.md), [capítulo 19](19-kiichi-tracked-system.md)) |
| A ré faz rubberband num servidor | O empurrão por script só rodava na máquina do motorista | v5 ([capítulo 18](18-multiplayer.md)) |
| Como motorista num cliente: cabeça congelada, olhar lento parado, câmera pula ao acelerar e virar, teclas presas | Limitação do motor no caminho do cliente dono de esteira. Não é causada pelos scripts do mod (medido com tudo removido) | Nenhuma. Singleplayer, host e os outros assentos funcionam ([capítulo 18](18-multiplayer.md)) |

### Suspensão e altura do casco

| Sintoma | Causa | Correção |
| --- | --- | --- |
| O casco fica alto parado; as rodas pendem abaixo da posição do modelo | O `MaxTravelDown` é maior que o afundamento estático m·g/(n·k). A mola fica solta (força zero) no curso todo para baixo | Ponha `MaxTravelDown` ≈ m·g/(n·k). 2A6: 62 300 · 9,81 / (14 · 450 000) = 0,097 m. Com 0.15 o casco ficou 53 mm alto, com 0.10 fica 3 mm baixo (medido) |
| O casco fica baixo ou bate no fim do curso | O `MaxTravelDown` é menor que o afundamento, ou o `SpringRate` está mole demais. O `SpringRate` está em kN/m (= N/mm) | A mesma regra. Recalcule o `SpringRate` em kN/m |
| O casco flutua ou afunda uma medida constante | O `RoadWheel.Radius` não é a distância do centro da roda à face da esteira em contato com o chão | Raio = altura do centro da roda acima do chão (2A6: 0,418 m para um aro de 0,345 m) |
| Nariz para cima ou para baixo parado | O centro de massa não está sobre o meio das rodas de apoio. O `CenterOfMass` é um deslocamento somado ao ponto COM do próprio modelo (um empty `COM_`), não uma posição absoluta | Mova o deslocamento em z: para a frente abaixa o nariz. 2A6: z 0 deu 0,954° de nariz para cima e z 0.46 deu 0,049° de nariz para baixo (medido): cerca de 2,1° por metro (2,06°/m calculado, 2,18°/m medido) |
| Uma mudança nos valores da suspensão não tem efeito | O mundo estava aberto quando você mudou o prefab | Veja [Fluxo de trabalho no Workbench](#fluxo-de-trabalho-no-workbench) |
| As rodas pendem quando o tanque está no ar ou de cabeça para baixo | Sem contato com o chão é curso todo para baixo | Esperado |
| Curso da suspensão acima do `MaxTravelUp` na telemetria (até +0,38 m) | O raio começa `RayStartOffsetUp` acima do topo do curso, então o contato pode ficar acima do batente. O sinal do próprio motor limita no `MaxTravelUp` | Esperado. Limite nos seus scripts |

O [capítulo 13](13-suspension.md) explica o modelo da mola e as medições.

### Trem de rolamento e elos da esteira

| Sintoma | Causa | Correção |
| --- | --- | --- |
| Só a roda de apoio dianteira esquerda segue o chão; os outros ossos se movem com outra roda ou nem se movem | `CarProcAnimComponent` com `ProcAnimParams` num veículo de esteira. No 1.8.0.13 ele deu a todos os ossos menos o `v_wheel_l01` os sinais de outra roda, com as duas ordens de parâmetros testadas (medido) | Deixe o `CarProcAnimComponent` vazio e use o [`LEO_RunningGearComponent`](11-scripts.md) |
| Rodas de apoio flutuando acima da corrente: rodas direitas 2–7 cerca de 5 cm altas paradas ([imagem](../images/before-wheels-floating.jpg)) | O mesmo mapeamento errado. Esses ossos provavelmente leem o sinal de uma roda-guia, roda motriz ou rolete, que não têm suspensão, então ficam na altura do modelo enquanto as rodas simuladas ficam mais baixas (deduzido) | A mesma correção |
| Algumas rodas giram com a outra lagarta | O mesmo mapeamento errado (as rodas esquerdas pares giravam com a lagarta direita, medido) | A mesma correção |
| Trem de rolamento por script: as rodas bambeiam ou giram no eixo errado | A orientação de repouso do osso não é identidade. O script gira em volta do eixo X do osso nos eixos do casco | Monte os ossos no centro da peça com orientação identidade ([capítulo 14](14-running-gear.md)) |
| Trem de rolamento por script: as rodas giram rápido ou devagar demais em relação aos elos | O `m_fRoadWheelRadius` ou o `m_fRollerRadius` não é o raio visual do aro | Meça os aros no modelo (2A6: 0,345 m e 0,0847 m) |
| Rodas e elos giram errado enquanto a roda de apoio da frente está no ar em ré | O script tira a velocidade da esteira da roda de apoio 0, cujo RPM muda sem contato (deduzido dos valores de RPM registrados) | Em aberto |
| Os braços da suspensão ficam parados enquanto a roda se move | Os braços têm peso no `v_body` | Dê novos pesos para `v_suspension_<s>0<k>`, de 0 no pivô do casco a 1 no cubo ([capítulo 14](14-running-gear.md)) |
| Os elos nativos pendem abaixo das rodas de apoio ou atrás da roda motriz ([parado](../images/before-rope-sag.jpg), [andando](../images/before-rope-sag-sprocket.jpg)) | A corda nativa tem gravidade mas não tem colisão com o chão. A folga dela (`TrackLength` longo demais para o caminho) pende nos vãos | Use a corrente por script. Menos folga esconde a barriga mas sai do lugar no curso todo para baixo ([capítulo 15](15-track-links.md)) |
| A corrente nativa sai da roda motriz depois de um capotamento ou de uma explosão de mina e fica fora ([capotamento](../images/before-rope-off-flip.jpg), [minas](../images/before-rope-off-mines.jpg), [roda motriz vazia](../images/before-bare-sprocket.jpg)) | As rodas perdem contato e caem até o curso todo para baixo, o que pede mais corrente do que existe. A corda é arrastada por cima da roda motriz e assenta do lado de fora. Não há API de script para reiniciá-la | Use a corrente por script. O executável Diag tem uma entrada *Reset tracks*, não testada |
| Os elos nativos andam mais devagar ou mais rápido que o chão | Os `TrackPart` 0 e 1 não são rodas de apoio: a velocidade da corrente usa o raio da peça 0 e o ângulo da peça 1 (engenharia reversa; com a roda-guia primeiro andaria a cerca de 63%) | Ponha as rodas de apoio primeiro |
| Os elos nativos ficam ao lado das rodas | O x do `TrackOffset1` / `TrackOffset2` não é o centro da esteira. O `TrackOffset1` é usado no primeiro `Track` | Ponha o x no centro da esteira (2A6: −1.3281 e 1.3281) |
| Os elos estão de cabeça para baixo ou deslocados das rodas | A origem ou os eixos do modelo do elo estão errados | Modele o elo como um elo do trecho de baixo: origem no centro da face em contato com o chão, +Y para as rodas, +Z para a frente ([capítulo 15](15-track-links.md)) |
| Nenhum elo aparece, as esteiras estáticas ficam, e `LEOTRACK no link chain after 5 s: static belts kept` | Corrente nativa: menos de 2 entradas no `TrackPositions`, `TrackLength` 0 ou sem modelo de `TrackSegment`. Corrente por script: um aviso `LEOGEAR` acima | Corrija os atributos da corrente ou a causa do `LEOGEAR` |
| Duas esteiras de elos uma em cima da outra | O `m_sNativeLinkName` não bate com o nome do arquivo do modelo do `TrackSegment`, então os elos nativos não são escondidos | Ponha no `m_sNativeLinkName` parte desse nome de arquivo |
| A esteira estática continua visível por cima dos elos em movimento | Falta o `LEO_TrackBeltComponent`, ou os nomes das malhas não batem (`LEOTRACK static belt mesh not found: <nome>`) | Acrescente-o e ajuste `m_sStaticBeltMeshes` e `m_sLinkModelName` |
| Rodas e elos param de se mexer longe da câmera | O `LEO_RunningGearComponent` pula a atualização além do `m_fMaxDistance` (300 m) | É de propósito. Aumente o valor se precisar |

### Scripts, diagnóstico e travamentos

| Sintoma | Causa | Correção |
| --- | --- | --- |
| O Workbench fecha quando o motor liga | Um script chamou `GetInputs()` | Tire a chamada. O `GetFloorNormalAngles()` tem o mesmo defeito |
| O Workbench fecha quando a depuração do veículo abre (*Show vehicle debug*, *Show Controller Diags*) | Nenhuma corrente de elos configurada. O desenho do diagnóstico lê os ponteiros da corda sem conferir se são nulos (travamento de 02/10/2026, engenharia reversa) | Configure a corrente nativa (2 ou mais `TrackPositions`, `TrackLength` acima de 0). Com a corrente, as duas opções rodaram sem travar em 06/10/2026. Salve o trabalho antes |
| `Pointer type Physics can only be used with local variables` | `Physics` como membro de classe | Use uma variável local em cada função |
| `GetThrottle()` sempre 0 | Função vazia na simulação de esteira | Leia o sinal `engineThrust` |
| `GetBrake()` devolve 6,0 | Freio de mão: `SetBreak(x, true)` guarda 6,0. O controlador o segura parado e nos giros no lugar | Esperado |
| Os getters das rodas devolvem 0, false ou um vetor zero; `TrackCount()` é 0 | A simulação ainda não carregou (fica válida cerca de 1 s depois de o veículo surgir) | Proteja com `IsValid()` e `TrackCount() < 2` ([Scripts prontos](11-scripts.md)) |
| `WheelGetPosition` devolve y ≈ −1,9 em todas as rodas | O vetor é relativo ao centro de massa da física, e displacement 0 quer dizer curso todo para baixo | Some o COM efetivo para ter as coordenadas do casco |
| `LEOTEL` e `LEOSUSP` imprimem linhas idênticas bit a bit | O corpo está dormindo (limites de sono) | Esperado |
| `LEOREV no pilot compartment found` | Compartimentos registrados depois do `OnPostInit` | Pegue o assento do motorista na hora de usar (já feito na v4) |
| `LEOREV native gearbox still in forward gear` | O controlador ainda não passou para a ré | Normal por até 0,5 s |
| `LEOREV native brake applied: 6.00` | O freio de mão está puxado | Solte. O empurrão espera o sinal de freio ficar abaixo de 0,05 |
| `LEOREV still moving forward: ... km/h` | O casco anda para a frente mais rápido que `m_fForwardCutoffKmh`; a simulação nativa freia primeiro | Normal |
| `LEOREV no controller / engine off` | O motor está desligado | Ligue o motor |
| `LEOGEAR <n> running gear bones missing or unreadable: the scripted chain is off` | Um nome de osso está diferente, ou a altura de repouso de um osso de roda está fora de 0,5–2 × `m_fRestHeight` | Confira os nomes dos ossos ([Scripts prontos](11-scripts.md)) e o `m_fRestHeight` |
| `LEOGEAR the simulation has <n> wheels, expected ...` | A lagarta tem menos `TrackPart` que `m_iRoadWheels` + 2 + `m_iRollers` | Ajuste os atributos ao prefab |
| `LEOGEAR link model not found: ... - the scripted chain is off` | O `m_sLinkModel` aponta para um `.xob` que não existe | Ponha o seu modelo de elo |
| `LEOGEAR no TrackedControllerComponent or no skeleton: running gear not animated` | O componente está num prefab sem controlador de esteira ou sem ossos | Ponha no casco de esteira |

### Fluxo de trabalho no Workbench

| Sintoma | Causa | Correção |
| --- | --- | --- |
| Uma mudança no prefab não tem efeito no *Play* | Um veículo num mundo que já está aberto mantém a simulação montada com o prefab antigo. Medido: o prefab dizia `MaxTravelDown` 0.10, o veículo continuava se comportando como 0.15 | Feche o mundo, recarregue os scripts sem mundo aberto e abra o mundo de novo |
| Assert `Resources are leaking! Check log!` (executável Diag, `GameApp.cpp` linha 1287) | Scripts recarregados com um mundo aberto. Os 77 recursos vazados são vanilla, nenhum do mod | Feche o mundo antes, ou clique em *Ignore* |
| O Workbench congela depois de você trocar o `.fbx` do casco | Reimportação automática com o mundo aberto (congelou uma vez, um núcleo da CPU a 100%) | Feche o World Editor antes de trocar modelos. Se congelar, encerre o processo e abra de novo |
| O Workbench fecha ao recompilar | Recompilado com o *Play* rodando | Pare o *Play* antes |

### Modelo e importação

| Sintoma | Causa | Correção |
| --- | --- | --- |
| Os ossos não aparecem no `.xob` | Importado sem esqueleto | *Export Skinning* e *Export Scene Hierarchy* ligados, *Merge Meshes* desligado |
| `Scene_Root` duplicado | O osso raiz no Blender tem esse nome | Renomeie para `v_body` |
| Erro de material nos colisores | O `material/metal.gamemat` padrão não existe | Use `armor_XXmm.gamemat` |

---

[← Scripts prontos](11-scripts.md) · [Índice](../../README.pt-BR.md) · [Suspensão e altura do casco →](13-suspension.md)
