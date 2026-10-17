> 🇧🇷 Português · [🇬🇧 English](../en/18-multiplayer.md)

[← Apêndice](17-appendix.md) · [Índice](../../README.pt-BR.md)

# Multiplayer

Este capítulo trata do que acontece com um veículo de esteira nativo quando um jogador o dirige de um cliente: como o motor o replica, o bug que impede o giro no lugar e a direção neutra nos servidores, a correção nos scripts do Leopard e a limitação do motor que sobra. Também explica como testar multiplayer num PC só com o Peer Tool do Workbench.

Tudo foi testado em 16 e 17/10/2026 com o Workbench 1.8.0.13 (Workbench como servidor mais um cliente do Peer Tool), depois de um primeiro relato de um testador num servidor dedicado. Cada fato diz como é conhecido: **medido** (logs), **engenharia reversa** (`ArmaReforgerWorkbenchSteamDiag.exe` 1.8.0.13, endereços no [Apêndice](17-appendix.md)), **observado** ou **não testado**.

### Em resumo

| Situação | Situação |
| --- | --- |
| Singleplayer, ou um host que dirige o próprio tanque | ✅ Funciona como no resto do guia |
| Servidor dedicado, giro no lugar / direção neutra parado | ❌ Nativo: o servidor nunca gira, o cliente faz rubberband. ✅ Corrigido pelo `LEO_TrackedReverseComponent` v5 (o cliente manda o freio de mão, o servidor repõe) |
| Servidor dedicado, ré | ❌ Nativo: o empurrão por script só rodava na máquina do motorista. ✅ A v5 roda no servidor também |
| Servidor dedicado, andar e virar em velocidade | ✅ Funciona de forma nativa |
| Servidor dedicado, o **motorista** num cliente | ⚠️ Limitação do motor: a cabeça do motorista não anima, o olhar fica lento parado, a câmera pula ao acelerar e virar, e os comandos podem ficar presos. Não dá para corrigir por script (veja abaixo) |
| Outros assentos num cliente (torre) | ✅ Normais |
| Trem de rolamento e corrente por script nos clientes | ✅ Animam no cliente do motorista e nos outros clientes (medido no peer: linhas `LEOGEAR` e `LEOTRACK`, elos visíveis). Nada roda num servidor dedicado, de propósito |

### Como um veículo de esteira é replicado

O servidor é a autoridade. O cliente do motorista **prevê** o veículo e manda um **move** a cada quadro; o servidor repete o move, compara o estado dele com o do cliente e aprova ou corrige. Tudo isso é nativo (`NwkTrackedMovementComponent`), por engenharia reversa:

| Passo | O que acontece | Onde |
| --- | --- | --- |
| Gravar | O cliente do motorista lê as entradas da simulação num move: direção, acelerador, freio, freio de mão, marcha, embreagem, estado da troca, as ações do motor (ligar/desligar) e a própria transformação e velocidades. Um move por quadro, guardado num anel de moves pendentes | Gravador `0x140fe5d10`, produtor `0x141075190` |
| Enviar | `Rpc_USendMove_S`, um `GenericArray<TrackedMinimalMove>` para o servidor | — |
| Repetir | O servidor copia o move para o controlador e aplica na simulação: `SetInputs(steer, throttle)`, `SetBreak(brake, handbrake)`, `SetClutch`, `SetGear`, depois as ações do motor | `0x14106e4d0` → `0x140fe7c70` → `0x140fe4940` |
| Comparar | Depois do próprio passo, o servidor compara posição, ângulo e velocidades com o relato do cliente, dentro das tolerâncias: distância |v|/300 limitada a 0,001–1,5 m, ângulo 0,003, velocidade 0,3, velocidade angular 0,25 | `0x141074940` → `0x14104d8f0`; padrões no construtor `0x14106c700` |
| Corrigir | Numa divergência o cliente pula para o estado do servidor e **ressimula os moves pendentes** num laço nativo. Nenhum script roda nesse laço | `0x141074770` → `0x141076250`, laço `0x141075cc0` |

As máquinas dos outros jogadores (proxies) não fazem nada nesse componente: elas interpolam o estado do servidor.

Dois controles de script existem no `NwkTrackedMovementComponent`: `SetAllowance(speedScaledDistance, distance, angle, linearVelocity, angularVelocity)` escreve as tolerâncias que o servidor usa na comparação, e `SetPrediction(bool)` só desliga a ressimulação do cliente. Nenhum dos dois lê o move.

### Bug 1: o freio de mão nunca chega ao servidor

O giro no lugar e a direção neutra precisam da **flag de freio de mão** da simulação. Em singleplayer o painel Debug mostra `Brake 6.0` em todo giro, e só o caminho do freio de mão guarda esse valor ([capítulo 16](16-debugging.md)).

No multiplayer a flag se perde no caminho (engenharia reversa, as duas instruções conferidas à mão):

- o gravador do move preenche o campo do freio de mão com `VehicleTrackedSimulation::IsHandbrakeOn` (`0x140fc23c0`), que é um stub: `call <conferência de validade>; xor al, al; ret`. Sempre devolve falso;
- a repetição no servidor chama o `SetBreak` pela ligação `0x140fc3c10`, que corta o freio em [0, 1]. O freio 6,0 chega como 1,0, com o freio de mão falso.

Então o servidor segura o tanque com freio 1,0 enquanto o acelerador e a embreagem repetidos aceleram o motor, o cliente do motorista gira, e o servidor o corrige a cada move: sem giro, motor no limitador, rubberband. Andar e virar em velocidade funcionam porque não há freio de mão envolvido.

**Medido** no servidor do Peer Tool com os scripts v4: sem giro; com a v5: `LEONET server brake=6.00 mirrored=61 wheelRpm L/R=-059.3 / 0058.1 ... steer=-1.00`, as duas lagartas girando em sentidos opostos no servidor.

### Bug 2: a ré por script só rodava na máquina do motorista

Uma força aplicada por script só existe onde foi aplicada. A v4 empurrava só na máquina do motorista; o servidor aplicava a marcha R e o acelerador 1 vindos do move, a ré nativa fica presa acima do `RpmMax` ([capítulo 9](09-bugs.md)), o casco do servidor não saía do lugar, e o cliente era corrigido de novo.

### A correção: LEO\_TrackedReverseComponent v5

O [`scripts/LEO_TrackedReverseComponent.c`](../../scripts/LEO_TrackedReverseComponent.c) agora tem três papéis:

| Máquina | O que faz |
| --- | --- |
| Cliente do motorista (dono, proxy) | Como a v4: lê as teclas e empurra o casco em ré. Também manda `{ré, direção, freio de mão}` para o servidor por RPC (`RpcAsk_DriveState`, `RplRcver.Server`): na mudança, com passos de 0,1 na direção, e um keep-alive de 0,5 s. O freio de mão é lido como `GetBrake() > 5.9` e mantido por 0,25 s, porque o controlador o solta em quadros isolados durante um giro |
| Servidor, motorista em outra máquina | Repõe o freio de mão com `SetBreak(1, true)` sempre que o freio repetido está em 1,0, no `EOnSimulate` **e** no `EOnPostSimulate` (o move é consumido na fase pós-simulação, então as duas ordens ficam cobertas). Aplica o mesmo empurrão de ré e o torque de guinada. Afrouxa as tolerâncias durante giro ou ré (`SetAllowance(false, 0.3, 0.05, 0.6, 0.3)`, restaurado para `(true, 300, 0.003, 0.3, 0.25)` 0,5 s depois), porque a ressimulação do cliente não reproduz nem o freio de mão nem o empurrão. Desliga o motor quando o motorista sai (só na autoridade) |
| Máquinas dos outros jogadores | Nada |

Atributos novos: `m_bMirrorHandbrake` (1), `m_bRelaxAllowance` (1), `m_bClientPrediction` (1; em 0 chama `SetPrediction(false)` no cliente do motorista, o que não ajudou em nada e fica para experimentos). Com `m_bDebug 1` o log mostra `LEONET server got drive state …`, `LEONET server allowances relaxed=…` e, uma vez por segundo durante giro ou ré, `LEONET server brake=… mirrored=… wheelRpm L/R=…`. Sinais opostos em `wheelRpm L/R` querem dizer que o próprio servidor está girando.

Não testado: servidor com latência real (50–150 ms). Espere uma correção a cada aperto e soltura de tecla, porque o RPC e o move chegam com alguns quadros de diferença.

### A limitação do motor: o motorista num cliente

Com o motorista num cliente, tanto em primeira quanto em terceira pessoa, a cabeça do personagem não anima, o olhar com o mouse fica lento com o tanque parado, a câmera pula longe quando o tanque acelera e vira, e uma tecla solta continua agindo por um tempo. No mesmo cliente, na mesma sessão, um jipe vanilla (M151A2) é normal; o mesmo tanque dirigido pela janela do host é normal; os assentos da torre no cliente são normais.

Tudo que dá para medir por script é idêntico entre o jipe e o tanque (medido com uma sonda temporária em `SCR_CharacterCameraHandlerComponent.OnBeforeCameraUpdate`):

| Medição | Jipe (funciona) | Leopard (quebrado) |
| --- | --- | --- |
| `RplComponent` `IsOwner` / `IsProxy` | true / true | true / true |
| `NwkMovementComponent` `IsSimulationEnabled` / `IsInterpolationEnabled` / `IsInterpolating` | false / true / false | false / true / false |
| Keyframes de câmera por segundo (as câmeras por script só atualizam em keyframes) | ~30 | ~30 |
| Personagem é filho do casco na hierarquia | sim | sim |
| Pulos de posição do casco (correções do servidor) | — | um pulo de 0,19 m numa sessão inteira |

O que foi tentado, cada item numa rodada separada, sem mudança nos sintomas:

1. `SetPrediction(false)` no cliente do motorista.
2. Esconder a tripulação desligado (`LEO_CrewViewComponent.m_bHideCrew 0`).
3. Todos os componentes `LEO_` tirados do prefab do casco, limites de sono tirados.
4. As classes modded de câmera tiradas do mod e a torre tirada do slot do casco: um casco só com os componentes vanilla e o bloco da simulação.
5. `EnableSimulation(true)` no cliente do motorista (o jipe também tem falso).
6. Posse devolvida ao servidor quando um jogador remoto senta no banco do motorista (`RplComponent.GiveExt(RplIdentity.Local(), true)`), com as entradas do motorista mandadas por RPC e o servidor dirigindo a simulação sozinho (`SetInputs`, `SetBreak`, `SetClutch`, `SetGear`, `ForceStartEngine`). A sonda confirmou `owner=false` no cliente; o motorista continuou igual.

De onde vem (engenharia reversa, em parte): os produtores de move do cliente dono do `NwkCarMovementComponent` (`0x141054b50`, 36 chamadas) e do `NwkTrackedMovementComponent` (`0x141075190`, 31 chamadas) quase não têm nada em comum, e o 1.8.0.13 não traz nenhum veículo de esteira que tivesse exercitado o de esteira. Os ticks dos controladores (`0x140fd8760` carro, `0x140fe60f0` esteira) e os leitores de entrada são estruturalmente idênticos, e os construtores dos componentes de rede escrevem os mesmos padrões. A peça exata que falta não foi achada. Foi relatado à Bohemia Interactive com esses endereços.

Consequência prática: veículos de esteira nativos funcionam para singleplayer, hosts e todos os assentos menos o do motorista em servidores dedicados. Para um veículo que precise ser dirigido de clientes hoje, a simulação de rodas (como os mods de tanque publicados usam) é o único caminho.

### Testando multiplayer num PC só: o Peer Tool

O Workbench consegue rodar um servidor e clientes extras do jogo ("peers") na mesma máquina.

1. World Editor, a seta ao lado do *Play*: escolha **Server localhost + PeerTool**. É um modo, não uma ação.
2. *Plugins* → *Settings* → **Peer Tool** (o diálogo `Configure` do plugin): ponha em *Executable* o `…\Arma Reforger\ArmaReforgerSteamDiag.exe` (a versão Diag, para o peer ter o menu de diagnóstico também) e acrescente uma entrada em *Peers Windows*. Os padrões estão certos: em janela, *Force Update*, *No Focus*, *Rpl Auto Join*. Largura e altura de 1280 × 720 deixam a janela manejável. Sem isso o modo sobe o servidor e nada mais: o executável padrão do plugin é `game.exe` e a lista de peers vem vazia.
3. *Play*. O Workbench vira o servidor; o peer abre com `-client -rpl-reconnect -rpl-timeout-disable -profile PeerPlugin1` e entra sozinho. O log mostra `Running command: …`.
4. O mundo de teste roda o modo Game Master: na janela do Workbench, libere uma facção jogável (US) e ponha um ponto de spawn, senão o peer fica esperando em "There are no available factions".
5. Dirija na janela do **peer**: ele é o cliente. A janela do Workbench é o servidor.

O peer usa um perfil novo, então a sensibilidade do mouse é a padrão, não a sua. Os logs dele ficam em `Documents\My Games\PeerPlugin1\logs\`.

Entradas do diagnóstico que ajudam (as duas janelas têm o menu): *Vehicles* → *Vehicle Networking and Reconciliation* → *S: Input/Output Diags = Sim In* traça `Brake` contra `BrakeClient` e `Handbrake` contra `HandbrakeClient`; *Determinism testing* → *SimState to CSV* escreve `$logs:VehicleTrackedStateDebug_<id>.txt` com uma linha por move (`MoveId`, servidor/cliente, `Corrected`, entradas, estado do motor); *Network Movement* → *Display Selection* / *NwkMovement Selection* / *Display buffer state* mostra o estado de replicação de um veículo ao vivo (Simulating, Interpolating, Producing, Consuming, SwitchingOwnership). Nem todas foram usadas neste trabalho.

### Perguntas em aberto

- Que passo nativo do caminho do cliente dono mantém o personagem do motorista vivo nos carros e falta nos veículos de esteira?
- Quais são as unidades da tolerância de ângulo (0,003), e que valores folgados mantêm giro e ré suaves com latência real?
- O espelho do freio de mão no servidor aguenta 100 ms ou mais de latência, ou a correção em cada transição fica visível?

---

[← Apêndice](17-appendix.md) · [Índice](../../README.pt-BR.md)
