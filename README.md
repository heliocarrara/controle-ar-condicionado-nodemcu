# Controle de Ar Condicionado NodeMCU (ESP8266)

Este projeto permite controlar um ar condicionado da marca TCL utilizando um microcontrolador NodeMCU (ESP8266) como emissor infravermelho (IR), disponibilizando uma API REST em rede local para enviar os comandos.

## 🚀 Funcionalidades

- Ligar e Desligar o ar condicionado via API.
- Ajustar a temperatura alvo (16°C a 31°C).
- Alterar o modo de operação (Frio, Quente, Ventilar, Secar, Automático).
- Ajustar a velocidade do ventilador (Auto, Baixo, Médio, Alto).
- Autenticação simples baseada em chave de API.

## 🛠️ Tecnologias e Ferramentas

- **Hardware:** NodeMCU (ESP8266) + LED Emissor IR.
- **Framework:** Arduino / PlatformIO.
- **Protocolo:** HTTP (REST API).
- **Formatos:** JSON.

## 📖 Documentação da API

Para detalhes sobre os endpoints, métodos e parâmetros suportados, consulte o arquivo [API_DOCS.md](API_DOCS.md).

<img width="949" height="983" alt="image" src="https://github.com/user-attachments/assets/d769ce25-ecd9-4460-800a-54d66298d9f2" />
<img width="967" height="983" alt="image" src="https://github.com/user-attachments/assets/e8e1422c-7197-44fb-a012-42f8ddba1e7f" />


## ⚙️ Como Usar

1. Clone o repositório.
2. Abra o projeto em uma IDE compatível (como VSCode + PlatformIO).
3. Configure as credenciais da sua rede Wi-Fi e a `API_KEY` desejada no código fonte (ex: `src/main.cpp`).
4. Faça o upload do código para a sua placa NodeMCU.
5. Monte o circuito do LED infravermelho no pino correspondente.
6. Encontre o IP do seu NodeMCU (via Monitor Serial) ou acesse via mDNS (ex: `http://meuar.local`).
7. Faça requisições HTTP para controlar seu ar condicionado.

## 📄 Licença

Sinta-se livre para usar e modificar este projeto.
