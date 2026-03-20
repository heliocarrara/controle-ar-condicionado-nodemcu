#include <Arduino.h> 
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 
#include <ESP8266mDNS.h>      
#include <ArduinoJson.h>      // Necessário para a API JSON
#include <IRremoteESP8266.h> 
#include <IRsend.h> 
#include <IRrecv.h> 
#include <IRutils.h> 
#include <ir_Tcl.h> 

// Configurações do Wi-Fi 
const char* ssid = "CEU-ITALIA"; 
const char* password = "C3u1t4L1a"; 

// Chave de Segurança da API (Mude para algo que só você saiba!)
const String API_KEY = "senha123";

#define PINO_TX 5 // D1 
#define PINO_RX 4 // D2 
#define LED_PLACA 2 

IRTcl112Ac ac(PINO_TX); 
ESP8266WebServer server(80); 

// --- FUNÇÕES DA API ---

// Função auxiliar para enviar resposta JSON
void enviarRespostaJSON(String mensagem, bool erro = false) { 
    JsonDocument doc; 
    doc["status"] = erro ? "error" : "success"; 
    doc["message"] = mensagem; 
    doc["device_state"] = ac.toString(); // Estado atual do ar no ESP
    
    String response; 
    serializeJson(doc, response); 
    server.send(erro ? 400 : 200, "application/json", response); 
} 

// Verificador de Autenticação
bool checkAuth() {
    if (!server.hasArg("key") || server.arg("key") != API_KEY) {
        enviarRespostaJSON("Acesso negado. Chave API (key) invalida ou ausente.", true);
        return false;
    }
    return true;
}

// Disparo do IR com Feedback Visual
void dispararIR() {
    digitalWrite(LED_PLACA, LOW); 
    ac.send(); 
    delay(50); // Pequeno delay visual para o LED
    digitalWrite(LED_PLACA, HIGH); 
}

// Endpoint: /api/power?key=senha123&status=on 
void handleApiPower() { 
    if (!checkAuth()) return;

    if (server.hasArg("status")) { 
        String status = server.arg("status"); 
        status.toLowerCase(); 
        if (status == "on") ac.on(); 
        else if (status == "off") ac.off(); 
        else {
            enviarRespostaJSON("Status invalido. Use 'on' ou 'off'.", true);
            return;
        }
        
        dispararIR();
        enviarRespostaJSON("Energia alterada para " + status); 
    } else { 
        enviarRespostaJSON("Faltando argumento 'status'", true); 
    } 
} 

// Endpoint: /api/temp?key=senha123&v=24 
void handleApiTemp() { 
    if (!checkAuth()) return;

    if (server.hasArg("v")) { 
        int t = server.arg("v").toInt(); 
        if (t >= 16 && t <= 31) { 
            ac.setTemp(t); 
            dispararIR();
            enviarRespostaJSON("Temperatura ajustada para " + String(t) + "C"); 
        } else { 
            enviarRespostaJSON("Temperatura fora do limite (16-31)", true); 
        } 
    } else {
        enviarRespostaJSON("Faltando argumento 'v'", true);
    }
} 

// Endpoint: /api/mode?key=senha123&v=cool
void handleApiMode() {
    if (!checkAuth()) return;

    if (server.hasArg("v")) {
        String mode = server.arg("v");
        mode.toLowerCase();
        
        if (mode == "cool") ac.setMode(kTcl112AcCool);
        else if (mode == "heat") ac.setMode(kTcl112AcHeat);
        else if (mode == "fan") ac.setMode(kTcl112AcFan);
        else if (mode == "dry") ac.setMode(kTcl112AcDry);
        else if (mode == "auto") ac.setMode(kTcl112AcAuto);
        else {
            enviarRespostaJSON("Modo invalido (cool, heat, fan, dry, auto)", true);
            return;
        }

        dispararIR();
        enviarRespostaJSON("Modo ajustado para " + mode);
    } else {
        enviarRespostaJSON("Faltando argumento 'v'", true);
    }
}

// Endpoint: /api/fan?key=senha123&v=auto
void handleApiFan() {
    if (!checkAuth()) return;

    if (server.hasArg("v")) {
        String fan = server.arg("v");
        fan.toLowerCase();

        if (fan == "auto") ac.setFan(kTcl112AcFanAuto);
        else if (fan == "low") ac.setFan(kTcl112AcFanLow);
        else if (fan == "med") ac.setFan(kTcl112AcFanMed);
        else if (fan == "high") ac.setFan(kTcl112AcFanHigh);
        else {
            enviarRespostaJSON("Ventilador invalido (auto, low, med, high)", true);
            return;
        }

        dispararIR();
        enviarRespostaJSON("Ventilador ajustado para " + fan);
    } else {
        enviarRespostaJSON("Faltando argumento 'v'", true);
    }
}

// Endpoint: /api/status?key=senha123
void handleApiStatus() { 
    if (!checkAuth()) return;
    enviarRespostaJSON("Status atual capturado"); 
} 

// Interface Web Simples (Para testes rápidos no celular)
void handleRoot() { 
    String html = "<html><head><meta charset='utf-8' name='viewport' content='width=device-width, initial-scale=1'>"; 
    html += "<style>body{font-family:Arial;text-align:center;padding:20px;background:#1e1e1e;color:white;}";
    html += "button{display:inline-block;width:45%;padding:15px;margin:5px;font-size:16px;border-radius:8px;border:none;background:#007BFF;color:white;cursor:pointer;}";
    html += ".full{width:92%;background:#28A745;} .off{background:#DC3545;} </style></head><body>"; 
    html += "<h1>TCL API Control</h1>"; 
    html += "<p>Acesse as rotas via <code>/api/...</code></p>";
    // Exemplo de chamadas da API usando fetch e javascript
    html += "<button class='full' onclick=\"fetch('/api/power?key=" + API_KEY + "&status=on').then(r=>r.json()).then(d=>alert(d.message))\">LIGAR</button>"; 
    html += "<button class='full off' onclick=\"fetch('/api/power?key=" + API_KEY + "&status=off').then(r=>r.json()).then(d=>alert(d.message))\">DESLIGAR</button>"; 
    html += "<hr>"; 
    html += "<button onclick=\"fetch('/api/temp?key=" + API_KEY + "&v=22').then(r=>r.json()).then(d=>alert(d.message))\">22°C</button>"; 
    html += "<button onclick=\"fetch('/api/temp?key=" + API_KEY + "&v=24').then(r=>r.json()).then(d=>alert(d.message))\">24°C</button>"; 
    html += "<button onclick=\"fetch('/api/mode?key=" + API_KEY + "&v=cool').then(r=>r.json()).then(d=>alert(d.message))\">Modo Frio</button>"; 
    html += "<button onclick=\"fetch('/api/fan?key=" + API_KEY + "&v=auto').then(r=>r.json()).then(d=>alert(d.message))\">Ventilador Auto</button>"; 
    html += "</body></html>"; 
    
    server.send(200, "text/html", html); 
}

void setup() { 
    Serial.begin(115200); 
    pinMode(LED_PLACA, OUTPUT); 
    digitalWrite(LED_PLACA, HIGH); 

    ac.begin(); 

    // Conexão Wi-Fi 
    WiFi.begin(ssid, password); 
    Serial.print("Conectando"); 
    while (WiFi.status() != WL_CONNECTED) { 
        delay(500); 
        Serial.print("."); 
    } 
    Serial.println("\nWiFi Conectado!"); 
    Serial.print("IP: "); Serial.println(WiFi.localIP()); 

    // Rotas da API
    server.on("/", HTTP_GET, handleRoot);
    server.on("/api/power", HTTP_GET, handleApiPower); 
    server.on("/api/temp", HTTP_GET, handleApiTemp); 
    server.on("/api/mode", HTTP_GET, handleApiMode); 
    server.on("/api/fan", HTTP_GET, handleApiFan); 
    server.on("/api/status", HTTP_GET, handleApiStatus); 
    
    server.begin(); 
    Serial.println("Servidor API JSON iniciado!"); 

    // Inicia o mDNS
    if (MDNS.begin("meuar")) { 
        Serial.println("MDNS iniciado! Acesse no navegador: http://meuar.local"); 
    }
} 

void loop() { 
    server.handleClient(); 
    MDNS.update();         
}