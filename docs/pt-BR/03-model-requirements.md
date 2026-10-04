> 🇧🇷 Português · [🇬🇧 English](../en/03-model-requirements.md)

[← Arquitetura](02-architecture.md) · [Índice](../../README.pt-BR.md) · [Passo a passo do zero →](04-from-scratch.md)

# Requisitos do modelo 3D

A simulação só precisa de **um osso por roda**, com o nome que você referenciar no prefab. O osso é o ponto de contato com o chão: a engine lança um raio para baixo a partir dele e usa o raio (`Radius`) da roda.

**Ossos obrigatórios por esteira** (os nomes são livres; estes são os do Leopard):

| Peça | Osso no Leopard | Posição | Observação |
| --- | --- | --- | --- |
| Roda tensora (idler) | `v_idler_l` / `v_idler_r` | Centro da roda dianteira | Precisa existir nas duas esteiras |
| Rodas de apoio | `v_wheel_l01`…`l07` / `v_wheel_r01`…`r07` | Centro de cada roda | Mínimo de 3 rodas por esteira, contando todas as peças |
| Roda motriz (sprocket) | `v_sprocket_l` / `v_sprocket_r` | Centro da roda traseira | É o raio dela que define a velocidade |
| Rolete de retorno | (opcional) | — | Tipo `Return Roller`; o Leopard não usa |

Regras que a engine verifica ao carregar:

- **Exatamente 2 esteiras.** Erro: `Invalid number of tracks! Must be 2!`
- **Pelo menos 3 rodas por esteira.** Erro: `Every track should have at least 3 wheels!`
- **A mesma quantidade de cada tipo nos dois lados.** Erro: `Wheels - ... mismatch between tracks`

**Orientação e escala** (Blender → Enfusion): modele em metros, frente do veículo para **+Y**, Z para cima e chão em z = 0. O exportador FBX padrão do Blender (forward -Z, up Y) leva o +Y do Blender para o +Z (frente) da Enfusion, e o +X continua sendo o lado direito. Portanto, sufixo `_l` = lado -X e `_r` = lado +X.

**Ossos no Blender:** um armature com um osso raiz que **não** se chame `Scene_Root` (o `.xob` já cria um nó raiz com esse nome; no Leopard ele se chama `v_body`). Cada roda, roda motriz e idler fica com 100% de peso no seu osso, sem misturar pesos.

**Colisores:** malhas `UCX_*` (convexas), com a propriedade personalizada `usage = "Vehicle"`. O material físico de cada colisor é definido no `.xob.meta`. Os materiais de blindagem vanilla são `Common/Materials/Game/Armor/armor_{1..100}mm.gamemat`; o padrão que o importador coloca (`{536BF67B2052B869}material/metal.gamemat`) não existe e gera erro.

**Importação no Workbench** (Resource Browser → botão direito → *Register and import* → *as Model*). Depois, no painel *Import Settings* do `.xob`:

| Opção | Valor | Por quê |
| --- | --- | --- |
| Merge Meshes | desligado | Senão casco, rodas e esteiras viram uma malha só |
| Export Skinning | ligado | Sem isso os ossos não entram no `.xob` |
| Export Scene Hierarchy | ligado | Mantém a hierarquia de ossos |

No `.meta` isso aparece como `MergeMeshes 0`, `ExportSkinning 1` e `ExportSceneHierarchy 1`.


---

[← Arquitetura](02-architecture.md) · [Índice](../../README.pt-BR.md) · [Passo a passo do zero →](04-from-scratch.md)
