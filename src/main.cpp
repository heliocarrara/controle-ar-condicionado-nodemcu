#include <Arduino.h> 
#include <ESP8266WiFi.h> 
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h> 
#include <ESP8266mDNS.h> 
#include <ESP8266HTTPClient.h>
#include <DHT.h> 
#include <IRremoteESP8266.h> 
#include <ir_Tcl.h> 
#include <ArduinoJson.h> 
#include "webpage.h" 
#include "secrets.h"
 
// --- CONFIGURAÇÕES --- 
 
#define DHTPIN 14    // Pino D5 
#define DHTTYPE DHT22 
#define PINO_TX 5    // Pino D1 
#define LED_PLACA 2 
#define TOTAL_RAM 81920 
 
// --- INSTÂNCIAS --- 
DHT dht(DHTPIN, DHTTYPE); 
IRTcl112Ac ac(PINO_TX); 
ESP8266WebServer server(80); 
 
float tempReal = 0; 
float umidadeReal = 0; 

// Histórico para o gráfico (últimos 20 minutos)
float historicoTemp[20];
int historicoIndex = 0;
unsigned long ultimoTempoHistorico = 0;
bool historicoCheio = false;

// Variáveis para o Termostato Automático
bool autoThermostat = false;
bool isAcOn = false;
unsigned long ultimoComandoAuto = 0;
float tempAlvo = 24.0;       // Temperatura que queremos atingir
float tolerancia = 1.0;      // Quantos graus acima do alvo para religar (Histerese)
String estadoAuto = "Aguardando"; // Progresso atual da automação
bool forcarEnvioAuto = false; // Força a reavaliação imediata e envio do IR

// Variáveis para o WhatsApp Bot
bool whatsappEnabled = false;
String whatsappServerUrl = "";
String ultimoStatusReg = "WhatsApp desativado";
String ultimoStatusPing = "WhatsApp desativado";

 
// --- LÓGICA DO SENSOR --- 
void lerSensor() { 
    static unsigned long ultimaLeitura = 0; 
    if (millis() - ultimaLeitura > 2000) { // DHT22 precisa de 2s 
        float t = dht.readTemperature(); 
        float h = dht.readHumidity(); 
        if (!isnan(t) && !isnan(h)) { 
            tempReal = t; 
            umidadeReal = h; 
            Serial.println("[DHT22] Temp: " + String(t, 1) + "C | Umidade: " + String(h, 0) + "%");
        } else {
            Serial.println("[DHT22] Erro: Falha ao ler do sensor! Verifique os fios e o pino.");
        }
        ultimaLeitura = millis(); 
    } 

    // Salva o historico a cada 1 minuto (60000 ms)
    if (millis() - ultimoTempoHistorico > 60000 || ultimoTempoHistorico == 0) {
        if (tempReal > 0) {
            historicoTemp[historicoIndex] = tempReal;
            historicoIndex++;
            if (historicoIndex >= 20) {
                historicoIndex = 0;
                historicoCheio = true;
            }
        }
        ultimoTempoHistorico = millis();
    }
} 
 
// --- PÁGINA WEB (INTERFACE) --- 
void handleRoot() { 
    server.send_P(200, "text/html", webpage_html); 
} 
 
// --- HELPERS ---
String getDeviceStateString() {
    String powerStr = ac.getPower() ? "On" : "Off";
    
    String modeStr = "Unknown";
    uint8_t m = ac.getMode();
    if (m == kTcl112AcCool) modeStr = "Cool";
    else if (m == kTcl112AcHeat) modeStr = "Heat";
    else if (m == kTcl112AcDry) modeStr = "Dry";
    else if (m == kTcl112AcFan) modeStr = "Fan";
    else if (m == kTcl112AcAuto) modeStr = "Auto";

    String fanStr = "Unknown";
    uint8_t f = ac.getFan();
    if (f == kTcl112AcFanAuto) fanStr = "Auto";
    else if (f == kTcl112AcFanMin) fanStr = "Min";
    else if (f == kTcl112AcFanLow) fanStr = "Low";
    else if (f == kTcl112AcFanMed) fanStr = "Med";
    else if (f == kTcl112AcFanHigh) fanStr = "High";

    return "Power: " + powerStr + ", Mode: " + modeStr + ", Temp: " + String((int)ac.getTemp()) + "C, Fan: " + fanStr;
}

bool validarApiKey() {
    if (!server.hasArg("key") || server.arg("key") != API_KEY) {
        JsonDocument doc;
        doc["status"] = "error";
        doc["message"] = "Chave de API invalida";
        String response;
        serializeJson(doc, response);
        server.send(401, "application/json", response);
        return false;
    }
    return true;
}

void registrarDispositivo() {
    if (WiFi.status() != WL_CONNECTED) {
        ultimoStatusReg = "Falha: WiFi desconectado";
        Serial.println("[BOT] Nao registrado: WiFi nao conectado.");
        return;
    }
    
    WiFiClient clientPlain;
    WiFiClientSecure clientSecure;
    HTTPClient http;
    String url = whatsappServerUrl + "/api/whatsapp/devices/register";
    
    Serial.println("[BOT] Tentando registrar em: " + url);
    
    bool isHttps = url.startsWith("https://");
    bool beginSuccess = false;
    
    if (isHttps) {
        clientSecure.setInsecure();
        clientSecure.setBufferSizes(1024, 1024); // Reduz uso de RAM do SSL de 32KB para 2KB
        beginSuccess = http.begin(clientSecure, url);
    } else {
        beginSuccess = http.begin(clientPlain, url);
    }
    
    if (beginSuccess) {
        http.setTimeout(10000);
        http.addHeader("Content-Type", "application/json");
        
        JsonDocument doc;
        doc["deviceId"] = "ar_sala";
        doc["name"] = "Ar Condicionado Sala";
        doc["localIp"] = "http://" + WiFi.localIP().toString();
        doc["apiKey"] = API_KEY;
        
        String requestBody;
        serializeJson(doc, requestBody);
        
        int httpResponseCode = http.POST(requestBody);
        if (httpResponseCode > 0) {
            ultimoStatusReg = "Sucesso (" + String(httpResponseCode) + ")";
            Serial.printf("[BOT] Registro concluido. Codigo HTTP: %d\n", httpResponseCode);
            String payload = http.getString();
            Serial.println("[BOT] Resposta: " + payload);
        } else {
            ultimoStatusReg = "Erro: " + http.errorToString(httpResponseCode);
            Serial.printf("[BOT] Falha no registro. Erro: %s\n", http.errorToString(httpResponseCode).c_str());
        }
        http.end();
    } else {
        ultimoStatusReg = "Erro ao conectar";
        Serial.println("[BOT] Erro ao iniciar conexao HTTP para registro.");
    }
}

void enviarPing() {
    if (WiFi.status() != WL_CONNECTED) {
        ultimoStatusPing = "Falha: WiFi desconectado";
        Serial.println("[BOT] Ping nao enviado: WiFi nao conectado.");
        return;
    }
    
    WiFiClient clientPlain;
    WiFiClientSecure clientSecure;
    HTTPClient http;
    String url = whatsappServerUrl + "/api/whatsapp/devices/ping";
    
    Serial.println("[BOT] Enviando ping para: " + url);
    
    bool isHttps = url.startsWith("https://");
    bool beginSuccess = false;
    
    if (isHttps) {
        clientSecure.setInsecure();
        clientSecure.setBufferSizes(1024, 1024); // Reduz uso de RAM do SSL de 32KB para 2KB
        beginSuccess = http.begin(clientSecure, url);
    } else {
        beginSuccess = http.begin(clientPlain, url);
    }
    
    if (beginSuccess) {
        http.setTimeout(10000);
        http.addHeader("Content-Type", "application/json");
        
        JsonDocument doc;
        doc["deviceId"] = "ar_sala";
        doc["localIp"] = "http://" + WiFi.localIP().toString();
        
        String requestBody;
        serializeJson(doc, requestBody);
        
        int httpResponseCode = http.POST(requestBody);
        if (httpResponseCode > 0) {
            ultimoStatusPing = "Sucesso (" + String(httpResponseCode) + ")";
            Serial.printf("[BOT] Ping enviado. Codigo HTTP: %d\n", httpResponseCode);
        } else {
            ultimoStatusPing = "Erro: " + http.errorToString(httpResponseCode);
            Serial.printf("[BOT] Falha no ping. Erro: %s\n", http.errorToString(httpResponseCode).c_str());
        }
        http.end();
    } else {
        ultimoStatusPing = "Erro ao conectar";
        Serial.println("[BOT] Erro ao iniciar conexao HTTP para ping.");
    }
}

// --- NOVOS HANDLERS DA API ---
void handleApiPower() {
    if (!validarApiKey()) return;
    if (!server.hasArg("status")) {
        JsonDocument doc;
        doc["status"] = "error";
        doc["message"] = "Parametro status obrigatorio";
        String response;
        serializeJson(doc, response);
        server.send(400, "application/json", response);
        return;
    }
    String status = server.arg("status");
    status.toLowerCase();
    
    bool valid = false;
    if (status == "on") {
        ac.on();
        valid = true;
    } else if (status == "off") {
        ac.off();
        valid = true;
    }
    
    if (!valid) {
        JsonDocument doc;
        doc["status"] = "error";
        doc["message"] = "Valor do status invalido. Use 'on' ou 'off'";
        String response;
        serializeJson(doc, response);
        server.send(400, "application/json", response);
        return;
    }
    
    digitalWrite(LED_PLACA, LOW);
    yield(); ac.send();
    digitalWrite(LED_PLACA, HIGH);
    isAcOn = ac.getPower();
    
    JsonDocument doc;
    doc["status"] = "success";
    doc["message"] = status == "on" ? "Ar condicionado ligado" : "Ar condicionado desligado";
    doc["device_state"] = getDeviceStateString();
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleApiTemp() {
    if (!validarApiKey()) return;
    if (!server.hasArg("v")) {
        JsonDocument doc;
        doc["status"] = "error";
        doc["message"] = "Parametro v (temperatura) obrigatorio";
        String response;
        serializeJson(doc, response);
        server.send(400, "application/json", response);
        return;
    }
    int temp = server.arg("v").toInt();
    if (temp < 16 || temp > 31) {
        JsonDocument doc;
        doc["status"] = "error";
        doc["message"] = "Temperatura deve ser entre 16 e 31 graus";
        String response;
        serializeJson(doc, response);
        server.send(400, "application/json", response);
        return;
    }
    
    ac.setTemp(temp);
    ac.on();
    digitalWrite(LED_PLACA, LOW);
    yield(); ac.send();
    digitalWrite(LED_PLACA, HIGH);
    isAcOn = ac.getPower();
    
    JsonDocument doc;
    doc["status"] = "success";
    doc["message"] = "Temperatura ajustada para " + String(temp) + "C";
    doc["device_state"] = getDeviceStateString();
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleApiMode() {
    if (!validarApiKey()) return;
    if (!server.hasArg("v")) {
        JsonDocument doc;
        doc["status"] = "error";
        doc["message"] = "Parametro v (modo) obrigatorio";
        String response;
        serializeJson(doc, response);
        server.send(400, "application/json", response);
        return;
    }
    String val = server.arg("v");
    val.toLowerCase();
    
    bool valid = true;
    if (val == "cool") {
        ac.setMode(kTcl112AcCool);
    } else if (val == "heat") {
        ac.setMode(kTcl112AcHeat);
    } else if (val == "fan") {
        ac.setMode(kTcl112AcFan);
    } else if (val == "dry") {
        ac.setMode(kTcl112AcDry);
    } else if (val == "auto") {
        ac.setMode(kTcl112AcAuto);
    } else {
        valid = false;
    }
    
    if (!valid) {
        JsonDocument doc;
        doc["status"] = "error";
        doc["message"] = "Modo invalido. Valores aceitos: cool, heat, fan, dry, auto";
        String response;
        serializeJson(doc, response);
        server.send(400, "application/json", response);
        return;
    }
    
    ac.on();
    digitalWrite(LED_PLACA, LOW);
    yield(); ac.send();
    digitalWrite(LED_PLACA, HIGH);
    isAcOn = ac.getPower();
    
    JsonDocument doc;
    doc["status"] = "success";
    doc["message"] = "Modo alterado para " + val;
    doc["device_state"] = getDeviceStateString();
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleApiFan() {
    if (!validarApiKey()) return;
    if (!server.hasArg("v")) {
        JsonDocument doc;
        doc["status"] = "error";
        doc["message"] = "Parametro v (velocidade) obrigatorio";
        String response;
        serializeJson(doc, response);
        server.send(400, "application/json", response);
        return;
    }
    String val = server.arg("v");
    val.toLowerCase();
    
    bool valid = true;
    if (val == "auto") {
        ac.setFan(kTcl112AcFanAuto);
    } else if (val == "low") {
        ac.setFan(kTcl112AcFanLow);
    } else if (val == "med") {
        ac.setFan(kTcl112AcFanMed);
    } else if (val == "high") {
        ac.setFan(kTcl112AcFanHigh);
    } else {
        valid = false;
    }
    
    if (!valid) {
        JsonDocument doc;
        doc["status"] = "error";
        doc["message"] = "Velocidade invalida. Valores aceitos: auto, low, med, high";
        String response;
        serializeJson(doc, response);
        server.send(400, "application/json", response);
        return;
    }
    
    digitalWrite(LED_PLACA, LOW);
    yield(); ac.send();
    digitalWrite(LED_PLACA, HIGH);
    isAcOn = ac.getPower();
    
    JsonDocument doc;
    doc["status"] = "success";
    doc["message"] = "Velocidade do ventilador alterada para " + val;
    doc["device_state"] = getDeviceStateString();
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}
 
// --- ENDPOINT API JSON --- 
void handleApiStatus() { 
    JsonDocument doc; 
    doc["status"] = "success"; 
    doc["temp_real"] = tempReal; 
    doc["umidade_real"] = umidadeReal; 
    
    // Estado do Ar Condicionado estruturado para o JavaScript
    JsonObject acState = doc["ac_state"].to<JsonObject>();
    acState["power"] = ac.getPower();
    acState["mode"] = ac.getMode();
    acState["temp"] = ac.getTemp();
    acState["fan"] = ac.getFan();
    acState["swingv"] = ac.getSwingVertical();
    acState["swingh"] = ac.getSwingHorizontal();
    acState["turbo"] = ac.getTurbo();
    acState["econo"] = ac.getEcono();
    acState["health"] = ac.getHealth();
    acState["light"] = ac.getLight();
    acState["quiet"] = ac.getQuiet();
    acState["timer_on"] = ac.getOnTimer();
    acState["timer_off"] = ac.getOffTimer();
    
    doc["ac_state_str"] = ac.toString(); // Retrocompatibilidade se necessário
    doc["auto_mode"] = autoThermostat;
    doc["auto_alvo"] = tempAlvo;
    doc["auto_status"] = estadoAuto;
    doc["whatsapp_enabled"] = whatsappEnabled;
    doc["whatsapp_url"] = whatsappServerUrl;
    doc["whatsapp_status"] = ultimoStatusReg;
    doc["whatsapp_ping_status"] = ultimoStatusPing;

    // Adiciona o historico (array JSON) ordenado do mais antigo pro mais novo
    JsonArray hist = doc["historico"].to<JsonArray>();
    int count = historicoCheio ? 20 : historicoIndex;
    int startIdx = historicoCheio ? historicoIndex : 0;
    
    for (int i = 0; i < count; i++) {
        int idx = (startIdx + i) % 20;
        hist.add(historicoTemp[idx]);
    }
     
    doc["free_heap"] = ESP.getFreeHeap(); 
    doc["total_heap"] = TOTAL_RAM; 
    doc["uptime_ms"] = millis(); 

    String response; 
    serializeJson(doc, response); 
    server.send(200, "application/json", response); 
} 
 
String getPublicIP() {
    WiFiClient client;
    HTTPClient http;
    String ip = "";
    if (http.begin(client, "http://ipv4.icanhazip.com/")) {
        int code = http.GET();
        if (code > 0) {
            ip = http.getString();
            ip.trim();
        }
        http.end();
    }
    return ip;
}

void handleApiPublicIp() {
    JsonDocument doc;
    doc["public_ip"] = getPublicIP();
    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}

// --- COMANDOS VIA FILA (COMMAND QUEUE - OPÇÃO C) ---

void enviarResultadoComando(int commandId, bool success, const JsonDocument& responseDoc) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[BOT-CMD] Sem WiFi para enviar resultado.");
        return;
    }

    WiFiClient clientPlain;
    WiFiClientSecure clientSecure;
    HTTPClient http;
    String url = whatsappServerUrl + "/api/whatsapp/devices/command-result";

    Serial.printf("[BOT-CMD] Enviando resultado para comando %d\n", commandId);

    bool isHttps = url.startsWith("https://");
    bool beginSuccess = false;

    if (isHttps) {
        clientSecure.setInsecure();
        clientSecure.setBufferSizes(1024, 1024); // Reduz uso de RAM do SSL de 32KB para 2KB
        beginSuccess = http.begin(clientSecure, url);
    } else {
        beginSuccess = http.begin(clientPlain, url);
    }

    if (beginSuccess) {
        http.setTimeout(10000);
        http.addHeader("Content-Type", "application/json");

        JsonDocument doc;
        doc["deviceId"] = "ar_sala";
        doc["apiKey"] = API_KEY;
        doc["commandId"] = commandId;
        doc["success"] = success;
        doc["response"] = responseDoc;

        String requestBody;
        serializeJson(doc, requestBody);

        int httpResponseCode = http.POST(requestBody);
        if (httpResponseCode > 0) {
            Serial.printf("[BOT-CMD] Resultado enviado. HTTP Code: %d\n", httpResponseCode);
        } else {
            Serial.printf("[BOT-CMD] Erro ao enviar resultado. HTTP: %s\n", http.errorToString(httpResponseCode).c_str());
        }
        http.end();
    } else {
        Serial.println("[BOT-CMD] Erro ao iniciar conexao HTTP para resultado.");
    }
}

void processarFilaComando(int commandId, String endpoint, JsonVariant params) {
    bool success = false;
    JsonDocument responseDoc;
    
    // Leitura atualizada dos sensores locais para que o retorno vá completo
    lerSensor();

    if (endpoint == "/api/power") {
        if (params.containsKey("status")) {
            String status = params["status"].as<String>();
            status.toLowerCase();
            if (status == "on") {
                ac.on();
                success = true;
            } else if (status == "off") {
                ac.off();
                success = true;
            }
        }
    } 
    else if (endpoint == "/api/temp") {
        if (params.containsKey("v")) {
            int temp = params["v"].as<int>();
            if (temp >= 16 && temp <= 31) {
                ac.setTemp(temp);
                ac.on();
                success = true;
            }
        }
    }
    else if (endpoint == "/api/mode") {
        if (params.containsKey("v")) {
            String val = params["v"].as<String>();
            val.toLowerCase();
            success = true;
            if (val == "cool") ac.setMode(kTcl112AcCool);
            else if (val == "heat") ac.setMode(kTcl112AcHeat);
            else if (val == "fan") ac.setMode(kTcl112AcFan);
            else if (val == "dry") ac.setMode(kTcl112AcDry);
            else if (val == "auto") ac.setMode(kTcl112AcAuto);
            else success = false;
            
            if (success) {
                ac.on();
            }
        }
    }
    else if (endpoint == "/api/fan") {
        if (params.containsKey("v")) {
            String val = params["v"].as<String>();
            val.toLowerCase();
            success = true;
            if (val == "auto") ac.setFan(kTcl112AcFanAuto);
            else if (val == "low") ac.setFan(kTcl112AcFanLow);
            else if (val == "med") ac.setFan(kTcl112AcFanMed);
            else if (val == "high") ac.setFan(kTcl112AcFanHigh);
            else success = false;
        }
    }
    else if (endpoint == "/api/status") {
        success = true;
    }

    if (success) {
        // Envia o sinal IR fisicamente para o Ar
        if (endpoint != "/api/status") {
            digitalWrite(LED_PLACA, LOW);
            yield(); ac.send();
            digitalWrite(LED_PLACA, HIGH);
            isAcOn = ac.getPower();
        }

        // Popula a resposta de sucesso com o estado do aparelho e dos sensores
        responseDoc["status"] = "success";
        responseDoc["temp_real"] = tempReal;
        responseDoc["umidade_real"] = umidadeReal;
        
        JsonObject acState = responseDoc["ac_state"].to<JsonObject>();
        acState["power"] = ac.getPower();
        acState["mode"] = ac.getMode();
        acState["temp"] = ac.getTemp();
        acState["fan"] = ac.getFan();
        acState["swingv"] = ac.getSwingVertical();
        acState["swingh"] = ac.getSwingHorizontal();
        acState["turbo"] = ac.getTurbo();
        acState["econo"] = ac.getEcono();
        acState["health"] = ac.getHealth();
        acState["light"] = ac.getLight();
        acState["quiet"] = ac.getQuiet();
        acState["timer_on"] = ac.getOnTimer();
        acState["timer_off"] = ac.getOffTimer();
    } else {
        responseDoc["status"] = "error";
        responseDoc["message"] = "Falha ao processar comando ou parametros invalidos";
    }

    enviarResultadoComando(commandId, success, responseDoc);
}

void verificarFilaComandos() {
    if (WiFi.status() != WL_CONNECTED) {
        return;
    }

    WiFiClient clientPlain;
    WiFiClientSecure clientSecure;
    HTTPClient http;
    String url = whatsappServerUrl + "/api/whatsapp/devices/poll-commands?deviceId=ar_sala&apiKey=" + API_KEY;

    bool isHttps = url.startsWith("https://");
    bool beginSuccess = false;

    if (isHttps) {
        clientSecure.setInsecure();
        clientSecure.setBufferSizes(1024, 1024); // Reduz uso de RAM do SSL de 32KB para 2KB
        beginSuccess = http.begin(clientSecure, url);
    } else {
        beginSuccess = http.begin(clientPlain, url);
    }

    int commandId = -1;
    String endpoint = "";
    JsonDocument commandParamsDoc;
    bool temComando = false;

    if (beginSuccess) {
        http.setTimeout(10000);
        int httpResponseCode = http.GET();
        if (httpResponseCode == HTTP_CODE_OK) {
            String payload = http.getString();
            
            // Tenta decodificar o JSON
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                if (doc.containsKey("commandId") && !doc["commandId"].isNull()) {
                    commandId = doc["commandId"].as<int>();
                    endpoint = doc["endpoint"].as<String>();
                    if (doc.containsKey("params")) {
                        commandParamsDoc = doc["params"];
                    }
                    temComando = true;
                    ultimoStatusPing = "Sucesso (Fila OK)";
                } else {
                    ultimoStatusPing = "Sucesso (Fila Vazia)";
                }
            } else {
                Serial.println("[BOT] Erro ao decodificar JSON do poll-commands.");
                ultimoStatusPing = "Erro JSON";
            }
        } else {
            Serial.printf("[BOT] Erro ao fazer poll: HTTP %d\n", httpResponseCode);
            ultimoStatusPing = "Erro HTTP " + String(httpResponseCode);
        }
        http.end(); // Libera a primeira conexao e memoria SSL aqui antes do processarFilaComando
    } else {
        Serial.println("[BOT] Erro ao conectar para poll-commands.");
        ultimoStatusPing = "Erro Conexao";
    }

    // Processa o comando se houver, agora que a primeira conexao de poll esta fechada
    if (temComando && commandId != -1) {
        Serial.printf("[BOT] Processando comando ID: %d, Rota: %s\n", commandId, endpoint.c_str());
        delay(100); // Aguarda o stack LwIP/SSL desalocar a RAM da conexao anterior
        processarFilaComando(commandId, endpoint, commandParamsDoc.as<JsonVariant>());
    }
}
 
// --- PROCESSADOR DE COMANDOS --- 
void handleCommand() { 
    if (server.hasArg("c")) { 
        String c = server.arg("c"); 
        bool sendIr = false;
        
        // Energia
        if (c == "ON") { ac.on(); sendIr = true; } 
        else if (c == "OFF") { ac.off(); sendIr = true; } 
        else if (c == "TOGGLE_POWER") {
            if (ac.getPower()) ac.off();
            else ac.on();
            sendIr = true;
        }
        
        // Temperatura Manual (Suporta decimais ex: T22.5)
        else if (c.startsWith("T") && !c.startsWith("TIMER")) { 
            ac.setTemp(c.substring(1).toFloat()); 
            ac.on(); 
            sendIr = true; 
        }
        
        // Modos de Operação
        else if (c == "MODE_COOL") { ac.setMode(kTcl112AcCool); ac.on(); sendIr = true; }
        else if (c == "MODE_HEAT") { ac.setMode(kTcl112AcHeat); ac.on(); sendIr = true; }
        else if (c == "MODE_DRY")  { ac.setMode(kTcl112AcDry); ac.on(); sendIr = true; }
        else if (c == "MODE_FAN")  { ac.setMode(kTcl112AcFan); ac.on(); sendIr = true; }
        else if (c == "MODE_AUTO") { ac.setMode(kTcl112AcAuto); ac.on(); sendIr = true; }
        
        // Velocidade do Ventilador
        else if (c == "FAN_AUTO") { ac.setFan(kTcl112AcFanAuto); sendIr = true; }
        else if (c == "FAN_MIN")  { ac.setFan(kTcl112AcFanMin); sendIr = true; }
        else if (c == "FAN_LOW")  { ac.setFan(kTcl112AcFanLow); sendIr = true; }
        else if (c == "FAN_MED")  { ac.setFan(kTcl112AcFanMed); sendIr = true; }
        else if (c == "FAN_HIGH") { ac.setFan(kTcl112AcFanHigh); sendIr = true; }
        
        // Direcionamento Vertical (Swing V)
        else if (c == "SWING_V_AUTO")    { ac.setSwingVertical(kTcl112AcSwingVOn); sendIr = true; }
        else if (c == "SWING_V_OFF")     { ac.setSwingVertical(kTcl112AcSwingVOff); sendIr = true; }
        else if (c == "SWING_V_HIGHEST") { ac.setSwingVertical(kTcl112AcSwingVHighest); sendIr = true; }
        else if (c == "SWING_V_HIGH")    { ac.setSwingVertical(kTcl112AcSwingVHigh); sendIr = true; }
        else if (c == "SWING_V_MID")     { ac.setSwingVertical(kTcl112AcSwingVMiddle); sendIr = true; }
        else if (c == "SWING_V_LOW")     { ac.setSwingVertical(kTcl112AcSwingVLow); sendIr = true; }
        else if (c == "SWING_V_LOWEST")  { ac.setSwingVertical(kTcl112AcSwingVLowest); sendIr = true; }
        
        // Direcionamento Horizontal (Swing H)
        else if (c == "SWING_H_ON")  { ac.setSwingHorizontal(true); sendIr = true; }
        else if (c == "SWING_H_OFF") { ac.setSwingHorizontal(false); sendIr = true; }
        
        // Funções Especiais
        else if (c == "TURBO_ON")   { ac.setTurbo(true); sendIr = true; }
        else if (c == "TURBO_OFF")  { ac.setTurbo(false); sendIr = true; }
        else if (c == "ECO_ON")     { ac.setEcono(true); sendIr = true; }
        else if (c == "ECO_OFF")    { ac.setEcono(false); sendIr = true; }
        else if (c == "HEALTH_ON")  { ac.setHealth(true); sendIr = true; }
        else if (c == "HEALTH_OFF") { ac.setHealth(false); sendIr = true; }
        else if (c == "LIGHT_ON")   { ac.setLight(true); sendIr = true; }
        else if (c == "LIGHT_OFF")  { ac.setLight(false); sendIr = true; }
        else if (c == "QUIET_ON")   { ac.setQuiet(true); sendIr = true; }
        else if (c == "QUIET_OFF")  { ac.setQuiet(false); sendIr = true; }
        
        // Temporizadores (Timers)
        else if (c.startsWith("TIMER_ON_")) {
            if (c == "TIMER_ON_OFF") ac.setOnTimer(0);
            else ac.setOnTimer(c.substring(9).toInt());
            sendIr = true;
        }
        else if (c.startsWith("TIMER_OFF_")) {
            if (c == "TIMER_OFF_OFF") ac.setOffTimer(0);
            else ac.setOffTimer(c.substring(10).toInt());
            sendIr = true;
        }
        
        // Termostato Inteligente (Controle por software no NodeMCU)
        else if (c == "AUTO_ON")  { autoThermostat = true; estadoAuto = "Avaliando..."; forcarEnvioAuto = true; }
        else if (c == "AUTO_OFF") { autoThermostat = false; estadoAuto = "Aguardando"; }
        else if (c.startsWith("ALVO_")) { 
            tempAlvo = c.substring(5).toFloat(); 
            forcarEnvioAuto = true; 
        }
        else if (c == "BOT_ENABLE") {
            whatsappEnabled = true;
            whatsappServerUrl = WHATSAPP_SERVER_URL;
            ultimoStatusReg = "Ativado, registrando...";
            ultimoStatusPing = "Ativado";
            registrarDispositivo();
        }
        else if (c == "BOT_DISABLE") {
            whatsappEnabled = false;
            ultimoStatusReg = "WhatsApp desativado";
            ultimoStatusPing = "WhatsApp desativado";
        }
        else if (c == "BOT_REGISTER") {
            if (whatsappEnabled) registrarDispositivo();
        }
        else if (c == "BOT_PING") {
            if (whatsappEnabled) enviarPing();
        }
        else if (c == "SET_BOT_URL") {
            if (server.hasArg("url")) {
                whatsappServerUrl = server.arg("url");
                Serial.println("[BOT] URL atualizada para: " + whatsappServerUrl);
            }
        }
 
        if (sendIr) {
            digitalWrite(LED_PLACA, LOW); 
            yield(); ac.send(); 
            digitalWrite(LED_PLACA, HIGH); 
            isAcOn = ac.getPower();
        }

        JsonDocument doc;
        doc["status"] = "ok";
        doc["message"] = "Comando executado: " + c;
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", response);
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
} 
 
void setup() { 
    whatsappServerUrl = WHATSAPP_SERVER_URL;
    Serial.begin(115200); 
    pinMode(LED_PLACA, OUTPUT); 
    digitalWrite(LED_PLACA, HIGH); 
     
    dht.begin(); 
    ac.begin(); 
    ac.setModel(tcl_ac_remote_model_t::TAC09CHSD); // Configura modelo completo da TCL com todos os botoes
 
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    unsigned long wifiStart = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 15000) { delay(500); Serial.print("."); }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("\nWiFi OK, IP: ");
        Serial.println(WiFi.localIP());
        if (MDNS.begin("meuar")) {
            Serial.println("mDNS OK: http://meuar.local");
        }
        if (whatsappEnabled) {
            registrarDispositivo();
        }
        String wan = getPublicIP();
        if (wan.length() > 0) {
            Serial.print("IP Público: ");
            Serial.println(wan);
        } else {
            Serial.println("IP Público: indisponível");
        }
    } else {
        Serial.println("\nWiFi falhou, iniciando AP");
        WiFi.mode(WIFI_AP);
        String apName = String("MEUAR-SETUP-") + String(ESP.getChipId(), HEX);
        WiFi.softAP(apName.c_str());
        Serial.print("AP SSID: ");
        Serial.println(apName);
        Serial.print("AP IP: ");
        Serial.println(WiFi.softAPIP());
    }
 
    server.on("/", handleRoot); 
    server.on("/cmd", handleCommand); 
    server.on("/api/status", handleApiStatus); 
    server.on("/api/public_ip", handleApiPublicIp);
    server.on("/api/power", handleApiPower);
    server.on("/api/temp", handleApiTemp);
    server.on("/api/mode", handleApiMode);
    server.on("/api/fan", handleApiFan);
    server.begin(); 
    Serial.printf("[RAM] Livre: %u / %u bytes (%.1f%% usado)\n", ESP.getFreeHeap(), TOTAL_RAM, (1.0 - (float)ESP.getFreeHeap() / TOTAL_RAM) * 100.0);
    Serial.println("Sistema Pronto!"); 
} 
 
void loop() { 
    // Reinicializa o ESP a cada 12 horas para garantir estabilidade e limpar a RAM
    if (millis() >= 43200000) {
        Serial.println("[SISTEMA] Reiniciando ESP8266 programado a cada 12 horas...");
        delay(200);
        ESP.restart();
    }

    server.handleClient(); 
    MDNS.update(); 
    
    // --- LÓGICA DO TERMOSTATO AUTOMÁTICO ---
    lerSensor();
    if (autoThermostat && tempReal > 0) {
        // Verifica a cada 60s ou imediatamente se o usuário alterou alguma configuração
        if (millis() - ultimoComandoAuto > 60000 || forcarEnvioAuto) {
            
            // Se a temperatura do quarto ficou quente demais, LIGA o Ar
            if (tempReal >= (tempAlvo + tolerancia)) {
                // Se o ar estiver desligado, ou se a gente acabou de forçar uma atualização:
                if (!isAcOn || forcarEnvioAuto) {
                    estadoAuto = "Forçando frio (Ar em 18C)";
                    Serial.println("Auto: " + estadoAuto);
                    
                    ac.on();
                    ac.setTemp(18); // Ajusta o Ar para 18°C para "enganar" o termostato interno dele e obrigar a gelar o quarto
                    
                    digitalWrite(LED_PLACA, LOW); 
                    yield(); ac.send(); 
                    digitalWrite(LED_PLACA, HIGH); 
                    
                    isAcOn = true;
                    ultimoComandoAuto = millis();
                    forcarEnvioAuto = false;
                } else {
                    estadoAuto = "Resfriando o quarto...";
                }
                
            // Se a temperatura do quarto atingiu o Alvo ou ficou mais frio, DESLIGA o ar.
            } else if (tempReal <= tempAlvo) {
                if (isAcOn || forcarEnvioAuto) {
                    estadoAuto = "Alvo atingido! Desligando Ar.";
                    Serial.println("Auto: " + estadoAuto);
                    
                    ac.off();
                    
                    digitalWrite(LED_PLACA, LOW); 
                    yield(); ac.send(); 
                    digitalWrite(LED_PLACA, HIGH); 
                    
                    isAcOn = false;
                    ultimoComandoAuto = millis();
                    forcarEnvioAuto = false;
                } else {
                    estadoAuto = "Aguardando esquentar...";
                }
            } else {
                // Zona Neutra: a temperatura está entre o alvo e o ponto de religar.
                if (forcarEnvioAuto) forcarEnvioAuto = false; // Apenas limpa a flag
                
                if (isAcOn) {
                     estadoAuto = "Resfriando o quarto...";
                } else {
                     estadoAuto = "Aguardando esquentar...";
                }
            }
        }
    }
    
    // --- CONSULTA DA FILA DE COMANDOS ---
    static unsigned long ultimoPollComando = 0;
    if (whatsappEnabled && (millis() - ultimoPollComando >= 10000 || ultimoPollComando == 0)) {
        ultimoPollComando = millis();
        verificarFilaComandos();
    }
    
    // --- LOG DE RAM LIVRE ---
    static unsigned long ultimoLogRam = 0;
    if (millis() - ultimoLogRam >= 60000 || ultimoLogRam == 0) {
        ultimoLogRam = millis();
        Serial.printf("[RAM] Livre: %u / %u bytes (%.1f%% usado)\n", ESP.getFreeHeap(), TOTAL_RAM, (1.0 - (float)ESP.getFreeHeap() / TOTAL_RAM) * 100.0);
    }

    // --- PING PERIÓDICO (HEARTBEAT) ---
    static unsigned long ultimoPing = 0;
    if (ultimoPing == 0) {
        ultimoPing = millis();
    }
    if (whatsappEnabled && millis() - ultimoPing >= 3600000) {
        ultimoPing = millis();
        enviarPing();
    }
}
