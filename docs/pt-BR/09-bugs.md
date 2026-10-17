> 🇧🇷 Português · [🇬🇧 English](../en/09-bugs.md)

[← Como funciona por dentro](08-internals.md) · [Índice](../../README.pt-BR.md) · [Ajuste com dados reais →](10-tuning.md)

# Bugs e limitações (1.8.0.13)

Todos os itens abaixo foram reproduzidos no Workbench 1.8.0.13 (executável Diag), só com o jogo base. Versões futuras podem corrigir alguns; teste de novo depois de cada atualização.

A última coluna diz como cada item é conhecido:

| Rótulo | Significado |
| --- | --- |
| Medido | Visto na telemetria ou no log, com números |
| Medido (2A7) | Medido na versão anterior deste guia, do Leopard 2A7; ainda a única evidência |
| Engenharia reversa | Lido no executável 1.8.0.13, não testado no jogo |
| Observado em vídeo | Visto quadro a quadro nos vídeos de teste |
| Não testado | Esperado, mas ninguém conferiu ainda |

### Resumo

#### Simulação, transmissão e controlador

| Problema | Sintoma | Contorno | Evidência |
| --- | --- | --- | --- |
| **A ré nativa está quebrada** | O motor sobe acima de `RpmMax` e nenhum torque chega às lagartas. Numa descida, só a gravidade move o tanque | Script que empurra o casco (`LEO_TrackedReverseComponent`, [Scripts prontos](11-scripts.md)) + `ThrottleReverseTarget 1`. Detalhes [abaixo](#reverse-in-detail) | Medido + engenharia reversa |
| O motorista sai com o motor ligado | Sem motorista, o controlador puxa o freio de mão (freio 6,0). Parado, ele então seleciona a ré (marcha 0, embreagem 1), e o bug da ré prende o motor em cerca de 4500 rpm com o tanque estacionado | `m_bStopEngineOnExit` (padrão 1) no `LEO_TrackedReverseComponent` desliga o motor quando o banco do motorista fica vazio. O log mostra `LEOREV driver left: engine stopped` | Medido |
| `ThrottleReverseTarget` abaixo de 1 | Ao segurar S parado, o tanque anda **para a frente** | Use `ThrottleReverseTarget 1` | Medido (2A7) |
| `.ct` vanilla incompleto | `Tracked - failed to load!` | Acrescente curvas de atrito, modelos de roda e lagartas | Medido (2A7) |
| Retardador padrão | Velocidade máxima travada em \~51–54 km/h | `RetarderBaseTorque 0`, `RetarderThrottleSpeed 20` | Medido (2A7) |
| `ThrottleTurbo` 0.2 | W sozinho dá só 80% do acelerador (`thrust=0.80` na telemetria) | Use o turbo ou baixe o `ThrottleTurbo` | Medido |
| `MaxClutchTorque` | Mudar o valor não muda nada | Ignore | Medido (2A7) + engenharia reversa |
| Giro no lugar (giro neutro) | O casco recua devagar a 0,2–3,8 km/h, porque a lagarta que anda para trás gira mais rápido. O controlador segura o freio 6,0. Com a embreagem acoplada, o motor fica em `RpmIdle` (700 rpm, 4601 N·m); com a embreagem aberta, em 2700 rpm | Nenhum conhecido. A causa está em aberto | Medido + observado em vídeo |

#### Suspensão, rodas e elos da esteira

| Problema | Sintoma | Contorno | Evidência |
| --- | --- | --- | --- |
| A altura do casco depende do `MaxTravelDown` | A mola tem força zero no curso todo para baixo. Se o `MaxTravelDown` for maior que o afundamento estático, o casco fica alto. 2A6 com `MaxTravelDown 0.15`: 5,3 cm acima da altura do modelo | Use `MaxTravelDown` ≈ m·g / (n·k). 2A6: 0.10 → o casco fica 3 mm acima da altura do modelo. Veja [Suspensão e altura do casco](13-suspension.md) | Medido |
| `CarProcAnimComponent` no trem de rolamento de esteira | A maioria dos ossos das rodas recebe os sinais de **outra** roda. No 2A6, só o `v_wheel_l01` estava certo. As rodas direitas 2–7 flutuavam cerca de 5 cm acima da corrente paradas, e as rodas esquerdas pares giravam com a lagarta direita. Reordenar os parâmetros (L01, R01, L02, …) não resolveu na única rodada que tentou (veja [Em aberto](#em-aberto)) | Deixe o `CarProcAnimComponent` vazio. Mova os ossos a partir da simulação com o `LEO_RunningGearComponent`. Veja [Rodas de apoio, roda-guia, roda motriz e roletes](14-running-gear.md) | Medido + observado em vídeo |
| Corrente de elos nativa (a corda do `TrackSegment`) | Uma corda 2D com gravidade, contato com os círculos das rodas e tração pela roda motriz, mas **sem colisão com o chão** e **sem API de script** para reiniciá-la. Com 5 cm de folga (`TrackLength 0.2101`) e o casco 5 cm alto, ela saiu da roda motriz depois de um capotamento e ficou fora. Com 12 cm (`0.2110`) a folga pendeu abaixo das rodas de apoio e atrás da roda motriz, e uma explosão de mina a jogou para fora de novo | Esconda os elos dela e desenhe os elos com a corrente por script (`LEO_TrackChain`). A corrente por script por construção não sai do lugar; **ainda não** foi testada capotando nem com mina. Veja [Elos da esteira](15-track-links.md) | Observado em vídeo + engenharia reversa |
| Ordem dos `TrackPart` | A velocidade da corrente é a velocidade angular da peça 1 vezes o `Radius` da peça 0, e o ângulo visual de cada peça deriva da peça 1. O carregador também precisa da roda motriz e da roda-guia no mesmo índice nas duas lagartas (`Wheels - %s mismatch between tracks`) | Rodas de apoio primeiro (índice 0–6), depois roda-guia, roda motriz e roletes de retorno. Use a mesma ordem nos dois lados | Engenharia reversa |
| `TrackThickness` | Guardado, nunca lido | Ponha a espessura do elo nos raios das rodas (raio da simulação = aro visual + espessura do elo) | Engenharia reversa |
| *Generate track positions* | A ação do editor existe no executável, mas não foi achada na interface do Workbench | Gere o `TrackPositions` offline com a `tools/trackpositions.py` ([Elos da esteira](15-track-links.md)) | Engenharia reversa |

#### API de script

| Problema | Sintoma | Contorno | Evidência |
| --- | --- | --- | --- |
| `VehicleTrackedSimulation.GetInputs()` | **Derruba** o Workbench quando chamado por script (escreve através de um ponteiro nulo) | Nunca chame. `GetFloorNormalAngles()` tem o mesmo defeito | Medido (2A7) + engenharia reversa |
| `GetThrottle()`, `SetThrottle()`, `IsHandbrakeOn()` | Sempre 0, não faz nada, sempre false | Leia o sinal `engineThrust`, ou use `GetGear()`, `GetClutch()` e `GetBrake()`. `GetBrake()` lê 6,0 para o freio de mão | Engenharia reversa |
| Getters das rodas antes de a simulação carregar | `TrackCount()` devolve 0, e todo getter devolve 0, false ou um vetor zero. Isso dura 0,8–1,3 s depois de o veículo surgir no *Play*. `GetGear()` lê 0 (= R) nesse tempo | Pegue a simulação com `TrackedControllerComponent.GetSimulation()` e use a proteção [abaixo](#script-api-rules) a cada frame | Medido + engenharia reversa |
| Índices das rodas e dados de contato | Só o `WheelGetPosition` confere o índice da roda. Nenhum getter confere o índice da lagarta, então índices fora da faixa leem memória alheia. O `WheelGetContactPosition` devolve o último ponto guardado mesmo sem contato | Mantenha `track < TrackCount()` e `wheel < WheelCount()`. Confira `WheelHasContact` antes de ler dados de contato | Engenharia reversa |
| `WheelGetPosition(track, wheel, displacement)` | Devolve o centro da roda em relação ao centro de massa do corpo (referencial interno da física), não no espaço do casco. Com displacement 0 é a posição no curso todo para baixo: com `MaxTravelDown 0.15`, as 7 rodas de apoio esquerdas do 2A6 leram y = −1,899 m (a lagarta direita não foi registrada) (0,4186 − 0,15 − 2,168). A unidade do argumento displacement é desconhecida | Some o centro de massa efetivo (ponto COM do modelo + `CenterOfMass`) para ter o espaço do casco. Para o centro da roda ao vivo, use `WheelGetContactPosition` + `WheelGetRadius` ao longo do eixo para cima do casco | Medido |

#### Diag, Workbench e multiplayer

| Problema | Sintoma | Contorno | Evidência |
| --- | --- | --- | --- |
| Diagnóstico do veículo **sem** corrente de elos | **Derruba** o Workbench (violação de acesso no PostFrame) quando *Show vehicle debug* ou *Show Controller Diags* foi aberto em 02/10/2026, na versão anterior, do 2A7, que não tinha corrente. O desenho do diagnóstico lê o ponteiro da corrente sem conferir se é nulo | Configure a corrente de elos primeiro (`TrackPositions` + `TrackLength`). Com a corrente presente (2A6, 06/10/2026), a depuração do veículo rodou uma sessão inteira de 5 minutos, e as entradas dela foram ligadas e desligadas sem travar. Veja [Diagnóstico do veículo e telemetria](16-debugging.md) | Medido (2A7) + engenharia reversa + observado em vídeo |
| Recarregar scripts durante o *Play* | **Derruba** o Workbench | Pare o *Play* antes de recarregar | Medido (2A7) |
| Recarregar scripts com um mundo aberto | Assert do Diag `Resources are leaking! Check log!` durante a recarga, 1–9 s depois de começar (77 recursos vanilla, nenhum do mod). Sete vezes em 05/10/2026 | Aperte *Ignore*. Fechar o mundo antes de recarregar talvez evite; não está confirmado | Medido |
| Edições do prefab com o mundo aberto | O veículo que já está no mundo mantém os valores antigos da simulação. O 2A6 continuou 5 cm alto, como com `MaxTravelDown 0.15`, depois de o prefab dizer 0.10 | Feche o mundo, recarregue os scripts sem mundo aberto e abra o mundo de novo | Medido |
| Trocar um modelo (`.fbx`) com o mundo aberto | O Workbench reimporta um `.fbx` sozinho quando ele muda. Trocar o modelo do casco com o mundo aberto travou o Workbench uma vez (um núcleo a 100%) | Feche o World Editor antes de trocar modelos. Se travar, encerre o processo e abra de novo | Medido |
| `AICarMovementComponent` na base vanilla | Aviso de que falta o `CarControllerComponent` | Inofensivo para dirigir manualmente | Medido |
| Empurrão de ré por script | No multiplayer só rodava na máquina do motorista: o casco do servidor ficava parado e o cliente fazia rubberband | A v5 roda o empurrão no servidor também, a partir do estado do motorista mandado por RPC ([capítulo 18](18-multiplayer.md)) | Medido |
| Freio de mão descartado do move de rede | Num servidor dedicado, virar parado não faz nada: o motor acelera e o casco faz rubberband. O campo de freio de mão do move vem do stub `IsHandbrakeOn` e a repetição corta o freio 6,0 em 1,0, então o servidor nunca entra no modo de giro ou direção neutra | v5: o cliente do motorista manda o freio de mão, o servidor repõe com `SetBreak(1, true)` ([capítulo 18](18-multiplayer.md)) | Medido + engenharia reversa |
| O motorista num cliente | Cabeça congelada, olhar lento parado, câmera pula ao acelerar e virar, comandos presos. Outros assentos, o host e o singleplayer funcionam; um jipe vanilla no mesmo estado funciona | Nenhuma encontrada: seis experimentos, inclusive um veículo de posse do servidor dirigido por entradas RPC, não mudaram nada. Relatado à Bohemia Interactive ([capítulo 18](18-multiplayer.md)) | Medido |
| Trem de rolamento e corrente de elos por script | Só visual, criado localmente. Nada roda num servidor dedicado | Medido num cliente do Peer Tool: os dois rodam, os elos aparecem. Proxies em servidor real não medidos | Medido (cliente) |

<a id="script-api-rules"></a>

### Regras da API de script

Todo script que lê a simulação de esteira deve começar assim:

```c
VehicleTrackedSimulation sim = controller.GetSimulation(); // controller = TrackedControllerComponent
if (!sim || !sim.IsValid() || sim.TrackCount() < 2)
	return; // not loaded yet: the getters return 0, false or a zero vector
```

- O `TrackCount()` devolve a constante 2 depois que a simulação carrega. Ele não lê o prefab, então `TrackCount() < 2` quer dizer "ainda não carregou" (engenharia reversa).
- O `WheelCount()` é o número de entradas `TrackPart` de **uma** lagarta. O 2A6 registra `WheelCount 13`: 7 rodas de apoio, roda-guia, roda motriz e 4 roletes de retorno.
- Os índices das rodas são (lagarta, índice do `TrackPart` na ordem do prefab). A lagarta 0 é o primeiro `Track` do prefab.
- Os getters funcionam com o motor desligado depois que a simulação fica válida. No 2A6 ela ficou válida 8–13 s antes de ligar o motor.

<a id="reverse-in-detail"></a>

### A ré em detalhe

O controlador funciona. Ele seleciona a ré (marcha 0), acopla a embreagem e acelera. O defeito está na simulação.

A reação da carga é dividida pela relação da marcha. A relação de ré é sempre negativa, então a reação **soma** RPM em vez de tirar. O RPM passa de `RpmMax`, onde o torque é zero. Nenhum torque chega às lagartas.

O RPM fica preso em `RpmMax + Δ`, e dá para calcular o Δ:

```math
\Delta = \frac{\text{Mass} \cdot 9.81 \cdot \frac{1}{240} \cdot \frac{30}{\pi}}{|\text{Reverse}| \cdot \text{Ratio} \cdot \text{Inertia}}
```

A previsão bate com a telemetria das duas versões (`RpmMax` 3000 em todas as linhas):

| Versão | Massa | `Ratio` | `Inertia` | `Reverse` | Previsto | Observado |
| --- | --- | --- | --- | --- | --- | --- |
| 2A7 (versão anterior) | 64500 kg | 4.9 | 6 | 4.0 | 3214,08 rpm | 3214,08–3214,10 rpm |
| 2A7 (versão anterior) | 64500 kg | 4.9 | 3 | 2.3 | 3744,64 rpm | 3744,63–3744,73 rpm |
| **2A6** | 62300 kg | 4.6 | 3 | 2.3 | 3766,14 rpm | 3766–3768 rpm |

O valor do 2A6 vem de 57 amostras `LEOTEL` na marcha 0 (sessões de 05/10 e 06/10/2026; 40 com acelerador a fundo, 17 com o acelerador solto) e do painel Engine do diagnóstico nos vídeos de teste A e B (06/10/2026). As linhas do 2A7 foram medidas na versão anterior.

**Nenhum valor do prefab conserta isso.** O `Ratio` do diferencial precisa ser positivo, e o sinal da ré é forçado. A embreagem, o `TransmissionRND` e o `ClutchCouple*Rpm` não mudam nada.

Com `ThrottleReverseTarget` abaixo de 1, o motor produz torque negativo. Multiplicado pela relação negativa, ele empurra o tanque para a frente.

**Outros sintomas do mesmo bug:**

| Situação | O que você vê | Evidência |
| --- | --- | --- |
| Ré por script rodando | O painel Engine lê 3766–3768 rpm e 0 N·m com acelerador a fundo, enquanto o script move o tanque. O `LEOTEL` lê 3766–3768 em 40 de 107 dessas amostras, e 3340–3890 nas outras | Observado em vídeo; medido |
| Acelerador solto em R, sem freio | O rpm fica em 3766–3768, com −471 N·m no painel Engine | Medido + observado em vídeo |
| Freio aplicado em R com a embreagem acoplada: freando no fim de uma ré, ou o freio 6,0 do controlador parado ou no fim de um giro | O rpm dispara para 3950–5074 (2A6, `LEOTEL`; 4469 e 4804 no painel Engine). O 2A7 chegou a cerca de 4445 | Medido; observado em vídeo; medido (2A7) |
| O motorista sai com o motor ligado | O controlador seleciona R parado, e o motor fica preso em cerca de 4500 rpm | Medido |

A fórmula acima prevê o patamar de 3766–3768 das duas primeiras linhas. Os valores mais altos com o freio aplicado ainda não têm explicação.

### O contorno usado no Leopard

O `LEO_TrackedReverseComponent` aplica uma força para trás no casco quando tudo isto vale:

- o motorista local segura `TrackedBrake` sem `TrackedThrottle`;
- o motor está ligado;
- o casco está parado ou já andando para trás (velocidade para a frente ≤ 1,5 km/h);
- a marcha nativa é ré ou neutro (índice ≤ 1);
- o freio nativo está solto (`GetBrake()` ≤ 0,05).

A força é `m_fLaunchForceG` × peso parado (0,3 g) e diminui até zero na velocidade máxima de ré do script. Um torque de guinada cuida do esterçamento na ré. Uma conferência separada desliga o motor quando o banco do motorista fica vazio (`m_bStopEngineOnExit`).

Medido no 2A6 (`LEOTEL`, 05/10/2026): de 0 a −21 km/h em cerca de 3,0 s, e −27,8 km/h depois de cerca de 7,5 s. A força foi de 176 kN a −1,3 km/h. A versão anterior, do 2A7, também chegou a −21 km/h em cerca de 3 s.

![Arrancada de ré com a depuração nativa do veículo ligada](../images/reverse-launch.gif)

*Arrancada de ré, lado esquerdo. O empurrão por script leva o tanque de 0 a −10,5 km/h em cerca de 1,2 s, enquanto o painel Engine fica preso em 3766 rpm com 0 N·m: o bug da ré nativa. Vídeo de teste A (06/10/2026), 134,0–137,6 s · [foto em tamanho cheio](../images/reverse-launch.jpg) em 136,0 s · Modelo: buh-late, CC BY 4.0, modificado ([créditos](../../CREDITS.md)).*

Mantenha a velocidade de ré do script abaixo do limite da ré nativa:

```math
v_{\text{max,rev}}\,[\text{km/h}] = \frac{\text{RpmMax}}{|\text{Reverse}| \cdot \text{Ratio}} \cdot \frac{2\pi}{60} \cdot r_{\text{sprocket}} \cdot 3.6
```

No 2A6: 3000 / (2,3 · 4,6) · 0,10472 · 0,34 · 3,6 ≈ 36,3 km/h. O script para em 31 km/h (`m_fMaxReverseSpeedKmh`). A versão anterior, do 2A7 (`Ratio` 4.9, roda motriz 0,355 m), tinha limite de 35,6 km/h.

### Outra opção: simulação de rodas

Os mods de tanque publicados usam a `VehicleWheeledSimulation` com um eixo por par de rodas. A curva vem de eixos que esterçam (+15° na frente, −15° atrás), como num carro. A ré funciona, mas o comportamento é menos fiel que o da esteira nativa com o script de ré. Nenhum dos mods publicados verificados configura a corrente de elos nativa.

### Em aberto

- **Em aberto:** por que o rpm chega a 3950–5074 em R quando o freio é aplicado com a embreagem acoplada, acima da previsão do Δ.
- **Em aberto:** durante um giro no lugar, o freio 6,0 é o freio de mão do controlador, ou o freio aplicado quando as rodas giram contra o sentido selecionado? É por isso que o rpm cai para a lenta? O recuo é uma característica do `TripleDifferential` e da configuração do giro neutro?
- **Em aberto:** o `CarProcAnimComponent` mapearia certo com parâmetros intercalados (L01, R01, L02, …) num mundo recém-aberto? A rodada que tentou essa ordem pode ter usado dados de prefab em cache.
- **Em aberto:** o RPM das rodas, ou a tração, depende da corda nativa? Dá para remover a corda (`TrackPositions` vazio ou `TrackLength 0`) para economizar CPU? Não testado.
- **Em aberto:** as estatísticas da corda por lagarta no diagnóstico leem `Sim: Paused`, `Substep: 0/25`, `Momentum 0` e `Energy 0` mesmo andando. O significado é desconhecido. *Track dynamic* e *Reset tracks* não foram testados.
- **Em aberto:** a corrente por script tira a velocidade da roda de apoio 0. Uma roda sem contato pode informar um RPM diferente (−230,6 rpm em ré enquanto as rodas no chão liam 0–20), então os elos por script podem girar errado enquanto essa roda estiver no ar.
- **Em aberto:** os elos por script podem estar montados de frente para trás invertidos (180° em volta do Y deles) em relação ao referencial nativo. Não conferido de perto.
- **Em aberto:** o custo por frame dos 146 elos nativos escondidos mais os 146 por script não foi medido.

---

[← Como funciona por dentro](08-internals.md) · [Índice](../../README.pt-BR.md) · [Ajuste com dados reais →](10-tuning.md)
