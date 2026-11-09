#include <Arduino.h> 
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 
#include <ESP8266mDNS.h> 
#include <ESP8266HTTPClient.h>
#include <DHT.h> 
#include <IRremoteESP8266.h> 
#include <IRsend.h>
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
DHT dht(DHTPIN, DHTTYPE, 15); // count=15 melhora leitura em ESP8266 com WiFi ativo
IRTcl112Ac ac(PINO_TX); 
IRsend tvIr(PINO_TX);
ESP8266WebServer server(80); 

bool tvOffTimerActive = false;
uint32_t tvOffTimerDeadlineMs = 0;
const uint16_t kTvOffTimerMaxMinutes = 720;
 
float tempReal = 0; 
float umidadeReal = 0; 

// Histórico para o gráfico (últimos 20 minutos)
float historicoTemp[20];
int historicoIndex = 0;
unsigned long ultimoTempoHistorico = 0;
bool historicoCheio = false;


uint32_t getTvOffTimerRemainingSeconds() {
    if (!tvOffTimerActive) return 0;
    int32_t remainingMs = (int32_t)(tvOffTimerDeadlineMs - millis());
    if (remainingMs <= 0) return 0;
    return ((uint32_t)remainingMs + 999) / 1000;
}

void sendTvIrCode(uint64_t code) {
    digitalWrite(LED_PLACA, LOW);
    yield();
    tvIr.sendSAMSUNG(code, 32, 1);
    digitalWrite(LED_PLACA, HIGH);
}

void processTvOffTimer() {
    if (tvOffTimerActive && (int32_t)(millis() - tvOffTimerDeadlineMs) >= 0) {
        tvOffTimerActive = false;
        sendTvIrCode(0xE0E040BF); // Samsung Power toggle; the TV provides no IR state feedback.
        Serial.println("[TV] Timer expirou; comando POWER enviado.");
    }
}

 
// --- LÓGICA DO SENSOR --- 
void lerSensor() { 
    static unsigned long ultimaLeitura = 0; 
    if (millis() - ultimaLeitura > 2000) {
        float t = NAN, h = NAN;
        for (int tentativa = 0; tentativa < 3; tentativa++) {
            t = dht.readTemperature(); 
            h = dht.readHumidity();
            if (!isnan(t) && !isnan(h)) break;
            delay(50);
        }
        if (!isnan(t) && !isnan(h)) { 
            tempReal = t; 
            umidadeReal = h; 
            Serial.println("[DHT22] Temp: " + String(t, 1) + "C | Umidade: " + String(h, 0) + "%");
        } else {
            Serial.printf("[DHT22] Falha apos 3 tentativas. Pino: %d | VCC: %.1fV | WiFi: %s\n",
                DHTPIN, 3.3, WiFi.status() == WL_CONNECTED ? "OK" : "OFF");
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
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent_P(webpage_html);
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
    doc["tv_off_timer_active"] = tvOffTimerActive;
    doc["tv_off_timer_remaining_seconds"] = getTvOffTimerRemainingSeconds();
    
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

// --- PROCESSADOR DE COMANDOS ---
void handleCommand() {
    if (server.hasArg("c")) {
        String c = server.arg("c");
        bool sendIr = false;
        uint64_t tvCode = 0;

        if (c == "TV_TIMER_CANCEL") {
            tvOffTimerActive = false;
            server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Timer da TV cancelado\"}");
            return;
        }
        if (c.startsWith("TV_TIMER_SET_")) {
            int minutes = c.substring(13).toInt();
            if (minutes < 1 || minutes > kTvOffTimerMaxMinutes) {
                server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Timer da TV deve ficar entre 1 e 720 minutos\"}");
                return;
            }
            tvOffTimerDeadlineMs = millis() + (uint32_t)minutes * 60000UL;
            tvOffTimerActive = true;
            server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Timer da TV definido\"}");
            return;
        }

        // Samsung TV remote (32-bit Samsung IR protocol).
        if (c == "TV_POWER") {
            tvOffTimerActive = false;
            tvCode = 0xE0E040BF;
        }
        else if (c == "TV_VOLUME_UP") tvCode = 0xE0E0E01F;
        else if (c == "TV_VOLUME_DOWN") tvCode = 0xE0E0D02F;
        else if (c == "TV_MUTE") tvCode = 0xE0E0F00F;
        else if (c == "TV_CHANNEL_UP") tvCode = 0xE0E048B7;
        else if (c == "TV_CHANNEL_DOWN") tvCode = 0xE0E008F7;
        else if (c == "TV_CHANNEL_LIST") tvCode = 0xE0E0D629;
        else if (c == "TV_UP") tvCode = 0xE0E006F9;
        else if (c == "TV_DOWN") tvCode = 0xE0E08679;
        else if (c == "TV_LEFT") tvCode = 0xE0E0A659;
        else if (c == "TV_RIGHT") tvCode = 0xE0E046B9;
        else if (c == "TV_ENTER") tvCode = 0xE0E016E9;
        else if (c == "TV_RETURN") tvCode = 0xE0E01AE5;
        else if (c == "TV_EXIT") tvCode = 0xE0E0B44B;
        else if (c == "TV_TOOLS") tvCode = 0xE0E0D22D;
        else if (c == "TV_MENU") tvCode = 0xE0E058A7;
        else if (c == "TV_SOURCE") tvCode = 0xE0E0807F;
        else if (c == "TV_0") tvCode = 0xE0E08877;
        else if (c == "TV_1") tvCode = 0xE0E020DF;
        else if (c == "TV_2") tvCode = 0xE0E0A05F;
        else if (c == "TV_3") tvCode = 0xE0E0609F;
        else if (c == "TV_4") tvCode = 0xE0E010EF;
        else if (c == "TV_5") tvCode = 0xE0E0906F;
        else if (c == "TV_6") tvCode = 0xE0E050AF;
        else if (c == "TV_7") tvCode = 0xE0E030CF;
        else if (c == "TV_8") tvCode = 0xE0E0B04F;
        else if (c == "TV_9") tvCode = 0xE0E0708F;

        if (tvCode != 0) {
            sendTvIrCode(tvCode);
        }
        
        // Energia
        if (tvCode != 0) { /* TV command sent above. */ }
        else if (c == "ON") { ac.on(); sendIr = true; }
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
        
        // Presets rápidos
        else if (c == "P24_MED") { ac.setTemp(24); ac.setFan(kTcl112AcFanMed); ac.on(); sendIr = true; }
        else if (c == "P23_MED") { ac.setTemp(23); ac.setFan(kTcl112AcFanMed); ac.on(); sendIr = true; }
        else if (c == "P22_LOW") { ac.setTemp(22); ac.setFan(kTcl112AcFanLow); ac.on(); sendIr = true; }

        else if (c == "RESTART") {
            JsonDocument doc;
            doc["status"] = "ok";
            doc["message"] = "Reiniciando...";
            String response;
            serializeJson(doc, response);
            server.send(200, "application/json", response);
            Serial.println("[SISTEMA] Reiniciando por comando do usuario...");
            delay(500);
            ESP.restart();
            return;
        }
 
        if (sendIr) {
            digitalWrite(LED_PLACA, LOW); 
            yield(); ac.send(); 
            digitalWrite(LED_PLACA, HIGH); 
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
    Serial.begin(115200); 
    pinMode(LED_PLACA, OUTPUT); 
    digitalWrite(LED_PLACA, HIGH); 
     
    dht.begin(); 
    ac.begin(); 
    tvIr.begin();
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
    if (millis() >= 43200000 && !tvOffTimerActive) {
        Serial.println("[SISTEMA] Reiniciando ESP8266 programado a cada 12 horas...");
        delay(200);
        ESP.restart();
    }

    server.handleClient(); 
    MDNS.update(); 
    processTvOffTimer();
    
    // Leitura contínua do sensor ambiente e atualização do histórico.
    lerSensor();
    
    // --- LOG DE RAM LIVRE ---
    static unsigned long ultimoLogRam = 0;
    if (millis() - ultimoLogRam >= 60000 || ultimoLogRam == 0) {
        ultimoLogRam = millis();
        Serial.printf("[RAM] Livre: %u / %u bytes (%.1f%% usado)\n", ESP.getFreeHeap(), TOTAL_RAM, (1.0 - (float)ESP.getFreeHeap() / TOTAL_RAM) * 100.0);
    }


}
