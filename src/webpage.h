#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>

// Arquivo gerado automaticamente pelo script pre_build_web.py.
// Nao edite este arquivo diretamente. Edite os arquivos em src/web/

const char webpage_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>TAC09CHSD AC</title>
  <!-- Google Fonts: Inter -->
  <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700;800&display=swap" rel="stylesheet">
  <!-- Lucide Icons -->
  <script src="https://unpkg.com/lucide@latest"></script>
  <!-- Chart.js -->
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
/* TAC09CHSD AC - Mockup Theme CSS */

:root {
  --bg-app: #f1f5f9;
  --bg-dark-header: #1e293b;
  --bg-dark-card: #27303f;
  --bg-screen: #b8c4b2;
  --border-screen: #a3b29d;
  --text-screen: #2b3528;
  --color-active-blue: #3b82f6;
  --color-power-red: #dc2626;
  --text-dark: #1e293b;
  --text-muted: #64748b;
  --bg-card: #f8fafc;
  --border-light: #cbd5e1;
  --font-family: 'Inter', sans-serif;
  
  /* Neumorphic / Soft Shadows */
  --shadow-out: 0 4px 6px -1px rgba(0, 0, 0, 0.08), 0 2px 4px -1px rgba(0, 0, 0, 0.04);
  --shadow-button: 0 2px 4px rgba(0, 0, 0, 0.05), inset 0 1px 0 rgba(255, 255, 255, 0.6);
  --shadow-button-active: inset 0 2px 4px rgba(0, 0, 0, 0.15);
  --transition-normal: 0.2s cubic-bezier(0.4, 0, 0.2, 1);
}

* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
  -webkit-tap-highlight-color: transparent;
}

body {
  font-family: var(--font-family);
  background-color: #cbd5e1;
  color: var(--text-dark);
  min-height: 100vh;
  display: flex;
  justify-content: center;
  align-items: center;
  padding: 0;
}

/* App Viewport Container (Simulates Mobile) */
.app-container {
  width: 100%;
  max-width: 430px;
  height: 100vh;
  height: 100dvh;
  background-color: var(--bg-app);
  display: flex;
  flex-direction: column;
  position: relative;
  overflow: hidden;
  box-shadow: 0 25px 50px -12px rgba(0, 0, 0, 0.25);
}

@media (min-height: 700px) and (min-width: 440px) {
  .app-container {
    height: 92vh;
    border-radius: 30px;
    border: 8px solid #0f172a;
  }
}

/* Header */
.app-header {
  background-color: var(--bg-dark-header);
  color: white;
  height: 56px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 1.25rem;
  flex-shrink: 0;
  z-index: 10;
}

.app-header h1 {
  font-size: 1.1rem;
  font-weight: 800;
  letter-spacing: 0.05em;
  text-align: center;
  text-transform: uppercase;
}

.btn-back {
  background: none;
  border: none;
  color: white;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  width: 32px;
  height: 32px;
  border-radius: 50%;
  transition: var(--transition-normal);
}

.btn-back:hover {
  background-color: rgba(255, 255, 255, 0.1);
}

.header-placeholder {
  width: 32px;
}

/* Main Content Area */
.app-content {
  flex: 1;
  overflow-y: auto;
  padding: 1rem;
  padding-bottom: 1.5rem; /* Removida a margem absoluta, agora usa fluxo flex */
}

/* Tab Panes */
.tab-pane {
  display: none;
  flex-direction: column;
  gap: 0.875rem;
  animation: fadeIn 0.25s ease-out;
}

.tab-pane.active {
  display: flex;
}

@keyframes fadeIn {
  from { opacity: 0; transform: translateY(6px); }
  to { opacity: 1; transform: translateY(0); }
}

.tab-title {
  margin-bottom: 0.5rem;
}

.tab-title h2 {
  font-size: 1.25rem;
  font-weight: 700;
  color: var(--text-dark);
}

.tab-title p {
  font-size: 0.825rem;
  color: var(--text-muted);
}

/* Info Cards (Ambient & City) */
.info-row-top {
  display: flex;
  gap: 0.75rem;
  width: 100%;
}

.info-card {
  flex: 1;
  background-color: var(--bg-card);
  border: 1px solid var(--border-light);
  border-radius: 12px;
  padding: 0.625rem 0.5rem;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.375rem;
  box-shadow: var(--shadow-button);
}

.info-icon {
  width: 14px;
  height: 14px;
  color: var(--text-muted);
}

.info-label {
  font-size: 0.675rem;
  font-weight: 600;
  color: var(--text-muted);
}

.info-val {
  font-size: 0.8rem;
  font-weight: 700;
  color: var(--text-dark);
  margin-left: 0.125rem;
}

.info-unit {
  font-size: 0.75rem;
  font-weight: 700;
  color: var(--text-muted);
}

/* Sage Green Target Temperature Display */
.temp-screen-card {
  background-color: var(--bg-screen);
  border: 1.5px solid var(--border-screen);
  border-radius: 18px;
  padding: 1.25rem;
  display: flex;
  flex-direction: column;
  position: relative;
  box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.06), 0 4px 12px rgba(0, 0, 0, 0.04);
}

.screen-status-indicator {
  position: absolute;
  top: 12px;
  left: 16px;
}

.led-dot {
  display: block;
  width: 12px;
  height: 7px;
  border-radius: 999px;
  background-color: #9ca3af;
  border: 1px solid rgba(0, 0, 0, 0.15);
  transition: var(--transition-normal);
}

.led-dot.active {
  background-color: #10b981;
  box-shadow: 0 0 6px rgba(16, 185, 129, 0.7);
}

.temp-control-inner {
  display: flex;
  align-items: center;
  justify-content: space-around;
  width: 100%;
  margin-top: 0.5rem;
}

.btn-dial {
  background-color: rgba(0, 0, 0, 0.04);
  border: 1.5px solid rgba(0, 0, 0, 0.12);
  width: 50px;
  height: 50px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  color: var(--text-screen);
  transition: var(--transition-normal);
}

.btn-dial:hover {
  background-color: rgba(0, 0, 0, 0.08);
}

.btn-dial:active {
  transform: scale(0.92);
  background-color: rgba(0, 0, 0, 0.12);
}

.btn-dial i {
  width: 22px;
  height: 22px;
  stroke-width: 2.5px;
}

.temp-display-value {
  display: flex;
  align-items: baseline;
  color: var(--text-screen);
  user-select: none;
}

.temp-display-value span:first-child {
  font-size: 3.75rem;
  font-weight: 800;
  letter-spacing: -0.05em;
  line-height: 1;
}

.degree-symbol {
  font-size: 1.75rem;
  font-weight: 700;
  margin-left: 0.125rem;
}

/* Operation Mode Rectangular Buttons Row */
.modes-row {
  display: flex;
  gap: 0.375rem;
  width: 100%;
}

.btn-mode-rect {
  flex: 1;
  background-color: var(--bg-card);
  border: 1px solid var(--border-light);
  border-radius: 10px;
  padding: 0.625rem 0.25rem;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 0.375rem;
  cursor: pointer;
  box-shadow: var(--shadow-button);
  transition: var(--transition-normal);
}

.btn-mode-rect:hover {
  background-color: #f1f5f9;
}

.btn-mode-rect:active {
  box-shadow: var(--shadow-button-active);
}

.btn-mode-rect i {
  width: 20px;
  height: 20px;
  color: var(--color-active-blue);
  stroke-width: 2.2px;
}

.btn-mode-rect span {
  font-size: 0.625rem;
  font-weight: 700;
  color: var(--text-dark);
}

/* Active Mode */
.btn-mode-rect.active {
  background-color: var(--color-active-blue);
  border-color: var(--color-active-blue);
}

.btn-mode-rect.active i {
  color: white;
}

.btn-mode-rect.active span {
  color: white;
}

/* Split Row: Fan and Oscillation */
.split-controls-row {
  display: flex;
  gap: 0.75rem;
  width: 100%;
}

.control-box {
  flex: 1;
  background-color: #e2e8f0;
  border: 1.5px solid #cbd5e1;
  border-radius: 14px;
  padding: 0.75rem;
  box-shadow: inset 0 1px 2px rgba(255, 255, 255, 0.4);
}

.control-box h3 {
  font-size: 0.7rem;
  font-weight: 800;
  color: var(--text-muted);
  margin-bottom: 0.625rem;
  display: flex;
  align-items: center;
  gap: 0.25rem;
  text-transform: uppercase;
}

.box-title-icon {
  width: 12px;
  height: 12px;
  stroke-width: 2.5px;
}

/* Fan Grid (2x2) */
.fan-grid-mockup {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: 0.375rem;
}

.btn-fan-mock {
  background-color: var(--bg-card);
  border: 1px solid var(--border-light);
  border-radius: 8px;
  padding: 0.625rem 0.25rem;
  font-family: var(--font-family);
  font-size: 0.65rem;
  font-weight: 800;
  color: var(--text-dark);
  cursor: pointer;
  box-shadow: var(--shadow-button);
  transition: var(--transition-normal);
  text-align: center;
}

.btn-fan-mock:hover {
  background-color: #f1f5f9;
}

.btn-fan-mock.active {
  background-color: var(--color-active-blue);
  border-color: var(--color-active-blue);
  color: white;
  box-shadow: var(--shadow-button-active);
}

/* Swing Buttons Column */
.swing-buttons-vertical {
  display: flex;
  flex-direction: column;
  gap: 0.375rem;
}

.btn-swing-mock {
  background-color: var(--bg-card);
  border: 1px solid var(--border-light);
  border-radius: 8px;
  padding: 0.625rem 0.5rem;
  font-family: var(--font-family);
  font-size: 0.65rem;
  font-weight: 800;
  color: var(--text-dark);
  cursor: pointer;
  box-shadow: var(--shadow-button);
  transition: var(--transition-normal);
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.375rem;
}

.btn-swing-mock:hover {
  background-color: #f1f5f9;
}

.btn-swing-mock i {
  width: 14px;
  height: 14px;
  stroke-width: 2.2px;
}

.btn-swing-mock.active {
  background-color: var(--color-active-blue);
  border-color: var(--color-active-blue);
  color: white;
  box-shadow: var(--shadow-button-active);
}

/* Specials Row */
.specials-row {
  display: flex;
  gap: 0.375rem;
  width: 100%;
}

.btn-special-rect {
  flex: 1;
  background-color: var(--bg-card);
  border: 1px solid var(--border-light);
  border-radius: 10px;
  padding: 0.625rem 0.25rem;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 0.375rem;
  cursor: pointer;
  box-shadow: var(--shadow-button);
  transition: var(--transition-normal);
}

.btn-special-rect:hover {
  background-color: #f1f5f9;
}

.btn-special-rect i {
  width: 18px;
  height: 18px;
  color: var(--text-muted);
  stroke-width: 2px;
  transition: var(--transition-normal);
}

.btn-special-rect span {
  font-size: 0.65rem;
  font-weight: 700;
  color: var(--text-dark);
}

.btn-special-rect.active {
  box-shadow: var(--shadow-button-active);
  background-color: #f1f5f9;
}

.btn-special-rect.active#btn-eco i { color: #10b981; }
.btn-special-rect.active#btn-health i { color: #14b8a6; }
.btn-special-rect.active#btn-turbo i { color: #f59e0b; }
.btn-special-rect.active#btn-light i { color: #eab308; }

/* Room Status Card */
.room-status-card {
  background-color: var(--bg-dark-card);
  border-radius: 12px;
  padding: 0.75rem 1rem;
  display: flex;
  justify-content: space-between;
  align-items: center;
  color: white;
  margin-top: 0.125rem;
}

.room-info {
  display: flex;
  flex-direction: column;
}

.room-label {
  font-size: 0.55rem;
  font-weight: 700;
  color: var(--text-muted);
  letter-spacing: 0.05em;
}

.room-name {
  font-size: 0.85rem;
  font-weight: 800;
  letter-spacing: -0.01em;
}

.connection-status {
  display: flex;
  flex-direction: column;
  align-items: flex-end;
}

.conn-text {
  font-size: 0.65rem;
  font-weight: 800;
  color: #10b981;
}

.conn-id {
  font-size: 0.55rem;
  color: var(--text-muted);
}

/* Power Button */
.power-button-container {
  width: 100%;
  margin-top: 0.25rem;
}

.btn-power-large {
  width: 100%;
  background-color: var(--color-power-red);
  border: none;
  border-radius: 14px;
  color: white;
  padding: 1.125rem;
  font-family: var(--font-family);
  font-size: 1.05rem;
  font-weight: 800;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.625rem;
  cursor: pointer;
  box-shadow: 0 4px 10px rgba(220, 38, 38, 0.35);
  transition: var(--transition-normal);
}

.btn-power-large:active {
  transform: scale(0.98);
  box-shadow: 0 2px 5px rgba(220, 38, 38, 0.2);
}

.btn-power-large i {
  width: 22px;
  height: 22px;
  stroke-width: 2.5px;
}

/* --- ABA 2: TIMER STYLING --- */
.card-timer-container {
  display: flex;
  flex-direction: column;
  gap: 1rem;
}

.timer-card-modern {
  background-color: var(--bg-card);
  border: 1px solid var(--border-light);
  border-radius: 16px;
  padding: 1.25rem;
  box-shadow: var(--shadow-out);
}

.timer-card-header {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  margin-bottom: 1rem;
  border-bottom: 1.5px solid #f1f5f9;
  padding-bottom: 0.5rem;
}

.timer-card-icon {
  width: 20px;
  height: 20px;
}
.timer-card-icon.green { color: #10b981; }
.timer-card-icon.red { color: var(--color-power-red); }

.timer-card-header h3 {
  font-size: 0.75rem;
  font-weight: 800;
  color: var(--text-muted);
  letter-spacing: 0.025em;
}

.timer-card-body {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 0.75rem;
}

.timer-display-time {
  font-size: 1.35rem;
  font-weight: 800;
  color: var(--text-dark);
  background-color: #f1f5f9;
  padding: 0.5rem 1.25rem;
  border-radius: 10px;
  border: 1.5px solid var(--border-light);
}

.timer-display-time.active {
  color: var(--color-active-blue);
  background-color: rgba(59, 130, 246, 0.06);
  border-color: rgba(59, 130, 246, 0.2);
}

.timer-adjust-row {
  display: flex;
  gap: 0.5rem;
  width: 100%;
}

.btn-adjust {
  flex: 2;
  background-color: #e2e8f0;
  border: 1px solid var(--border-light);
  color: var(--text-dark);
  padding: 0.625rem;
  border-radius: 8px;
  font-weight: 700;
  font-size: 0.8rem;
  cursor: pointer;
  transition: var(--transition-normal);
}

.btn-adjust:hover { background-color: #cbd5e1; }

.btn-clear-timer {
  flex: 1;
  background-color: rgba(239, 68, 68, 0.1);
  border: 1px solid rgba(239, 68, 68, 0.2);
  color: var(--color-power-red);
  border-radius: 8px;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
}
.btn-clear-timer:hover { background-color: rgba(239, 68, 68, 0.2); }
.btn-clear-timer i { width: 16px; height: 16px; }

/* --- ABA 3: AJUSTES / AUTOMACAO STYLING --- */
.thermostat-card-modern {
  background-color: var(--bg-card);
  border: 1px solid var(--border-light);
  border-radius: 16px;
  padding: 1.25rem;
  box-shadow: var(--shadow-out);
  display: flex;
  flex-direction: column;
  gap: 0.875rem;
}

.therm-status-header {
  display: flex;
  justify-content: space-between;
  border-bottom: 1.5px solid #f1f5f9;
  padding-bottom: 0.625rem;
}

.therm-status-info {
  display: flex;
  flex-direction: column;
  gap: 0.125rem;
}

.therm-status-info .lbl {
  font-size: 0.65rem;
  font-weight: 600;
  color: var(--text-muted);
}

.therm-status-info .badge {
  font-size: 0.7rem;
  font-weight: 800;
  color: var(--text-muted);
}
.therm-status-info .badge.active { color: #14b8a6; }

.therm-status-info .val {
  font-size: 0.95rem;
  font-weight: 800;
}

.therm-status-message {
  display: flex;
  justify-content: space-between;
  background-color: #f1f5f9;
  padding: 0.5rem 0.75rem;
  border-radius: 8px;
  font-size: 0.775rem;
}
.therm-status-message .lbl { color: var(--text-muted); font-weight: 600; }
.therm-status-message .status-desc { font-weight: 700; color: var(--text-dark); }

.therm-adjust-section {
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
}

.therm-adjust-section .section-label {
  font-size: 0.7rem;
  font-weight: 700;
  color: var(--text-muted);
}

.btn-group-auto {
  display: flex;
  gap: 0.25rem;
}

.btn-auto-temp {
  flex: 1;
  background-color: #f1f5f9;
  border: 1px solid var(--border-light);
  border-radius: 8px;
  padding: 0.5rem 0;
  font-family: var(--font-family);
  font-size: 0.775rem;
  font-weight: 700;
  cursor: pointer;
  transition: var(--transition-normal);
  text-align: center;
}
.btn-auto-temp:hover { background-color: #cbd5e1; }
.btn-auto-temp.active {
  background-color: #14b8a6;
  border-color: #14b8a6;
  color: white;
}

.btn-auto-toggle-row {
  display: flex;
  gap: 0.5rem;
}
.btn-auto-on {
  flex: 2;
  background-color: rgba(20, 184, 166, 0.1);
  border: 1px solid rgba(20, 184, 166, 0.2);
  color: #14b8a6;
  padding: 0.75rem;
  border-radius: 10px;
  font-weight: 700;
  font-size: 0.85rem;
  cursor: pointer;
}
.btn-auto-on.active {
  background-color: #14b8a6;
  color: white;
  border-color: #14b8a6;
}
.btn-auto-off {
  flex: 1;
  background-color: #e2e8f0;
  border: 1px solid var(--border-light);
  color: var(--text-dark);
  border-radius: 10px;
  font-weight: 700;
  cursor: pointer;
}

/* Chart Card Modern */
.chart-card-modern {
  background-color: var(--bg-card);
  border: 1px solid var(--border-light);
  border-radius: 16px;
  padding: 1.25rem;
  box-shadow: var(--shadow-out);
}
.chart-card-modern h3 {
  font-size: 0.7rem;
  font-weight: 800;
  color: var(--text-muted);
  margin-bottom: 0.75rem;
}
.chart-wrapper {
  height: 180px;
  position: relative;
}

/* Rodapé Navbar */
.app-navbar {
  flex-shrink: 0;
  position: relative;
  height: 60px;
  background-color: var(--bg-dark-header);
  display: flex;
  align-items: center;
  justify-content: space-around;
  border-top: 1px solid rgba(255, 255, 255, 0.06);
  z-index: 10;
}

.nav-item {
  background: none;
  border: none;
  color: #94a3b8;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 0.25rem;
  cursor: pointer;
  transition: var(--transition-normal);
  flex: 1;
}

.nav-item:hover {
  color: white;
}

.nav-item i {
  width: 20px;
  height: 20px;
  stroke-width: 2.2px;
}

.nav-item span {
  font-size: 0.6rem;
  font-weight: 700;
  letter-spacing: 0.05em;
}

/* Nav Item Active */
.nav-item.active {
  color: var(--color-active-blue);
}

/* Custom full-width fan row */
.fan-box-full {
  background-color: #e2e8f0;
  border: 1.5px solid #cbd5e1;
  border-radius: 14px;
  padding: 0.75rem;
  box-shadow: inset 0 1px 2px rgba(255, 255, 255, 0.4);
}
.fan-box-full h3 {
  font-size: 0.7rem;
  font-weight: 800;
  color: var(--text-muted);
  margin-bottom: 0.625rem;
  display: flex;
  align-items: center;
  gap: 0.25rem;
  text-transform: uppercase;
}
.fan-row-full {
  display: flex;
  gap: 0.5rem;
  width: 100%;
}
.fan-row-full .btn-fan-mock {
  flex: 1;
}

/* Custom swing box in adjustments */
.swing-box-ajustes {
  background-color: var(--bg-card);
  border: 1px solid var(--border-light);
  border-radius: 16px;
  padding: 1.25rem;
  box-shadow: var(--shadow-out);
}
.swing-box-ajustes h3 {
  font-size: 0.7rem;
  font-weight: 800;
  color: var(--text-muted);
  margin-bottom: 0.75rem;
  display: flex;
  align-items: center;
  gap: 0.25rem;
  text-transform: uppercase;
}
.swing-buttons-row {
  display: flex;
  gap: 0.5rem;
  width: 100%;
}
.swing-buttons-row .btn-swing-mock {
  flex: 1;
  padding: 0.875rem 0.5rem;
}

/* Custom device info card in adjustments */
.device-info-card {
  background-color: var(--bg-card);
  border: 1px solid var(--border-light);
  border-radius: 16px;
  padding: 1.25rem;
  box-shadow: var(--shadow-out);
  display: flex;
  flex-direction: column;
  gap: 0.625rem;
}


</style>
</head>
<body>

  <div class="app-container">
    
    <!-- HEADER -->
    <header class="app-header">
      <button class="btn-back" aria-label="Voltar">
        <i data-lucide="chevron-left"></i>
      </button>
      <h1>TAC09CHSD AC</h1>
      <div class="header-placeholder"></div>
    </header>

    <!-- CONTEÚDO PRINCIPAL (COM ABAS) -->
    <main class="app-content">

      <!-- ABA 1: INÍCIO (PAINEL PRINCIPAL) -->
      <div id="tab-inicio" class="tab-pane active">
        
        <!-- Cards de Leitura Rápida -->
        <div class="info-row-top">
          <div class="info-card">
            <i data-lucide="home" class="info-icon"></i>
            <span class="info-label">AMBIENTE:</span>
            <span class="info-val" id="room-temp">--</span>
            <span class="info-unit">&deg;C</span>
          </div>
          <div class="info-card">
            <i data-lucide="map-pin" class="info-icon"></i>
            <span class="info-label">JD. ITÁLIA:</span>
            <span class="info-val" id="cuiaba-temp">--</span>
            <span class="info-unit">&deg;C</span>
          </div>
        </div>

        <!-- Widget Central de Temperatura Alvo -->
        <div class="temp-screen-card">
          <div class="screen-status-indicator">
            <span class="led-dot" id="ac-led-dot"></span>
          </div>
          
          <div class="temp-control-inner">
            <button class="btn-dial" onclick="adjustTemp(-0.5)" aria-label="Diminuir Temperatura">
              <i data-lucide="minus"></i>
            </button>
            <div class="temp-display-value">
              <span id="target-temp">22</span>
              <span class="degree-symbol">&deg;C</span>
            </div>
            <button class="btn-dial" onclick="adjustTemp(0.5)" aria-label="Aumentar Temperatura">
              <i data-lucide="plus"></i>
            </button>
          </div>
        </div>

        <!-- Grade de Modos de Operação -->
        <div class="modes-row">
          <button class="btn-mode-rect" id="mode-cool" onclick="sendCmd('MODE_COOL')">
            <i data-lucide="snowflake"></i>
            <span>FRIO</span>
          </button>
          <button class="btn-mode-rect" id="mode-heat" onclick="sendCmd('MODE_HEAT')">
            <i data-lucide="sun"></i>
            <span>CALOR</span>
          </button>
          <button class="btn-mode-rect" id="mode-dry" onclick="sendCmd('MODE_DRY')">
            <i data-lucide="droplet"></i>
            <span>SECO</span>
          </button>
          <button class="btn-mode-rect" id="mode-fan" onclick="sendCmd('MODE_FAN')">
            <i data-lucide="fan"></i>
            <span>VENT.</span>
          </button>
          <button class="btn-mode-rect" id="mode-auto" onclick="sendCmd('MODE_AUTO')">
            <i data-lucide="sparkles"></i>
            <span>AUTO</span>
          </button>
        </div>

        <!-- Velocidade da Ventoinha (Largura Total) -->
        <div class="control-box fan-box-full">
          <h3><i data-lucide="wind" class="box-title-icon"></i> FAN VELOCIDADE</h3>
          <div class="fan-row-full">
            <button class="btn-fan-mock" id="fan-auto" onclick="sendCmd('FAN_AUTO')">AUTO</button>
            <button class="btn-fan-mock" id="fan-min" onclick="sendCmd('FAN_MIN')">SILEN.</button>
            <button class="btn-fan-mock" id="fan-low" onclick="sendCmd('FAN_LOW')">BAIXA</button>
            <button class="btn-fan-mock" id="fan-med" onclick="sendCmd('FAN_MED')">MÉDIA</button>
            <button class="btn-fan-mock" id="fan-high" onclick="sendCmd('FAN_HIGH')">ALTA</button>
          </div>
        </div>

        <!-- Funções Especiais -->
        <div class="specials-row">
          <button class="btn-special-rect" id="btn-eco" onclick="toggleSpecial('eco')">
            <i data-lucide="leaf"></i>
            <span>ECO</span>
          </button>
          <button class="btn-special-rect" id="btn-health" onclick="toggleSpecial('health')">
            <i data-lucide="shield"></i>
            <span>SAÚDE</span>
          </button>
          <button class="btn-special-rect" id="btn-turbo" onclick="toggleSpecial('turbo')">
            <i data-lucide="zap"></i>
            <span>TURBO</span>
          </button>
          <button class="btn-special-rect" id="btn-light" onclick="toggleSpecial('light')">
            <i data-lucide="lightbulb"></i>
            <span>LUZ</span>
          </button>
        </div>

        <!-- Botão Ligar/Desligar Grande Vermelho -->
        <div class="power-button-container">
          <button class="btn-power-large" id="btn-power" onclick="togglePower()">
            <i data-lucide="power"></i>
            <span>LIGAR / DESLIGAR</span>
          </button>
        </div>

      </div>

      <!-- ABA 2: TIMER -->
      <div id="tab-timer" class="tab-pane">
        <div class="tab-title">
          <h2>Temporizadores</h2>
          <p>Agende o ligamento ou desligamento automático do aparelho.</p>
        </div>

        <div class="card-timer-container">
          <div class="timer-card-modern">
            <div class="timer-card-header">
              <i data-lucide="play-circle" class="timer-card-icon green"></i>
              <h3>LIGAR APARELHO EM:</h3>
            </div>
            <div class="timer-card-body">
              <span class="timer-display-time" id="timer-on-val">Desativado</span>
              <div class="timer-adjust-row">
                <button class="btn-adjust" onclick="adjustTimer('ON', -20)">-20m</button>
                <button class="btn-adjust" onclick="adjustTimer('ON', 20)">+20m</button>
                <button class="btn-clear-timer" onclick="clearTimer('ON')"><i data-lucide="trash-2"></i></button>
              </div>
            </div>
          </div>

          <div class="timer-card-modern">
            <div class="timer-card-header">
              <i data-lucide="stop-circle" class="timer-card-icon red"></i>
              <h3>DESLIGAR APARELHO EM:</h3>
            </div>
            <div class="timer-card-body">
              <span class="timer-display-time" id="timer-off-val">Desativado</span>
              <div class="timer-adjust-row">
                <button class="btn-adjust" onclick="adjustTimer('OFF', -20)">-20m</button>
                <button class="btn-adjust" onclick="adjustTimer('OFF', 20)">+20m</button>
                <button class="btn-clear-timer" onclick="clearTimer('OFF')"><i data-lucide="trash-2"></i></button>
              </div>
            </div>
          </div>
        </div>
      </div>

      <!-- ABA 3: AJUSTES -->
      <div id="tab-ajustes" class="tab-pane">
        <div class="tab-title">
          <h2>Termostato Inteligente</h2>
          <p>Automação por software para manter a temperatura do quarto estável.</p>
        </div>

        <div class="thermostat-card-modern">
          <div class="therm-status-header">
            <div class="therm-status-info">
              <span class="lbl">Modo Termostato:</span>
              <span class="badge" id="thermostat-mode-badge">DESLIGADO</span>
            </div>
            <div class="therm-status-info">
              <span class="lbl">Alvo Definido:</span>
              <span class="val" id="thermostat-target-val">-- &deg;C</span>
            </div>
          </div>
          
          <div class="therm-status-message">
            <span class="lbl">Ação Atual:</span>
            <span class="status-desc" id="thermostat-status-desc">Aguardando</span>
          </div>

          <div class="therm-adjust-section">
            <span class="section-label">Definir Temperatura Alvo</span>
            <div class="btn-group-auto">
              <button class="btn-auto-temp" id="btn-alvo-18" onclick="sendCmd('ALVO_18')">18&deg;</button>
              <button class="btn-auto-temp" id="btn-alvo-22" onclick="sendCmd('ALVO_22')">22&deg;</button>
              <button class="btn-auto-temp" id="btn-alvo-23" onclick="sendCmd('ALVO_23')">23&deg;</button>
              <button class="btn-auto-temp" id="btn-alvo-24" onclick="sendCmd('ALVO_24')">24&deg;</button>
              <button class="btn-auto-temp" id="btn-alvo-25" onclick="sendCmd('ALVO_25')">25&deg;</button>
            </div>
          </div>

          <div class="btn-auto-toggle-row">
            <button class="btn-auto-on" id="btn-auto-on" onclick="sendCmd('AUTO_ON')">Ativar Termostato</button>
            <button class="btn-auto-off" id="btn-auto-off" onclick="sendCmd('AUTO_OFF')">Desativar</button>
          </div>
        </div>

        <!-- Oscilação (Movida da aba Início) -->
        <div class="control-box swing-box-ajustes">
          <h3><i data-lucide="sliders" class="box-title-icon"></i> OSCILAÇÃO DO VENTO</h3>
          <div class="swing-buttons-row">
            <button class="btn-swing-mock" id="btn-swingv" onclick="toggleSwingV()">
              <i data-lucide="arrow-up-down"></i>
              VERT (VERTICAL)
            </button>
            <button class="btn-swing-mock" id="btn-swingh" onclick="toggleSwingH()">
              <i data-lucide="arrow-left-right"></i>
              HORIZ (LATERAL)
            </button>
          </div>
        </div>

        <!-- Informações de Conexão -->
        <div class="device-info-card">
          <div class="info-row-new">
            <span>Status da Conexão:</span>
            <span class="badge" id="conn-text-badge">CONECTADO</span>
          </div>
          <div class="info-row-new">
            <span>ID do Dispositivo:</span>
            <span class="value-small">TAC09CHSD_0921</span>
          </div>
        </div>

        <!-- Histórico Gráfico -->
        <div class="chart-card-modern">
          <h3><i data-lucide="trending-up" class="box-title-icon"></i> HISTÓRICO DE TEMPERATURA</h3>
          <div class="chart-wrapper">
            <canvas id="tempChart"></canvas>
          </div>
        </div>
      </div>

    </main>

    <!-- NAVEGAÇÃO INFERIOR -->
    <nav class="app-navbar">
      <button class="nav-item active" id="nav-inicio" onclick="switchTab('inicio')">
        <i data-lucide="home"></i>
        <span>INÍCIO</span>
      </button>
      <button class="nav-item" id="nav-timer" onclick="switchTab('timer')">
        <i data-lucide="calendar"></i>
        <span>TIMER</span>
      </button>
      <button class="nav-item" id="nav-ajustes" onclick="switchTab('ajustes')">
        <i data-lucide="settings"></i>
        <span>AJUSTES</span>
      </button>
    </nav>

  </div>

  <script>
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

</script>
</body>
</html>

)rawliteral";

#endif // WEBPAGE_H
