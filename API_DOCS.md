# Documentação da API - Controle Ar Condicionado TCL

Esta API permite o controle remoto de um Ar Condicionado da marca TCL utilizando um NodeMCU (ESP8266) como emissor infravermelho.

## URL Base
Se o mDNS estiver funcionando corretamente na sua rede:
`http://meuar.local`

Ou pelo IP diretamente (consulte o Monitor Serial para descobrir o IP atribuído):
`http://192.168.x.x`

---

## Autenticação
Todos os endpoints da API exigem uma chave de segurança passada via *query parameter* `key`.

*   **Chave Padrão:** `senha123` (Pode ser alterada na variável `API_KEY` do arquivo `main.cpp`).

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
