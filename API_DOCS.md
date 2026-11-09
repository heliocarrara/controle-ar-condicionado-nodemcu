# Documentação da API - Casa IR

Esta API permite consultar o sensor ambiente e enviar comandos para o NodeMCU (ESP8266), que atua como emissor infravermelho na rede local. O ar-condicionado Bravolt usa o protocolo TCL112AC de estado completo.

## URL Base
Se o mDNS estiver funcionando corretamente na sua rede:
`http://meuar.local`

Ou pelo IP diretamente (consulte o Monitor Serial para descobrir o IP atribuído):
`http://192.168.x.x`

---

## Autenticação
Todos os endpoints da API exigem uma chave de segurança passada via *query parameter* `key`.

*   A chave `API_KEY` é configurada em `include/secrets.h`.

---

## Formato de Resposta (JSON)
Todas as respostas da API são devolvidas no formato JSON com a seguinte estrutura:

```json
{
  "status": "success", // ou "error"
  "message": "Mensagem descritiva da ação",
  "device_state": "Power: On, Mode: Cool, Temp: 24C, Fan: Auto"
}
```

---

## Endpoints

### 1. Ligar / Desligar
Controla a energia do ar condicionado. Ao enviar a requisição, o sinal IR é disparado imediatamente.

*   **Endpoint:** `/api/power`
*   **Método:** `GET`
*   **Parâmetros:**
    *   `key`: Chave da API (obrigatório)
    *   `status`: `on` ou `off` (obrigatório)

**Exemplo de uso:**
```bash
curl "http://meuar.local/api/power?key=senha123&status=on"
```

---

### 2. Ajustar Temperatura
Ajusta a temperatura alvo do ar condicionado. O valor permitido é entre 16 e 31 graus.

*   **Endpoint:** `/api/temp`
*   **Método:** `GET`
*   **Parâmetros:**
    *   `key`: Chave da API (obrigatório)
    *   `v`: Valor da temperatura em inteiros, ex: `24` (obrigatório)

**Exemplo de uso:**
```bash
curl "http://meuar.local/api/temp?key=senha123&v=22"
```

---

### 3. Alterar Modo de Operação
Define o modo de funcionamento (Frio, Quente, Ventilar, Secar, Automático).

*   **Endpoint:** `/api/mode`
*   **Método:** `GET`
*   **Parâmetros:**
    *   `key`: Chave da API (obrigatório)
    *   `v`: Modo desejado (obrigatório). Valores aceitos:
        *   `cool` (Frio)
        *   `heat` (Quente)
        *   `fan` (Ventilar)
        *   `dry` (Desumidificar)
        *   `auto` (Automático)

**Exemplo de uso:**
```bash
curl "http://meuar.local/api/mode?key=senha123&v=cool"
```

---

### 4. Ajustar Velocidade do Ventilador (Fan)
Altera a velocidade do vento do ar condicionado.

*   **Endpoint:** `/api/fan`
*   **Método:** `GET`
*   **Parâmetros:**
    *   `key`: Chave da API (obrigatório)
    *   `v`: Velocidade desejada (obrigatório). Valores aceitos:
        *   `auto` (Automático)
        *   `low` (Baixo)
        *   `med` (Médio)
        *   `high` (Alto)

**Exemplo de uso:**
```bash
curl "http://meuar.local/api/fan?key=senha123&v=auto"
```

---

### 5. Consultar Status Atual
Retorna o estado virtual atual armazenado no NodeMCU. *Nota: Este é o estado que o NodeMCU "acredita" que o ar está, baseado nos últimos comandos enviados. Se o ar for controlado pelo controle remoto físico, este status ficará desatualizado.*

*   **Endpoint:** `/api/status`
*   **Método:** `GET`
*   **Parâmetros:**
    *   `key`: Chave da API (obrigatório)

**Exemplo de uso:**
```bash
curl "http://meuar.local/api/status?key=senha123"
```

---

## Interface Web de Teste
A API possui uma interface HTML básica embutida para facilitar testes manuais pelo navegador de um celular ou PC.

*   **Acesso:** `http://meuar.local/`

Nesta interface, os botões utilizam JavaScript (`fetch()`) para realizar as chamadas para os endpoints descritos acima, apresentando as respostas em caixas de alerta.

## Comandos da TV Samsung

A interface envia comandos IR Samsung de 32 bits pela rota local `/cmd?c=...`: `TV_POWER`, `TV_VOLUME_UP`, `TV_VOLUME_DOWN`, `TV_MUTE`, `TV_CHANNEL_UP`, `TV_CHANNEL_DOWN`, `TV_CHANNEL_LIST`, `TV_UP`, `TV_DOWN`, `TV_LEFT`, `TV_RIGHT`, `TV_ENTER`, `TV_RETURN`, `TV_EXIT`, `TV_TOOLS`, `TV_MENU`, `TV_SOURCE` e `TV_0` até `TV_9`. O temporizador local usa `TV_TIMER_SET_<minutos>` (1 a 720) e `TV_TIMER_CANCEL`.

O controle LE-7270 e a TV precisam ser conferidos juntos; como o infravermelho não recebe confirmação da TV, o status exibido não representa o estado real do aparelho.
