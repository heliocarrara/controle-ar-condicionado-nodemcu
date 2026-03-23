#include <Arduino.h> 
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 
#include <ESP8266mDNS.h> 
#include <DHT.h> 
#include <IRremoteESP8266.h> 
#include <ir_Tcl.h> 
#include <ArduinoJson.h> 
 
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
    String html = "<!DOCTYPE html><html lang='pt-BR'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Controle do Ar</title>";
    html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>"; // Adiciona biblioteca do gráfico
    html += "<style>";
    html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f4f7f6; color: #333; margin: 0; padding: 20px; display: flex; flex-direction: column; align-items: center; }";
    html += ".card { background: white; border-radius: 15px; box-shadow: 0 4px 15px rgba(0,0,0,0.1); padding: 20px; margin-bottom: 20px; width: 100%; max-width: 400px; text-align: center; box-sizing: border-box; }";
    html += "h2 { margin-top: 0; color: #2c3e50; font-size: 1.2rem; border-bottom: 2px solid #eee; padding-bottom: 10px; }";
    html += ".btn-group { display: flex; flex-wrap: wrap; justify-content: center; gap: 10px; margin-top: 15px; }";
    html += "button { border: none; border-radius: 8px; padding: 12px; font-size: 1rem; font-weight: bold; cursor: pointer; transition: transform 0.1s; flex: 1 1 calc(33% - 10px); min-width: 60px; }";
    html += "button:active { transform: scale(0.95); }";
    html += ".btn-on { background-color: #2ecc71; color: white; flex: 1 1 45%; }";
    html += ".btn-off { background-color: #e74c3c; color: white; flex: 1 1 45%; }";
    html += ".btn-temp { background-color: #3498db; color: white; flex: 1 1 calc(20% - 10px); padding: 12px 5px; }";
    html += ".btn-fan { background-color: #9b59b6; color: white; flex: 1 1 calc(20% - 10px); padding: 12px 5px; }";
    html += ".btn-timer { background-color: #f1c40f; color: #333; }";
    html += ".btn-auto-on { background-color: #1abc9c; color: white; flex: 1 1 45%; }";
    html += ".btn-auto-off { background-color: #95a5a6; color: white; flex: 1 1 45%; }";
    html += ".info-row { display: flex; justify-content: space-between; font-size: 0.9rem; margin: 5px 0; color: #555; }";
    html += "canvas { max-width: 100%; height: auto; }";
    html += "</style>";
    html += "</head><body>";
    
    // --- LINHA 1: Ligar / Desligar ---
    html += "<div class='card'><h2>Energia</h2>";
    html += "<div class='btn-group'>";
    html += "<button class='btn-on' onclick='cmd(\"ON\")'>LIGAR</button>";
    html += "<button class='btn-off' onclick='cmd(\"OFF\")'>DESLIGAR</button>";
    html += "</div></div>";

    // --- LINHA 2: Temperaturas Manuais ---
    html += "<div class='card'><h2>Temperatura Manual</h2>";
    html += "<div class='btn-group'>";
    html += "<button class='btn-temp' onclick='cmd(\"T18\")'>18&deg;</button>";
    html += "<button class='btn-temp' onclick='cmd(\"T22\")'>22&deg;</button>";
    html += "<button class='btn-temp' onclick='cmd(\"T23\")'>23&deg;</button>";
    html += "<button class='btn-temp' onclick='cmd(\"T24\")'>24&deg;</button>";
    html += "<button class='btn-temp' onclick='cmd(\"T25\")'>25&deg;</button>";
    html += "</div></div>";

    // --- LINHA 3: Ventilação ---
    html += "<div class='card'><h2>Velocidade do Vento</h2>";
    html += "<div class='btn-group'>";
    html += "<button class='btn-fan' onclick='cmd(\"FAN1\")'>N1</button>";
    html += "<button class='btn-fan' onclick='cmd(\"FAN2\")'>N2</button>";
    html += "<button class='btn-fan' onclick='cmd(\"FAN3\")'>N3</button>";
    html += "<button class='btn-fan' onclick='cmd(\"FAN4\")'>N4</button>";
    html += "<button class='btn-fan' onclick='cmd(\"FAN5\")'>N5</button>";
    html += "</div></div>";

    // --- LINHA 4: Timers ---
    html += "<div class='card'><h2>Timer de Desligamento</h2>";
    html += "<div class='btn-group'>";
    html += "<button class='btn-timer' onclick='cmd(\"TIMER_30M\")'>30 Min</button>";
    html += "<button class='btn-timer' onclick='cmd(\"TIMER_1H\")'>1 Hora</button>";
    html += "<button class='btn-timer' onclick='cmd(\"TIMER_8H\")'>8 Horas</button>";
    html += "</div></div>";

    // --- LINHA 5: Controle Automático ---
    html += "<div class='card'><h2>Termostato Inteligente</h2>";
    html += "<div class='info-row'><span>Modo Auto:</span><b id='auto_mode'>--</b></div>";
    html += "<div class='info-row'><span>Alvo Atual:</span><b id='alvo'>-- &deg;C</b></div>";
    html += "<div class='info-row'><span>Ação do Robô:</span><b id='auto_status'>--</b></div>";
    
    html += "<div class='btn-group' style='margin-top:15px;'>";
    html += "<button class='btn-temp' onclick='cmd(\"ALVO_18\")'>18&deg;</button>";
    html += "<button class='btn-temp' onclick='cmd(\"ALVO_22\")'>22&deg;</button>";
    html += "<button class='btn-temp' onclick='cmd(\"ALVO_23\")'>23&deg;</button>";
    html += "<button class='btn-temp' onclick='cmd(\"ALVO_24\")'>24&deg;</button>";
    html += "<button class='btn-temp' onclick='cmd(\"ALVO_25\")'>25&deg;</button>";
    html += "</div>";
    
    html += "<div class='btn-group' style='margin-top:10px;'>";
    html += "<button class='btn-auto-on' onclick='cmd(\"AUTO_ON\")'>LIGAR AUTO</button>";
    html += "<button class='btn-auto-off' onclick='cmd(\"AUTO_OFF\")'>DESLIGAR AUTO</button>";
    html += "</div></div>";

    // --- CABEÇALHO ---
    html += "<div class='card'>";
    html += "<h2><span id='temp'>--</span>&deg;C | <span id='umid'>--</span>%</h2>";
    html += "<p style='margin:5px 0'>Ar Condicionado: <b id='status'>--</b></p>";
    html += "</div>";

    // --- GRÁFICO (Últimos 20 min) ---
    html += "<div class='card'><h2>Histórico (20 min)</h2>";
    html += "<canvas id='graficoTemp'></canvas>";
    html += "</div>";

    // --- SCRIPTS DO DASHBOARD ---
    html += "<script>";
    html += "let grafico;";
    html += "function initChart() {";
    html += "  const ctx = document.getElementById('graficoTemp').getContext('2d');";
    html += "  grafico = new Chart(ctx, {";
    html += "    type: 'line',";
    html += "    data: { labels: [], datasets: [{ label: 'Temp (°C)', data: [], borderColor: '#e74c3c', backgroundColor: 'rgba(231, 76, 60, 0.2)', borderWidth: 2, fill: true, tension: 0.3 }] },";
    html += "    options: { responsive: true, animation: false, scales: { y: { suggestedMin: 20, suggestedMax: 30 } }, plugins: { legend: { display: false } } }";
    html += "  });";
    html += "}";

    html += "function cmd(c) { fetch('/cmd?c='+c).then(r=>r.text()).then(t=>atualizar()); }";
    
    html += "function atualizar() {";
    html += "  fetch('/api/status').then(r=>r.json()).then(d=>{";
    html += "    document.getElementById('temp').innerText = parseFloat(d.temp_real).toFixed(1);";
    html += "    document.getElementById('umid').innerText = parseFloat(d.umidade_real).toFixed(0);";
    html += "    document.getElementById('status').innerText = d.ac_state;";
    html += "    document.getElementById('auto_mode').innerText = d.auto_mode ? 'LIGADO' : 'DESLIGADO';";
    html += "    document.getElementById('alvo').innerText = d.auto_alvo;";
    html += "    document.getElementById('auto_status').innerText = d.auto_status;";
    
    // Atualiza o gráfico se houver dados
    html += "    if(d.historico && d.historico.length > 0) {";
    html += "       let labels = Array.from({length: d.historico.length}, (_, i) => `-${d.historico.length - i}m`);";
    html += "       labels[labels.length-1] = 'Agora';";
    html += "       grafico.data.labels = labels;";
    html += "       grafico.data.datasets[0].data = d.historico;";
    html += "       grafico.update();";
    html += "    }";
    html += "  }).catch(e=>console.log('Erro'));";
    html += "}";
    
    html += "window.onload = function() { initChart(); atualizar(); setInterval(atualizar, 2000); };";
    html += "</script>";
    
    html += "</body></html>";
    server.send(200, "text/html", html); 
} 
 
// --- ENDPOINT API JSON --- 
void handleApiStatus() { 
    lerSensor(); 
    JsonDocument doc; 
    doc["status"] = "success"; 
    doc["temp_real"] = tempReal; 
    doc["umidade_real"] = umidadeReal; 
    doc["ac_state"] = ac.toString(); 
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
 
// --- PROCESSADOR DE COMANDOS --- 
void handleCommand() { 
    if (server.hasArg("c")) { 
        String c = server.arg("c"); 
        
        bool sendIr = false;
        if (c == "ON") { ac.on(); isAcOn = true; sendIr = true; } 
        else if (c == "OFF") { ac.off(); isAcOn = false; sendIr = true; } 
        else if (c.startsWith("T")) { ac.setTemp(c.substring(1).toInt()); ac.on(); isAcOn = true; sendIr = true; }
        else if (c == "AUTO_ON") { autoThermostat = true; estadoAuto = "Avaliando..."; forcarEnvioAuto = true; }
        else if (c == "AUTO_OFF") { autoThermostat = false; estadoAuto = "Aguardando"; }
        else if (c == "ALVO_22") { tempAlvo = 22.0; forcarEnvioAuto = true; }
        else if (c == "ALVO_24") { tempAlvo = 24.0; forcarEnvioAuto = true; }
        else if (c == "ALVO_25") { tempAlvo = 25.0; forcarEnvioAuto = true; }
        else if (c == "ALVO_26") { tempAlvo = 26.0; forcarEnvioAuto = true; }
        
        // Ventilação
        else if (c.startsWith("FAN")) { 
            int fanSpeed = c.substring(3).toInt(); 
            ac.setFan(fanSpeed); // Assume que a biblioteca TCL suporta 1-5. Se não, ajustar depois.
            sendIr = true; 
        }
        
        // Timers (Isso dependerá muito de como a biblioteca do seu TCL trata timers)
        // Se a biblioteca não suportar, teremos que fazer um timer por software no próprio NodeMCU
        // Por hora, vou deixar os comandos preparados para a gente implementar.
        else if (c == "TIMER_30M") { /* TODO: Implementar Timer 30m */ }
        else if (c == "TIMER_1H") { /* TODO: Implementar Timer 1h */ }
        else if (c == "TIMER_8H") { /* TODO: Implementar Timer 8h */ }
 
        if (sendIr) {
            digitalWrite(LED_PLACA, LOW); 
            ac.send(); 
            digitalWrite(LED_PLACA, HIGH); 
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
 
    WiFi.begin(ssid, password); 
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); } 
     
    if (MDNS.begin("meuar")) { 
        Serial.println("Acesse: `http://meuar.local` "); 
    } 
 
    server.on("/", handleRoot); 
    server.on("/cmd", handleCommand); 
    server.on("/api/status", handleApiStatus); 
    server.begin(); 
    Serial.println("Sistema Pronto!"); 
} 
 
void loop() { 
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