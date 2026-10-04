> 🇧🇷 Português · [🇬🇧 English](../en/10-tuning.md)

[← Bugs e limitações (1.8.0.13)](09-bugs.md) · [Índice](../../README.pt-BR.md) · [Scripts prontos →](11-scripts.md)

# Afinação com dados reais

O objetivo é partir da ficha técnica do veículo real e chegar a valores que andam certo no jogo. Use a ordem abaixo; cada passo depende do anterior.

### Dados para levantar

| Dado | Leopard 2A7 | Vai para |
| --- | --- | --- |
| Massa em ordem de combate | 64.5 t | `RigidBody.Mass` |
| Potência e rotação | 1100 kW a 2600 rpm (MTU MB 873 Ka-501) | `MaxPower`, `RpmMaxPower` |
| Torque máximo | \~4700 N·m a \~1600 rpm | `MaxTorque`, `RpmMaxTorque` |
| Lenta, corte, limite | 700 / 2700 / 3000 rpm | `RpmIdle`, `RpmRedline`, `RpmMax` |
| Marchas | Renk HSWL 354: 4 à frente, 2 rés | `Forward`, `Reverse` (o sim aceita só uma ré) |
| Velocidade máxima | \~70 km/h à frente, \~31 km/h em ré | Cálculo do `Ratio` |
| Raio da roda motriz | 0.355 m (medido no modelo) | `Sprocket.Radius` |
| Rodas de apoio | 7 por lado, raio 0.35 m + sapata | `RoadWheel.Radius` |
| Curso da suspensão | \~350 mm para cima | `MaxTravelUp` |
| Área frontal | \~3.75 m × 2.5 m | `ReferenceArea` |

### 1. Massa e centro de massa

Use a massa real em kg. Coloque o `CenterOfMass` baixo, perto do piso do casco. No Leopard: `0 1 -0.3`.

### 2. Motor

Copie potência, torque e rotações da ficha. Depois ajuste os três valores que não estão na ficha:

| Atributo | Regra | Leopard |
| --- | --- | --- |
| `Friction` | Cerca de 10% do `MaxTorque` | 470 |
| `Steepness` | 50 (os motores vanilla usam de 4 a 50) | 50 |
| `Inertia` | Comece em 3; mais alto deixa a rotação lenta para subir | 3 |

Com `Friction` 1400 (30% do torque), o Leopard acelerava devagar e a ré nem vencia o atrito do próprio motor.

### 3. Câmbio

Transmissões reais de tanque têm conversor de torque, que multiplica o torque por cerca de 2 a 2.5 vezes na saída. A simulação não tem conversor. Compense encurtando as primeiras marchas.

Leopard: `Forward { 6.0 3.3 1.8 1.0 }`, `Reverse 2.3`.

### 4. Redução final pela velocidade máxima

Escolha o `Ratio` para que a marcha mais alta atinja a velocidade real na rotação de potência máxima:

```math
R_d = \frac{\text{RpmMaxPower} \cdot \frac{2\pi}{60} \cdot r_{\text{sprocket}} \cdot 3.6}{g_{\text{última}} \cdot v_{\text{máx}}}
```

Leopard: 2600 · 0.10472 · 0.355 · 3.6 / (1.0 · 71) ≈ **4.9**.

Confira a tabela de velocidades por marcha, ainda a 2600 rpm:

| Marcha | Relação | Velocidade |
| --- | --- | --- |
| 1ª | 6.0 | 11.8 km/h |
| 2ª | 3.3 | 21.5 km/h |
| 3ª | 1.8 | 39.5 km/h |
| 4ª | 1.0 | 71.0 km/h |

Medido em jogo: 74.3 km/h no plano, com a rotação passando um pouco de 2600.

### 5. Suspensão

Calcule a mola pela carga estática de cada roda e pelo afundamento desejado:

```math
k = \frac{\text{Mass} \cdot 9.81}{n_{\text{rodas}} \cdot \text{afundamento}}
```

Leopard: 64500 · 9.81 / (14 · 0.10 m) ≈ 452 000 N/m. Isso dá `SpringRate 450`, se a unidade for N/mm (provável, não confirmada).

Para o amortecedor, use uma fração ζ do amortecimento crítico:

```math
c = \zeta \cdot 2\sqrt{k \cdot m_{\text{roda}}}, \qquad m_{\text{roda}} = \frac{\text{Mass}}{n_{\text{rodas}}}
```

Com ζ = 0.3: c ≈ 27 000 N·s/m. O Leopard usa 25 000 na compressão e 35 000 no retorno.

### 6. Freios

Regra prática: a soma dos `BrakeTorque` deve ficar perto de massa × desaceleração × raio. Para 0.6 g no Leopard, isso dá cerca de 140 000 N·m. O prefab tem 14 × 6000 + 2 × 30000 = 144 000 N·m.

### 7. Aerodinâmica, retardador e controlador

- `ReferenceArea` = largura × altura. `DragCoefficient` em torno de 0.9 para um casco quadrado.
- `RetarderBaseTorque 0` e `RetarderThrottleSpeed 20`, senão o retardador limita a velocidade.
- `ThrottleReverseTarget 1`.

### 8. Medir com telemetria

1. Adicione o `LEO_TelemetryComponent` (seção [Scripts prontos](11-scripts.md)) ao prefab.
2. Dirija: arrancada até a máxima, freada, ré e giro no lugar.
3. Abra o log da sessão: `Documentos\My Games\ArmaReforgerWorkbench\logs\logs_<data>\console.log`.
4. Filtre as linhas `LEOTEL`. Cada linha mostra velocidade, rotação, acelerador, marcha, embreagem e freio.
5. Compare com as fórmulas:

| O que olhar | Se estiver errado |
| --- | --- |
| Velocidade máxima menor que a calculada | Retardador ligado, `Friction` alta ou motor fraco |
| Velocidade travada sempre no mesmo valor | `RetarderThrottleSpeed` baixo |
| Rotação acima de `RpmMax` com `gear=0` | É o bug da ré. Normal sem o script |
| `thrust=0.80` com W | `ThrottleTurbo` 0.2. Normal |
| Velocidade por marcha diferente da tabela | Raio da roda motriz ou `Ratio` errados |

---

[← Bugs e limitações (1.8.0.13)](09-bugs.md) · [Índice](../../README.pt-BR.md) · [Scripts prontos →](11-scripts.md)
