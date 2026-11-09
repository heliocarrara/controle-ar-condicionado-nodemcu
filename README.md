# Casa IR — controle de aparelhos por infravermelho

Este projeto usa um NodeMCU (ESP8266) como emissor infravermelho e disponibiliza uma interface web e uma API REST na rede local. O app está sendo preparado para o ar-condicionado Bravolt TECH12BROIN/BRAVOLTIND12K e para uma TV Samsung não smart.

## 🚀 Funcionalidades

- **Energia:** Ligar e Desligar o ar condicionado.
- **Ajuste de Temperatura:** Definir a temperatura alvo de 16°C a 31°C em passos de **1°C**.
- **Presets rápidos:** Botões de atalho "24°C Média", "23°C Média", "22°C Baixa" na tela inicial.
- **Modos de Operação:** Frio, Quente, Secar, Ventilar e Automático.
- **Velocidade do Ventilador:** Automático, Silencioso, Baixo, Médio e Alto (Máximo).
- **Direcionamento do Vento (Swing):** Oscilação vertical automática ou fixação em 5 posições distintas; controle de oscilação horizontal (esquerda/direita).
- **Funções Especiais:** Modo Eco (Modo Econômico), Turbo (Resfriamento Rápido), Visor LED (liga/desliga visor de temperatura do aparelho), Quiet (Silencioso) e Filtro Health.
- **Temporizadores (Timers):** Programar o desligamento ou ligamento em passos de 20 minutos (até 12 horas).
- **Sensor ambiente:** Leitura de temperatura e umidade pelo DHT22 e gráfico de temperatura dos últimos 20 minutos; não controla automaticamente o ar.
- **TV Samsung:** Controle por infravermelho para energia, volume, canais, navegação e números, além de temporizador local para desligar em intervalos ajustáveis de 30 minutos.
- **Ar-condicionado Bravolt 12.000 BTU:** Controle pelo protocolo TCL112AC em estado completo, compatível com o mapeamento atual da biblioteca.
- **Monitor de RAM:** Percentual de uso da memória com barra visual na tela inicial e log no serial a cada 60s.
- **Reinicialização via Web:** Botão "Reiniciar NodeMCU" na aba Ajustes.
- **Autenticação simples** baseada em chave de API para endpoints REST.

---

## 🖥️ Arquitetura do Frontend & Automação de Build (Transparente)

Para manter o desenvolvimento limpo e fácil de atualizar, a interface Web foi separada do arquivo de código-fonte C++ (`src/main.cpp`). O fluxo é estruturado da seguinte forma:

```
[src/web/index.html]   ──┐
[src/web/style.css]    ──┼─► [pre_build_web.py] ─► [src/webpage.h] ─► [Compilação C++]
[src/web/app.js]       ──┘  (Automação do PlatformIO)   (Flash PROGMEM)
```

1. **Arquivos Separados:** O design está dividido em arquivos limpos de frontend dentro da pasta [src/web/](file:///c:/Users/helio/source/repos/nodemcu/NodeMCU-ESP8266/src/web/):
   * [index.html](file:///c:/Users/helio/source/repos/nodemcu/NodeMCU-ESP8266/src/web/index.html) (HTML5 Semântico com emoji unicode).
   * [style.css](file:///c:/Users/helio/source/repos/nodemcu/NodeMCU-ESP8266/src/web/style.css) (Estilização responsiva, fonte do sistema, **sem CDN**).
   * [app.js](file:///c:/Users/helio/source/repos/nodemcu/NodeMCU-ESP8266/src/web/app.js) (Sincronização assíncrona, gráfico canvas nativo, **sem Chart.js**).
2. **Script de Pré-compilação:** No arquivo [platformio.ini](file:///c:/Users/helio/source/repos/nodemcu/NodeMCU-ESP8266/platformio.ini), configuramos um script extra em Python: [pre_build_web.py](file:///c:/Users/helio/source/repos/nodemcu/NodeMCU-ESP8266/pre_build_web.py).
3. **Fusão Automática:** Toda vez que você compilar ou fizer o upload do projeto (`Build` ou `Upload` no VSCode), o PlatformIO executa esse script silenciosamente nos bastidores. O script lê o HTML, CSS e JS, junta tudo em um único código embutido inline (inlining) e gera automaticamente o arquivo [webpage.h](file:///c:/Users/helio/source/repos/nodemcu/NodeMCU-ESP8266/src/webpage.h) como uma constante literal em C++ armazenada na Flash (`PROGMEM`).

> [!TIP]
> **Por que essa abordagem?**
> * **Para o Desenvolvedor:** Facilidade total para alterar a interface usando realce de sintaxe adequado para HTML, CSS e JavaScript sem poluir o `main.cpp`.
> * **Para o ESP8266:** Ao carregar a página no celular ou PC, o navegador faz apenas **1 requisição HTTP** para o NodeMCU, reduzindo o tempo de carregamento e eliminando falhas de conexão TCP (comum quando o ESP8266 tenta carregar múltiplos arquivos estáticos consecutivamente).
> * **Para o Upload:** Não é necessário carregar uma imagem LittleFS/SPIFFS separadamente. Todo o processo continua sendo feito pelo botão padrão de Upload do PlatformIO.

---

## 🛠️ Tecnologias e Ferramentas

- **Hardware:** NodeMCU (ESP8266) + LED Emissor IR + Sensor DHT22.
- **Framework:** Arduino / PlatformIO.
- **Bibliotecas:** `IRremoteESP8266` (TCL112AC para o Bravolt e Samsung IR para a TV), `DHT sensor library`, `ArduinoJson`.
- **Frontend:** HTML5, CSS3 Custom Properties, Vanilla JavaScript (Fetch API + ES7). **Zero dependências externas** — sem CDN, sem Google Fonts, sem bibliotecas de ícones ou gráficos.

---

## 📖 Documentação da API

Para detalhes sobre os endpoints, métodos e parâmetros suportados para integração com Home Assistant ou outras ferramentas, consulte o arquivo [API_DOCS.md](API_DOCS.md).

---

<img width="949" height="983" alt="image" src="https://github.com/user-attachments/assets/d769ce25-ecd9-4460-800a-54d66298d9f2" />
<img width="967" height="983" alt="image" src="https://github.com/user-attachments/assets/e8e1422c-7197-44fb-a012-42f8ddba1e7f" />


## ⚙️ Como Usar

1. Clone o repositório.
2. Abra o projeto no VSCode com a extensão **PlatformIO** instalada.
3. Configure as credenciais Wi-Fi e a `API_KEY` em `include/secrets.h` (arquivo local ignorado pelo Git).
4. *(Opcional)* Se quiser alterar o visual ou lógica da interface, edite os arquivos correspondentes na pasta [src/web/](file:///c:/Users/helio/source/repos/nodemcu/NodeMCU-ESP8266/src/web/).
5. Conecte o NodeMCU na porta USB e clique no botão **Upload** do PlatformIO. O script de compilação web rodará de forma totalmente transparente antes de compilar o C++.
6. Monte o circuito do LED infravermelho no pino de transmissão (`PINO_TX = 5`, que equivale ao pino D1 do NodeMCU).
7. Encontre o IP do seu NodeMCU (via Monitor Serial) ou acesse via mDNS no navegador de qualquer dispositivo na mesma rede: `http://meuar.local`.

---

## 📄 Licença

Sinta-se livre para usar e modificar este projeto.
