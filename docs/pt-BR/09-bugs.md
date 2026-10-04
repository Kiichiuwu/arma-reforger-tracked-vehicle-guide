> 🇧🇷 Português · [🇬🇧 English](../en/09-bugs.md)

[← Como funciona por dentro](08-internals.md) · [Índice](../../README.pt-BR.md) · [Afinação com dados reais →](10-tuning.md)

# Bugs e limitações da versão 1.8.0.13

Todos os itens abaixo foram reproduzidos no Workbench 1.8.0.13. Versões futuras podem corrigir alguns deles; teste de novo depois de cada atualização.

### Resumo

| Problema | Sintoma | Contorno |
| --- | --- | --- |
| **Ré nativa quebrada** | Motor sobe acima de `RpmMax` e o tanque não sai do lugar. Na descida, só a gravidade move | Script que empurra o casco (seção [Scripts prontos](11-scripts.md)) + `ThrottleReverseTarget 1` |
| `ThrottleReverseTarget` abaixo de 1 | Ao segurar S parado, o tanque anda **para a frente** | Use `ThrottleReverseTarget 1` |
| `.ct` vanilla incompleto | `Tracked - failed to load!` | Adicione curvas de atrito, modelos de roda e esteiras |
| Retardador padrão | Velocidade máxima travada em \~51–54 km/h | `RetarderBaseTorque 0`, `RetarderThrottleSpeed 20` |
| `ThrottleTurbo` 0.2 | W sozinho dá só 80% do acelerador | Use o turbo ou reduza `ThrottleTurbo` |
| `VehicleTrackedSimulation.GetInputs()` | **Crash** do Workbench ao chamar em script | Nunca chame. `GetFloorNormalAngles()` tem o mesmo defeito |
| Diag *Show vehicle debug* / *Show Controller Diags* | **Crash** ao abrir com um veículo de esteira | Não use esses diags se não houver corrente visual de elos |
| `GetThrottle()`, `SetThrottle()`, `IsHandbrakeOn()` | Sempre 0, não fazem nada, sempre false | Leia o sinal `engineThrust` ou use `GetGear()` e `GetClutch()` |
| `MaxClutchTorque` | Mudar o valor não muda nada | Ignore |
| Corrente visual de elos | O asset padrão (elo do M113) não vem no jogo | Esteira estática no modelo, ou animação própria |
| Recarregar scripts durante o *Play* | **Crash** do Workbench | Pare o *Play* antes de recarregar |
| `AICarMovementComponent` no base vanilla | Aviso de que falta `CarControllerComponent` | Inofensivo para direção manual |
| Empurrão de ré por script | Em multiplayer, só roda na máquina do motorista | Pendente: rodar no servidor ou enviar por RPC |

### A ré em detalhe

O controlador funciona. Ele seleciona a ré (marcha 0), acopla a embreagem e acelera. O defeito está na simulação.

A reação da carga é dividida pela relação da marcha. A relação de ré é sempre negativa, então a reação **soma** rotação em vez de tirar. A rotação passa de `RpmMax`, onde o torque é zero. Nenhum torque chega às esteiras.

A rotação fica presa em `RpmMax + Δ`, e Δ pode ser calculado:

```math
\Delta = \frac{\text{Mass} \cdot 9.81 \cdot \frac{1}{240} \cdot \frac{30}{\pi}}{|\text{Reverse}| \cdot \text{Ratio} \cdot \text{Inertia}}
```

A previsão bate com a telemetria do Leopard (64500 kg, `RpmMax` 3000, `Ratio` 4.9):

| Inertia | Reverse | Previsto | Observado |
| --- | --- | --- | --- |
| 6 | 4.0 | 3214.08 rpm | 3214.08–3214.10 rpm |
| 3 | 2.3 | 3744.64 rpm | 3744.63–3744.73 rpm |

**Nenhum valor do prefab conserta isso.** O `Ratio` do diferencial precisa ser positivo, e o sinal da ré é forçado. Embreagem, `TransmissionRND` e `ClutchCouple*Rpm` não mudam nada.

Com `ThrottleReverseTarget` abaixo de 1, o motor gera torque negativo. Multiplicado pela relação negativa, ele empurra o tanque para a frente.

### O contorno usado no Leopard

Um componente de script aplica uma força para trás no casco quando:

- o motorista local segura `TrackedBrake` sem acelerar;
- o motor está ligado;
- o casco está parado ou já andando para trás;
- a marcha nativa é ré ou neutro (índice ≤ 1);
- o freio nativo está solto (sinal abaixo de 0.05).

A força diminui até zero perto da velocidade máxima de ré. Também há um torque de giro para virar em ré. Resultado medido: de 0 a −21 km/h em cerca de 3 s.

![Ré por script com curva](../images/reverse-steering.gif)

*A ré por script no teste em vídeo, virando enquanto anda para trás. Mais exemplos em [Exemplos em vídeo](05-video-examples.md).*

Mantenha a velocidade de ré do script abaixo do teto nativo da ré:

```math
v_{\text{máx,ré}}\,[\text{km/h}] = \frac{\text{RpmMax}}{|\text{Reverse}| \cdot \text{Ratio}} \cdot \frac{2\pi}{60} \cdot r_{\text{sprocket}} \cdot 3.6
```

No Leopard: 3000 / (2.3 · 4.9) · 0.10472 · 0.355 · 3.6 ≈ 35.6 km/h. O script para em 31 km/h.

### Outra opção: simulação de rodas

Os mods de tanque publicados usam `VehicleWheeledSimulation` com um eixo por par de rodas. A curva vem de eixos que esterçam (+15° na frente, −15° atrás), como num carro. A ré funciona, mas o comportamento é menos fiel que a esteira nativa com o script de ré.

---

[← Como funciona por dentro](08-internals.md) · [Índice](../../README.pt-BR.md) · [Afinação com dados reais →](10-tuning.md)
