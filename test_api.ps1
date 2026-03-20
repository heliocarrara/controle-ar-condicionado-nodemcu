$baseUrl = "http://meuar.local"
# Se o seu Windows não resolver "meuar.local", troque a linha acima pelo IP, exemplo:
# $baseUrl = "http://192.168.100.32"

$key = "senha123"

# Lista de endpoints que vamos testar em sequência
$endpoints = @(
    "/api/status?key=$key",
    "/api/power?key=$key&status=on",
    "/api/temp?key=$key&v=24",
    "/api/mode?key=$key&v=cool",
    "/api/fan?key=$key&v=high",
    "/api/power?key=$key&status=off"
)

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host " Iniciando teste da API do Ar Condicionado " -ForegroundColor Cyan
Write-Host "==========================================`n"
Write-Host "Pressione Ctrl+C a qualquer momento para cancelar.`n"

# Loop infinito para ficar testando repetidamente (ou remova o while para rodar só uma vez)
while ($true) {
    foreach ($endpoint in $endpoints) {
        $url = "${baseUrl}${endpoint}"
        Write-Host "-> Chamando: $endpoint" -ForegroundColor Yellow
        
        try {
            # Faz a requisição HTTP GET para a nossa API
            $response = Invoke-RestMethod -Uri $url -Method Get -TimeoutSec 5
            
            # Mostra o resultado que o ESP8266 retornou (JSON parseado automaticamente pelo PowerShell)
            if ($response.status -eq "success") {
                Write-Host "   Sucesso: $($response.message)" -ForegroundColor Green
            } else {
                Write-Host "   Erro na API: $($response.message)" -ForegroundColor Red
            }
            Write-Host "   Estado do Ar: $($response.device_state)" -ForegroundColor DarkGray
            
        } catch {
            Write-Host "   Falha de conexão: $_" -ForegroundColor Red
            Write-Host "   O NodeMCU está ligado e na mesma rede?" -ForegroundColor Red
        }
        
        Write-Host "   [Aguardando 10 segundos...]`n"
        Start-Sleep -Seconds 10
    }
    
    Write-Host "--- Ciclo completo! Recomeçando... ---`n" -ForegroundColor Magenta
}
