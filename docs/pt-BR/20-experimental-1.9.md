> 🇧🇷 Português · [🇬🇧 English](../en/20-experimental-1.9.md)

[← Kiichi Tracked System](19-kiichi-tracked-system.md) · [Índice](../../README.pt-BR.md)

# 1.9.0.10 Experimental

O que mudou para veículos de esteira no Arma Reforger **1.9.0.10 Experimental**, conferido em 19 e 20/10/2026 com o Workbench Experimental e rodadas headless automáticas. É uma versão experimental: teste de novo a cada build nova antes de confiar em qualquer coisa daqui.

Cada fato diz como é conhecido: **medido** (logs), **engenharia reversa** (`ArmaReforgerWorkbenchSteamDiag.exe` Experimental) ou **observado** (vídeo).

### Em resumo

| Ponto | 1.8.0.13 | 1.9.0.10 Experimental |
| --- | --- | --- |
| API de script para veículos de esteira | — | Sem mudança |
| Freio de mão no move de rede | Sempre falso (stub) | Sempre falso (o mesmo stub) |
| Força da suspensão das rodas de apoio | Sinal correto | **Sinal invertido** |
| Casco parado depois do spawn | Estável | **Jogado para coordenadas potência de dois** em uns 50–80% dos spawns |
| Recarregar scripts no Workbench | Nenhum efeito visto | **Corrompe o estado da simulação** |
| FPS do Workbench com a simulação nativa | Não medido | 12 fps com motorista, 45 sem (uma cena, observado) |
| Propriedades de estabilização da torre no prefab | Existem | `TurretInitStabilization` e `TurretEnabledStabilization` removidas (o prefab avisa propriedades obsoletas) |

A simulação nativa de esteira não dá para usar nesta build. O [Kiichi Tracked System](19-kiichi-tracked-system.md) não usa ela e funciona igual à 1.8 (medido).

### Força da suspensão com o sinal errado (medido)

Um Leopard 2A6 parado é empurrado **para dentro** do chão: −37,7 m/s² nos primeiros passos, uma força de uns 1,74 MN, que é 14 rodas de apoio × `SpringRate` 450 kN/m × 0,3 m (o `MaxTravelUp` inteiro). Depois o casco pula, capota ou é lançado. Com o `LEO_RunningGearComponent` mexendo os ossos de pivô das rodas, vira NaN e derruba o Workbench.

O único contorno achado é `SpringRate`, `CompressionDamper` e `RelaxationDamper` negativos. Ele faz o casco ficar em pé na 1.9.0.10 Experimental e quebraria na 1.8, então nunca publique isso num mod da 1.8. A `Rotation` do pivô é ignorada para o eixo da suspensão.

### Casco jogado para coordenadas potência de dois (medido)

Em uns 50–80% dos spawns, o casco parado é movido nos primeiros passos para coordenadas que são potências de dois, ou lançado a velocidades absurdas (84 339 m/s para baixo numa rodada registrada), e continua capotando. Acontece com ou sem os scripts do mod, com ou sem a corrente nativa, e com qualquer sinal de mola. Valores assim apontam para estado não inicializado na engine, não para os ajustes da física. O `EOnSimulate` dos scripts roda num passo fixo de 16,67 ms durante isso, então não é um passo de tempo longo.

Harness usado: um servidor dedicado no mundo de teste, 50 s por rodada, esperando o `Frame start`, várias rodadas por configuração.

### Recarregar scripts corrompe a simulação (medido)

Recarregar scripts no Workbench Experimental com um veículo de esteira no mundo corrompe a simulação dele: o próximo spawn é lançado a uns 10⁹ m/s. Reinicie o Workbench em vez de recarregar.

### O stub do freio de mão continua lá (engenharia reversa)

No exe do Workbench Experimental, o `IsHandbrakeOn` de esteira (`0x141016B00`) continua `<guarda de simulação pronta>; xor al, al; ret`: a simulação nativa de esteira não tem estado de freio de mão para informar. O gravador do move (`0x141034CA0`) guarda isso como float em `move + 0x0C`, então o freio de mão gravado é sempre 0. O `0x140EEFD70` (`cmp [sim+0x19F0], 2`) é só a guarda de prontidão que todo getter chama.

O move de carro (`0x140D20940`) guarda direção, acelerador, freio, freio de mão, marcha, embreagem e dois campos do controlador. O move de esteira guarda o par de inputs, freio, freio de mão (sempre 0), marcha, embreagem e mais um campo: o mesmo layout, tirando o freio de mão. Então, como na 1.8, girar no lugar num servidor dedicado exige que o próprio servidor aplique o freio de mão ([capítulo 18](18-multiplayer.md)).

### Outras notas

- O Workbench Experimental guarda as configurações dos plugins numa chave de registro separada: configure o Peer Tool de novo lá ([capítulo 18](18-multiplayer.md#testando-multiplayer-num-pc-só-o-peer-tool)).

---

[← Kiichi Tracked System](19-kiichi-tracked-system.md) · [Índice](../../README.pt-BR.md)
