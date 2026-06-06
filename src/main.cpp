#include <Arduino.h> 
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 
#include <ESP8266mDNS.h> 
#include <ESP8266HTTPClient.h>
#include <DHT.h> 
#include <IRremoteESP8266.h> 
#include <ir_Tcl.h> 
#include <ArduinoJson.h> 
#include "webpage.h" 
 
// --- CONFIGURAÇÕES --- 
const char* ssid = "CEU-ITALIA"; 
const char* password = "C3u1t4L1a"; 
 
#define DHTPIN 14    // Pino D5 
#define DHTTYPE DHT22 
#define PINO_TX 5    // Pino D1 
#define LED_PLACA 2 
 
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
    lerSensor(); 
    server.send_P(200, "text/html", webpage_html); 
} 
 
// --- ENDPOINT API JSON --- 
void handleApiStatus() { 
    lerSensor(); 
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

    // Adiciona o historico (array JSON) ordenado do mais antigo pro mais novo
    JsonArray hist = doc["historico"].to<JsonArray>();
    int count = historicoCheio ? 20 : historicoIndex;
    int startIdx = historicoCheio ? historicoIndex : 0;
    
    for (int i = 0; i < count; i++) {
        int idx = (startIdx + i) % 20;
        hist.add(historicoTemp[idx]);
    }
     
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
 
        if (sendIr) {
            digitalWrite(LED_PLACA, LOW); 
            ac.send(); 
            digitalWrite(LED_PLACA, HIGH); 
            isAcOn = ac.getPower(); // Sincroniza o estado virtual de energia para o termostato automático
        }
 
        server.sendHeader("Location", "/"); 
        server.send(303); 
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
    server.begin(); 
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
                    ac.send(); 
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
                    ac.send(); 
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
}
