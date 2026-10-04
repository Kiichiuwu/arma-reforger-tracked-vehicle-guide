> 🇧🇷 Português · [🇬🇧 English](../en/01-overview.md)

[← Sobre este guia](00-about.md) · [Índice](../../README.pt-BR.md) · [Arquitetura →](02-architecture.md)

# Visão geral

O Arma Reforger 1.8.0.13 (engine 192142) já traz uma simulação de esteira de verdade, a `VehicleTrackedSimulation`, e ela funciona para andar para a frente, fazer curva e girar parado — mas tem um bug que impede a ré, contornável por script. Nenhum veículo vanilla usa esse sistema ainda: ele existe só como um prefab base escondido, `Prefabs/Vehicles/Core/TrackedVehicle_Base.et`.

Este guia foi escrito a partir do mod Leopard 2A7. Tudo aqui foi testado no jogo com telemetria ou recuperado por engenharia reversa do executável do Workbench e conferido por uma segunda análise independente.

|  | Esteira nativa (`VehicleTrackedSimulation`) | Método antigo dos mods (7 eixos de rodas) |
| --- | --- | --- |
| Física | Duas esteiras, cada uma com roda motriz, tensora e rodas de apoio | Um carro com 7 eixos; eixos da frente e de trás esterçam ao contrário |
| Giro parado (pivot) | Sim, nativo | Não; só aproximação |
| Ré | Bug na 1.8.0.13: precisa de script ([seção de bugs](09-bugs.md)) | Funciona |
| Animação da esteira | Corrente de elos nativa (opcional) ou script | Sempre script |
| Quem usa | Ninguém no vanilla; nenhum dos 243 mods instalados testados | M113, WCS M1A1/M2A2/BMP-3, CIE T-72, Zagoria T-80U |
| Documentação oficial | Nenhuma | Herda de veículos de rodas vanilla |

Resumo prático: dá para fazer um tanque com esteira nativa só com o jogo base, sem dependências. É preciso configurar o bloco de simulação à mão (o template vanilla não basta), definir duas curvas de atrito obrigatórias, ajustar o controlador e adicionar um script de ré.

---

[← Sobre este guia](00-about.md) · [Índice](../../README.pt-BR.md) · [Arquitetura →](02-architecture.md)
