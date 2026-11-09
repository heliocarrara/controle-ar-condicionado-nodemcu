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
  <title>Casa IR</title>
  <style>
/* Casa IR - Theme CSS (zero external dependencies) */

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
  --font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
  
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

.header-placeholder {
  width: 32px;
}

.app-content {
  flex: 1;
  overflow-y: auto;
  padding: 1rem;
  padding-bottom: 1.5rem;
}

.device-switch {
  display: flex;
  gap: 0.5rem;
  margin-bottom: 0.875rem;
  padding: 0.25rem;
  border-radius: 14px;
  background: #e2e8f0;
}

.device-switch-btn {
  flex: 1;
  min-height: 42px;
  border: 0;
  border-radius: 11px;
  background: transparent;
  color: var(--text-muted);
  font: inherit;
  font-size: 0.8rem;
  font-weight: 700;
  cursor: pointer;
}

.device-switch-btn.active {
  color: #fff;
  background: var(--bg-dark-header);
  box-shadow: var(--shadow-out);
}

.device-panel[hidden],
.nav-item[hidden] {
  display: none !important;
}

.device-panel {
  display: flex;
  flex-direction: column;
  gap: 0.875rem;
}

.tv-remote-card {
  display: flex;
  flex-direction: column;
  gap: 1.25rem;
  padding: 1rem;
  border: 1px solid var(--border-light);
  border-radius: 18px;
  background: var(--bg-card);
  box-shadow: var(--shadow-out);
}

.tv-remote-heading {
  display: flex;
  align-items: center;
  gap: 0.75rem;
}

.tv-screen-icon { font-size: 1.8rem; }
.tv-remote-heading h2 { font-size: 1.05rem; }
.tv-remote-heading p, .tv-note { color: var(--text-muted); font-size: 0.75rem; }
.tv-status-dot { width: 10px; height: 10px; margin-left: auto; border-radius: 50%; background: #10b981; }

.tv-power-button {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.6rem;
  min-height: 58px;
  border: 0;
  border-radius: 14px;
  background: var(--color-power-red);
  color: #fff;
  font: inherit;
  font-size: 0.9rem;
  font-weight: 800;
  cursor: pointer;
}

.tv-power-button span { font-size: 1.35rem; }
.tv-control-group { display: flex; flex-direction: column; gap: 0.55rem; }
.tv-control-label { color: var(--text-muted); font-size: 0.68rem; font-weight: 800; letter-spacing: 0.08em; }
.tv-control-row { display: flex; align-items: center; justify-content: center; gap: 0.75rem; }

.tv-control-button {
  min-width: 54px;
  min-height: 48px;
  padding: 0.5rem 0.85rem;
  border: 1px solid var(--border-light);
  border-radius: 13px;
  background: #fff;
  color: var(--text-dark);
  font: inherit;
  font-size: 1.1rem;
  font-weight: 700;
  cursor: pointer;
  box-shadow: var(--shadow-button);
}

.tv-control-button-wide { flex: 1; font-size: 0.85rem; }
.tv-control-hint { min-width: 70px; text-align: center; color: var(--text-muted); font-size: 0.85rem; }
.tv-source-button { width: 100%; font-size: 0.85rem; }
.tv-note { line-height: 1.4; text-align: center; }

.tv-dpad {
  display: grid;
  grid-template-columns: repeat(3, 58px);
  justify-content: center;
  gap: 0.45rem;
}

.tv-dpad .tv-control-button { min-width: 58px; padding: 0.4rem; }
.tv-dpad .tv-ok-button { border-radius: 50%; background: #e2e8f0; }
.tv-action-row { flex-wrap: wrap; }
.tv-action-row .tv-control-button { flex: 1; min-width: 82px; font-size: 0.78rem; }

.tv-number-pad {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 0.5rem;
}

.tv-number-pad .tv-control-button { width: 100%; }

.tv-timer-card {
  display: flex;
  flex-direction: column;
  gap: 0.75rem;
  padding: 0.9rem;
  border: 1px solid var(--border-light);
  border-radius: 16px;
  background: #eef2ff;
}

.tv-timer-heading {
  display: flex;
  align-items: center;
  justify-content: space-between;
}

.tv-timer-heading h3 { font-size: 0.95rem; }
.tv-timer-heading p { margin-top: 0.2rem; color: #4338ca; font-size: 0.85rem; font-weight: 800; }
.tv-timer-icon { font-size: 1.5rem; }
.tv-timer-controls { display: grid; grid-template-columns: repeat(3, 1fr); gap: 0.4rem; }
.tv-timer-controls .tv-control-button { min-width: 0; padding: 0.5rem 0.25rem; font-size: 0.72rem; }
.tv-timer-controls .tv-timer-quick { background: #4338ca; color: #fff; border-color: #4338ca; }
.tv-timer-cancel { align-self: center; padding: 0.4rem 0.6rem; border: 0; background: transparent; color: #b91c1c; font: inherit; font-size: 0.8rem; font-weight: 700; cursor: pointer; }

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

.info-row-top {
  display: flex;
  gap: 0.5rem;
  width: 100%;
}

.info-card {
  flex: 1;
  background-color: var(--bg-card);
  border: 1px solid var(--border-light);
  border-radius: 12px;
  padding: 0.625rem 0.4rem;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.25rem;
  box-shadow: var(--shadow-button);
}

.info-label {
  font-size: 0.6rem;
  font-weight: 600;
  color: var(--text-muted);
  white-space: nowrap;
}

.info-val {
  font-size: 0.8rem;
  font-weight: 700;
  color: var(--text-dark);
}

.info-unit {
  font-size: 0.65rem;
  font-weight: 700;
  color: var(--text-muted);
}

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
  font-size: 1.5rem;
  font-weight: 700;
  font-family: var(--font-family);
}

.btn-dial:hover {
  background-color: rgba(0, 0, 0, 0.08);
}

.btn-dial:active {
  transform: scale(0.92);
  background-color: rgba(0, 0, 0, 0.12);
}

.dial-symbol {
  font-size: 1.8rem;
  font-weight: 700;
  line-height: 1;
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
  gap: 0.25rem;
  cursor: pointer;
  box-shadow: var(--shadow-button);
  transition: var(--transition-normal);
  font-family: var(--font-family);
}

.btn-mode-rect:hover {
  background-color: #f1f5f9;
}

.btn-mode-rect:active {
  box-shadow: var(--shadow-button-active);
}

.mode-icon {
  font-size: 1.2rem;
  line-height: 1;
}

.btn-mode-rect span:last-child {
  font-size: 0.6rem;
  font-weight: 700;
  color: var(--text-dark);
}

.btn-mode-rect.active {
  background-color: var(--color-active-blue);
  border-color: var(--color-active-blue);
}

.btn-mode-rect.active span:last-child {
  color: white;
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

.btn-swing-mock.active {
  background-color: var(--color-active-blue);
  border-color: var(--color-active-blue);
  color: white;
  box-shadow: var(--shadow-button-active);
}

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
  gap: 0.25rem;
  cursor: pointer;
  box-shadow: var(--shadow-button);
  transition: var(--transition-normal);
  font-family: var(--font-family);
}

.btn-special-rect:hover {
  background-color: #f1f5f9;
}

.btn-special-rect span:first-child {
  font-size: 1.1rem;
  line-height: 1;
}

.btn-special-rect span:last-child {
  font-size: 0.65rem;
  font-weight: 700;
  color: var(--text-dark);
}

.btn-special-rect.active {
  box-shadow: var(--shadow-button-active);
  background-color: #f1f5f9;
}

.btn-special-rect.active#btn-eco span:first-child { color: #10b981; }
.btn-special-rect.active#btn-health span:first-child { color: #14b8a6; }
.btn-special-rect.active#btn-turbo span:first-child { color: #f59e0b; }
.btn-special-rect.active#btn-light span:first-child { color: #eab308; }

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
  gap: 0.5rem;
  cursor: pointer;
  box-shadow: 0 4px 10px rgba(220, 38, 38, 0.35);
  transition: var(--transition-normal);
}

.btn-power-large:active {
  transform: scale(0.98);
  box-shadow: 0 2px 5px rgba(220, 38, 38, 0.2);
}

.btn-power-large span:first-child {
  font-size: 1.3rem;
}

/* Presets rápidos */
.presets-row {
  display: flex;
  gap: 0.375rem;
  width: 100%;
}

.btn-preset {
  flex: 1;
  background-color: var(--bg-card);
  border: 1px solid var(--border-light);
  border-radius: 10px;
  padding: 0.75rem 0.25rem;
  font-family: var(--font-family);
  font-size: 0.72rem;
  font-weight: 700;
  color: var(--text-dark);
  cursor: pointer;
  box-shadow: var(--shadow-button);
  transition: var(--transition-normal);
  text-align: center;
  line-height: 1.2;
}
.btn-preset:hover {
  background-color: #e2e8f0;
}
.btn-preset:active {
  box-shadow: var(--shadow-button-active);
}

/* RAM card with bar */
.ram-card {
  flex-direction: column;
  gap: 0.25rem;
  padding: 0.5rem 0.5rem;
}

.ram-info {
  display: flex;
  align-items: center;
  gap: 0.25rem;
  width: 100%;
}

.ram-bar-bg {
  width: 100%;
  height: 5px;
  background-color: #e2e8f0;
  border-radius: 4px;
  overflow: hidden;
}

.ram-bar-fill {
  height: 100%;
  background-color: var(--color-active-blue);
  border-radius: 4px;
  transition: width 0.5s ease;
  width: 0%;
}

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
  font-family: var(--font-family);
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
  font-size: 1rem;
  font-weight: 700;
}
.btn-clear-timer:hover { background-color: rgba(239, 68, 68, 0.2); }

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
  font-family: var(--font-family);
}

.nav-item:hover {
  color: white;
}

.nav-icon {
  font-size: 1.1rem;
  line-height: 1;
}

.nav-item span:last-child {
  font-size: 0.6rem;
  font-weight: 700;
  letter-spacing: 0.05em;
}

.nav-item.active {
  color: var(--color-active-blue);
}

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

.btn-restart {
  width: 100%;
  background-color: rgba(239, 68, 68, 0.08);
  border: 1px solid rgba(239, 68, 68, 0.2);
  color: var(--color-power-red);
  padding: 0.75rem;
  border-radius: 10px;
  font-weight: 700;
  font-size: 0.85rem;
  cursor: pointer;
  font-family: var(--font-family);
  transition: var(--transition-normal);
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.4rem;
}
.btn-restart:hover {
  background-color: rgba(239, 68, 68, 0.15);
}
.btn-restart:active {
  transform: scale(0.97);
}

</style>
</head>
<body>

  <div class="app-container">
    
    <!-- HEADER -->
    <header class="app-header">
      <div class="header-placeholder"></div>
      <h1>CASA IR</h1>
      <div class="header-placeholder"></div>
    </header>

    <!-- CONTEÚDO PRINCIPAL (COM ABAS) -->
    <main class="app-content">

      <div class="device-switch" role="tablist" aria-label="Selecionar aparelho">
        <button class="device-switch-btn active" id="device-select-ac" onclick="switchDevice('ac')" role="tab" aria-selected="true">❄ Ar-condicionado</button>
        <button class="device-switch-btn" id="device-select-tv" onclick="switchDevice('tv')" role="tab" aria-selected="false">📺 Televisão</button>
      </div>

      <!-- ABA 1: INÍCIO -->
      <div id="tab-inicio" class="tab-pane active">
        
        <!-- Cards de Leitura Rápida -->
        <div class="info-row-top">
          <div class="info-card">
            <span class="info-label">AMBIENTE:</span>
            <span class="info-val" id="room-temp">--</span>
            <span class="info-unit">&deg;C</span>
          </div>
          <div class="info-card">
            <span class="info-label">CRISTO REI:</span>
            <span class="info-val" id="cristorei-temp">--</span>
            <span class="info-unit">&deg;C</span>
          </div>
          <div class="info-card ram-card">
            <div class="ram-info">
              <span class="info-label">RAM:</span>
              <span class="info-val" id="ram-pct">--</span>
              <span class="info-unit">% usado</span>
            </div>
            <div class="ram-bar-bg">
              <div class="ram-bar-fill" id="ram-bar"></div>
            </div>
          </div>
        </div>

        <section class="device-panel" id="device-panel-ac" aria-label="Controle do ar-condicionado">
        <!-- Widget Central de Temperatura Alvo -->
        <div class="temp-screen-card">
          <div class="screen-status-indicator">
            <span class="led-dot" id="ac-led-dot"></span>
          </div>
          
          <div class="temp-control-inner">
            <button class="btn-dial" onclick="adjustTemp(-1)" aria-label="Diminuir Temperatura">
              <span class="dial-symbol">&minus;</span>
            </button>
            <div class="temp-display-value">
              <span id="target-temp">22</span>
              <span class="degree-symbol">&deg;C</span>
            </div>
            <button class="btn-dial" onclick="adjustTemp(1)" aria-label="Aumentar Temperatura">
              <span class="dial-symbol">+</span>
            </button>
          </div>
        </div>

        <!-- Grade de Modos de Operação -->
        <div class="modes-row">
          <button class="btn-mode-rect" id="mode-cool" onclick="sendCmd('MODE_COOL')">
            <span class="mode-icon">❄</span>
            <span>FRIO</span>
          </button>
          <button class="btn-mode-rect" id="mode-heat" onclick="sendCmd('MODE_HEAT')">
            <span class="mode-icon">☀</span>
            <span>CALOR</span>
          </button>
          <button class="btn-mode-rect" id="mode-dry" onclick="sendCmd('MODE_DRY')">
            <span class="mode-icon">💧</span>
            <span>SECO</span>
          </button>
          <button class="btn-mode-rect" id="mode-fan" onclick="sendCmd('MODE_FAN')">
            <span class="mode-icon">🌀</span>
            <span>VENT.</span>
          </button>
          <button class="btn-mode-rect" id="mode-auto" onclick="sendCmd('MODE_AUTO')">
            <span class="mode-icon">⚡</span>
            <span>AUTO</span>
          </button>
        </div>

        <!-- Velocidade da Ventoinha -->
        <div class="control-box fan-box-full">
          <h3>💨 FAN VELOCIDADE</h3>
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
            <span>🌿</span>
            <span>ECO</span>
          </button>
          <button class="btn-special-rect" id="btn-health" onclick="toggleSpecial('health')">
            <span>🛡</span>
            <span>SAÚDE</span>
          </button>
          <button class="btn-special-rect" id="btn-turbo" onclick="toggleSpecial('turbo')">
            <span>⚡</span>
            <span>TURBO</span>
          </button>
          <button class="btn-special-rect" id="btn-light" onclick="toggleSpecial('light')">
            <span>💡</span>
            <span>LUZ</span>
          </button>
          <button class="btn-special-rect" id="btn-quiet" onclick="toggleSpecial('quiet')">
            <span>🔇</span>
            <span>SILENCIOSO</span>
          </button>
        </div>

        <!-- Botão Ligar/Desligar -->
        <div class="power-button-container">
          <button class="btn-power-large" id="btn-power" onclick="togglePower()">
            <span>⏻</span>
            <span>LIGAR / DESLIGAR</span>
          </button>
        </div>

        <!-- Presets Rápidos -->
        <div class="presets-row">
          <button class="btn-preset" onclick="sendCmd('P24_MED')">24°C Média</button>
          <button class="btn-preset" onclick="sendCmd('P23_MED')">23°C Média</button>
          <button class="btn-preset" onclick="sendCmd('P22_LOW')">22°C Baixa</button>
        </div>
        </section>

        <section class="device-panel tv-panel" id="device-panel-tv" aria-label="Controle da televisão Samsung" hidden>
          <div class="tv-remote-card">
            <div class="tv-remote-heading">
              <span class="tv-screen-icon">📺</span>
              <div>
                <h2>TV Samsung</h2>
                <p>Controle IR LE-7270</p>
              </div>
              <span class="tv-status-dot" aria-label="Pronta para enviar comandos"></span>
            </div>

            <button class="tv-power-button" onclick="sendCmd('TV_POWER')"><span>⏻</span> LIGAR / DESLIGAR</button>

            <div class="tv-control-group">
              <span class="tv-control-label">VOLUME</span>
              <div class="tv-control-row">
                <button class="tv-control-button" onclick="sendCmd('TV_VOLUME_DOWN')" aria-label="Diminuir volume">−</button>
                <button class="tv-control-button tv-control-button-wide" onclick="sendCmd('TV_MUTE')">🔇 Mudo</button>
                <button class="tv-control-button" onclick="sendCmd('TV_VOLUME_UP')" aria-label="Aumentar volume">+</button>
              </div>
            </div>

            <div class="tv-control-group">
              <span class="tv-control-label">CANAIS</span>
              <div class="tv-control-row">
                <button class="tv-control-button" onclick="sendCmd('TV_CHANNEL_DOWN')">− CH</button>
                <button class="tv-control-button tv-control-button-wide" onclick="sendCmd('TV_CHANNEL_LIST')">☷ CH LIST</button>
                <button class="tv-control-button" onclick="sendCmd('TV_CHANNEL_UP')">+ CH</button>
              </div>
            </div>

            <div class="tv-control-group">
              <span class="tv-control-label">NAVEGAÇÃO</span>
              <div class="tv-dpad">
                <span></span>
                <button class="tv-control-button" onclick="sendCmd('TV_UP')" aria-label="Para cima">▲</button>
                <span></span>
                <button class="tv-control-button" onclick="sendCmd('TV_LEFT')" aria-label="Para a esquerda">◀</button>
                <button class="tv-control-button tv-ok-button" onclick="sendCmd('TV_ENTER')">OK</button>
                <button class="tv-control-button" onclick="sendCmd('TV_RIGHT')" aria-label="Para a direita">▶</button>
                <span></span>
                <button class="tv-control-button" onclick="sendCmd('TV_DOWN')" aria-label="Para baixo">▼</button>
                <span></span>
              </div>
              <div class="tv-control-row tv-action-row">
                <button class="tv-control-button" onclick="sendCmd('TV_RETURN')">↩ Voltar</button>
                <button class="tv-control-button" onclick="sendCmd('TV_MENU')">☰ Menu</button>
                <button class="tv-control-button" onclick="sendCmd('TV_EXIT')">Sair</button>
              </div>
            </div>

            <div class="tv-control-group">
              <span class="tv-control-label">ATALHOS</span>
              <div class="tv-control-row">
                <button class="tv-control-button tv-control-button-wide" onclick="sendCmd('TV_TOOLS')">⚙ Tools</button>
                <button class="tv-control-button tv-control-button-wide" onclick="sendCmd('TV_SOURCE')">▣ Source</button>
              </div>
            </div>

            <div class="tv-control-group">
              <span class="tv-control-label">NÚMEROS</span>
              <div class="tv-number-pad">
                <button class="tv-control-button" onclick="sendCmd('TV_1')">1</button>
                <button class="tv-control-button" onclick="sendCmd('TV_2')">2</button>
                <button class="tv-control-button" onclick="sendCmd('TV_3')">3</button>
                <button class="tv-control-button" onclick="sendCmd('TV_4')">4</button>
                <button class="tv-control-button" onclick="sendCmd('TV_5')">5</button>
                <button class="tv-control-button" onclick="sendCmd('TV_6')">6</button>
                <button class="tv-control-button" onclick="sendCmd('TV_7')">7</button>
                <button class="tv-control-button" onclick="sendCmd('TV_8')">8</button>
                <button class="tv-control-button" onclick="sendCmd('TV_9')">9</button>
                <span></span>
                <button class="tv-control-button" onclick="sendCmd('TV_0')">0</button>
                <span></span>
              </div>
            </div>

            <div class="tv-timer-card">
              <div class="tv-timer-heading">
                <div>
                  <h3>Desligar automaticamente</h3>
                  <p id="tv-timer-countdown" aria-live="polite">Desativado</p>
                </div>
                <span class="tv-timer-icon">⏲</span>
              </div>
              <div class="tv-timer-controls">
                <button class="tv-control-button" onclick="adjustTvTimer(-30)" aria-label="Diminuir timer em 30 minutos">− 30 min</button>
                <button class="tv-control-button tv-timer-quick" onclick="setTvTimer(30)">30 min rápido</button>
                <button class="tv-control-button" onclick="adjustTvTimer(30)" aria-label="Aumentar timer em 30 minutos">+ 30 min</button>
              </div>
              <button class="tv-timer-cancel" onclick="cancelTvTimer()">Cancelar temporizador</button>
              <p class="tv-note">Ao chegar a zero, o Node envia o comando POWER por infravermelho. Como a TV não confirma o estado, deixe-a ligada e evite usar o botão POWER durante a contagem.</p>
            </div>
            <p class="tv-note">Usa códigos Samsung IR de 32 bits; a resposta será conferida na TV.</p>
          </div>
        </section>

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
              <h3>▶ LIGAR APARELHO EM:</h3>
            </div>
            <div class="timer-card-body">
              <span class="timer-display-time" id="timer-on-val">Desativado</span>
              <div class="timer-adjust-row">
                <button class="btn-adjust" onclick="adjustTimer('ON', -20)">-20m</button>
                <button class="btn-adjust" onclick="adjustTimer('ON', 20)">+20m</button>
                <button class="btn-clear-timer" onclick="clearTimer('ON')">✕</button>
              </div>
            </div>
          </div>

          <div class="timer-card-modern">
            <div class="timer-card-header">
              <h3>⏹ DESLIGAR APARELHO EM:</h3>
            </div>
            <div class="timer-card-body">
              <span class="timer-display-time" id="timer-off-val">Desativado</span>
              <div class="timer-adjust-row">
                <button class="btn-adjust" onclick="adjustTimer('OFF', -20)">-20m</button>
                <button class="btn-adjust" onclick="adjustTimer('OFF', 20)">+20m</button>
                <button class="btn-clear-timer" onclick="clearTimer('OFF')">✕</button>
              </div>
            </div>
          </div>
        </div>
      </div>

      <!-- ABA 3: AJUSTES -->
      <div id="tab-ajustes" class="tab-pane">
        <div class="tab-title">
          <h2>Ajustes do aparelho</h2>
          <p>Configure opções do ar-condicionado e da conexão.</p>
        </div>

        <!-- Oscilação -->
        <div class="control-box swing-box-ajustes">
          <h3>🌬 OSCILAÇÃO DO VENTO</h3>
          <div class="swing-buttons-row">
            <button class="btn-swing-mock" id="btn-swingv" onclick="toggleSwingV()">
              ⇅ VERT (VERTICAL)
            </button>
            <button class="btn-swing-mock" id="btn-swingh" onclick="toggleSwingH()">
              ⇄ HORIZ (LATERAL)
            </button>
          </div>
        </div>

        <!-- Informações de Conexão -->
        <div class="device-info-card">
          <div class="info-row-new">
            <span>Status da Conexão:</span>
            <span class="badge" id="conn-text-badge">CONECTADO</span>
          </div>
          <button class="btn-restart" onclick="sendCmd('RESTART')">🔄 Reiniciar NodeMCU</button>
        </div>

        <!-- Histórico Gráfico -->
        <div class="chart-card-modern">
          <h3>📈 HISTÓRICO DE TEMPERATURA (20 min)</h3>
          <div class="chart-wrapper">
            <canvas id="tempChart"></canvas>
          </div>
        </div>
      </div>

    </main>

    <!-- NAVEGAÇÃO INFERIOR -->
    <nav class="app-navbar">
      <button class="nav-item active" id="nav-inicio" onclick="switchTab('inicio')">
        <span class="nav-icon">🏠</span>
        <span>INÍCIO</span>
      </button>
      <button class="nav-item" id="nav-timer" onclick="switchTab('timer')">
        <span class="nav-icon">⏱</span>
        <span>TIMER</span>
      </button>
      <button class="nav-item" id="nav-ajustes" onclick="switchTab('ajustes')">
        <span class="nav-icon">⚙</span>
        <span>AJUSTES</span>
      </button>
    </nav>

  </div>

  <script>
// Casa IR - Frontend Controller Logic (zero external dependencies)

let currentAcState = {};
let isUpdating = false;
let currentDevice = 'ac';
let tvTimerActive = false;
let tvTimerRemainingSeconds = 0;

// Inicialização
window.onload = function() {
  updateStatus();
  fetchCristoReiWeather();
  
  // Loop de polling a cada 2 segundos
  setInterval(updateStatus, 2000);
  
  // Atualiza clima a cada 5 minutos
  setInterval(fetchCristoReiWeather, 300000);
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

window.switchDevice = function(device) {
  currentDevice = device === 'tv' ? 'tv' : 'ac';
  const isTv = currentDevice === 'tv';
  document.getElementById('device-panel-ac').hidden = isTv;
  document.getElementById('device-panel-tv').hidden = !isTv;
  document.getElementById('device-select-ac').classList.toggle('active', !isTv);
  document.getElementById('device-select-tv').classList.toggle('active', isTv);
  document.getElementById('device-select-ac').setAttribute('aria-selected', String(!isTv));
  document.getElementById('device-select-tv').setAttribute('aria-selected', String(isTv));

  // AC timers and settings do not apply to the TV.
  document.getElementById('nav-timer').hidden = isTv;
  document.getElementById('nav-ajustes').hidden = isTv;
  if (isTv && !document.getElementById('tab-inicio').classList.contains('active')) {
    window.switchTab('inicio');
  }
};

// Busca o clima atual de Cristo Rei, Várzea Grande, via Open-Meteo.
async function fetchCristoReiWeather() {
  try {
    const res = await fetch('https://api.open-meteo.com/v1/forecast?latitude=-15.640970&longitude=-56.104550&current_weather=true');
    if (res.ok) {
      const data = await res.json();
      if (data && data.current_weather) {
        const tempVal = Math.round(data.current_weather.temperature);
        document.getElementById('cristorei-temp').innerText = tempVal;
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

    tvTimerActive = Boolean(data.tv_off_timer_active);
    tvTimerRemainingSeconds = Number(data.tv_off_timer_remaining_seconds) || 0;
    renderTvTimer();
    
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

    // Atualiza o gráfico
    if (data.historico && data.historico.length > 0) {
      drawChart(data.historico);
    }

    // Badge de conexão
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
  toggleUiClass('btn-quiet', state.quiet);

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

function renderTvTimer() {
  const display = document.getElementById('tv-timer-countdown');
  if (!display) return;
  if (!tvTimerActive || tvTimerRemainingSeconds <= 0) {
    display.innerText = 'Desativado';
    return;
  }

  const totalMinutes = Math.ceil(tvTimerRemainingSeconds / 60);
  const hours = Math.floor(totalMinutes / 60);
  const minutes = totalMinutes % 60;
  const remaining = hours > 0 ? `${hours}h ${minutes}min` : `${minutes} min`;
  display.innerText = `Faltam ${remaining}`;
}

window.setTvTimer = function(minutes) {
  sendCmd(`TV_TIMER_SET_${minutes}`);
};

window.adjustTvTimer = function(deltaMinutes) {
  const currentMinutes = tvTimerActive ? Math.ceil(tvTimerRemainingSeconds / 60) : 0;
  const nextMinutes = Math.min(720, currentMinutes + deltaMinutes);
  if (nextMinutes <= 0) {
    window.cancelTvTimer();
    return;
  }
  window.setTvTimer(nextMinutes);
};

window.cancelTvTimer = function() {
  sendCmd('TV_TIMER_CANCEL');
};

window.togglePower = function() {
  sendCmd('TOGGLE_POWER');
};

window.adjustTemp = function(diff) {
  if (currentAcState.temp === undefined) return;
  let newTemp = Math.round(parseFloat(currentAcState.temp) + diff);
  if (newTemp < 16) newTemp = 16;
  if (newTemp > 31) newTemp = 31;
  sendCmd(`T${newTemp}`);
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
  const key = { 'turbo': 'turbo', 'eco': 'econo', 'health': 'health', 'light': 'light', 'quiet': 'quiet' }[name];
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

</script>
</body>
</html>

)rawliteral";

#endif // WEBPAGE_H
