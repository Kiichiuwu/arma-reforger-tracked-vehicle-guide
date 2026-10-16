> 🇧🇷 Português · [🇬🇧 English](../en/03-model-requirements.md)

[← Arquitetura](02-architecture.md) · [Índice](../../README.pt-BR.md) · [Passo a passo do zero →](04-from-scratch.md)

# Requisitos do modelo 3D

Para andar, a simulação só precisa de **um osso por peça do trem de rolamento**, com o nome que você usa no prefab. Para mostrar suspensão em movimento, rodas girando e elos de esteira, o modelo precisa de um pouco mais: um osso de suspensão por roda de apoio, um osso por rolete de retorno, malhas estáticas das esteiras com nome e um elo de esteira como modelo separado. Este capítulo lista tudo isso, com os valores do Leopard 2A6.

Todas as posições abaixo estão no espaço do casco como a Enfusion vê: x para a direita, y para cima, z para a frente, plano do chão em y = 0. "∓" quer dizer negativo do lado esquerdo e positivo do lado direito.

### O que o osso de uma peça significa para a simulação

Cada `TrackPart` do prefab aponta para um osso pelo seu `Pivot`. Esse osso é o **centro da peça na posição do modelo (de repouso)**. Ele não é o ponto de contato com o chão.

Para uma roda de apoio, a simulação faz assim:

| Passo | O que acontece | Como se sabe |
| --- | --- | --- |
| 1 | Um raio começa `RayStartOffsetUp` acima do topo do curso da suspensão e corre ao longo do eixo para baixo do casco | Início: descrição do atributo no executável 1.8.0.13, "Extension of raycast start above top position of suspension". Direção: deduzida do passo 2 |
| 2 | O centro da roda fica no ponto de contato mais `Radius`, ao longo do eixo para cima do casco | Medido no 2A6: essa regra bateu com o sinal de suspensão do próprio motor com diferença média de cerca de 1 mm (0,9–1,7 mm por roda, 106 frames) |
| 3 | O curso é a distância desse centro até o osso, de −`MaxTravelDown` a +`MaxTravelUp` | Medido no 2A6 |

Então **`Radius` é a distância do centro da roda até a face de contato da esteira**, não até o aro de borracha. Fica perto do raio do aro mais a espessura do elo. Se a face de contato da esteira estiver no plano do chão do seu modelo, `Radius` é igual à altura do osso da roda de apoio (2A6: osso a 0,4186 m, `Radius` 0.418). [Suspensão e altura do casco](13-suspension.md) explica a mola e a altura do casco.

A roda-guia, a roda motriz e os roletes de retorno não têm suspensão; ficam onde estão os ossos deles. O `Radius` deles segue a mesma ideia: o raio da face de contato dos elos em volta daquela peça. Esses círculos também são o caminho da corrente de elos, nativa ou por script ([Elos da esteira](15-track-links.md)).

![Lado direito do Leopard 2A6 parado com a sobreposição de depuração nativa: círculos brancos em volta de cada osso de roda chegam à face de contato dos elos](../images/debug-overlay-both-tracks.jpg)

*Vídeo de teste A (06/10/2026), 0:30, tanque parado na pista. A sobreposição de depuração do veículo desenha cada `TrackPart` como um círculo branco do seu `Radius` da simulação em volta do osso. Os círculos das rodas de apoio chegam à face de contato dos elos, abaixo dos aros visíveis; os círculos pequenos no ramo de cima são os roletes de retorno. As linhas com marcas são as cordas das correntes nativas das duas lagartas. Modelo "Leopard 2 A6" de buh-late, CC BY 4.0, modificado ([créditos](../../CREDITS.md)).*

### Ossos e raios das peças (Leopard 2A6)

| Peça | Ossos (esquerda / direita) | Posição do osso (m) | `Radius` da simulação (m) | Raio visual (m) |
| --- | --- | --- | --- | --- |
| Rodas de apoio | `v_wheel_l01`…`l07` / `v_wheel_r01`…`r07` | x ∓1,3288, y 0,4186, z 2,478 / 1,662 / 0,889 / 0,160 / −0,722 / −1,596 / −2,478 (da frente para trás) | 0.418 | aro 0,345 |
| Roda-guia (frente) | `v_idler_l` / `v_idler_r` | x ∓1,3298, y 0,840, z 3,128 | 0.335 | 0,264 |
| Roda motriz (trás) | `v_sprocket_l` / `v_sprocket_r` | x ∓1,3307, y 0,824, z −3,297 | 0.34 | cerca de 0,29–0,32 nos dentes (não medido com precisão; nenhum script usa) |
| Roletes de retorno | `v_roller_l01`…`l04` / `v_roller_r01`…`r04` | y 1,078, z 2,131 / 1,341 / −0,400 / −2,058; x ∓1,4193 (roletes 01 e 03), ∓1,1593 (02 e 04) | 0.152 | 0,0847 |

Como os raios da simulação foram encontrados (medido no modelo): a face interna dos elos fica a 0,256 m do centro da roda-guia e a 0,262 m do centro da roda motriz, e um elo tem 0,076 m de espessura. 0,256 + 0,076 ≈ 0,335 e 0,262 + 0,076 ≈ 0,34. Sobre os roletes de retorno, a face externa do ramo de cima fica 0,144–0,159 m acima do centro dos roletes, daí 0,152.

Observações:

- **O `Radius` da roda motriz define a velocidade do veículo.** No 2A6, velocidade = velocidade angular da roda motriz × 0,34 m (medido: mediana de 0,344 m em 162 amostras em 1ª marcha; a velocidade da corrente na sobreposição de depuração dividida pela velocidade angular dela dá 0,3400). Veja [Ajuste com dados reais](10-tuning.md).
- **Os roletes de retorno são opcionais.** O 2A6 tem 4 por lado, declarados com `Type "Return Roller"` e um modelo `Roller`. Testado: eles carregam, contam em `WheelCount()` (13 peças por lagarta no 2A6) e os círculos deles sustentam o ramo de cima da corrente. O efeito deles na direção não foi medido.
- **Os ossos das rodas são o que os scripts leem.** O `LEO_RunningGearComponent` tira a posição de repouso de cada peça do osso dela e monta a corrente por script em volta desses círculos ([capítulo 14](14-running-gear.md)).

### Regras que a engine confere ao carregar

| Regra | Erro no log |
| --- | --- |
| Exatamente 2 lagartas | `Invalid number of tracks! Must be 2!` |
| Pelo menos 3 entradas `TrackPart` por lagarta | `Wheels - configuration is missing! Every track should have at least 3 wheels!` |
| A roda-guia e a roda motriz na mesma posição da lista nos dois lados | `Wheels - ... mismatch between tracks` |
| `Mass` > 0, `Radius` > 0 e `BrakeTorque` ≥ 0 em todo modelo de roda | `TrackWheel's mass is incorrect! (must be greater than zero)`, e o mesmo para o raio e o torque de freio |

As mensagens são textos do executável 1.8.0.13 (engenharia reversa). A simulação guarda uma só contagem de peças para as duas lagartas, então use a mesma lista nos dois lados (engenharia reversa). A ordem das peças na lista também importa ([passo 5 do passo a passo](04-from-scratch.md#5-declare-as-lagartas)).

### Orientação e escala

Modelo em metros (Blender → Enfusion), com a frente do veículo para **+Y**, Z para cima e o chão em z = 0. O exportador FBX padrão do Blender (frente −Z, cima Y) leva o +Y do Blender para o +Z da Enfusion (frente) e o +Z do Blender para o +Y da Enfusion (cima). O eixo +X continua sendo o lado direito. Então o sufixo `_l` = lado −X e `_r` = lado +X.

### Ossos no Blender

**Uma armature, com um osso raiz que não se chama `Scene_Root`.** O importador do `.xob` já cria um nó raiz com esse nome, e um osso com o mesmo nome colide com ele. Num submodelo pequeno do 2A6, esse osso junto com a falta de um colisor deu `Provided entity is not valid for procedural animation`; os dois foram corrigidos ao mesmo tempo. O osso raiz do casco do 2A6 é o `v_body`; todos os ossos de peças ficam pendurados nele.

**Osso raiz da torre.** Se a torre é um modelo separado e o prefab dela herda de uma torre vanilla do LAV25, a `VehicleAnimationComponent` herdada (`LAV25.agr` / `LAV25_vehicle.asi`) anima um osso chamado `v_body`. No 2A6, um osso raiz da torre com esse nome foi parar 0,5455 m à frente do lugar dele no jogo. Dê outro nome ao osso raiz do modelo da torre (2A6: `v_turret_root`). O casco continua com `v_body`.

**Orientação de repouso.** No Blender, aponte todos os ossos de peças para a frente (+Y) com roll 0. Assim os ossos do 2A6 têm orientação de repouso identidade na Enfusion: os eixos locais de um osso são os eixos do casco (X direita, Y cima, Z frente). Os scripts dependem disso. Eles movem um osso de suspensão ao longo do próprio Y e giram uma roda em volta do próprio X.

**Pesos.** Nada de pesos misturados em peças que giram:

| Malha | Peso |
| --- | --- |
| Cada roda de apoio, roda-guia, roda motriz e rolete de retorno | 100% no próprio osso |
| Braços da suspensão | Misturado entre `v_body` e o osso de suspensão da roda (veja abaixo) |
| Casco, esteiras estáticas e tudo o que não se mexe | 100% `v_body` |

### Rig de suspensão das rodas de apoio

A simulação move cada roda de apoio para cima e para baixo. O braço da suspensão precisa acompanhar esse curso, mas não o giro da roda, então curso e giro precisam de ossos separados. O 2A6 dá a cada roda de apoio esta cadeia de ossos:

```text
v_body
└─ v_suspension_l01        movido ao longo de Y pelo curso da suspensão
   └─ v_wheel_l01_rotator  não movido (posição de esterçamento do layout vanilla de três ossos)
      └─ v_wheel_l01       girado em volta de X; o Pivot do TrackPart aponta para cá
```

- Os três ossos copiam a cabeça, a cauda e o roll do osso da roda, então ficam no centro da roda e a pose de repouso não muda.
- O `Pivot` do prefab continua no `v_wheel_<s>0<k>`. Os ossos de suspensão são só para a animação.
- A malha da roda mantém 100% de peso no `v_wheel_<s>0<k>` e herda o curso dos pais.
- **Os braços da suspensão acompanham a roda.** Cada braço tem peso linear de 0 no pivô do casco (a ponta do braço mais longe do cubo) a 1 no cubo, entre `v_body` e `v_suspension_<s>0<k>`. Nenhum vértice se move.
- A roda-guia, a roda motriz e cada rolete de retorno mantêm um osso de giro cada.
- O osso `_rotator` vem do layout de três ossos do `wheel_suspension.pap` vanilla (roda, suspensão, rotator). O `LEO_RunningGearComponent` nunca mexe nele. Um rig sem ele não foi testado.

O rig do 2A6 foi acrescentado ao modelo pronto por um script do Blender que só adicionou ossos (cada osso de roda passou a ser filho da sua nova cadeia) e mudou os pesos dos braços; ele conferiu que nenhum vértice se moveu.

### Nomes de ossos que os scripts esperam

O `LEO_RunningGearComponent` ([capítulo 14](14-running-gear.md)) procura os ossos pelo nome, com *s* = `l` ou `r` e *k* = 1…9:

| Osso | Usado para |
| --- | --- |
| `v_suspension_<s>0<k>` | Curso da roda de apoio (translação ao longo de Y) |
| `v_wheel_<s>0<k>` | Giro da roda de apoio; a altura de repouso dele é o zero do curso |
| `v_idler_<s>`, `v_sprocket_<s>` | Giro |
| `v_roller_<s>0<k>` | Giro |

- O `0` é literal. Mais de 9 rodas de apoio ou roletes por lado exigem mudar o script.
- Um osso que falta, ou um osso de roda de apoio com altura fora de 0,5–2 × `m_fRestHeight` (padrão 0.4186), conta como faltando. O script então registra `LEOGEAR ... running gear bones missing or unreadable` e desliga a corrente por script. Defina `m_fRestHeight` com a altura do centro das suas rodas de apoio.
- O `LEO_TelemetryComponent` lê `v_wheel_<s>01`…`07` e `v_sprocket_<s>` para a linha `LEOSUSP`. O 7 é fixo nesse script.

Se os seus ossos usam outro padrão (por exemplo `v_wheel_L01`), renomeie no modelo ou mude os textos de nome nos scripts.

### Ponto do centro de massa

O modelo do casco do 2A6 tem um objeto vazio `COM_hull`, colocado no centroide sólido do colisor principal do casco, `UCX_hull_01`: casco (0,0002, 1,168, −0,381). A engine usa esse ponto. O atributo `CenterOfMass` do `RigidBody` é um **deslocamento** a partir desse ponto, não uma posição. O executável o descreve como "Offset of center of mass. Relative to one defined in model (if applicable)." Medido no 2A6: o centro de massa da física é `COM_hull` mais o deslocamento, com diferença de até 0,01 mm.

Em aberto: qual ponto a engine usa quando o modelo não tem esse objeto. [Suspensão e altura do casco](13-suspension.md#centro-de-massa) mostra como posicionar e nivelar o centro de massa.

### Modelo do elo da esteira

A corrente de elos nativa e a corrente por script criam uma entidade por elo com uma malha que você fornece ([Elos da esteira](15-track-links.md)). Faça um elo como modelo próprio:

| Requisito | Por quê | Leopard 2A6 |
| --- | --- | --- |
| Um `.fbx` → `.xob` separado com um único elo | Cada entidade de elo usa essa malha | `Leopard2A6_track_link.xob` |
| Modelado como um elo do **ramo de baixo**: origem no centro do elo (centro do par de sapatas, meio do comprimento), face de contato em y = 0, +Y para cima em direção às rodas, +Z para a frente ao longo da esteira, X atravessado | A corrente nativa coloca um elo do ramo de baixo com rotação identidade (engenharia reversa). A corrente por script põe a origem no caminho dela, na face de contato | Origem no centro do par de sapatas, face de contato em 0 |
| No Blender | A mesma regra com o exportador padrão | Face de contato em z = 0, +Z para cima, +Y para a frente |
| Sem colisores | Elos não têm física: os nativos são entidades simples, os por script não são rastreáveis | `.xob.meta` sem colisores |
| O material da esteira | — | `Leopard2A6_track.emat` |
| LODs, se quiser | — | LOD0 118 vértices / 180 faces, LOD1 47 / 60 |
| Meça o passo e a espessura | O passo define `TrackLength` e o número de elos; a espessura é mais ou menos a diferença entre o raio do aro e o da simulação | Passo 0,2101 m (média dos 71 elos do modelo), espessura 0,076 m |
| Um nome de arquivo reconhecível | Os scripts acham as entidades de elo por parte do nome do modelo | `Leopard2A6_track_link` |

**Como o elo do 2A6 foi feito.** A esteira do modelo tem 71 elos idênticos por lado, cada um com 6 ilhas de malha (2 sapatas, 2 conectores das pontas, uma guia central, um chifre). Um script do Blender, rodando numa cópia do arquivo, recortou um elo do ramo de baixo sob a roda de apoio 3, levou a origem para o centro do par de sapatas com a face de contato em 0, pegou as ilhas correspondentes do LOD1 e exportou os dois LODs. Todo elo da esteira original mapeia o atlas UV inteiro, então rolar a textura não consegue imitar o movimento da esteira neste modelo (medido).

**Mantenha as esteiras estáticas como malhas com nome.** Deixe as esteiras originais do modelo no casco, com peso 100% no `v_body` e com nomes que um script ache (2A6: `track_l` e `track_r`, LOD0–3). O `LEO_TrackBeltComponent` as esconde pelo nome quando existem pelo menos 2 entidades de elo. Se nenhum elo aparecer em 5 s, ele as mantém e avisa, para uma corrente quebrada nunca deixar o tanque sem esteira.

**Em aberto: sentido do elo na corrente por script.** A corrente por script vira cada elo 180° em volta do próprio Y em relação à corrente nativa, então frente e trás ficam trocadas. O elo do 2A6 não é simétrico de frente para trás (vai de −0,097 a +0,186 m ao longo do comprimento), então a esteira por script pode mostrá-lo invertido. Isso não foi conferido de perto ([capítulo 15](15-track-links.md)).

### Colisores

Malhas `UCX_*` (convexas), com a propriedade personalizada `usage = "Vehicle"`. O material físico de cada colisor é definido no `.xob.meta`. Os materiais de blindagem vanilla são `Common/Materials/Game/Armor/armor_{1..100}mm.gamemat`. O padrão que o importador atribui (`{536BF67B2052B869}material/metal.gamemat`) não existe e causa erro.

Os colisores físicos do casco do 2A6 (`LayerPreset "Vehicle"`) são `UCX_hull_01`, `UCX_skirt_l` e `UCX_skirt_r`. As rodas de apoio não têm colisores: a simulação acha o chão com os raios dela.

### Importação no Workbench

Resource Browser → botão direito → *Register and import* → *as Model*. Depois, no painel *Import Settings* do `.xob`:

| Opção | Valor | Por quê |
| --- | --- | --- |
| Merge Meshes | desligado | Senão casco, rodas e esteiras viram uma única malha |
| Export Skinning | ligado | Sem isso, os ossos não entram no `.xob` |
| Export Scene Hierarchy | ligado | Mantém a hierarquia de ossos |

No `.meta`, isso aparece como `MergeMeshes 0`, `ExportSkinning 1` e `ExportSceneHierarchy 1`.

Depois de importar, abra o `.xob` e confira a lista de ossos. O casco do 2A6 ganhou 28 ossos com o rig de suspensão (14 `v_suspension_*` e 14 ossos `_rotator`).

**Feche o mundo antes de substituir o `.fbx` do casco.** O Workbench reimporta um `.fbx` sozinho quando o arquivo muda. Com o mundo aberto no World Editor, essa recarga a quente do casco do 2A6 travou o Workbench uma vez (05/10/2026); sem mundo aberto, a reimportação levou 6,5 s.

---

[← Arquitetura](02-architecture.md) · [Índice](../../README.pt-BR.md) · [Passo a passo do zero →](04-from-scratch.md)
