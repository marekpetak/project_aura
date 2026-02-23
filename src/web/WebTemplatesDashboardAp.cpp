// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#include "web/WebTemplates.h"

namespace WebTemplates {

const char kDashboardPageTemplateAp[] PROGMEM = R"HTML_DASH_AP(
<!doctype html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Aura Dashboard (AP Offline)</title>
  <style>
    :root {
      --bg: #0b1020;
      --panel: #121a2f;
      --panel-2: #0f1629;
      --text: #e8edf7;
      --muted: #9aa7c0;
      --accent: #3dd6c6;
      --warn: #f59e0b;
      --danger: #ef4444;
      --ok: #22c55e;
      --border: #24314f;
    }
    * { box-sizing: border-box; }
    body {
      margin: 0;
      padding: 16px;
      background: radial-gradient(1200px 600px at 100% -20%, #17244a, var(--bg));
      color: var(--text);
      font-family: "Segoe UI", Roboto, Arial, sans-serif;
    }
    .wrap { max-width: 1100px; margin: 0 auto; }
    .top {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 12px;
      margin-bottom: 14px;
      flex-wrap: wrap;
    }
    .title { font-size: 22px; font-weight: 700; letter-spacing: 0.02em; }
    .subtitle { font-size: 12px; color: var(--muted); margin-top: 2px; }
    .actions { display: flex; gap: 8px; flex-wrap: wrap; }
    button, .link-btn {
      border: 1px solid var(--border);
      background: var(--panel);
      color: var(--text);
      border-radius: 10px;
      padding: 9px 12px;
      font-size: 13px;
      cursor: pointer;
      text-decoration: none;
    }
    button:hover, .link-btn:hover { border-color: #37518a; }
    .status {
      border: 1px solid #355f8e;
      background: #0e233d;
      color: #b8d7ff;
      border-radius: 10px;
      padding: 10px 12px;
      margin-bottom: 14px;
      font-size: 13px;
      line-height: 1.4;
    }
    .status.warn {
      border-color: #7f5a1e;
      background: #2a2110;
      color: #ffd89a;
    }
    .grid {
      display: grid;
      grid-template-columns: repeat(auto-fill, minmax(150px, 1fr));
      gap: 10px;
      margin-bottom: 14px;
    }
    .card {
      background: linear-gradient(180deg, var(--panel), var(--panel-2));
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 10px;
    }
    .card .k { color: var(--muted); font-size: 11px; text-transform: uppercase; letter-spacing: 0.08em; }
    .card .v { margin-top: 6px; font-size: 22px; font-weight: 700; line-height: 1.1; }
    .card .u { color: var(--muted); font-size: 12px; margin-left: 5px; }
    .sec {
      background: linear-gradient(180deg, var(--panel), var(--panel-2));
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 12px;
      margin-bottom: 12px;
    }
    .sec h3 {
      margin: 0 0 10px;
      font-size: 14px;
      color: var(--muted);
      text-transform: uppercase;
      letter-spacing: 0.08em;
      font-weight: 700;
    }
    .charts {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
      gap: 10px;
    }
    .chart-box {
      border: 1px solid var(--border);
      border-radius: 10px;
      background: #0c1428;
      padding: 8px;
    }
    .chart-head {
      display: flex;
      justify-content: space-between;
      align-items: baseline;
      margin-bottom: 6px;
    }
    .chart-name { font-size: 13px; font-weight: 600; }
    .chart-latest { font-size: 12px; color: var(--muted); }
    svg { width: 100%; height: 74px; display: block; }
    .events { display: grid; gap: 7px; }
    .ev {
      border: 1px solid var(--border);
      border-radius: 8px;
      background: #0c1428;
      padding: 8px 10px;
      font-size: 13px;
    }
    .ev-top {
      display: flex;
      justify-content: space-between;
      gap: 10px;
      color: var(--muted);
      font-size: 11px;
      margin-bottom: 4px;
    }
    .ev-msg { line-height: 1.35; }
    .ev.critical { border-color: #7f1d1d; }
    .ev.warning { border-color: #7f5a1e; }
    .meta {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(190px, 1fr));
      gap: 6px 12px;
      font-size: 13px;
    }
    .meta b { color: var(--muted); font-weight: 600; margin-right: 6px; }
    .muted { color: var(--muted); }
    .ok { color: var(--ok); }
    .warn-c { color: var(--warn); }
    .danger { color: var(--danger); }
  </style>
</head>
<body>
  <div class="wrap">
    <div class="top">
      <div>
        <div class="title">Aura Dashboard</div>
        <div class="subtitle">AP offline mode (no CDN dependencies)</div>
      </div>
      <div class="actions">
        <button id="refreshBtn" type="button">Refresh</button>
        <a class="link-btn" href="/">WiFi Setup</a>
      </div>
    </div>

    <div id="status" class="status">Loading...</div>

    <div id="metrics" class="grid"></div>

    <div class="sec">
      <h3>Core Charts (24h)</h3>
      <div id="charts" class="charts"></div>
    </div>

    <div class="sec">
      <h3>Recent Events</h3>
      <div id="events" class="events muted">Loading...</div>
    </div>

    <div class="sec">
      <h3>Device</h3>
      <div id="deviceMeta" class="meta muted">Loading...</div>
    </div>
  </div>

  <script>
    const METRICS = [
      { key: "co2", label: "CO2", unit: "ppm", digits: 0 },
      { key: "temp", label: "Temperature", unit: "C", digits: 1 },
      { key: "rh", label: "Humidity", unit: "%", digits: 0 },
      { key: "pressure", label: "Pressure", unit: "hPa", digits: 0 },
      { key: "pm25", label: "PM2.5", unit: "ug/m3", digits: 1 },
      { key: "pm10", label: "PM10", unit: "ug/m3", digits: 1 },
      { key: "pm1", label: "PM1.0", unit: "ug/m3", digits: 1 },
      { key: "pm4", label: "PM4.0", unit: "ug/m3", digits: 1 },
      { key: "pm05", label: "PM0.5", unit: "#/cm3", digits: 0 },
      { key: "voc", label: "VOC", unit: "idx", digits: 0 },
      { key: "nox", label: "NOx", unit: "idx", digits: 0 },
      { key: "hcho", label: "HCHO", unit: "ppb", digits: 0 },
      { key: "co", label: "CO", unit: "ppm", digits: 1 }
    ];

    const CHARTS = [
      { key: "co2", label: "CO2", unit: "ppm", color: "#3dd6c6", digits: 0 },
      { key: "temperature", label: "Temperature", unit: "C", color: "#f59e0b", digits: 1 },
      { key: "humidity", label: "Humidity", unit: "%", color: "#60a5fa", digits: 0 },
      { key: "pressure", label: "Pressure", unit: "hPa", color: "#22c55e", digits: 0 }
    ];

    function $(id) { return document.getElementById(id); }

    function fmt(value, digits) {
      if (typeof value !== "number" || !Number.isFinite(value)) return "--";
      return value.toFixed(digits);
    }

    function esc(value) {
      const str = String(value == null ? "" : value);
      return str
        .replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;")
        .replace(/"/g, "&quot;")
        .replace(/'/g, "&#39;");
    }

    function statusText(text, mode) {
      const el = $("status");
      el.textContent = text;
      el.className = mode === "warn" ? "status warn" : "status";
    }

    async function getJson(url) {
      const response = await fetch(url, { cache: "no-store" });
      if (!response.ok) throw new Error("HTTP " + response.status + " for " + url);
      return response.json();
    }

    function renderMetrics(statePayload) {
      const sensors = (statePayload && statePayload.sensors) || {};
      const html = METRICS.map((m) => {
        const val = sensors[m.key];
        return (
          '<div class="card">' +
            '<div class="k">' + esc(m.label) + "</div>" +
            '<div class="v">' + fmt(val, m.digits) + '<span class="u">' + m.unit + "</span></div>" +
          "</div>"
        );
      }).join("");
      $("metrics").innerHTML = html || '<div class="muted">No metrics</div>';
    }

    function polylineFromValues(values, width, height, pad) {
      const good = values.filter((v) => typeof v === "number" && Number.isFinite(v));
      if (!good.length) return "";
      let min = Math.min.apply(null, good);
      let max = Math.max.apply(null, good);
      if (max <= min) {
        max = min + 1;
      }
      const span = max - min;
      const last = values.length - 1;
      const pts = [];
      for (let i = 0; i < values.length; i++) {
        const v = values[i];
        if (typeof v !== "number" || !Number.isFinite(v)) continue;
        const x = pad + (last > 0 ? (i / last) * (width - pad * 2) : (width / 2));
        const y = height - pad - ((v - min) / span) * (height - pad * 2);
        pts.push(x.toFixed(1) + "," + y.toFixed(1));
      }
      return pts.join(" ");
    }

    function renderCharts(chartsPayload) {
      const timestamps = Array.isArray(chartsPayload && chartsPayload.timestamps) ? chartsPayload.timestamps : [];
      const series = Array.isArray(chartsPayload && chartsPayload.series) ? chartsPayload.series : [];
      const map = {};
      series.forEach((s) => { if (s && typeof s.key === "string") map[s.key] = s; });

      const width = 320;
      const height = 74;
      const pad = 6;

      const html = CHARTS.map((c) => {
        const s = map[c.key];
        const values = (s && Array.isArray(s.values)) ? s.values : [];
        const latest = (s && typeof s.latest === "number" && Number.isFinite(s.latest)) ? s.latest : null;
        const points = polylineFromValues(values, width, height, pad);
        const latestTs = timestamps.length ? timestamps[timestamps.length - 1] : null;
        const latestLabel = (typeof latestTs === "number" && Number.isFinite(latestTs))
          ? new Date(latestTs * 1000).toLocaleTimeString([], { hour: "2-digit", minute: "2-digit", hour12: false })
          : "--:--";

        return (
          '<div class="chart-box">' +
            '<div class="chart-head">' +
              '<div class="chart-name">' + esc(c.label) + "</div>" +
              '<div class="chart-latest">' + fmt(latest, c.digits) + " " + c.unit + " @ " + latestLabel + "</div>" +
            "</div>" +
            '<svg viewBox="0 0 ' + width + " " + height + '" preserveAspectRatio="none">' +
              '<polyline fill="none" stroke="' + c.color + '" stroke-width="2.2" points="' + points + '"></polyline>' +
            "</svg>" +
          "</div>"
        );
      }).join("");

      $("charts").innerHTML = html || '<div class="muted">No chart data</div>';
    }

    function renderEvents(eventsPayload) {
      const events = Array.isArray(eventsPayload && eventsPayload.events) ? eventsPayload.events : [];
      if (!events.length) {
        $("events").innerHTML = '<div class="muted">No events</div>';
        return;
      }
      const html = events.slice(0, 12).map((e) => {
        const sev = String(e && e.severity ? e.severity : "info");
        const levelClass = (sev === "critical" || sev === "danger") ? "critical" : (sev === "warning" ? "warning" : "");
        const type = e && e.type ? e.type : "SYSTEM";
        const msg = e && e.message ? e.message : "Event";
        const ts = (typeof e.ts_ms === "number" && Number.isFinite(e.ts_ms))
          ? Math.floor(e.ts_ms / 1000) + "s"
          : "--";
        return (
          '<div class="ev ' + levelClass + '">' +
            '<div class="ev-top"><span>' + esc(type) + "</span><span>" + esc(sev) + " / " + esc(ts) + "</span></div>" +
            '<div class="ev-msg">' + esc(msg) + "</div>" +
          "</div>"
        );
      }).join("");
      $("events").innerHTML = html;
    }

    function renderMeta(statePayload) {
      const network = (statePayload && statePayload.network) || {};
      const system = (statePayload && statePayload.system) || {};
      const derived = (statePayload && statePayload.derived) || {};
      const mode = network.mode || "--";
      const mqttConnected = network.mqtt_connected === true;

      const entries = [
        ["Mode", mode],
        ["SSID", network.wifi_ssid || "--"],
        ["IP", network.ip || "--"],
        ["Hostname", network.hostname || "--"],
        ["MQTT", mqttConnected ? "Connected" : "Disconnected"],
        ["Firmware", system.firmware || "--"],
        ["Build", [system.build_date, system.build_time].filter(Boolean).join(" ") || "--"],
        ["Uptime", system.uptime || derived.uptime || "--"],
      ];

      $("deviceMeta").innerHTML = entries.map((row) => {
        const value = String(row[1]);
        const valueClass = (row[0] === "MQTT")
          ? (mqttConnected ? "ok" : "danger")
          : "";
        return '<div><b>' + esc(row[0]) + ":</b><span class=\"" + valueClass + "\">" + esc(value) + "</span></div>";
      }).join("");
    }

    let updating = false;
    async function refreshAll() {
      if (updating) return;
      updating = true;
      statusText("Refreshing data...", "");
      let hadError = false;
      try {
        const state = await getJson("/api/state");
        renderMetrics(state);
        renderMeta(state);
      } catch (err) {
        hadError = true;
        statusText("State API error: " + (err && err.message ? err.message : "unknown"), "warn");
      }

      try {
        const charts = await getJson("/api/charts?group=core&window=24h");
        renderCharts(charts);
      } catch (err) {
        hadError = true;
        statusText("Charts API error: " + (err && err.message ? err.message : "unknown"), "warn");
      }

      try {
        const events = await getJson("/api/events");
        renderEvents(events);
      } catch (err) {
        hadError = true;
        statusText("Events API error: " + (err && err.message ? err.message : "unknown"), "warn");
      }

      const now = new Date();
      if (!hadError) {
        statusText(
          "AP offline dashboard active. Last update: " +
            now.toLocaleTimeString([], { hour: "2-digit", minute: "2-digit", second: "2-digit", hour12: false }),
          ""
        );
      }
      updating = false;
    }

    $("refreshBtn").addEventListener("click", refreshAll);
    refreshAll();
    setInterval(refreshAll, 10000);
  </script>
</body>
</html>
)HTML_DASH_AP";

} // namespace WebTemplates
