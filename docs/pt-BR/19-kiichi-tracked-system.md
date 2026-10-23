> 🇧🇷 Português · [🇬🇧 English](../en/19-kiichi-tracked-system.md)

[← Multiplayer](18-multiplayer.md) · [Índice](../../README.pt-BR.md) · [1.9 Experimental →](20-experimental-1.9.md)

# Kiichi Tracked System

O Kiichi Tracked System é uma simulação de veículo de esteira escrita inteira em script: um componente, o [`LEO_TrackSimComponent`](../../scripts/LEO_TrackSimComponent.c). Ele não usa a `VehicleTrackedSimulation`. O casco é um corpo rígido dinâmico comum, e o componente aplica todas as forças nele no `EOnSimulate` (o passo fixo da física), só no servidor.

Ele existe porque a simulação nativa não tem conserto por script: o move de rede descarta o freio de mão, a ré não acopla, o caminho do cliente dono num veículo de esteira está inacabado ([capítulo 18](18-multiplayer.md)), e na 1.9.0.10 Experimental a simulação nativa nem consegue manter um casco parado no chão ([capítulo 20](20-experimental-1.9.md)). Um sistema que nunca toca no caminho nativo evita tudo isso, e o mesmo código roda na 1.8 e na 1.9.

Ele reaproveita os **valores de ajuste** do prefab nativo do Leopard (relações de marcha, redução final, curva do motor, raio das rodas, constante de mola, a massa do casco no `RigidBody`), não a simulação nativa.

Cada fato diz como é conhecido: **medido** (logs de rodadas automáticas), **projeto** (o que o código faz) ou **não testado**.

### Em resumo

| | Vanilla (nativa, com o conserto do `LEO_TrackedDriveComponent`) | Kiichi Tracked System |
| --- | --- | --- |
| Física | `VehicleTrackedSimulation` (lado da engine) | Script, num corpo rígido comum |
| Giro no lugar / direção neutra num servidor dedicado | ✅ Só porque o próprio servidor aplica o freio de mão | ✅ As esteiras contra-giram (projeto) |
| Ré | ⚠️ Um empurrão no casco; a ré nativa não acopla | ✅ O motor gira as esteiras para trás |
| Motorista num cliente | Posse do servidor, inputs por RPC; a replicação nativa de movimento move o casco | Posse do servidor, inputs por RPC; replicação própria da transformação com dead reckoning |
| 1.8.0.13 | ✅ | ✅ (medido) |
| 1.9.0.10 Experimental | ❌ A simulação nativa quebra antes de andar | ✅ (medido) |
| Overlay nativo de debug, corrente nativa | Disponíveis | Indisponíveis (sem simulação nativa); linhas `LEOSIM` no log no lugar |

### O modelo

Tudo roda no servidor, uma vez por passo da física:

- **Suspensão.** Um raycast por roda de apoio, a partir da posição modelada da roda (ossos `v_suspension_<l|r>0<k>`), ao longo do eixo para baixo do casco. Mola e amortecedor agem ao longo do eixo para cima do casco, no ponto da roda, com uma pré-carga para o casco descansar perto da altura modelada. As rodas que tocam o chão carregam o casco e formam a área de contato.
- **Trem de força.** Curva de torque do motor (torque máximo, potência máxima, marcha lenta, rpm máximo, atrito), embreagem que patina na marcha lenta, câmbio automático por rpm, redução final, roda motriz. O torque do motor gira as duas rodas motrizes, então **cada esteira tem a própria velocidade**, e o rpm acompanha as esteiras pelo câmbio.
- **Tração.** Em cada roda de apoio em contato, a esteira desliza contra o chão por (velocidade da esteira − velocidade do chão naquela roda). A força de cisalhamento cresce com o deslizamento e satura em μ × a carga daquela roda. A soma dessas forças move o casco; a reação delas carrega a esteira e, por ela, o motor. Mais rodas no chão dão mais força de tração; uma esteira que gira mais rápido que o chão patina e puxa menos.
- **Direção (embreagem e freio).** Só frente move as duas esteiras igual. Frente + lado passa parte da força do motor para a esteira de fora e freia um pouco a de dentro. Só lado em movimento freia forte a esteira de dentro e deixa a de fora rolar, então o casco gira enquanto desacelera. Quase parado, a direção contra-gira as esteiras: um pivô de verdade. O deslizamento lateral das rodas da frente e de trás é a resistência à curva, e ele cai com a velocidade.
- **Ré.** Com freio apertado e o tanque parado, o câmbio seleciona a ré e o motor gira as esteiras para trás.
- **Freios e aderência.** Os freios agem nas esteiras. Cada roda em contato tem aderência lateral contra deslizar de lado, mais resistência ao rolamento.

Todos os coeficientes são atributos, listados com os padrões no [capítulo 11](11-scripts.md#leo_tracksimcomponent-kiichi-tracked-system).

### Multiplayer

O servidor é dono do veículo e da simulação (projeto, medido):

| Máquina | O que faz |
| --- | --- |
| Servidor | Roda a simulação. Toma o veículo de volta se a engine entregá-lo ao cliente do motorista (um cliente dono de um veículo com a simulação nativa inerte trava, medido). Replica velocidade das esteiras, rpm, marcha, estado do motor e a transformação do casco (posição, ângulos, velocidade) como `RplProp`, 20 vezes por segundo |
| Cliente do motorista | Lê as teclas do motorista (`TrackedContext`) a cada quadro e envia acelerador, freio, direção e freio de mão pelo `LEO_PlayerController` a 20 Hz e a cada mudança; ligar/desligar o motor vai por canal confiável |
| Todo cliente | Segue a transformação replicada do casco com dead reckoning e suavização (`m_fProxySmoothing`). O corpo rígido do casco nunca é simulado ali. Roda os raycasts da suspensão em modo visual (sem forças), para as rodas seguirem o chão sob o casco replicado, e alimenta o trem de rolamento e a corrente de elos com a velocidade replicada das esteiras |

As regras de replicação em que isso se apoia estão no [capítulo 18](18-multiplayer.md#regras-de-replicação-para-veículos-por-script-medidas). O código de cliente roda pela call queue, porque um veículo replicado não recebe eventos de entidade nos clientes.

### Os três prefabs do mod

O mod do Workshop traz três prefabs do Leopard 2A6, filhos de um `Leopard2A6_base.et` comum que não tem sistema de direção ([exemplo](../../examples/leopard2a6/Prefabs_Leopard2A6_variants.et.txt)):

| Prefab | Nome no Game Master | Sistema de direção |
| --- | --- | --- |
| `Leopard2A6.et` | Leopard 2A6 (vanilla tracked simulation) | `VehicleTrackedSimulation` nativa + `LEO_TrackedDriveComponent` |
| `Leopard2A6_LEO18.et` | Leopard 2A6 (Kiichi Tracked System, 1.8) | `LEO_TrackSimComponent` |
| `Leopard2A6_LEO19.et` | Leopard 2A6 (Kiichi Tracked System, 1.9) | `LEO_TrackSimComponent` |

Os prefabs Kiichi da 1.8 e da 1.9 são iguais, só muda o nome; são dois para cada versão do jogo ter uma entrada ao lado da vanilla, para comparar lado a lado. Os prefabs Kiichi não têm bloco `VehicleTrackedSimulation`, então o log mostra `Tracked - failed to load!` e `RoadWheel - configuration is missing!` quando um deles aparece. É esperado: a simulação nativa fica inerte.

### Usando no seu tanque

1. Copie todos os scripts de [`scripts/`](../../scripts/) ([capítulo 11](11-scripts.md#como-instalar)).
2. Monte o prefab do casco sobre o `TrackedVehicle_Base.et` como sempre, com `RigidBody` (massa, centro de massa), `CarProcAnimComponent` vazio, `LEO_RunningGearComponent` e `LEO_TrackBeltComponent`, mas **sem** o bloco `VehicleTrackedSimulation`.
3. Arme os ossos de suspensão `v_suspension_<l|r>01` … `0<n>` nos centros modelados das rodas de apoio: os raycasts começam ali. Se faltar um osso, o log diz `LEOSIM … suspension bones missing` e o layout do Leopard é usado.
4. Acrescente o `LEO_TrackSimComponent` e ajuste os atributos com os dados do seu veículo: número e raio das rodas de apoio, constante de mola por roda e pré-carga (o casco descansa na altura modelada quando n · `m_fSpringRate` · `m_fPreload` ≈ m · g; Leopard: 62300 · 9,81 / (14 · 0,10) ≈ 437 000 N/m, ajustado para 450 000), curva do motor, relações de marcha, redução final, raio da roda motriz.
5. Teste com `m_bDebug 1` (uma linha `LEOSIM` por segundo: velocidade, taxa de guinada, inputs, rpm, marcha, velocidade das esteiras, rodas em contato, carga por lado) e desligue para lançar.

### Medido

Rodadas automáticas do jogo, 22/10/2026, 1.8.0.13, mundo de teste em Arland, Leopard 2A6 (62,3 t):

| Rodada | Resultado |
| --- | --- |
| Servidor headless, inputs de teste por script (acelerador 1 por 7 s, depois direção total, depois solta) | 0 a 38,7 km/h em uns 6 s, marchas 2 a 4, guinada até 0,72 rad/s, para, sem instabilidade |
| Host sentado como motorista, mesmos inputs | O motor liga ao acelerar, 0 a 40 km/h, guinada até 1,1 rad/s |
| Servidor dedicado Diag + cliente Diag, o jogador do cliente sentado como motorista, inputs pelo caminho real do RPC | A cópia do cliente acompanha o casco até 40 km/h, na curva e na parada. Sem crash nos dois lados |

O mesmo componente foi medido na 1.9.0.10 Experimental em 20/10/2026 com o mesmo harness, com o mesmo comportamento.

### Limites conhecidos

- **Ainda não dirigido por uma pessoa num cliente.** Tudo acima vem de inputs por script. Sensação, câmera e latência não foram testadas.
- **Deslize parado.** Parado numa inclinação leve sem freio, o casco desliza a 1–2 cm/s (medido). Não corrigido.
- **Sem overlay nativo de debug** e sem corrente nativa: a corrente por script do [capítulo 15](15-track-links.md) é a única.
- **Som** usa o componente de som vanilla do veículo; rpm e marcha são replicados, mas ainda não alimentam os sinais de som.

---

[← Multiplayer](18-multiplayer.md) · [Índice](../../README.pt-BR.md) · [1.9 Experimental →](20-experimental-1.9.md)
