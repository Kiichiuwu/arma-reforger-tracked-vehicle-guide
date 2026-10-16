# Credits / Créditos

## 3D model / Modelo 3D

| | |
| --- | --- |
| Title / Título | Leopard 2 A6 |
| Author / Autor | buh-late (display name / nome exibido: "buh") — <https://sketchfab.com/buh-late> |
| Source / Fonte | <https://sketchfab.com/3d-models/leopard-2-a6-7cb23d5322df4b409a880de635826067> |
| Published / Publicado | 2024-01-17 |
| License / Licença | Creative Commons Attribution 4.0 International (CC BY 4.0) — <https://creativecommons.org/licenses/by/4.0/> |
| Modified version / Versão modificada | [⬇ Leopard2A6_Enfusion_model_v1.zip](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/releases/download/leopard2a6-model-v1/Leopard2A6_Enfusion_model_v1.zip) (FBX + textures / texturas), CC BY 4.0 |
| Used by / Usado por | Workshop mod / mod no Workshop [Leopard 2A6 - Tracked Base](https://reforger.armaplatform.com/workshop/7E4ABBD41FEDAC42), GUID `7E4ABBD41FEDAC42` |

**EN** — The Leopard 2A6 used in the example mod, the screenshots and the GIFs is a modified version
of this model. Changes made for Enfusion:

- split into parts and re-parented (new hierarchy);
- bones added for the road wheels, idler, sprocket and return rollers, plus a suspension rig
  (`v_suspension_l01` → `v_wheel_l01_rotator` → `v_wheel_l01`, the same for every road wheel) with
  the suspension arms separated from the wheels and re-weighted so they swing with them;
- the track belt's modifiers applied (a static belt of 71 links per side);
- colliders added;
- LODs added;
- textures repacked into Enfusion's BCR/NMO maps (normal map green channel flipped, roughness
  derived from the original maps);
- one track link cut out of the hull's track mesh as a separate model, for the moving link chain.

Credit: "Leopard 2 A6" by buh-late, licensed under CC BY 4.0. Changes were made; the author does not
endorse this project. CC BY 4.0 asks for credit, a link to the license and a note of the changes; it
has no ShareAlike clause.

**PT** — O Leopard 2A6 usado no mod de exemplo, nas fotos e nos GIFs é uma versão modificada deste
modelo. Alterações feitas para a Enfusion:

- separado em peças e com hierarquia nova;
- ossos adicionados para as rodas de apoio, a roda-guia, a roda motriz e os roletes de retorno, mais
  um rig de suspensão (`v_suspension_l01` → `v_wheel_l01_rotator` → `v_wheel_l01`, igual para cada
  roda de apoio) com os braços da suspensão separados das rodas e reponderados para acompanharem
  as rodas;
- modificadores da esteira aplicados (uma esteira fixa de 71 elos por lado);
- colisores adicionados;
- LODs adicionados;
- texturas reempacotadas nos mapas BCR/NMO da Enfusion (canal verde do mapa de normais invertido,
  rugosidade derivada dos mapas originais);
- um elo da esteira recortado da malha da esteira do casco, como modelo separado, para a corrente de
  elos animada.

Crédito: "Leopard 2 A6" de buh-late, licenciado sob CC BY 4.0. Houve modificações; o autor não
endossa este projeto. A CC BY 4.0 pede crédito, link para a licença e indicação das alterações; ela
não tem cláusula de compartilhamento pela mesma licença (ShareAlike).

## Previous example model / Modelo do exemplo anterior

**EN** — The first edition of this guide used a Leopard 2A7 built from
["Leopard 2 A7"](https://sketchfab.com/3d-models/leopard-2-a7-b29d1cb2e65e4fa8a6e99f88e11b013b) by
[king_st0ne](https://sketchfab.com/kingstonlee96), published 2023-05-25 and licensed under
[CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/). That model was split into parts,
re-parented, rescaled and rigged with bones and colliders for Enfusion. Its images and prefab block
are no longer in the current files. They stay in the git history under the tag
[`v1-leopard2a7`](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/tree/v1-leopard2a7),
where they remain under CC BY-SA 4.0 with credit to king_st0ne. Changes were made; the original
author does not endorse this project.

**PT** — A primeira edição deste guia usou um Leopard 2A7 feito a partir de
["Leopard 2 A7"](https://sketchfab.com/3d-models/leopard-2-a7-b29d1cb2e65e4fa8a6e99f88e11b013b) de
[king_st0ne](https://sketchfab.com/kingstonlee96), publicado em 25/05/2023 e licenciado sob
[CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/). Esse modelo foi separado em peças,
ganhou hierarquia nova, foi reescalado e recebeu ossos e colisores para a Enfusion. As imagens e o
bloco de prefab dele não estão mais nos arquivos atuais. Eles continuam no histórico do git, na tag
[`v1-leopard2a7`](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/tree/v1-leopard2a7),
onde seguem sob CC BY-SA 4.0, com crédito a king_st0ne. Houve modificações; o autor original não
endossa este projeto.

## Game / Jogo

Arma Reforger and Enfusion are trademarks of Bohemia Interactive a.s. Screenshots and GIFs were
taken in the Enfusion Workbench with the base game. This project is not affiliated with Bohemia
Interactive.

Arma Reforger e Enfusion são marcas da Bohemia Interactive a.s. As capturas e os GIFs foram feitos
no Enfusion Workbench com o jogo base. Este projeto não tem vínculo com a Bohemia Interactive.

## Guide and scripts / Guia e scripts

Kiichi — research, documentation, Enforce Script components, `tools/trackpositions.py` / pesquisa,
documentação, componentes em Enforce Script, `tools/trackpositions.py`.
