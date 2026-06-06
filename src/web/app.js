// TAC09CHSD AC - Frontend Controller Logic

let tempChart = null;
let currentAcState = {};
let currentAutoState = {};
let isUpdating = false;

// Inicialização
window.onload = function() {
  initChart();
  updateStatus();
  fetchCuiabaWeather();
  
  // Inicializa os ícones do Lucide
  lucide.createIcons();

  // Loop de polling a cada 2 segundos
  setInterval(updateStatus, 2000);
  
  // Atualiza clima de Cuiabá a cada 5 minutos
  setInterval(fetchCuiabaWeather, 300000);
};

// Alternar entre Abas (Single Page App)
window.switchTab = function(tabName) {
  // Oculta todas as panes e remove classe ativa da nav
  const panes = document.querySelectorAll('.tab-pane');
  panes.forEach(pane => pane.classList.remove('active'));
  
  const navItems = document.querySelectorAll('.nav-item');
  navItems.forEach(item => item.classList.remove('active'));
  
  // Ativa a pane selecionada e o botão correspondente
  document.getElementById(`tab-${tabName}`).classList.add('active');
  document.getElementById(`nav-${tabName}`).classList.add('active');
  
  // Recria os ícones Lucide da página se necessário
  lucide.createIcons();
};

// Busca clima real de Cuiabá usando API pública sem chave (Open-Meteo)
async function fetchCuiabaWeather() {
  try {
    const res = await fetch('https://api.open-meteo.com/v1/forecast?latitude=-15.6010&longitude=-56.0974&current_weather=true');
    if (res.ok) {
      const data = await res.json();
      if (data && data.current_weather) {
        const tempVal = Math.round(data.current_weather.temperature);
        document.getElementById('cuiaba-temp').innerText = tempVal;
      }
    }
  } catch (error) {
    console.log('Erro ao buscar clima de Cuiabá:', error);
  }
}

// Inicializa o Gráfico de Histórico
function initChart() {
  const ctx = document.getElementById('tempChart').getContext('2d');
  
  // Cria gradiente azul sutil
  const gradient = ctx.createLinearGradient(0, 0, 0, 150);
  gradient.addColorStop(0, 'rgba(59, 130, 246, 0.25)');
  gradient.addColorStop(1, 'rgba(59, 130, 246, 0.00)');

  tempChart = new Chart(ctx, {
    type: 'line',
    data: {
      labels: [],
      datasets: [{
        label: 'Temperatura (°C)',
        data: [],
        borderColor: '#3b82f6',
        backgroundColor: gradient,
        borderWidth: 2.5,
        fill: true,
        tension: 0.4,
        pointBackgroundColor: '#3b82f6',
        pointBorderColor: '#f8fafc',
        pointHoverBackgroundColor: '#fff',
        pointHoverBorderColor: '#3b82f6',
        pointRadius: 3.5,
        pointHoverRadius: 5.5
      }]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      animation: {
        duration: 300
      },
      scales: {
        y: {
          suggestedMin: 18,
          suggestedMax: 30,
          grid: {
            color: '#e2e8f0'
          },
          ticks: {
            color: '#64748b',
            font: { family: 'Inter', size: 10 }
          }
        },
        x: {
          grid: {
            display: false
          },
          ticks: {
            color: '#64748b',
            font: { family: 'Inter', size: 9 }
          }
        }
      },
      plugins: {
        legend: {
          display: false
        },
        tooltip: {
          backgroundColor: '#1e293b',
          titleFont: { family: 'Inter', size: 11 },
          bodyFont: { family: 'Inter', size: 11 }
        }
      }
    }
  });
}

// Atualiza o Status do Dispositivo (API GET)
async function updateStatus() {
  if (isUpdating) return;
  isUpdating = true;

  try {
    const response = await fetch('/api/status');
    if (!response.ok) throw new Error('Falha na comunicação');
    
    const data = await response.json();
    
    // Atualiza leituras de sensores
    if (data.temp_real !== undefined) {
      document.getElementById('room-temp').innerText = parseFloat(data.temp_real).toFixed(1);
    }

    // Processa Estado do Ar (objeto JSON estruturado)
    if (data.ac_state && typeof data.ac_state === 'object') {
      currentAcState = data.ac_state;
      syncAcUi(currentAcState);
    }

    // Processa Termostato Auto
    currentAutoState = {
      mode: data.auto_mode,
      alvo: data.auto_alvo,
      status: data.auto_status
    };
    syncAutoUi(currentAutoState);

    // Atualiza o grafico
    if (data.historico && data.historico.length > 0 && tempChart) {
      const labels = Array.from({ length: data.historico.length }, (_, i) => {
        const minsAgo = data.historico.length - 1 - i;
        return minsAgo === 0 ? 'Agora' : `-${minsAgo}m`;
      });
      
      tempChart.data.labels = labels;
      tempChart.data.datasets[0].data = data.historico;
      tempChart.update('none'); // Atualização sem resetar animação completa
    }

    // Atualiza badge de conexao
    document.getElementById('conn-text-badge').innerText = 'CONECTADO';
    document.getElementById('conn-text-badge').style.color = '#10b981';

  } catch (error) {
    console.error('Erro ao buscar status:', error);
    document.getElementById('conn-text-badge').innerText = 'OFFLINE';
    document.getElementById('conn-text-badge').style.color = '#ef4444';
  } finally {
    isUpdating = false;
  }
}

// Sincroniza a UI do Painel Principal (Início)
function syncAcUi(state) {
  // 1. LED status
  const ledDot = document.getElementById('ac-led-dot');
  if (state.power) {
    ledDot.classList.add('active');
  } else {
    ledDot.classList.remove('active');
  }

  // 2. Temperatura Alvo (Mostra decimal se houver, ex: 22 ou 22.5)
  if (state.temp !== undefined) {
    const tempVal = parseFloat(state.temp);
    document.getElementById('target-temp').innerText = tempVal % 1 === 0 ? tempVal.toFixed(0) : tempVal.toFixed(1);
  }

  // 3. Modos
  const modes = ['cool', 'heat', 'dry', 'fan', 'auto'];
  modes.forEach(m => document.getElementById(`mode-${m}`).classList.remove('active'));
  
  const modeMap = {
    3: 'cool',
    1: 'heat',
    2: 'dry',
    7: 'fan',
    8: 'auto'
  };
  const activeModeName = modeMap[state.mode];
  if (activeModeName) {
    document.getElementById(`mode-${activeModeName}`).classList.add('active');
  }

  // 4. Velocidade do Ventilador (Suporta todos os 5 níveis da TCL)
  const fanButtons = ['auto', 'min', 'low', 'med', 'high'];
  fanButtons.forEach(speed => document.getElementById(`fan-${speed}`).classList.remove('active'));
  
  const fanMap = {
    0: 'auto',
    1: 'min',
    2: 'low',
    3: 'med',
    5: 'high'
  };
  const activeFanName = fanMap[state.fan];
  if (activeFanName) {
    document.getElementById(`fan-${activeFanName}`).classList.add('active');
  }

  // 5. Oscilações (Swing Vert / Swing Horiz)
  const swingVBtn = document.getElementById('btn-swingv');
  if (state.swingv === 7) { // kTcl112AcSwingVOn (Oscilando)
    swingVBtn.classList.add('active');
  } else {
    swingVBtn.classList.remove('active');
  }

  const swingHBtn = document.getElementById('btn-swingh');
  if (state.swingh) {
    swingHBtn.classList.add('active');
  } else {
    swingHBtn.classList.remove('active');
  }

  // 6. Funções Especiais
  toggleUiClass('btn-eco', state.econo);
  toggleUiClass('btn-health', state.health);
  toggleUiClass('btn-turbo', state.turbo);
  toggleUiClass('btn-light', state.light);

  // 7. Timers
  updateTimerDisplay('timer-on-val', state.timer_on);
  updateTimerDisplay('timer-off-val', state.timer_off);
}

// Auxiliares de classe
function toggleUiClass(id, isActive) {
  const el = document.getElementById(id);
  if (isActive) {
    el.classList.add('active');
  } else {
    el.classList.remove('active');
  }
}

function updateTimerDisplay(id, valInMinutes) {
  const el = document.getElementById(id);
  if (valInMinutes > 0) {
    el.innerText = formatMins(valInMinutes);
    el.classList.add('active');
  } else {
    el.innerText = 'Desativado';
    el.classList.remove('active');
  }
}

function formatMins(totalMins) {
  const hrs = Math.floor(totalMins / 60);
  const mins = totalMins % 60;
  if (hrs > 0) {
    return mins > 0 ? `${hrs}h ${mins}m` : `${hrs} Horas`;
  }
  return `${mins} Minutos`;
}

// Sincroniza a aba do Termostato Inteligente (Ajustes)
function syncAutoUi(state) {
  const badge = document.getElementById('thermostat-mode-badge');
  const btnOn = document.getElementById('btn-auto-on');
  const btnOff = document.getElementById('btn-auto-off');

  if (state.mode) {
    badge.innerText = 'ATIVADO';
    badge.classList.add('active');
    btnOn.classList.add('active');
    btnOff.classList.remove('active');
  } else {
    badge.innerText = 'DESLIGADO';
    badge.classList.remove('active');
    btnOn.classList.remove('active');
    btnOff.classList.add('active');
  }

  document.getElementById('thermostat-target-val').innerHTML = `${parseFloat(state.alvo).toFixed(1)} &deg;C`;
  document.getElementById('thermostat-status-desc').innerText = state.status;

  const alvos = [18, 22, 23, 24, 25];
  alvos.forEach(val => {
    const btn = document.getElementById(`btn-alvo-${val}`);
    if (state.alvo === val) {
      btn.classList.add('active');
    } else {
      btn.classList.remove('active');
    }
  });
}

// Envio de comandos HTTP
async function sendCmd(command) {
  try {
    const response = await fetch(`/cmd?c=${command}`);
    if (response.ok) {
      updateStatus();
    }
  } catch (error) {
    console.error('Erro ao enviar comando:', error);
  }
}

// Alternar liga/desliga geral pelo botão LIGAR/DESLIGAR
window.togglePower = function() {
  sendCmd('TOGGLE_POWER');
};

// Ajustar temperatura em 0.5 graus (+/-)
window.adjustTemp = function(diff) {
  if (currentAcState.temp === undefined) return;
  let newTemp = parseFloat(currentAcState.temp) + diff;
  if (newTemp < 16.0) newTemp = 16.0;
  if (newTemp > 31.0) newTemp = 31.0;
  sendCmd(`T${newTemp.toFixed(1)}`);
};

// Alternar Swing Vertical
window.toggleSwingV = function() {
  if (currentAcState.swingv === 7) {
    sendCmd('SWING_V_OFF');
  } else {
    sendCmd('SWING_V_AUTO');
  }
};

// Alternar Swing Horizontal
window.toggleSwingH = function() {
  if (currentAcState.swingh) {
    sendCmd('SWING_H_OFF');
  } else {
    sendCmd('SWING_H_ON');
  }
};

// Alternar funções especiais (Eco, Turbo, Luz, Health)
window.toggleSpecial = function(name) {
  const key = {
    'turbo': 'turbo',
    'eco': 'econo',
    'health': 'health',
    'light': 'light'
  }[name];
  
  const isCurrentlyActive = currentAcState[key];
  const cmdPrefix = name.toUpperCase();
  
  if (isCurrentlyActive) {
    sendCmd(`${cmdPrefix}_OFF`);
  } else {
    sendCmd(`${cmdPrefix}_ON`);
  }
};

// Temporizadores (Timers)
window.adjustTimer = function(type, offsetMins) {
  const key = type === 'ON' ? 'timer_on' : 'timer_off';
  let currentVal = currentAcState[key] || 0;
  
  let newVal = currentVal + offsetMins;
  if (newVal < 0) newVal = 0;
  if (newVal > 720) newVal = 720; // 12 horas máx

  if (newVal === 0) {
    window.clearTimer(type);
  } else {
    sendCmd(`TIMER_${type}_${newVal}`);
  }
};

window.clearTimer = function(type) {
  sendCmd(`TIMER_${type}_OFF`);
};
