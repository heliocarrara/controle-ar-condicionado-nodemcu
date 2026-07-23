// TAC09CHSD AC - Frontend Controller Logic (zero external dependencies)

let currentAcState = {};
let currentAutoState = {};
let isUpdating = false;

// Inicialização
window.onload = function() {
  updateStatus();
  fetchCuiabaWeather();
  
  // Loop de polling a cada 2 segundos
  setInterval(updateStatus, 2000);
  
  // Atualiza clima a cada 5 minutos
  setInterval(fetchCuiabaWeather, 300000);
};

// Alternar entre Abas
window.switchTab = function(tabName) {
  const panes = document.querySelectorAll('.tab-pane');
  panes.forEach(pane => pane.classList.remove('active'));
  
  const navItems = document.querySelectorAll('.nav-item');
  navItems.forEach(item => item.classList.remove('active'));
  
  document.getElementById(`tab-${tabName}`).classList.add('active');
  document.getElementById(`nav-${tabName}`).classList.add('active');
};

// Busca clima real de Cuiabá via Open-Meteo
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
    console.log('Erro ao buscar clima:', error);
  }
}

// Desenha gráfico de linhas no canvas (sem dependências externas)
function drawChart(data) {
  const canvas = document.getElementById('tempChart');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const dpr = window.devicePixelRatio || 1;
  const rect = canvas.getBoundingClientRect();
  canvas.width = rect.width * dpr;
  canvas.height = rect.height * dpr;
  ctx.scale(dpr, dpr);
  const w = rect.width;
  const h = rect.height;

  ctx.clearRect(0, 0, w, h);
  if (!data || data.length < 2) {
    ctx.fillStyle = '#64748b';
    ctx.font = '12px sans-serif';
    ctx.textAlign = 'center';
    ctx.fillText('Aguardando dados...', w / 2, h / 2);
    return;
  }

  const pad = { top: 10, bottom: 20, left: 30, right: 10 };
  const chartW = w - pad.left - pad.right;
  const chartH = h - pad.top - pad.bottom;
  let min = Math.min(...data);
  let max = Math.max(...data);
  const range = max - min || 1;
  const margin = range * 0.15;
  min -= margin;
  max += margin;

  // Grid
  ctx.strokeStyle = '#e2e8f0';
  ctx.lineWidth = 1;
  for (let i = 0; i <= 4; i++) {
    const y = pad.top + (chartH * i) / 4;
    ctx.beginPath();
    ctx.moveTo(pad.left, y);
    ctx.lineTo(w - pad.right, y);
    ctx.stroke();
    const val = (max - (range * i) / 4).toFixed(1);
    ctx.fillStyle = '#64748b';
    ctx.font = '9px sans-serif';
    ctx.textAlign = 'right';
    ctx.fillText(val, pad.left - 4, y + 3);
  }

  // Linha
  ctx.beginPath();
  ctx.strokeStyle = '#3b82f6';
  ctx.lineWidth = 2.5;
  ctx.lineJoin = 'round';
  for (let i = 0; i < data.length; i++) {
    const x = pad.left + (chartW * i) / (data.length - 1);
    const y = pad.top + chartH - ((data[i] - min) / (max - min)) * chartH;
    if (i === 0) ctx.moveTo(x, y);
    else ctx.lineTo(x, y);
  }
  ctx.stroke();

  // Área preenchida
  const lastIdx = data.length - 1;
  const lastX = pad.left + (chartW * lastIdx) / (data.length - 1);
  const lastY = pad.top + chartH - ((data[lastIdx] - min) / (max - min)) * chartH;
  const firstX = pad.left;
  const firstY = pad.top + chartH - ((data[0] - min) / (max - min)) * chartH;
  const grad = ctx.createLinearGradient(0, pad.top, 0, pad.top + chartH);
  grad.addColorStop(0, 'rgba(59, 130, 246, 0.2)');
  grad.addColorStop(1, 'rgba(59, 130, 246, 0.0)');
  ctx.lineTo(lastX, lastY);
  ctx.lineTo(lastX, pad.top + chartH);
  ctx.lineTo(firstX, pad.top + chartH);
  ctx.lineTo(firstX, firstY);
  ctx.closePath();
  ctx.fillStyle = grad;
  ctx.fill();

  // Labels do eixo X
  ctx.fillStyle = '#64748b';
  ctx.font = '9px sans-serif';
  ctx.textAlign = 'center';
  for (let i = 0; i < data.length; i++) {
    const x = pad.left + (chartW * i) / (data.length - 1);
    const minsAgo = data.length - 1 - i;
    const label = minsAgo === 0 ? 'Agora' : `-${minsAgo}`;
    ctx.fillText(label, x, h - 4);
  }

  // Pontos
  ctx.fillStyle = '#3b82f6';
  for (let i = 0; i < data.length; i++) {
    const x = pad.left + (chartW * i) / (data.length - 1);
    const y = pad.top + chartH - ((data[i] - min) / (max - min)) * chartH;
    ctx.beginPath();
    ctx.arc(x, y, 3.5, 0, Math.PI * 2);
    ctx.fill();
    ctx.strokeStyle = '#f8fafc';
    ctx.lineWidth = 1.5;
    ctx.stroke();
  }
}

// Atualiza o Status do Dispositivo
async function updateStatus() {
  if (isUpdating) return;
  isUpdating = true;

  try {
    const response = await fetch('/api/status');
    if (!response.ok) throw new Error('Falha na comunicação');
    
    const data = await response.json();
    
    // Atualiza temperatura ambiente
    if (data.temp_real !== undefined) {
      document.getElementById('room-temp').innerText = parseFloat(data.temp_real).toFixed(1);
    }

    // Atualiza RAM livre (porcentagem + barra)
    if (data.free_heap !== undefined && data.total_heap > 0) {
      const usedPct = ((1 - data.free_heap / data.total_heap) * 100).toFixed(1);
      document.getElementById('ram-pct').innerText = usedPct;
      document.getElementById('ram-bar').style.width = usedPct + '%';
    }

    // Processa Estado do Ar
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

    // Atualiza o gráfico
    if (data.historico && data.historico.length > 0) {
      drawChart(data.historico);
    }

    // Badge de conexão
    document.getElementById('conn-text-badge').innerText = 'CONECTADO';
    document.getElementById('conn-text-badge').style.color = '#10b981';

    // Sincroniza UI do WhatsApp
    syncBotUi(data);

  } catch (error) {
    console.error('Erro ao buscar status:', error);
    document.getElementById('conn-text-badge').innerText = 'OFFLINE';
    document.getElementById('conn-text-badge').style.color = '#ef4444';
  } finally {
    isUpdating = false;
  }
}

// Sincroniza a UI do Painel Principal
function syncAcUi(state) {
  const ledDot = document.getElementById('ac-led-dot');
  if (state.power) {
    ledDot.classList.add('active');
  } else {
    ledDot.classList.remove('active');
  }

  if (state.temp !== undefined) {
    const tempVal = parseFloat(state.temp);
    document.getElementById('target-temp').innerText = tempVal % 1 === 0 ? tempVal.toFixed(0) : tempVal.toFixed(1);
  }

  const modes = ['cool', 'heat', 'dry', 'fan', 'auto'];
  modes.forEach(m => document.getElementById(`mode-${m}`).classList.remove('active'));
  
  const modeMap = { 3: 'cool', 1: 'heat', 2: 'dry', 7: 'fan', 8: 'auto' };
  const activeModeName = modeMap[state.mode];
  if (activeModeName) {
    document.getElementById(`mode-${activeModeName}`).classList.add('active');
  }

  const fanButtons = ['auto', 'min', 'low', 'med', 'high'];
  fanButtons.forEach(speed => document.getElementById(`fan-${speed}`).classList.remove('active'));
  
  const fanMap = { 0: 'auto', 1: 'min', 2: 'low', 3: 'med', 5: 'high' };
  const activeFanName = fanMap[state.fan];
  if (activeFanName) {
    document.getElementById(`fan-${activeFanName}`).classList.add('active');
  }

  const swingVBtn = document.getElementById('btn-swingv');
  if (state.swingv === 7) {
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

  toggleUiClass('btn-eco', state.econo);
  toggleUiClass('btn-health', state.health);
  toggleUiClass('btn-turbo', state.turbo);
  toggleUiClass('btn-light', state.light);

  updateTimerDisplay('timer-on-val', state.timer_on);
  updateTimerDisplay('timer-off-val', state.timer_off);
}

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

// Sincroniza a aba do Termostato Inteligente
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

// Sincroniza UI do WhatsApp Bot
function syncBotUi(data) {
  const badge = document.getElementById('whatsapp-badge');
  const statusBadge = document.getElementById('whatsapp-status-badge');
  const btnOn = document.getElementById('btn-bot-on');
  const btnOff = document.getElementById('btn-bot-off');

  const enabled = data.whatsapp_enabled;
  if (enabled) {
    badge.innerText = 'ATIVADO';
    badge.style.color = '#14b8a6';
    btnOn.classList.add('active');
    btnOff.classList.remove('active');
  } else {
    badge.innerText = 'DESATIVADO';
    badge.style.color = '#64748b';
    btnOn.classList.remove('active');
    btnOff.classList.add('active');
  }

  if (data.whatsapp_status) {
    statusBadge.innerText = data.whatsapp_status;
    if (data.whatsapp_status.includes('Sucesso')) {
      statusBadge.style.color = '#10b981';
    } else if (data.whatsapp_status.includes('Erro') || data.whatsapp_status.includes('Falha')) {
      statusBadge.style.color = '#ef4444';
    } else {
      statusBadge.style.color = '#64748b';
    }
  }
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

window.togglePower = function() {
  sendCmd('TOGGLE_POWER');
};

window.adjustTemp = function(diff) {
  if (currentAcState.temp === undefined) return;
  let newTemp = parseFloat(currentAcState.temp) + diff;
  if (newTemp < 16.0) newTemp = 16.0;
  if (newTemp > 31.0) newTemp = 31.0;
  sendCmd(`T${newTemp.toFixed(1)}`);
};

window.toggleSwingV = function() {
  if (currentAcState.swingv === 7) {
    sendCmd('SWING_V_OFF');
  } else {
    sendCmd('SWING_V_AUTO');
  }
};

window.toggleSwingH = function() {
  if (currentAcState.swingh) {
    sendCmd('SWING_H_OFF');
  } else {
    sendCmd('SWING_H_ON');
  }
};

window.toggleSpecial = function(name) {
  const key = { 'turbo': 'turbo', 'eco': 'econo', 'health': 'health', 'light': 'light' }[name];
  const isCurrentlyActive = currentAcState[key];
  const cmdPrefix = name.toUpperCase();
  
  if (isCurrentlyActive) {
    sendCmd(`${cmdPrefix}_OFF`);
  } else {
    sendCmd(`${cmdPrefix}_ON`);
  }
};

window.adjustTimer = function(type, offsetMins) {
  const key = type === 'ON' ? 'timer_on' : 'timer_off';
  let currentVal = currentAcState[key] || 0;
  
  let newVal = currentVal + offsetMins;
  if (newVal < 0) newVal = 0;
  if (newVal > 720) newVal = 720;

  if (newVal === 0) {
    window.clearTimer(type);
  } else {
    sendCmd(`TIMER_${type}_${newVal}`);
  }
};

window.clearTimer = function(type) {
  sendCmd(`TIMER_${type}_OFF`);
};
