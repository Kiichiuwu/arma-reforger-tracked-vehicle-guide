> 🇧🇷 Português · [🇬🇧 English](../en/14-running-gear.md)

[← Suspensão e altura do casco](13-suspension.md) · [Índice](../../README.pt-BR.md) · [Elos da esteira: a corda nativa e a corrente por script →](15-track-links.md)

# Rodas de apoio, roda-guia, roda motriz e roletes

Este capítulo trata das peças sobre as quais a esteira corre. Explica como a simulação as define, o tamanho de cada uma para a simulação e para o olho, como são numeradas, como lê-las por script e como fazê-las se mexer no modelo. A física da suspensão está em [Suspensão e altura do casco](13-suspension.md). Os elos estão em [Elos da esteira](15-track-links.md).

**Em resumo.** A simulação calcula cada roda de apoio, roda-guia, roda motriz e rolete de retorno, mas não anima o modelo do jeito certo. O `CarProcAnimComponent`, o jeito vanilla de animar rodas, dá à maioria dos ossos das rodas de esteira os sinais de outra roda no 1.8.0.13. O que funciona é um script que lê a simulação roda por roda e move os ossos por conta própria: o `LEO_RunningGearComponent`.

| Item | Situação | Como se sabe |
| --- | --- | --- |
| Rodas de apoio, roda-guia, roda motriz e roletes de retorno na simulação | ✅ Carregam e rodam: 13 peças por lagarta no Leopard | Medido (`WheelCount`, telemetria) |
| Roletes de retorno (`Type "Return Roller"`) | ✅ Carregam, entram no `WheelCount` e dão forma à corrente de elos nativa. Efeito na direção não medido | Medido (`WheelCount`); a parte da corrente é engenharia reversa e aparece na sobreposição da depuração |
| Ler cada roda por script | ✅ Getters indexados por (lagarta, peça) | Medido |
| `CarProcAnimComponent` em rodas de esteira | ❌ A maioria dos ossos recebe o giro e o curso de outra roda | Medido em duas rodadas |
| Ossos das rodas movidos por script (`LEO_RunningGearComponent`) | ✅ Osso a menos de 3 mm da física em 90–95% das amostras | Medido |
| O mesmo script nas máquinas dos outros jogadores (proxies) | ⚠️ Não testado | — |

Todos os valores são do Leopard 2A6 (`Leopard2A6_base.et`, bloco completo em [examples/leopard2a6](../../examples/leopard2a6/VehicleTrackedSimulation_Leopard2A6.et.txt)). Tudo foi testado no Workbench 1.8.0.13. As imagens vêm dos vídeos de teste listados em [Exemplos em vídeo](05-video-examples.md).

### Tipos de peça e modelos

Cada peça de uma lagarta é um `TrackPart` em `Tracks { Track Left { Wheels { ... } } }`. O `Type` dela escolhe um de quatro modelos, que ficam dentro do `Simulation Tracked`. Todo modelo tem `Mass`, `Radius` e `BrakeTorque`. Só o `RoadWheel` tem um bloco `Suspension`.

| `Type` do `TrackPart` | Bloco do modelo | Valor do enum | Leopard: `Mass` / `Radius` / `BrakeTorque` | Notas |
| --- | --- | --- | --- | --- |
| `Road Wheel` (o padrão: não escreva `Type`) | `RoadWheel RoadWheel "{id}"` | 0 | 110 kg / 0,418 m / 6000 N·m | Tem a suspensão e o raio até o chão ([capítulo 13](13-suspension.md)) |
| `Sprocket` | `Sprocket Sprocket "{id}"` | 1 | 250 kg / 0,34 m / 30000 N·m | O raio dela define a velocidade da esteira |
| `Idler` | `Idler Idler "{id}"` | 2 | 150 kg / 0,335 m / 0 | — |
| `Return Roller` | `Roller Roller "{id}"` | 3 | 20 kg / 0,152 m / 0 | Escreva `Type "Return Roller"` com as aspas: o valor em texto é aceito |

Os nomes dos tipos e os valores do enum são engenharia reversa do executável 1.8.0.13. O efeito de `Mass` e `BrakeTorque` em cada tipo de peça não foi isolado.

O modelo do rolete de retorno e uma peça de rolete ficam assim:

```
Roller Roller "{EC9CBBD192126CCB}" {
 Mass 20
 Radius 0.152
 BrakeTorque 0
}
```

```
TrackPart Roller_L01 {
 Type "Return Roller"
 Pivot Pivot "{495CE3DBFC7DD5D5}" {
  Bone "v_roller_l01"
 }
}
```

O motor recusa um modelo com valor errado. As mensagens (textos do executável) são:

- `TrackWheel's mass is incorrect! (must be greater than zero)`
- `TrackWheel's radius is incorrect! (must be greater than zero)`
- `TrackWheel's brake torque is incorrect! (must be greater than or equal to zero)`

**O osso pivô é o centro da peça**, no eixo dela. Não é o ponto de contato. O centro de uma roda de apoio fica `Radius` acima do contato com o chão ([capítulo 13](13-suspension.md)).

**O que cada `Radius` faz:**

| Peça | Efeito do `Radius` | Como se sabe |
| --- | --- | --- |
| Roda de apoio | Altura do casco: centro da roda = ponto de contato + `Radius` ao longo do eixo para cima do casco. O `WheelGetRPM` da roda de apoio é a velocidade da esteira ÷ esse `Radius` | Medido: o sinal de suspensão do motor bateu com (altura do contato + `WheelGetRadius`) com diferença média de 1,25 mm |
| Roda motriz | Velocidade da esteira: velocidade = velocidade angular da roda motriz × `Radius` | Medido: raio efetivo de 0,344 m em 1ª e 0,346 m em 3ª pela telemetria; o Speed ÷ AngVel da sobreposição da depuração dá exatamente 0,3400 |
| Roda-guia, rolete de retorno | Nenhum efeito na direção foi medido | Não testado |
| Todas as peças | O círculo daquela peça na corrente de elos nativa e na sobreposição da depuração | Engenharia reversa (corrente), observado em vídeo (sobreposição) |

### Raio da simulação contra raio visual

**Regra: o `Radius` da simulação é a distância do centro da peça até a face da esteira em contato com o chão** (a face externa dos elos). Não é o aro que você vê. Para uma peça sobre a qual ou em volta da qual a esteira corre, isso é o raio visual do aro mais cerca de uma espessura de elo.

O motivo é a geometria da roda de apoio. A esteira fica entre o aro e o chão, e a simulação não tem uma espessura de esteira própria. O `TrackThickness` existe, mas o motor guarda e nunca lê (engenharia reversa). Então o `Radius` da roda precisa incluir a esteira para o casco ficar na altura do modelo.

| Peça | `Radius` da simulação (m) | Raio visual (m) | Diferença (m) | Até onde o raio da simulação chega |
| --- | --- | --- | --- | --- |
| Roda de apoio | 0,418 | 0,345 (aro) | +0,073 | Face dos elos em contato com o chão, embaixo da roda |
| Roda-guia | 0,335 | 0,264 (aro) | +0,071 | Face externa dos elos que a envolvem (a face interna deles fica a 0,256) |
| Roda motriz | 0,34 | cerca de 0,29–0,32 (dentes, não medido com precisão) | — | Face externa dos elos que a envolvem (a face interna deles fica a 0,262) |
| Rolete de retorno | 0,152 | 0,0847 (aro) | +0,067 | Face externa do trecho de cima acima dele (0,144–0,159 acima do centro do rolete) |

A sapata do elo do 2A6 tem 0,0757–0,076 m de espessura. Os raios dos aros e as faces interna e externa foram medidos no modelo. O raio dos dentes da roda motriz aparece nas nossas notas como 0,29, 0,305 e 0,3189. Nenhum script o usa.

Duas conferências confirmam a regra no 2A6:

- Os ossos das rodas de apoio ficam em y = 0,4186 m. Com `Radius` 0.418, uma roda na altura do modelo toca y = 0,0006, 0,6 mm acima do plano do chão do modelo (y = 0).
- A sobreposição da depuração do veículo desenha cada peça como um círculo do raio da simulação. Em volta das rodas de apoio esses círculos chegam ao fundo dos elos, bem fora dos aros (imagem abaixo).

![Leopard 2A6 parado com a sobreposição da depuração do veículo: círculos brancos no raio da simulação em volta de cada roda de apoio, roda-guia, roda motriz e rolete de retorno](../images/debug-overlay-stationary.jpg)

*Vídeo de teste A (06/10/2026), 109,0 s: tanque parado, `Show vehicle debug` ligado. Os círculos brancos de 12 lados são desenhados no `Radius` da simulação de cada peça. Em volta das rodas de apoio eles chegam à face dos elos em contato com o chão, fora dos aros visíveis. Os círculos pequenos são os roletes de retorno. A linha vermelha com marcas ciano é a corda nativa; ela corre pelo fundo dos elos por script. Modelo do Leopard 2A6 por buh-late, CC BY 4.0, modificado ([créditos](../../CREDITS.md)).*

**Como medir o seu próprio modelo:**

1. Roda de apoio: a altura do centro da roda acima do plano do chão do modelo, com o modelo apoiado nas esteiras. Isso é mais ou menos o raio do aro mais a espessura do elo.
2. Roda-guia e roda motriz: a distância do centro até a face externa dos elos que a envolvem.
3. Rolete de retorno: a distância do centro até a face externa do trecho de cima acima dele.

Um `Radius` de roda de apoio que não bate faz o casco flutuar ou afundar pela diferença ([capítulo 13](13-suspension.md)). Os raios da roda-guia, da roda motriz e dos roletes também definem o caminho da corrente de elos nativa ([capítulo 15](15-track-links.md)).

**Posições de repouso dos ossos no 2A6** (espaço do modelo do casco, metros; x para a direita, y para cima, z para a frente; ossos esquerdos em −x):

| Ossos (esquerdo / direito) | x | y | z |
| --- | --- | --- | --- |
| `v_wheel_l01` / `v_wheel_r01` | ∓1,3288 | 0,4186 | 2,478 |
| `v_wheel_l02` / `r02` | ∓1,3288 | 0,4186 | 1,662 |
| `v_wheel_l03` / `r03` | ∓1,3288 | 0,4186 | 0,889 |
| `v_wheel_l04` / `r04` | ∓1,3288 | 0,4186 | 0,160 |
| `v_wheel_l05` / `r05` | ∓1,3288 | 0,4186 | −0,722 |
| `v_wheel_l06` / `r06` | ∓1,3288 | 0,4186 | −1,596 |
| `v_wheel_l07` / `r07` | ∓1,3288 | 0,4186 | −2,478 |
| `v_idler_l` / `v_idler_r` | ∓1,3298 | 0,840 | 3,128 |
| `v_sprocket_l` / `v_sprocket_r` | ∓1,3307 | 0,824 | −3,297 |
| `v_roller_l01` / `r01` | ∓1,4193 | 1,078 | 2,131 |
| `v_roller_l02` / `r02` | ∓1,1593 | 1,078 | 1,341 |
| `v_roller_l03` / `r03` | ∓1,4193 | 1,078 | −0,400 |
| `v_roller_l04` / `r04` | ∓1,1593 | 1,078 | −2,058 |

Os roletes de retorno ficam desencontrados para os lados no modelo do 2A6. Os ossos `v_suspension_*` e `v_wheel_*_rotator` do rig abaixo têm a mesma posição do osso `v_wheel_*` deles.

### Ordem e índices dos TrackPart

**Liste cada lagarta nesta ordem:** rodas de apoio da frente para trás, depois a roda-guia, depois a roda motriz, depois os roletes de retorno. Use a mesma ordem nos dois lados.

| Índice | Lagarta esquerda | Lagarta direita | `Type` | Modelo | Osso pivô |
| --- | --- | --- | --- | --- | --- |
| 0–6 | `Wheel_L01` … `Wheel_L07` | `Wheel_R01` … `Wheel_R07` | nenhum (Road Wheel) | `RoadWheel` | `v_wheel_l01` … `l07` |
| 7 | `Idler_L` | `Idler_R` | `Idler` | `Idler` | `v_idler_l` |
| 8 | `Sprocket_L` | `Sprocket_R` | `Sprocket` | `Sprocket` | `v_sprocket_l` |
| 9–12 | `Roller_L01` … `Roller_L04` | `Roller_R01` … `Roller_R04` | `"Return Roller"` | `Roller` | `v_roller_l01` … `l04` |

**O que o carregador confere** (engenharia reversa, com os textos de erro do executável):

- Exatamente 2 lagartas: `Invalid number of tracks! Must be %d!` (o número é 2)
- Pelo menos 3 peças por lagarta: `Wheels - configuration is missing! Every track should have at least 3 wheels!`
- A roda motriz e a roda-guia no mesmo índice nas duas lagartas: `Wheels - %s mismatch between tracks`

**Por que as rodas de apoio vêm primeiro.** O motor dá papéis especiais aos índices 0 e 1 (engenharia reversa do executável 1.8.0.13):

- A cada passo da simulação ele toma a variação de ângulo da peça **1** como velocidade angular de referência ω. O ângulo visual de todas as outras peças avança ω·r₁/rⱼ.
- Ele move a corrente de elos nativa com o raio da peça **0**: velocidade da corrente = r₀·ω.

As duas, portanto, precisam ser rodas de apoio. A ordem anterior punha a roda-guia primeiro, na versão do 2A7 e no primeiro prefab do 2A6. Com essa ordem e o primeiro `Radius` da roda-guia do 2A6, de 0.2641, a corrente teria andado a 0,2641 / 0,418 ≈ 63% da velocidade da superfície das rodas de apoio. A versão do 2A7 dirigia normalmente com a roda-guia primeiro, então a ordem importa para a corrente e para os scripts, não para a direção, até onde testamos.

**Índices na API de script.** Todo getter de roda recebe `(track, part)`. A `track` 0 é o primeiro bloco `Track` em `Tracks` (`Track Left` no Leopard). A `part` é a posição do `TrackPart` na lista daquela lagarta, a partir de 0. O `WheelCount()` é o número de peças **por lagarta**: 13 no 2A6. A linha de log `LEOGEAR simulation: 2 tracks, WheelCount 13` confirma, e a engenharia reversa também.

Se você reordenar as peças, todo índice guardado em outro lugar muda junto: os seus scripts, e qualquer coisa que se refira a uma roda pelo número (o [capítulo 6](06-converting.md) fala do `m_iWheelId`).

### Lendo as rodas por script

Pegue a simulação pelo controlador e confira se ela carregou antes de ler qualquer coisa:

```c
TrackedControllerComponent controller = TrackedControllerComponent.Cast(owner.FindComponent(TrackedControllerComponent));
if (!controller)
	return;

VehicleTrackedSimulation simulation = controller.GetSimulation();
if (!simulation || !simulation.IsValid() || simulation.TrackCount() < 2)
	return; // not loaded yet: every wheel getter returns 0, false or a zero vector

if (simulation.WheelCount() < 7) // parts per track: 13 on the 2A6
	return; // fewer parts than the 7 road wheels read below

for (int track = 0; track < 2; track++)
{
	for (int k = 0; k < 7; k++) // the road wheels are parts 0..6
	{
		float rpm = simulation.WheelGetRPM(track, k);
		if (!simulation.WheelHasContact(track, k))
			continue; // the contact position is stale without contact

		vector contact = owner.CoordToLocal(simulation.WheelGetContactPosition(track, k));
		float centreY = contact[1] + simulation.WheelGetRadius(track, k); // wheel centre height, hull space
	}
}
```

**A proteção de prontidão.** Todo getter conferido no executável (`TrackCount`, `WheelCount` e os getters de roda usados neste capítulo) testa primeiro um estado interno de "simulação carregada". Até lá devolve 0, `false` ou o vetor zero. O `TrackCount()` devolve a constante 2 depois que a simulação carrega; ele não lê o prefab. Então `TrackCount() < 2` quer dizer "ainda não carregou" (engenharia reversa). No *Play*, a simulação ficou válida 0,78–1,26 s depois de a entidade do veículo ser criada, bem antes de ligar o motor (medido em duas sessões). Os getters funcionam com o motor desligado.

**Os getters** (`VehicleTrackedSimulation`, 1.8.0.13):

| Método | Devolve | Notas |
| --- | --- | --- |
| `TrackCount()` | 2 depois de carregar, 0 antes | Use como teste de prontidão |
| `WheelCount()` | Peças por lagarta (13) | Por lagarta, não das duas lagartas juntas |
| `WheelHasContact(track, part)` | `bool` | `false` no ar ou capotado |
| `WheelGetContactPosition(track, part)` | Posição no mundo do contato com o chão | Devolve um valor guardado sem conferir o contato (engenharia reversa): teste o `WheelHasContact` antes. Converta com `owner.CoordToLocal` |
| `WheelGetContactNormal` / `WheelGetContactMaterial` / `WheelGetContactEntity` | Normal, `GameMaterial`, entidade | Não usados aqui |
| `WheelGetContactLiquidState` / `...LiquidMaterial` / `...LiquidPosition` | Contato com água | Não usados aqui |
| `WheelGetRPM(track, part)` | Rotações por minuto, com sinal | A velocidade angular interna (rad/s) × 9,549296 (engenharia reversa). Veja [RPM das rodas](#rpm-das-rodas) |
| `WheelGetRadius(track, part)` | O `Radius` do modelo | A documentação vanilla chama de raio "inicial" |
| `WheelGetRollingDrag` / `WheelSetRollingDrag` | Porcentagem de resistência ao rolamento | Não testado |
| `WheelGetPosition(track, part, displacement)` | Centro da roda no referencial da física | Veja abaixo |

**Regras:**

- Pegue a simulação com `TrackedControllerComponent.GetSimulation()`. Um comentário antigo no `LEO_TelemetryComponent.c` diz que os getters travavam quando a simulação vinha do `FindComponent`. Esse travamento foi ligado depois ao `GetInputs()`, então o `FindComponent` provavelmente é seguro, mas não foi testado de novo.
- **Nunca chame `GetInputs(out, out)`.** A ligação dele escreve no endereço 0 e derruba o Workbench. O `GetFloorNormalAngles` tem o mesmo defeito (engenharia reversa; veja [Bugs e limitações](09-bugs.md)).
- **Confira os índices por conta própria.** Só o `WheelGetPosition` confere o índice da peça. Nenhum getter confere o índice da lagarta. Um índice fora da faixa lê memória alheia (engenharia reversa). Mantenha `track < TrackCount()` e `part < WheelCount()`.

**`WheelGetPosition` e o centro de massa.** Ele devolve o centro da roda em relação ao centro de massa da física do veículo, com os eixos do casco. `displacement` 0 é a suspensão toda estendida (curso todo para baixo). O centro de massa da física é o ponto de centro de massa do modelo mais o deslocamento `CenterOfMass` do `RigidBody` ([capítulo 13](13-suspension.md)). Para ter o espaço do casco, some os dois:

```math
\vec p_{\text{hull}} = \text{WheelGetPosition}(t, w, 0) + \vec c_{\text{model COM point}} + \vec c_{\text{CenterOfMass}}
```

No 2A6 essa soma é (0,0002; 2,1678; 0,0791): o empty `COM_hull` em (0,0002; 1,1678; −0,3809) mais `CenterOfMass 0 1 0.46`. Na rodada 1 (`MaxTravelDown` 0.15), o `WheelGetPosition(0, wheel, 0)` devolveu (−1,32898; −1,89918; z − 0,07906) em todas as rodas de apoio esquerdas, sendo z o z do osso. Somar o centro de massa dá y = 0,26858 = 0,41858 − 0,15, o centro no curso todo para baixo, com precisão de 0,01 mm (medido). O valor muda sempre que você muda o `CenterOfMass`, então não o fixe no código. O `LEO_RunningGearComponent` não usa este getter; ele trabalha a partir do ponto de contato. A unidade do `displacement` é desconhecida: só o 0 foi testado.

A mesma rodada mostra que a simulação usa a **pose de bind** dos ossos pivô. O `WheelGetPosition` ficou idêntico nas 131 amostras enquanto o `CarProcAnimComponent` movia os ossos das rodas em até 0,30 m. Animar os ossos não volta para a física (deduzido dessa medição).

### RPM das rodas

Todos os valores abaixo foram medidos com a telemetria `LEOSUSP` e `LEOTEL` e a sobreposição da depuração do veículo ([capítulo 16](16-debugging.md)).

**Sinal.** O `WheelGetRPM` tem o sinal da velocidade do casco para a frente. Isso valeu em 87 de 87 quadros da rodada 1 com todas as rodas esquerdas em contato e mais de 2 km/h. Com 1 km/h de limite e incluindo a perda de contato, valeu em 103 de 106; as exceções tinham perdido o contato ou estavam quase paradas.

**O raio da roda motriz define a velocidade.** Andando reto e constante com a embreagem acoplada, velocidade ÷ velocidade angular da roda motriz (rpm de feedback do motor ÷ (marcha × 4,6)) deu um raio efetivo de 0,344 m em 1ª (mediana, 162 amostras) e 0,346 m em 3ª, contra `Radius` 0.34. O Speed ÷ AngVel por lagarta da sobreposição da depuração dá 0,3400 em todos os quadros lidos. Exemplo a 23 km/h: AngVel 18,413 rad/s, Speed 6,2605 m/s.

**O RPM das rodas de apoio segue a mesma esteira.** RPM da roda de apoio = rpm da roda motriz × 0,34 / 0,418. A razão foi 0,9999 (mediana de 42 quadros da rodada 1, embreagem acoplada, todas as rodas em contato). Todas as peças de uma lagarta compartilham uma única velocidade de esteira, e o RPM da roda de apoio é essa velocidade dividida pelo `Radius` da roda de apoio. Se o `WheelGetRPM` da roda-guia, da roda motriz e dos roletes também é a velocidade da esteira ÷ o próprio `Radius` não foi conferido.

| Conversão (prefab atual) | Valor | Como se sabe |
| --- | --- | --- |
| `WheelGetRPM` da roda de apoio por km/h de velocidade da esteira | 6,346 rpm | Calculado com `Radius` 0.418 |
| Rpm da roda motriz por km/h de velocidade da esteira | 7,802 rpm | Calculado com `Radius` 0.34 |
| Velocidade do casco ÷ velocidade angular da roda de apoio | 0,38–0,42 m andando reto e constante; mediana de 0,398 m em todos os quadros em movimento | Medido. Abaixo de 0,418, o que combina com o deslizamento da esteira: com tração a sobreposição mostrou as esteiras 3–6% mais rápidas que o casco (observado em vídeo) |

**Pequenas diferenças entre as rodas.** As 7 rodas de apoio de uma lagarta nem sempre informam exatamente o mesmo RPM. Na rodada 1 foram idênticas em 90 de 131 quadros. Nos outros mostraram um gradiente suave da frente para trás, abaixo de 2% (por exemplo, de 172,2 a 173,6 rpm). Uma roda sem contato pode informar algo bem diferente: rodas no ar em ré leram −230,6 rpm enquanto as do chão liam de 0 a 20. Esse valor é a velocidade da esteira em ré no `RpmMax`: 3000 / (2,3 × 4,6) × 0,34 / 0,418 = 230,6.

### Por que o CarProcAnimComponent falha em veículos de esteira

O `TrackedVehicle_Base.et` traz um `CarProcAnimComponent "{5D6501223785D009}"` vazio. Em veículos de rodas esse componente leva os sinais de simulação de cada roda para animações procedurais (`.pap`) nos ossos dela. Nós o preenchemos no Leopard de esteira e medimos o que cada osso recebia.

#### O que as animações de roda vanilla esperam

| Arquivo | Vagas de osso | Sinais |
| --- | --- | --- |
| `{CCE4D4889B1BB408}anims/proc/wheel_rotation.pap` | `wheel` | `rotation` → gira o `wheel` em volta do X (graus; faixa do editor −360…360) |
| `{FEFBB47B33FB500F}anims/proc/wheel_suspension.pap` | `wheel`, `suspension`, `rotator` | `rotation` → gira o `wheel` em volta do X; `suspension` → move o `suspension` ao longo do Y; `steering` → gira o `rotator` |

Cada roda recebe uma entrada `ProcAnimParams`. O `BoneNames` preenche as vagas de osso em ordem. Este é o bloco usado na rodada 1 para a primeira roda de apoio:

```
CarProcAnimComponent "{5D6501223785D009}" {
 Parameters {
  ProcAnimParams "{F43F56CC537A1C10}" {
   ResourceName "{FEFBB47B33FB500F}anims/proc/wheel_suspension.pap"
   BoneNames {
    "v_wheel_l01" "v_suspension_l01" "v_wheel_l01_rotator"
   }
  }
  ...
```

A faixa da entrada de suspensão no editor do `.pap` é −1…1, mas é só a faixa da prévia. Na simulação de esteira o sinal de suspensão está em metros ao longo do Y do osso, de −`MaxTravelDown` a +`MaxTravelUp`. Na rodada 1 os ossos se moveram exatamente entre −0,150 e +0,300 (medido). Sem contato com o chão o sinal é exatamente −`MaxTravelDown`. Quando o contato fica acima da compressão total, ele fica em +`MaxTravelUp`.

#### As duas rodadas

| | Rodada 1 (05/10/2026, 22:30) | Rodada 2 (22:55) |
| --- | --- | --- |
| Configuração | `wheel_suspension.pap` nas 14 rodas de apoio, `wheel_rotation.pap` nas rodas-guia, rodas motrizes e roletes | Igual |
| Ordem das 26 entradas | Por lado: L01…L07, roda-guia, roda motriz, roletes, depois o lado direito | Intercalada: L01, R01, L02, R02 … depois rodas-guia, rodas motrizes, roletes |
| Dados | 131 quadros de telemetria comparando cada osso com as rodas simuladas | Telemetria parado e 22 amostras em movimento; vídeo revisto quadro a quadro |
| Curso | Os ossos L01, L03, L05, L07 bateram com as rodas de apoio **esquerdas** 1, 2, 3, 4 (erros médios de 1,7, 1,3, 1,0, 0,9 mm em 106 quadros com contato total). Comparar cada osso com a própria roda deu erro médio de 16,1 mm. L02, L04, L06 ficaram a 15 mm ou mais de todas as rodas esquerdas | Parado, os ossos direitos R02–R07 ficaram +51 a +54 mm acima das rodas simuladas; o R01 ficou a −4 mm da sua |
| Giro | L01 = L03 = L05 = L07 em 123 de 131 quadros; L02 = L04 = L06 em 107 de 131 | Nas 22 amostras em movimento: L02 = L04 = L06 = R01; R07 = `Sprocket_R`; `Sprocket_L` = L01 |
| Visto no jogo | Relatado: rodas não acompanhando a corrente, só a da frente certa, umas subindo e descendo e outras não | Rodas de apoio direitas 2–7 flutuando acima da corrente (imagem abaixo) |

![Lado direito do Leopard 2A6 em lenta: a roda de apoio 1 apoia nos elos, as rodas de apoio 2 a 7 flutuam acima deles](../images/before-wheels-floating.jpg)

*Vídeo de diagnóstico 2 (05/10/2026, 22:56), 9,0 s: motor em lenta na pista, rodas animadas pelo `CarProcAnimComponent` (rodada 2). Só a roda de apoio 1 (à direita, a da frente) apoia nos elos. As rodas 2–7 flutuam acima deles porque os ossos delas leem os sinais de uma roda-guia, de uma roda motriz ou de um rolete, que não têm curso de suspensão. Elas ficam na altura do modelo enquanto as rodas simuladas e os elos nativos ficam mais baixos (esta rodada ainda tinha `MaxTravelDown` 0.15). Telemetria: +51 a +54 mm. Modelo do Leopard 2A6 por buh-late, CC BY 4.0, modificado ([créditos](../../CREDITS.md)).*

A primeira instalação usava só o `wheel_rotation.pap` (giro) nos 26 ossos. A única reclamação então era a falta de movimento vertical. O giro sozinho esconde o problema andando reto, porque todas as rodas de apoio dos dois lados giram quase na mesma taxa. O erro apareceu quando o curso da suspensão foi mapeado (nossa leitura das duas instalações).

#### O modelo que explica as duas rodadas

Uma regra de busca prevê todas as igualdades que observamos. O componente acha a roda pelo nome do osso pivô e a transforma num índice global g = 13 · lagarta + peça. Depois lê os sinais de uma tabela organizada por eixo (esquerda 0, direita 0, esquerda 1, direita 1, …):

```math
g = 13 \cdot \text{track} + \text{part} \quad\Rightarrow\quad \text{reads track } (g \bmod 2),\ \text{part } \lfloor g/2 \rfloor
```

| Peça esquerda | g | Lê os sinais de | Peça direita | g | Lê os sinais de |
| --- | --- | --- | --- | --- | --- |
| L01 | 0 | roda de apoio esquerda 1 (a dela) | R01 | 13 | roda de apoio direita 7 |
| L02 | 1 | roda de apoio direita 1 | R02 | 14 | roda-guia esquerda |
| L03 | 2 | roda de apoio esquerda 2 | R03 | 15 | roda-guia direita |
| L04 | 3 | roda de apoio direita 2 | R04 | 16 | roda motriz esquerda |
| L05 | 4 | roda de apoio esquerda 3 | R05 | 17 | roda motriz direita |
| L06 | 5 | roda de apoio direita 3 | R06 | 18 | rolete esquerdo 1 |
| L07 | 6 | roda de apoio esquerda 4 | R07 | 19 | rolete direito 1 |
| `Idler_L` | 7 | roda de apoio direita 4 | `Idler_R` | 20 | rolete esquerdo 2 |
| `Sprocket_L` | 8 | roda de apoio esquerda 5 | `Sprocket_R` | 21 | rolete direito 2 |
| `Roller_L01` | 9 | roda de apoio direita 5 | `Roller_R01` | 22 | rolete esquerdo 3 |
| `Roller_L02` | 10 | roda de apoio esquerda 6 | `Roller_R02` | 23 | rolete direito 3 |
| `Roller_L03` | 11 | roda de apoio direita 6 | `Roller_R03` | 24 | rolete esquerdo 4 |
| `Roller_L04` | 12 | roda de apoio esquerda 7 | `Roller_R04` | 25 | rolete direito 4 |

Só o `v_wheel_l01` recebe os próprios sinais. As previsões batem com as medições: L01, L03, L05, L07 leem as rodas de apoio esquerdas 1–4. L02, L04, L06 leem rodas de apoio direitas, então giram com a R01. R02–R07 leem peças sem suspensão, então nunca se movem na vertical. R07 e `Sprocket_R` leem dois roletes do mesmo raio, então giram juntos. `Sprocket_L` e L01 leem duas rodas de apoio esquerdas.

**Ressalva.** Esse modelo é deduzido. A rodada 1 não consegue distingui-lo de outra regra: "a entrada *i* lê a vaga intercalada *i*". Com a ordem por lado as duas regras dão o mesmo mapeamento. A rodada 2 deveria ter decidido entre elas, mas a simulação dela ainda usava os dados antigos do prefab: ficava cerca de 5 cm baixa, o comportamento do `MaxTravelDown` 0.15, embora o prefab já dissesse 0.10 (o [capítulo 13](13-suspension.md) explica o cache do mundo aberto). Então a rodada 2 talvez também não tenha usado a ordem intercalada. A ordem intercalada num mundo recém-aberto nunca foi testada.

**Mesmo um mapeamento certo giraria algumas peças na taxa errada.** O motor tira o ângulo visual de cada peça da peça 1, escalado pelos raios da simulação (engenharia reversa). Se esse ângulo é o que o sinal `rotation` carrega, as rodas de apoio girariam a velocidade da esteira ÷ 0,418 em vez de ÷ 0,345 (17% devagar demais para os aros delas), e os roletes a ÷ 0,152 em vez de ÷ 0,0847 (44% devagar demais). A roda-guia e a roda motriz ficariam certas. Isso foi calculado, não visto, porque o mapeamento nunca funcionou.

**Veredito: não use o `CarProcAnimComponent` no trem de rolamento de esteira no 1.8.0.13.** Deixe o `{5D6501223785D009}` vazio, como o `TrackedVehicle_Base.et` o tem. Se você converter um veículo de rodas, apague os `ProcAnimParams` das rodas dele. Mova os ossos por script no lugar.

### A solução por script: LEO_RunningGearComponent

O [`LEO_RunningGearComponent`](../../scripts/LEO_RunningGearComponent.c) lê a simulação por (lagarta, peça) a cada quadro e move os ossos por conta própria. Ele anima o curso e o giro das rodas de apoio e o giro das rodas-guia, rodas motrizes e roletes de retorno. Ele também move a corrente de elos por script ([capítulo 15](15-track-links.md)). Usa só a API vanilla.

**A cada quadro ele:**

1. Pega a simulação e sai até ela carregar (`TrackCount() < 2`).
2. Uma vez, confere se `WheelCount()` ≥ rodas de apoio + roda-guia + roda motriz + roletes, e registra `LEOGEAR`.
3. Calcula a distância que cada lagarta andou neste quadro.
4. Para cada roda de apoio: calcula o curso da suspensão, move o `v_suspension_<s>0<k>` e gira o `v_wheel_<s>0<k>`.
5. Gira a roda-guia, a roda motriz e os roletes de retorno.
6. Move os elos por script, se o `m_bScriptedChain` estiver ligado.

#### Instalação

1. Copie o [`LEO_RunningGearComponent.c`](../../scripts/LEO_RunningGearComponent.c) **e** o [`LEO_TrackChain.c`](../../scripts/LEO_TrackChain.c) para `scripts/Game/Vehicle/` no seu mod. O componente se refere ao `LEO_TrackChain` mesmo com a corrente por script desligada.
2. Recompile os scripts sem mundo aberto (o [capítulo 16](16-debugging.md) explica as armadilhas da recarga).
3. No prefab do casco, deixe o `CarProcAnimComponent` vazio e acrescente o componente:

   ```
   CarProcAnimComponent "{5D6501223785D009}" {
   }
   LEO_RunningGearComponent "{5292A83C2D6FC873}" {
   }
   ```

4. Dê ao casco o rig de ossos abaixo, liste os `TrackPart` na ordem acima e ponha nos atributos os valores do seu modelo.

A lista completa de componentes do casco do 2A6 está em [examples/leopard2a6/HullComponents_Leopard2A6.et.txt](../../examples/leopard2a6/HullComponents_Leopard2A6.et.txt). [Scripts prontos](11-scripts.md) cobre a instalação em geral.

#### Atributos

| Atributo | Padrão (Leopard) | Função |
| --- | --- | --- |
| `m_iRoadWheels` | 7 | Rodas de apoio por lagarta: `TrackPart` 0 … n−1, ossos `v_suspension_<s>0<k>` e `v_wheel_<s>0<k>` |
| `m_iRollers` | 4 | Roletes de retorno por lagarta (as peças depois da roda motriz), ossos `v_roller_<s>0<k>` |
| `m_fRoadWheelRadius` | 0.345 | Raio visual do aro das rodas de apoio, onde elas rolam sobre a face interna da esteira [m] |
| `m_fRollerRadius` | 0.0847 | Raio visual do aro dos roletes de retorno [m] |
| `m_fRestHeight` | 0.4186 | Altura modelada do centro da roda [m], usada se um osso de roda não puder ser lido |
| `m_fMaxTravelDown` | 0.10 | O mesmo valor do `MaxTravelDown` da `Suspension` |
| `m_fMaxTravelUp` | 0.30 | O mesmo valor do `MaxTravelUp` da `Suspension` |
| `m_fDropRate` | 8 | Quão rápido uma roda cai até o curso todo para baixo quando perde contato, e volta quando pousa [1/s] |
| `m_bScriptedChain` | 1 | Move os elos da esteira por script e esconde os elos da corda nativa ([capítulo 15](15-track-links.md)) |
| `m_sLinkModel` | `{B50AEDC702735F50}…/Leopard2A6_track_link.xob` | Modelo do elo da corrente por script |
| `m_iLinks` | 73 | Elos por lagarta |
| `m_sNativeLinkName` | `Leopard2A6_track_link` | Parte do nome do modelo do elo nativo, usada para achar e esconder os elos nativos |
| `m_fMaxDistance` | 300 | Além desta distância da câmera, ossos e elos não são atualizados [m] |

Os raios da roda-guia e da roda motriz não são atributos. O script os lê da simulação (`WheelGetRadius`), porque a esteira envolve essas peças (veja [Giro](#spin)). Mantenha `m_fMaxTravelDown` e `m_fMaxTravelUp` iguais ao bloco `Suspension`, senão o script limita o curso com limites diferentes dos da física.

#### O rig de ossos (Blender)

Cada roda de apoio precisa de um osso que sobe e desce e de um osso que gira. O 2A6 usa a mesma cadeia de ossos do `wheel_suspension.pap` vanilla:

| Osso | Pai | Posição | Movido pelo script | Pele presa a ele |
| --- | --- | --- | --- | --- |
| `v_body` | (raiz) | — | Não | Casco; braços da suspensão com peso 1 − t |
| `v_suspension_l01` | `v_body` | Centro da roda | Move-se em Y pelo curso | Braço da suspensão com peso t |
| `v_wheel_l01_rotator` | `v_suspension_l01` | Centro da roda | Não (vaga de esterçamento do `.pap` vanilla) | Nada |
| `v_wheel_l01` | `v_wheel_l01_rotator` | Centro da roda | Gira em volta do X. Também é o osso `Pivot` da simulação | Malha da roda, 100% |
| `v_idler_l`, `v_sprocket_l`, `v_roller_l01` … `l04` | `v_body` | Centro da peça | Giram em volta do X | Malha da peça, 100% |

O mesmo para o lado direito (`_r`). Todos esses ossos têm orientação de repouso identidade, então o espaço do osso tem os eixos do casco: Y é para cima e X é o eixo da roda. No Blender, aponte cada osso para a frente (+Y) com roll 0, como no 2A6. As matrizes do script supõem essa orientação. O script não usa o osso rotator; você pode deixá-lo de fora se nunca usar o `CarProcAnimComponent`.

**Passos** (o rig do 2A6 foi acrescentado por um script do Blender, `add_suspension.py`, que não faz parte deste repositório):

1. Para cada roda de apoio, acrescente `v_suspension_<s>0<k>` e `v_wheel_<s>0<k>_rotator`. Copie head, tail e roll do `v_wheel_<s>0<k>`, para que a pose de repouso não mude.
2. Ligue os pais: osso de suspensão sob o `v_body`, rotator sob o osso de suspensão, e o osso de roda existente sob o rotator.
3. Mantenha a malha da roda com peso 100% no `v_wheel_<s>0<k>`. Ela herda o curso do osso de suspensão.
4. Dê novos pesos a cada braço da suspensão para que ele siga a roda. Ache o pivô do braço no casco: o grupo de vértices mais longe do cubo (a menos de 8 cm do mais distante). Projete cada vértice no eixo pivô → cubo para ter o t, limitado a 0…1. Dê peso t no `v_suspension_<s>0<k>` e 1 − t no `v_body`. No 2A6 os braços têm 0,87 m; na roda da frente o pivô fica 0,61 m para dentro, 0,51 m à frente e 0,36 m acima do cubo. Essa mistura linear cisalha o braço em vez de girá-lo em volta do pivô: move a ponta do cubo com a roda e mantém a ponta do pivô fixa.
5. Confira que nenhum vértice se moveu e que só os pesos dos braços mudaram. Depois ponha alguns ossos de suspensão para cima e para baixo e renderize uma imagem de teste. O script do 2A6 moveu rodas alternadas em +0,15 e −0,12 m e mediu o fundo das rodas.
6. Exporte como sempre (Add Leaf Bones desligado, Only Deform Bones desligado) e reimporte o `.xob` com *Export Skinning* e *Export Scene Hierarchy* ligados ([capítulo 3](03-model-requirements.md)). Feche o World Editor antes: trocar o modelo do casco com um mundo aberto congelou o Workbench uma vez.

No 2A6 isso acrescenta 28 ossos (14 de suspensão e 14 rotators); o esqueleto do casco tem 56.

#### Movendo ossos com SetBoneMatrix

O `Animation.SetBoneMatrix(owner, bone, mat)` recebe um **delta local sobre a pose de bind**: a matriz identidade deixa o osso em repouso. O `SCR_DestructionTireComponent` vanilla o usa do mesmo jeito (identidade mais um pequeno deslocamento por osso de pneu). O script escreve todos os ossos de novo a cada quadro.

```c
vector mat[4];
Math3D.MatrixIdentity4(mat);
mat[3] = Vector(0, travel, 0);                  // suspension bone: up by the travel
anim.SetBoneMatrix(owner, suspensionBone, mat);
```

As posições de repouso são lidas uma vez no `OnPostInit` com `anim.GetBoneMatrix`, antes que o componente mova qualquer coisa.

#### Curso da suspensão

Com contato com o chão o osso segue a simulação exatamente:

```math
\text{travel} = \operatorname{clamp}\left(y_{\text{contact}} + R_{\text{sim}} - y_{\text{rest}},\ -\text{MaxTravelDown},\ +\text{MaxTravelUp}\right)
```

Aqui *y*<sub>contact</sub> é a altura do `WheelGetContactPosition` no espaço do casco (`owner.CoordToLocal`), *R*<sub>sim</sub> é o `WheelGetRadius` e *y*<sub>rest</sub> é a altura do osso da roda na pose de bind (0,4186). É a mesma geometria da simulação: centro da roda = contato + `Radius` ao longo do eixo para cima do casco ([capítulo 13](13-suspension.md)).

**Por que limitar.** O curso calculado pelo contato pode sair da faixa, porque o raio começa 0,5 m acima da compressão total. Ele chegou a +0,382 m (sessão das 00:12), +0,367 m (rodada 1) e +0,348 m (sobre minas na rodada 4), contra o limite de +0,30. Também passou alguns milímetros de −`MaxTravelDown` (−0,107 m com 0.10). O sinal do próprio motor fica dentro dos limites (medido).

**Perda de contato.** Sem contato não há ponto de contato, e a simulação pendura a roda no curso todo para baixo. Pular direto para lá pareceria um estalo. Então o script leva o osso suavemente para −`MaxTravelDown`. Quando a roda pousa ele volta suavemente para a altura do contato, até a diferença ficar abaixo de 5 mm; daí em diante segue a simulação exatamente de novo:

```math
\text{travel} \leftarrow \text{travel} + (\text{target} - \text{travel}) \cdot \left(1 - e^{-k\,\Delta t}\right), \qquad k = 8\ \text{s}^{-1}
```

Aqui *k* é o `m_fDropRate`. O fator não depende da taxa de quadros. Com *k* = 8 o osso cobre 63% da diferença em 0,125 s e 95% em 0,37 s (calculado).

<a id="spin"></a>

#### Giro: peças que rolam e peças que são envolvidas

O script primeiro calcula quanto a esteira andou neste quadro, em relação ao casco. Ele usa a velocidade da esteira na face em contato com o chão, lida da roda de apoio 0:

```math
d = \text{WheelGetRPM}(t, 0) \cdot \frac{2\pi}{60} \cdot \text{WheelGetRadius}(t, 0) \cdot \Delta t
```

*d* positivo quer dizer que o trecho de cima anda para a frente. Depois cada peça gira *d* dividido pelo raio em que ela encontra a esteira:

- **As rodas de apoio e os roletes de retorno rolam** sobre a face interna da esteira. Eles giram *d* ÷ o **raio visual do aro** deles (0,345 e 0,0847).
- **A roda-guia e a roda motriz são envolvidas** pela esteira. Elas giram com os elos em volta delas, no raio do caminho da corrente: o **raio da simulação** delas (0,335 e 0,34). Os elos por script correm nesse caminho ([capítulo 15](15-track-links.md)), então os dentes da roda motriz e os elos ficam no mesmo passo.

```math
\Delta\theta_{\text{road wheel, roller}} = \frac{d}{r_{\text{rim}}} \qquad\qquad \Delta\theta_{\text{idler, sprocket}} = \frac{d}{R_{\text{sim}}}
```

| Peça | Raio usado | rpm por km/h de velocidade da esteira |
| --- | --- | --- |
| Roda de apoio (`WheelGetRPM`, simulação) | 0,418 | 6,35 |
| Roda de apoio (script, rolando no aro) | 0,345 | 7,69 |
| Roda-guia (script, envolvida) | 0,335 | 7,92 |
| Roda motriz (script, envolvida; igual à simulação) | 0,34 | 7,80 |
| Rolete de retorno (script, rolando no aro) | 0,0847 | 31,3 |

As taxas são calculadas como 1 / (3,6 · r) · 60 / 2π. As rodas de apoio visíveis giram 21% mais rápido que o RPM da roda de apoio na simulação, porque os aros delas são menores que o raio da simulação.

**Sinal e matriz.** Um ângulo positivo move o topo da peça para a frente (+Z). É assim que toda peça gira quando a esteira anda para a frente, rolando ou envolvida. O script guarda um ângulo por tipo de peça e lagarta e escreve esta matriz:

```c
float c = Math.Cos(angle);
float s = Math.Sin(angle);
mat[0] = Vector(1, 0, 0);
mat[1] = Vector(0, c, s);
mat[2] = Vector(0, -s, c);
mat[3] = vector.Zero;
anim.SetBoneMatrix(owner, wheelBone, mat);     // rotation about the axle (X)
```

![Lado direito do Leopard 2A6 na grama: os elos por script envolvem a roda motriz traseira e passam por baixo das rodas de apoio e por cima dos roletes de retorno](../images/sprocket-wrap.gif)

*Vídeo de teste A (06/10/2026), 208,8–211,0 s · [foto em tamanho cheio](../images/sprocket-wrap.jpg) (209,5 s): lado direito na grama a cerca de 8 km/h em 1ª. Rodas e elos são movidos pelo `LEO_RunningGearComponent` e pelo `LEO_TrackChain`. As rodas de apoio e os roletes rolam sobre a face interna dos elos; a roda motriz gira com os elos que a envolvem. Só os marcadores de suspensão da sobreposição da depuração estão ligados. Modelo do Leopard 2A6 por buh-late, CC BY 4.0, modificado ([créditos](../../CREDITS.md)).*

#### Distância, servidores e multiplayer

- **Distância.** Além do `m_fMaxDistance` (300 m) da câmera atual (`GetWorld().GetCurrentCamera`), o script pula a escrita dos ossos e dos elos. Ele continua atualizando o curso e os ângulos, para que nada pule quando a câmera chega mais perto. O esconder dos elos nativos continua rodando.
- **Servidor dedicado.** O componente sai no `OnPostInit` quando `System.IsConsoleApp()` é verdadeiro, então não faz nada num servidor dedicado.
- **Multiplayer.** O script é só visual e não precisa de replicação: cada máquina lê a própria simulação e move ossos locais. Se os getters das rodas são válidos nos proxies (veículos dirigidos por outro jogador) e se o script os anima lá **não foi testado**.

#### Mensagens de log

| Mensagem | Significado |
| --- | --- |
| `LEOGEAR simulation: 2 tracks, WheelCount 13` | Normal: registrada uma vez quando a simulação carrega |
| `LEOGEAR the simulation has N wheels, expected ...` | O `WheelCount()` é menor que `m_iRoadWheels` + 2 + `m_iRollers`. O componente para |
| `LEOGEAR N running gear bones missing or unreadable: the scripted chain is off` | Um nome de osso não existe, ou um osso de roda lê fora de 0,5–2 × `m_fRestHeight`. Os ossos que existem continuam se movendo; a corrente por script é desligada |
| `LEOGEAR no TrackedControllerComponent or no skeleton: running gear not animated` | O casco não tem controlador de esteira ou não tem esqueleto |
| `LEOGEAR link model not found: ...` / `LEOGEAR could not spawn all track links` | Problemas da corrente por script ([capítulo 15](15-track-links.md)) |

#### Limites

- **Os nomes dos ossos são fixos:** `v_suspension_<s>0<k>`, `v_wheel_<s>0<k>`, `v_idler_<s>`, `v_sprocket_<s>`, `v_roller_<s>0<k>`, com `<s>` = `l` ou `r`. O índice é escrito como `0` + k, então mais de 9 rodas de apoio ou roletes por lado pedem uma pequena mudança no código.
- **A ordem das peças é fixa:** rodas de apoio 0 … n−1, roda-guia n, roda motriz n+1, depois os roletes.
- **A roda-guia precisa estar na frente e a roda motriz atrás.** A corrente por script monta a esteira nessa ordem. Para um casco com tração dianteira, troque os círculos da roda motriz e da roda-guia no `CreateChains` (não testado).
- **A velocidade da esteira vem da roda de apoio 0.** Quando essa roda está no ar o RPM dela pode diferir do resto da lagarta (−230,6 rpm em ré no exemplo acima). As rodas e os elos podem então girar na taxa errada até ela pousar. Uma roda no chão ou a roda motriz pode ser uma fonte melhor; isso não foi testado.
- **O custo por quadro** não foi medido.

#### Resultados

| Sessão | Versão do script | Osso − física (`diff_mm` do `LEOSUSP`) |
| --- | --- | --- |
| Rodada 3, 05/10/2026 23:18 | Primeira versão, suavização a cada quadro | 70% das amostras dentro de 3 mm; transientes até 83 mm em obstáculos |
| 05/10/2026 23:58 e 06/10/2026 00:12 | Depois da revisão abaixo | Mediana 0; 95% e 90% das amostras dentro de 3 mm. Valores maiores (até 124 mm) só enquanto uma roda volta depois de perder contato |

As 7 rodas de apoio de uma lagarta mostram o mesmo giro, por construção. Os resultados também foram conferidos a olho nas duas sessões e no vídeo de teste A, na pista, na grama e nas pedras. Leia o `LEOSUSP` e as outras linhas da telemetria como descrito no [capítulo 16](16-debugging.md).

Os poucos milímetros que sobram costumam mudar de forma constante da frente para trás. Nas sessões das 23:58 e 00:12, 83 das 112 amostras com contato total e mais de 1 mm de diferença fizeram isso (medido). Uma pequena diferença na inclinação do casco entre o momento em que o script lê os contatos e o momento em que a telemetria os lê daria esse padrão; isso não está provado.

**Revisão.** Uma revisão adversarial do componente olhou três áreas: a API, a matemática e a execução. Os achados e o que aconteceu com eles:

| Achado | Situação |
| --- | --- |
| A suavização a cada quadro fazia as rodas atrasarem em relação à física | Corrigido: suavização só na perda de contato e no pouso |
| A roda-guia e a roda motriz giravam 17–27% rápido demais com os raios visuais (0,264, cerca de 0,29) | Corrigido: elas giram no raio da simulação |
| Os getters não devolvem nada útil antes de a simulação carregar | Corrigido: proteção `TrackCount() < 2` |
| Um osso de roda que não pode ser lido estraga a altura de repouso | Corrigido: reserva `m_fRestHeight` |
| Índices fora da faixa leem memória alheia | Corrigido: `WheelCount()` conferido uma vez e registrado |
| O `EOnFrame` pode rodar antes da pose final do casco no quadro, então os ossos poderiam atrasar um quadro. A corrente de elos nativa posiciona os elos dela no `EOnPostFrame` (engenharia reversa) | Em aberto, pouca confiança |
| Proxies no multiplayer | Em aberto, não testado |

### Itens em aberto

- **Atraso de um quadro.** O `EOnPostFrame` eliminaria os poucos milímetros que sobram enquanto o casco inclina?
- **Fonte da velocidade da esteira.** Que RPM deveria mover as rodas e os elos quando a roda de apoio 0 está no ar: outra roda no chão, a roda motriz, ou a média das rodas no chão?
- **RPM das outras peças.** O `WheelGetRPM` da roda-guia, da roda motriz e dos roletes é igual à velocidade da esteira ÷ o próprio `Radius`?
- **Dependência da corrente nativa.** Uma nota de pesquisa diz que o sinal de rotação nativo fica em 0 até a corrente de elos existir. Se o RPM das rodas ou a tração dependem da corrente nativa nunca foi testado tirando-a. O 2A6 a mantém rodando, escondida ([capítulo 15](15-track-links.md)).
- **`CarProcAnimComponent` com entradas intercaladas** num mundo recém-aberto: não testado.
- **`displacement` do `WheelGetPosition`:** só o 0 foi testado; a unidade é desconhecida.
- **Proxies:** os getters das rodas são válidos nos veículos dos outros jogadores, e o script os anima?
- **Roda-guia e roletes de retorno:** o efeito deles na direção não foi medido.
- **Raio dos dentes da roda motriz:** não medido com precisão (0,29–0,32 m nas nossas notas). Nenhum script o usa.

---

[← Suspensão e altura do casco](13-suspension.md) · [Índice](../../README.pt-BR.md) · [Elos da esteira: a corda nativa e a corrente por script →](15-track-links.md)
