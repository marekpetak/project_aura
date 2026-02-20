// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#include "web/WebTemplates.h"

namespace WebTemplates {

const char kDashboardPageTemplate[] PROGMEM = R"HTML_DASH(
<!doctype html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Aura Web Page Preview</title>
  <script>
    window.__auraDepFailed = [];
    window.__auraDepFail = function (name, src) {
      window.__auraDepFailed.push(name + ": " + src);
    };
  </script>
  <script src="https://cdn.tailwindcss.com"></script>
  <script crossorigin src="https://cdn.jsdelivr.net/npm/react@18/umd/react.development.js"
          onerror="if(!this.dataset.fb){this.dataset.fb='1';this.src='https://unpkg.com/react@18/umd/react.development.js';}else{window.__auraDepFail('react', this.src);}"></script>
  <script crossorigin src="https://cdn.jsdelivr.net/npm/react-dom@18/umd/react-dom.development.js"
          onerror="if(!this.dataset.fb){this.dataset.fb='1';this.src='https://unpkg.com/react-dom@18/umd/react-dom.development.js';}else{window.__auraDepFail('react-dom', this.src);}"></script>
  <script src="https://cdn.jsdelivr.net/npm/@babel/standalone@7.26.7/babel.min.js"
          onerror="if(!this.dataset.fb){this.dataset.fb='1';this.src='https://unpkg.com/@babel/standalone@7.26.7/babel.min.js';}else{window.__auraDepFail('babel', this.src);}"></script>
  <style>
    html, body { margin: 0; background: #030712; }
    #preview-error {
      display: none;
      position: fixed;
      inset: 12px;
      z-index: 9999;
      background: rgba(127, 29, 29, 0.95);
      color: #fee2e2;
      border: 1px solid #fca5a5;
      border-radius: 10px;
      padding: 12px;
      font: 12px/1.4 Consolas, "Courier New", monospace;
      white-space: pre-wrap;
      overflow: auto;
    }
  </style>
</head>
<body>
  <pre id="preview-error"></pre>
  <div id="root"></div>
  <script>
    function auraShowError(message) {
      var box = document.getElementById("preview-error");
      box.style.display = "block";
      box.textContent = message;
    }

    window.addEventListener("error", function (event) {
      // Ignore raw script resource load errors here; dependency checker handles those.
      if (!event.error && event.target && event.target.tagName === "SCRIPT") {
        return;
      }
      var where = "";
      if (event.filename) {
        where = "\nAt: " + event.filename + ":" + (event.lineno || 0) + ":" + (event.colno || 0);
      }
      auraShowError(
        "Preview runtime error:\n" +
        (event.error && event.error.stack ? event.error.stack : (event.message || "Unknown script error")) +
        where
      );
    });

    window.addEventListener("load", function () {
      setTimeout(function () {
        var missingCore = [];
        if (!window.React) missingCore.push("React");
        if (!window.ReactDOM) missingCore.push("ReactDOM");
        if (!window.Babel) missingCore.push("Babel");

        if (!missingCore.length) return;

        var hint = (navigator && navigator.onLine === false)
          ? "Phone appears offline. In AP mode, CDN scripts may be unreachable without internet."
          : "Some CDN scripts failed to load on this network/browser.";
        var failed = (window.__auraDepFailed && window.__auraDepFailed.length)
          ? ("\nFailed: " + window.__auraDepFailed.join(", "))
          : "";

        auraShowError(
          "Web dashboard dependencies failed to load.\n" +
          "Missing: " + missingCore.join(", ") + failed + "\n" +
          hint + "\n" +
          "Try home Wi-Fi with internet or another browser."
        );
      }, 700);
    });
  </script>
  <script type="text/babel" data-presets="react">
/*__INLINE_APP_START__*/
// Auto-generated from web-page.ini.
// Run .\sync-preview.ps1 after editing web-page.ini.
const { useState, useMemo, useEffect } = React;

const iconBaseProps = {
  fill: "none",
  stroke: "currentColor",
  strokeWidth: 2,
  strokeLinecap: "round",
  strokeLinejoin: "round",
  "aria-hidden": "true",
};

const svgIcon = (draw) => ({ size = 14, className = "" }) => (
  <svg viewBox="0 0 24 24" width={size} height={size} className={className} {...iconBaseProps}>
    {draw}
  </svg>
);

// Icon fallbacks for no-build local preview (replace text glyph stubs).
const Moon = svgIcon(<path d="M21 12.8A9 9 0 1 1 11.2 3a7 7 0 1 0 9.8 9.8z" />);
const Bell = svgIcon(<><path d="M18 15.5V11a6 6 0 0 0-12 0v4.5L4.5 17h15L18 15.5z" /><path d="M10 18a2 2 0 0 0 4 0" /></>);
const Sun = svgIcon(<><circle cx="12" cy="12" r="4" /><path d="M12 2v2.2M12 19.8V22M4.2 4.2l1.6 1.6M18.2 18.2l1.6 1.6M2 12h2.2M19.8 12H22M4.2 19.8l1.6-1.6M18.2 5.8l1.6-1.6" /></>);
const RotateCw = svgIcon(<><path d="M21 2v6h-6" /><path d="M3 12a9 9 0 0 1 15-6.7L21 8" /><path d="M3 22v-6h6" /><path d="M21 12a9 9 0 0 1-15 6.7L3 16" /></>);
const Plus = svgIcon(<path d="M12 5v14M5 12h14" />);
const Minus = svgIcon(<path d="M5 12h14" />);
const Pencil = svgIcon(<><path d="M12 20h9" /><path d="M16.5 3.5a2.1 2.1 0 1 1 3 3L7 19l-4 1 1-4 12.5-12.5z" /></>);
const Check = svgIcon(<path d="M20 6 9 17l-5-5" />);
const X = svgIcon(<path d="m6 6 12 12M18 6 6 18" />);

// ============ VISUAL PLACEHOLDERS (NO SENSOR LOGIC) ============
const SENSOR_PLACEHOLDER_ANCHORS = [
  { time: '00:00', co2: 640, temp: 21.3, rh: 52, pm05: 92, pm1: 4.2, pm25: 6.4, pm4: 7.6, pm10: 9.3, voc: 118, nox: 22, pressure: 1015.6, hcho: 18, co: 0.5, mold: 2.8 },
  { time: '02:00', co2: 660, temp: 21.1, rh: 53, pm05: 88, pm1: 4.4, pm25: 6.8, pm4: 8.1, pm10: 9.8, voc: 122, nox: 23, pressure: 1015.4, hcho: 18, co: 0.5, mold: 2.9 },
  { time: '04:00', co2: 700, temp: 20.9, rh: 54, pm05: 95, pm1: 4.7, pm25: 7.2, pm4: 8.6, pm10: 10.2, voc: 126, nox: 24, pressure: 1015.1, hcho: 19, co: 0.6, mold: 3.0 },
  { time: '06:00', co2: 760, temp: 21.0, rh: 55, pm05: 104, pm1: 5.1, pm25: 7.8, pm4: 9.2, pm10: 10.9, voc: 132, nox: 26, pressure: 1014.8, hcho: 20, co: 0.6, mold: 3.2 },
  { time: '08:00', co2: 820, temp: 21.7, rh: 50, pm05: 126, pm1: 5.8, pm25: 8.9, pm4: 10.3, pm10: 12.1, voc: 148, nox: 34, pressure: 1014.3, hcho: 22, co: 0.7, mold: 3.4 },
  { time: '10:00', co2: 870, temp: 22.4, rh: 47, pm05: 148, pm1: 6.4, pm25: 10.2, pm4: 11.8, pm10: 13.6, voc: 166, nox: 41, pressure: 1013.9, hcho: 24, co: 0.8, mold: 3.7 },
  { time: '12:00', co2: 940, temp: 23.1, rh: 45, pm05: 196, pm1: 8.1, pm25: 13.8, pm4: 15.4, pm10: 18.9, voc: 202, nox: 68, pressure: 1013.5, hcho: 31, co: 1.2, mold: 4.2 },
  { time: '14:00', co2: 980, temp: 23.4, rh: 44, pm05: 238, pm1: 9.0, pm25: 15.5, pm4: 17.2, pm10: 20.4, voc: 224, nox: 75, pressure: 1013.2, hcho: 34, co: 1.4, mold: 4.4 },
  { time: '16:00', co2: 930, temp: 23.0, rh: 46, pm05: 176, pm1: 7.4, pm25: 12.4, pm4: 14.1, pm10: 16.8, voc: 190, nox: 56, pressure: 1013.0, hcho: 29, co: 1.0, mold: 4.1 },
  { time: '18:00', co2: 890, temp: 22.6, rh: 48, pm05: 152, pm1: 6.7, pm25: 10.8, pm4: 12.5, pm10: 14.9, voc: 170, nox: 47, pressure: 1012.8, hcho: 25, co: 0.8, mold: 3.8 },
  { time: '20:00', co2: 860, temp: 22.1, rh: 50, pm05: 136, pm1: 6.0, pm25: 9.7, pm4: 11.1, pm10: 13.2, voc: 158, nox: 39, pressure: 1012.6, hcho: 23, co: 0.7, mold: 3.5 },
  { time: '22:00', co2: 830, temp: 21.8, rh: 51, pm05: 120, pm1: 5.4, pm25: 8.6, pm4: 9.9, pm10: 11.8, voc: 146, nox: 32, pressure: 1012.4, hcho: 21, co: 0.6, mold: 3.3 },
];

const SENSOR_FIELDS = ['co2', 'temp', 'rh', 'pm05', 'pm1', 'pm25', 'pm4', 'pm10', 'voc', 'nox', 'pressure', 'hcho', 'co', 'mold'];
const SENSOR_HISTORY_STEP_MIN = 5;
const SENSOR_NON_NEGATIVE_FIELDS = new Set(['co2', 'rh', 'pm05', 'pm1', 'pm25', 'pm4', 'pm10', 'voc', 'nox', 'hcho', 'co', 'mold']);
const SENSOR_WIGGLE = {
  co2: 6.0,
  temp: 0.08,
  rh: 0.35,
  pm05: 6.0,
  pm1: 0.12,
  pm25: 0.18,
  pm4: 0.22,
  pm10: 0.28,
  voc: 3.0,
  nox: 1.6,
  pressure: 0.05,
  hcho: 0.35,
  co: 0.03,
  mold: 0.05,
};

const hhmmToMinutes = (hhmm) => {
  const [hh, mm] = hhmm.split(':').map(Number);
  return hh * 60 + mm;
};

const minutesToHhmm = (minutes) => {
  const normalized = ((minutes % 1440) + 1440) % 1440;
  const hh = Math.floor(normalized / 60);
  const mm = normalized % 60;
  return `${String(hh).padStart(2, '0')}:${String(mm).padStart(2, '0')}`;
};

const buildFiveMinuteHistory = (anchors) => {
  if (!anchors || anchors.length < 2) return anchors || [];

  const wrapAnchor = { ...anchors[0], time: '24:00' };
  const cycle = [...anchors, wrapAnchor];
  const points = [];

  for (let i = 0; i < cycle.length - 1; i++) {
    const from = cycle[i];
    const to = cycle[i + 1];
    const fromMin = hhmmToMinutes(from.time);
    let toMin = hhmmToMinutes(to.time);
    if (toMin <= fromMin) toMin += 1440;
    const segmentMinutes = toMin - fromMin;

    for (let offset = 0; offset < segmentMinutes; offset += SENSOR_HISTORY_STEP_MIN) {
      const t = offset / segmentMinutes;
      const eased = (1 - Math.cos(Math.PI * t)) / 2; // Smoother than linear
      const envelope = Math.sin(Math.PI * t); // Keep wiggle at zero on anchors
      const point = { time: minutesToHhmm(fromMin + offset) };

      SENSOR_FIELDS.forEach((field, fieldIndex) => {
        const a = from[field];
        const b = to[field];
        const base = a + (b - a) * eased;
        const phase = (fieldIndex + 1) * 0.9;
        const wiggle = Math.sin(((fromMin + offset) / 42) + phase) * (SENSOR_WIGGLE[field] || 0) * envelope;
        let value = base + wiggle;
        if (SENSOR_NON_NEGATIVE_FIELDS.has(field) && value < 0) value = 0;
        point[field] = Number(value.toFixed(2));
      });

      points.push(point);
    }
  }

  return points;
};

const SENSOR_PLACEHOLDER_HISTORY = buildFiveMinuteHistory(SENSOR_PLACEHOLDER_ANCHORS);

const SENSOR_PLACEHOLDER_DERIVED = {
  ah: 9.4,
  dewPoint: 11.2,
  delta3h: -0.4,
  delta24h: -3.2,
  uptime: '3d 12h 45m',
};

const PREVIEW_HOSTNAME = (() => {
  const rawHost = (typeof window !== 'undefined' && window.location && window.location.hostname)
    ? window.location.hostname
    : '';
  const cleanHost = rawHost.replace(/\.local$/i, '').trim();
  return cleanHost || 'aura';
})();

const formatChartTime = (epochSeconds) => {
  if (typeof epochSeconds !== 'number' || !Number.isFinite(epochSeconds)) {
    return '';
  }
  const date = new Date(epochSeconds * 1000);
  if (Number.isNaN(date.getTime())) {
    return '';
  }
  return date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', hour12: false });
};

const formatHeaderTime = (date) =>
  date.toLocaleTimeString([], {
    hour: '2-digit',
    minute: '2-digit',
    hour12: false,
  });

const formatHeaderDate = (date) =>
  date
    .toLocaleDateString('en-GB', {
      day: '2-digit',
      month: 'short',
      year: 'numeric',
    })
    .toUpperCase();

const parseChartApiPayload = (payload) => {
  if (!payload || payload.success !== true || !Array.isArray(payload.timestamps) || !Array.isArray(payload.series)) {
    throw new Error('Invalid chart payload');
  }

  const timestamps = payload.timestamps;
  const pointCount = timestamps.length;
  const points = Array.from({ length: pointCount }, (_, index) => ({
    time: formatChartTime(timestamps[index]),
    epoch: typeof timestamps[index] === 'number' && Number.isFinite(timestamps[index]) ? timestamps[index] : null,
  }));
  const latest = {};

  payload.series.forEach((series) => {
    if (!series || typeof series.key !== 'string') return;
    const values = Array.isArray(series.values) ? series.values : [];
    latest[series.key] =
      typeof series.latest === 'number' && Number.isFinite(series.latest) ? series.latest : null;

    for (let i = 0; i < pointCount; i++) {
      const raw = values[i];
      points[i][series.key] = typeof raw === 'number' && Number.isFinite(raw) ? raw : null;
    }
  });

  // Keep compatibility with current local keys.
  points.forEach((point) => {
    if (Object.prototype.hasOwnProperty.call(point, 'temperature')) point.temp = point.temperature;
    if (Object.prototype.hasOwnProperty.call(point, 'humidity')) point.rh = point.humidity;
  });
  if (Object.prototype.hasOwnProperty.call(latest, 'temperature')) latest.temp = latest.temperature;
  if (Object.prototype.hasOwnProperty.call(latest, 'humidity')) latest.rh = latest.humidity;

  let latestEpoch = null;
  for (let i = timestamps.length - 1; i >= 0; i--) {
    const ts = timestamps[i];
    if (typeof ts === 'number' && Number.isFinite(ts)) {
      latestEpoch = ts;
      break;
    }
  }

  return { points, latest, latestEpoch };
};

const finiteNumberOrNull = (value) =>
  typeof value === 'number' && Number.isFinite(value) ? value : null;

const stringOrNull = (value) =>
  typeof value === 'string' && value.trim().length > 0 ? value : null;

const parseStateApiPayload = (payload) => {
  if (!payload || payload.success !== true) {
    throw new Error('Invalid state payload');
  }

  const sensors = payload.sensors || {};
  const derived = payload.derived || {};
  const network = payload.network || {};
  const system = payload.system || {};

  return {
    current: {
      co2: finiteNumberOrNull(sensors.co2),
      temp: finiteNumberOrNull(sensors.temp),
      rh: finiteNumberOrNull(sensors.rh),
      pressure: finiteNumberOrNull(sensors.pressure),
      pm05: finiteNumberOrNull(sensors.pm05),
      pm1: finiteNumberOrNull(sensors.pm1),
      pm25: finiteNumberOrNull(sensors.pm25),
      pm4: finiteNumberOrNull(sensors.pm4),
      pm10: finiteNumberOrNull(sensors.pm10),
      voc: finiteNumberOrNull(sensors.voc),
      nox: finiteNumberOrNull(sensors.nox),
      hcho: finiteNumberOrNull(sensors.hcho),
      co: finiteNumberOrNull(sensors.co),
    },
    derived: {
      ah: finiteNumberOrNull(derived.ah),
      dewPoint: finiteNumberOrNull(derived.dew_point),
      mold: finiteNumberOrNull(derived.mold),
      delta3h: finiteNumberOrNull(derived.pressure_delta_3h),
      delta24h: finiteNumberOrNull(derived.pressure_delta_24h),
      uptime: stringOrNull(derived.uptime),
    },
    connectivity: {
      wifiSsid: stringOrNull(network.wifi_ssid),
      hostname: stringOrNull(network.hostname),
      ip: stringOrNull(network.ip),
      rssi: finiteNumberOrNull(network.rssi),
      mqttBroker: stringOrNull(network.mqtt_broker),
      mqttConnected: typeof network.mqtt_connected === 'boolean' ? network.mqtt_connected : null,
    },
    system: {
      firmware: stringOrNull(system.firmware),
      buildDate: stringOrNull(system.build_date),
      buildTime: stringOrNull(system.build_time),
      uptime: stringOrNull(system.uptime),
    },
  };
};

const formatEventAge = (ageSeconds) => {
  if (typeof ageSeconds !== 'number' || !Number.isFinite(ageSeconds) || ageSeconds < 0) {
    return '--';
  }
  if (ageSeconds < 60) return `${Math.round(ageSeconds)}s ago`;
  if (ageSeconds < 3600) return `${Math.floor(ageSeconds / 60)}m ago`;
  if (ageSeconds < 86400) return `${Math.floor(ageSeconds / 3600)}h ago`;
  return `${Math.floor(ageSeconds / 86400)}d ago`;
};

const parseEventsApiPayload = (payload) => {
  if (!payload || payload.success !== true || !Array.isArray(payload.events)) {
    throw new Error('Invalid events payload');
  }

  const uptimeSeconds = finiteNumberOrNull(payload.uptime_s);
  const entries = payload.events
    .map((entry) => {
      const tsMs = finiteNumberOrNull(entry?.ts_ms);
      const ageSeconds =
        uptimeSeconds !== null && tsMs !== null ? Math.max(0, uptimeSeconds - Math.floor(tsMs / 1000)) : null;

      const severityRaw = stringOrNull(entry?.severity);
      const severity =
        severityRaw === 'critical' || severityRaw === 'danger' || severityRaw === 'warning' || severityRaw === 'info'
          ? (severityRaw === 'critical' ? 'critical' : severityRaw)
          : 'info';

      return {
        time: formatEventAge(ageSeconds),
        type: stringOrNull(entry?.type) || 'SYSTEM',
        message: stringOrNull(entry?.message) || 'Event',
        severity,
      };
    })
    .filter((entry) => entry.message.length > 0);

  return entries.reverse();
};

// ============ THRESHOLDS & COLORS ============
const thresholds = {
  // Synced to firmware thresholds (AppConfig.h + UiController.cpp)
  co2: { good: 800, moderate: 1000, bad: 1500 },
  pm05: { good: 250, moderate: 600, bad: 1200 }, // #/cm3
  pm25: { good: 12, moderate: 35, bad: 55 },
  pm1: { good: 10, moderate: 25, bad: 50 },
  pm4: { good: 25, moderate: 50, bad: 75 },
  pm10: { good: 54, moderate: 154, bad: 254 },
  voc: { good: 150, moderate: 250, bad: 350 },
  nox: { good: 50, moderate: 100, bad: 200 },
  hcho: { good: 30, moderate: 60, bad: 100 }, // ppb
  co: { good: 9, moderate: 35, bad: 100 }, // ppm
  temp: { good: [20, 25], moderate: [18, 26], bad: [16, 28] },
  rh: { good: [40, 60], moderate: [30, 65], bad: [20, 70] },
  dewPoint: { good: [11, 16], moderate: [9, 18], bad: [5, 21] }, // C
  ah: { good: [7, 15], moderate: [5, 18], bad: [4, 20] }, // g/m3
  mold: { good: 2, moderate: 4, bad: 7 }, // 0-10 index
  pressureDelta3h: { good: 1.0, moderate: 3.0, bad: 6.0 }, // abs(hPa)
  pressureDelta24h: { good: 2.0, moderate: 6.0, bad: 10.0 }, // abs(hPa)
};

const getStatus = (value, threshold) => {
  if (Array.isArray(threshold.good)) {
    // Range-based (temp, humidity)
    if (value >= threshold.good[0] && value <= threshold.good[1]) return 'good';
    if (value >= threshold.moderate[0] && value <= threshold.moderate[1]) return 'moderate';
    return 'bad';
  } else {
    // Upper limit based
    if (value <= threshold.good) return 'good';
    if (value <= threshold.moderate) return 'moderate';
    if (value <= threshold.bad) return 'bad';
    return 'critical';
  }
};

const statusColors = {
  good: '#22c55e',
  moderate: '#facc15',
  bad: '#f97316',
  critical: '#ef4444',
};

const statusLabels = {
  good: 'Good',
  moderate: 'Moderate',
  bad: 'Poor',
  critical: 'Hazard',
};

const fallbackStatusColor = '#9ca3af';

const statusColorOf = (status) => statusColors[status] || fallbackStatusColor;

const hexToRgb = (hexColor) => {
  if (typeof hexColor !== 'string') return null;
  const trimmed = hexColor.trim();
  if (!trimmed.startsWith('#')) return null;

  let hex = trimmed.slice(1);
  if (hex.length === 3) {
    hex = hex.split('').map((ch) => ch + ch).join('');
  }
  if (hex.length !== 6 || !/^[0-9a-fA-F]{6}$/.test(hex)) return null;

  const num = parseInt(hex, 16);
  return {
    r: (num >> 16) & 255,
    g: (num >> 8) & 255,
    b: num & 255,
  };
};

const rgba = (hexColor, alpha) => {
  const rgb = hexToRgb(hexColor);
  if (!rgb) return `rgba(156, 163, 175, ${alpha})`;
  return `rgba(${rgb.r}, ${rgb.g}, ${rgb.b}, ${alpha})`;
};

const statusPillStyle = (status) => {
  const color = statusColorOf(status);
  return {
    color,
    borderColor: rgba(color, 0.38),
    backgroundColor: rgba(color, 0.14),
  };
};

const statusTextStyle = (status) => ({
  color: statusColorOf(status),
});

const statusSurfaceStyle = (status) => {
  const color = statusColorOf(status);
  return {
    color,
    borderColor: rgba(color, 0.32),
    backgroundColor: rgba(color, 0.12),
  };
};

// ============ COMPONENTS ============

const StatusPill = ({ status, compact = false }) => {
  const sizeClass = compact
    ? 'px-2 py-0.5 text-[10px] md:text-[11px]'
    : 'px-2.5 py-1 text-[11px] md:text-xs';
  const fallbackPillStyle = {
    color: '#e5e7eb',
    borderColor: 'rgba(107, 114, 128, 0.35)',
    backgroundColor: 'rgba(75, 85, 99, 0.2)',
  };
  return (
    <span
      className={`${sizeClass} rounded-full border font-semibold`}
      style={status ? statusPillStyle(status) : fallbackPillStyle}
    >
      {statusLabels[status] || 'N/A'}
    </span>
  );
};

const isFiniteNumber = (v) => typeof v === 'number' && Number.isFinite(v);
const clampNumber = (value, min, max) => Math.max(min, Math.min(max, value));

const splitSeriesSegments = (points) => {
  const segments = [];
  let current = [];
  points.forEach((point) => {
    if (!isFiniteNumber(point.value)) {
      if (current.length) segments.push(current);
      current = [];
      return;
    }
    current.push(point);
  });
  if (current.length) segments.push(current);
  return segments;
};

const buildSmoothPath = (segment) => {
  if (!segment?.length) return '';
  if (segment.length === 1) {
    const p = segment[0];
    return `M ${p.x.toFixed(2)} ${p.y.toFixed(2)}`;
  }
  if (segment.length === 2) {
    const [p0, p1] = segment;
    return `M ${p0.x.toFixed(2)} ${p0.y.toFixed(2)} L ${p1.x.toFixed(2)} ${p1.y.toFixed(2)}`;
  }

  let d = `M ${segment[0].x.toFixed(2)} ${segment[0].y.toFixed(2)}`;
  for (let i = 0; i < segment.length - 1; i++) {
    const p0 = segment[i - 1] || segment[i];
    const p1 = segment[i];
    const p2 = segment[i + 1];
    const p3 = segment[i + 2] || p2;

    const cp1x = p1.x + (p2.x - p0.x) / 6;
    const cp1y = clampNumber(p1.y + (p2.y - p0.y) / 6, 0, 100);
    const cp2x = p2.x - (p3.x - p1.x) / 6;
    const cp2y = clampNumber(p2.y - (p3.y - p1.y) / 6, 0, 100);

    d += ` C ${cp1x.toFixed(2)} ${cp1y.toFixed(2)}, ${cp2x.toFixed(2)} ${cp2y.toFixed(2)}, ${p2.x.toFixed(2)} ${p2.y.toFixed(2)}`;
  }
  return d;
};

const formatChartValue = (value, unit = '') => {
  if (!isFiniteNumber(value)) return '-';
  const base = value.toFixed(1);
  const unitTrimmed = (unit || '').trim();
  return unitTrimmed ? `${base} ${unitTrimmed}` : base;
};

const formatMinMaxNumber = (value) => {
  if (!isFiniteNumber(value)) return '-';
  const abs = Math.abs(value);
  if (abs >= 100) return value.toFixed(0);
  return value.toFixed(1);
};

const formatMinMaxValue = (value, unit = '') => {
  const base = formatMinMaxNumber(value);
  const unitTrimmed = (unit || '').trim();
  if (base === '-' || !unitTrimmed) return base;
  return `${base} ${unitTrimmed}`;
};

const SvgTrendChart = ({ data = [], lines = [], lineColors = [], showGrid = true, unit = '' }) => {
  const [hoverIndex, setHoverIndex] = useState(null);
  const gradientSeed = useMemo(() => `sg_${Math.random().toString(36).slice(2, 9)}`, []);

  const model = useMemo(() => {
    const normalizedLines = lines
      .map((line, index) => ({
        key: line.key,
        name: line.name || line.key.toUpperCase(),
        color: lineColors[index] || line.color || '#22c55e',
      }))
      .filter((line) => line.key);

    if (!normalizedLines.length || data.length < 2) return null;

    const values = [];
    normalizedLines.forEach((line) => {
      data.forEach((row) => {
        const raw = row?.[line.key];
        if (isFiniteNumber(raw)) values.push(raw);
      });
    });

    if (!values.length) return null;

    let min = Math.min(...values);
    let max = Math.max(...values);
    if (!Number.isFinite(min) || !Number.isFinite(max)) return null;

    const spread = max - min;
    const pad = spread > 1e-6 ? spread * 0.12 : Math.max(Math.abs(max) * 0.08, 1);
    let yMin = min - pad;
    let yMax = max + pad;
    if (values.every((v) => v >= 0) && yMin < 0) yMin = 0;
    if (Math.abs(yMax - yMin) < 1e-6) {
      yMin -= 1;
      yMax += 1;
    }

    const xFor = (index) => (data.length <= 1 ? 0 : (index / (data.length - 1)) * 100);
    const yFor = (value) => ((yMax - value) / (yMax - yMin)) * 100;

    const lineModels = normalizedLines.map((line) => {
      const points = data.map((row, index) => {
        const raw = row?.[line.key];
        const value = isFiniteNumber(raw) ? raw : null;
        return {
          i: index,
          x: xFor(index),
          y: value === null ? null : yFor(value),
          value,
          time: row?.time || '--:--',
        };
      });

      const segments = splitSeriesSegments(points);
      const linePaths = segments.map((segment) => buildSmoothPath(segment)).filter((d) => d.length > 0);
      const areaPaths = segments
        .filter((segment) => segment.length >= 2)
        .map((segment) => {
          const d = buildSmoothPath(segment);
          const first = segment[0];
          const last = segment[segment.length - 1];
          return `${d} L ${last.x.toFixed(2)} 100 L ${first.x.toFixed(2)} 100 Z`;
        });

      return { ...line, points, segments, linePaths, areaPaths };
    });

    return { lineModels, xFor, yFor };
  }, [data, lines, lineColors]);

  if (!model) {
    return (
      <div className="w-full h-full rounded-lg border border-dashed border-gray-600/60 bg-gray-900/35 flex items-center justify-center px-3">
        <span className="text-[11px] md:text-xs text-gray-400 text-center">No data / Awaiting data</span>
      </div>
    );
  }

  const hoverActive = hoverIndex !== null && hoverIndex >= 0 && hoverIndex < data.length;
  const hoverRatio = hoverActive && data.length > 1 ? hoverIndex / (data.length - 1) : 0.5;
  const hoverX = hoverActive ? model.xFor(hoverIndex) : null;
  const tooltipLeft = clampNumber(hoverRatio * 100, 12, 88);
  const hoverTimeText = hoverActive ? (data[hoverIndex]?.time || '--:--') : '--:--';

  const tooltipItems = hoverActive
    ? model.lineModels
        .map((line) => {
          const point = line.points[hoverIndex];
          if (!point || !isFiniteNumber(point.value)) return null;
          return {
            key: line.key,
            name: line.name,
            color: line.color,
            text: formatChartValue(point.value, unit),
            y: point.y,
          };
        })
        .filter(Boolean)
    : [];

  const setHoverFromClientX = (clientX, target) => {
    const rect = target.getBoundingClientRect();
    if (!rect.width || data.length < 2) return;
    const ratio = clampNumber((clientX - rect.left) / rect.width, 0, 1);
    setHoverIndex(Math.round(ratio * (data.length - 1)));
  };

  const handleMouseMove = (event) => setHoverFromClientX(event.clientX, event.currentTarget);
  const handleTouchMove = (event) => {
    if (!event.touches || !event.touches[0]) return;
    setHoverFromClientX(event.touches[0].clientX, event.currentTarget);
  };

  return (
    <div className="relative w-full h-full">
      <svg viewBox="0 0 100 100" preserveAspectRatio="none" className="w-full h-full">
        <defs>
          {model.lineModels.map((line) => (
            <linearGradient key={`grad_${line.key}`} id={`${gradientSeed}_${line.key}`} x1="0" y1="0" x2="0" y2="1">
              <stop offset="5%" stopColor={line.color} stopOpacity={0.28} />
              <stop offset="95%" stopColor={line.color} stopOpacity={0} />
            </linearGradient>
          ))}
        </defs>

        {showGrid && (
          <>
            <line x1="0" y1="25" x2="100" y2="25" stroke="#374151" strokeWidth="0.7" strokeDasharray="2.5 2.5" vectorEffect="non-scaling-stroke" />
            <line x1="0" y1="50" x2="100" y2="50" stroke="#374151" strokeWidth="0.7" strokeDasharray="2.5 2.5" vectorEffect="non-scaling-stroke" />
            <line x1="0" y1="75" x2="100" y2="75" stroke="#374151" strokeWidth="0.7" strokeDasharray="2.5 2.5" vectorEffect="non-scaling-stroke" />
          </>
        )}

        {model.lineModels.map((line) =>
          line.areaPaths.map((pathD, idx) => (
            <path key={`area_${line.key}_${idx}`} d={pathD} fill={`url(#${gradientSeed}_${line.key})`} />
          ))
        )}

        {hoverActive && hoverX !== null && (
          <line
            x1={hoverX}
            y1="0"
            x2={hoverX}
            y2="100"
            stroke="#64748b"
            strokeWidth="0.8"
            strokeDasharray="2 2"
            vectorEffect="non-scaling-stroke"
          />
        )}

        {model.lineModels.map((line) =>
          line.linePaths.map((pathD, idx) => (
            <path
              key={`line_${line.key}_${idx}`}
              d={pathD}
              fill="none"
              stroke={line.color}
              strokeWidth="2"
              strokeLinecap="round"
              strokeLinejoin="round"
              vectorEffect="non-scaling-stroke"
            />
          ))
        )}
      </svg>

      <div
        className="absolute inset-0"
        onMouseMove={handleMouseMove}
        onMouseLeave={() => setHoverIndex(null)}
        onTouchStart={handleTouchMove}
        onTouchMove={handleTouchMove}
        onTouchEnd={() => setHoverIndex(null)}
      />

      {hoverActive && (
        <div
          className="absolute z-10 pointer-events-none bg-gray-900/95 border border-gray-600 rounded-lg shadow-xl px-2.5 py-2 text-[11px] md:text-xs"
          style={{ left: `${tooltipLeft}%`, top: '6px', transform: 'translateX(-50%)' }}
        >
          <div className={`text-gray-400 font-medium ${tooltipItems.length > 0 ? 'mb-1' : ''}`}>{hoverTimeText}</div>
          {tooltipItems.length > 0 && (
            <div className="space-y-1">
              {tooltipItems.map((item) => (
                <div key={`tt_${item.key}`} className="flex items-center justify-between gap-3 min-w-[120px]">
                  <span className="font-medium" style={{ color: item.color }}>{item.name}</span>
                  <span className="text-white font-semibold">{item.text}</span>
                </div>
              ))}
            </div>
          )}
        </div>
      )}

      {hoverActive &&
        hoverX !== null &&
        tooltipItems.map((item) => (
          <div
            key={`dot_overlay_${item.key}`}
            className="absolute pointer-events-none rounded-full border border-slate-900"
            style={{
              left: `${hoverX}%`,
              top: `${item.y}%`,
              width: '8px',
              height: '8px',
              backgroundColor: item.color,
              boxShadow: `0 0 0 4px color-mix(in srgb, ${item.color} 28%, transparent)`,
              transform: 'translate(-50%, -50%)',
            }}
          />
        ))}
    </div>
  );
};

const HeroMetric = ({ value, status, history = [] }) => {
  const advice = {
    good: 'Air is stable. Ventilation is optional.',
    moderate: 'Ventilation recommended in the next hour.',
    bad: 'Open windows or increase airflow now.',
    critical: 'Poor air quality. Ventilate immediately.',
  };
  const co2SeriesAll = history.length
    ? history.map((point) => ({ time: point.time, co2: isFiniteNumber(point.co2) ? point.co2 : null }))
    : [{ time: '--:--', co2: value }];

  const co2SeriesWindow = co2SeriesAll.slice(Math.max(0, co2SeriesAll.length - 36)); // 3h, 5-minute step
  const co2Series = [...co2SeriesWindow];
  const lastCo2Point = co2Series[co2Series.length - 1];
  if (!lastCo2Point || !isFiniteNumber(lastCo2Point.co2) || Math.abs(lastCo2Point.co2 - value) > 0.01) {
    co2Series.push({
      time: lastCo2Point?.time || '--:--',
      co2: value,
    });
  }

  const co2ValidPoints = co2Series.filter((point) => isFiniteNumber(point.co2));
  const co2Stats = co2ValidPoints.reduce(
    (acc, point) => ({
      min: Math.min(acc.min, Number(point.co2)),
      max: Math.max(acc.max, Number(point.co2)),
    }),
    { min: value, max: value }
  );
  const co2BasePoint = co2ValidPoints.length > 0 ? co2ValidPoints[0] : null;
  const delta3h = co2BasePoint ? value - Number(co2BasePoint.co2) : 0;
  const deltaColorClass = delta3h > 20 ? 'text-orange-300' : delta3h < -20 ? 'text-cyan-300' : 'text-gray-300';

  return (
    <div className="bg-gradient-to-br from-gray-800 to-gray-900 rounded-2xl p-5 md:p-7 border border-gray-700/60 shadow-xl h-full flex flex-col">
      <div className="flex items-start justify-between gap-4">
        <div>
          <div className="text-[12px] md:text-sm uppercase tracking-wider text-gray-400 font-semibold">CO2 Level</div>
          <div className="mt-3 flex items-end gap-2">
            <span className="text-6xl md:text-7xl font-semibold leading-none" style={{ color: statusColors[status] }}>{value.toFixed(0)}</span>
            <span className="text-base md:text-lg text-gray-400 pb-1">ppm</span>
          </div>
        </div>
        <StatusPill status={status} />
      </div>
      <div className="mt-4 h-2.5 bg-gray-700/80 rounded-full overflow-hidden">
        <div
          className="h-full rounded-full transition-all duration-700"
          style={{ width: `${Math.min((value / thresholds.co2.bad) * 100, 100)}%`, backgroundColor: statusColors[status] }}
        />
      </div>
      <div className="mt-3 text-sm md:text-base text-gray-300">{advice[status]}</div>

      <div className="mt-5 pt-4 border-t border-gray-700/60">
        <div className="flex items-center justify-between">
          <span className="text-[10px] md:text-xs uppercase tracking-wide text-gray-400 font-semibold">3h Trend</span>
          <span className={`text-xs md:text-sm font-semibold ${deltaColorClass}`}>
            {delta3h > 0 ? '+' : ''}{delta3h.toFixed(0)} ppm
          </span>
        </div>
        <div className="mt-2 h-20 md:h-24">
          <SvgTrendChart
            data={co2Series}
            lines={[{ key: 'co2' }]}
            lineColors={[statusColors[status]]}
            showGrid={true}
            unit="ppm"
          />
        </div>
        <div className="mt-2 flex items-center justify-between text-[11px] md:text-xs text-gray-400">
          <span>min {co2Stats.min.toFixed(0)} ppm</span>
          <span>max {co2Stats.max.toFixed(0)} ppm</span>
        </div>
      </div>
    </div>
  );
};

const ClimateOverview = ({
  temp,
  tempStatus,
  rh,
  rhStatus,
  dewPoint,
  ah,
  mold,
  moldStatus,
  pressure,
  delta3h,
  delta24h,
  pressureTrend3h,
  pressureTrend24h,
}) => {
  const statusRank = { good: 0, moderate: 1, bad: 2, critical: 3 };
  const climateStatus = [tempStatus, rhStatus, moldStatus].reduce(
    (worst, status) => (statusRank[status] > statusRank[worst] ? status : worst),
    'good'
  );
  const dewPointStatus = getStatus(dewPoint, thresholds.dewPoint);
  const ahStatus = getStatus(ah, thresholds.ah);
  const miniCardClass = "rounded-xl bg-gray-700/30 border border-gray-600/40 p-3 md:p-4";
  const climateLabelClass = "text-[10px] md:text-[11px] uppercase tracking-[0.08em] text-gray-400 font-semibold whitespace-nowrap";
  const unitClass = "text-sm md:text-base text-gray-300 leading-none self-end";

  return (
    <div className="bg-gradient-to-br from-gray-800 to-gray-900 rounded-2xl px-5 pt-5 pb-3 md:px-6 md:pt-6 md:pb-4 border border-gray-700/60 h-full flex flex-col">
      <div className="flex items-start justify-between gap-3">
        <div className="text-[12px] md:text-sm uppercase tracking-wide text-gray-300 font-semibold">Climate</div>
        <StatusPill status={climateStatus} />
      </div>

      <div className="mt-4 flex-1 flex flex-col justify-between gap-3">
        <div className="grid grid-cols-2 gap-3 md:gap-4">
          <div className={miniCardClass}>
            <div className="text-[10px] md:text-xs uppercase tracking-wide text-gray-400 font-semibold">Temperature</div>
            <div className="mt-2 flex items-end gap-1.5">
              <span className="text-3xl md:text-4xl font-semibold leading-none" style={{ color: statusColors[tempStatus] }}>{temp.toFixed(1)}</span>
              <span className={unitClass}>{'\u00B0C'}</span>
            </div>
          </div>
          <div className={miniCardClass}>
            <div className="text-[10px] md:text-xs uppercase tracking-wide text-gray-400 font-semibold">Humidity</div>
            <div className="mt-2 flex items-end gap-1.5">
              <span className="text-3xl md:text-4xl font-semibold leading-none" style={{ color: statusColors[rhStatus] }}>{rh.toFixed(0)}</span>
              <span className={unitClass}>%</span>
            </div>
          </div>
        </div>

        <div className="grid grid-cols-1 md:grid-cols-3 gap-3 md:gap-4">
          <div className={`${miniCardClass} h-full`}>
            <div className={climateLabelClass}>Mold Risk</div>
            <div className="mt-1.5 flex items-end gap-1.5">
              <span className="text-3xl md:text-4xl font-semibold leading-none" style={{ color: statusColors[moldStatus] }}>{mold.toFixed(1)}</span>
              <span className={unitClass}>/10</span>
            </div>
          </div>

          <div className={`${miniCardClass} h-full`}>
            <div className={climateLabelClass}>Dew Point</div>
            <div className="mt-1.5 flex items-end gap-1.5">
              <span className="text-3xl md:text-4xl font-semibold leading-none" style={{ color: statusColors[dewPointStatus] }}>{dewPoint.toFixed(1)}</span>
              <span className={unitClass}>{'\u00B0C'}</span>
            </div>
          </div>

          <div className={`${miniCardClass} h-full`}>
            <div className={climateLabelClass}>Abs Humidity</div>
            <div className="mt-1.5 flex items-end gap-1.5">
              <span className="text-3xl md:text-4xl font-semibold leading-none" style={{ color: statusColors[ahStatus] }}>{ah.toFixed(1)}</span>
              <span className={unitClass}>{'g/m\u00B3'}</span>
            </div>
          </div>
        </div>

        <div className={`${miniCardClass} py-2.5 md:py-3`}>
          <div className="flex flex-col sm:flex-row sm:items-end sm:justify-between gap-2">
            <div>
              <div className="text-[10px] md:text-xs uppercase tracking-wide text-gray-400 font-semibold">Pressure</div>
              <div className="mt-1 flex items-end gap-1.5">
                <span className="text-2xl md:text-3xl font-semibold leading-none text-white">{pressure.toFixed(1)}</span>
                <span className={unitClass}>hPa</span>
              </div>
            </div>
            <div className="grid grid-cols-2 gap-2.5 sm:min-w-[200px]">
              <div className="rounded-md border px-3 py-2" style={pressureTrend3h.surfaceStyle}>
                <div className="text-[11px] md:text-xs text-gray-400 leading-none">3h</div>
                <div className="mt-1 text-base md:text-lg font-semibold leading-none" style={pressureTrend3h.textStyle}>
                  {delta3h > 0 ? '+' : ''}{delta3h.toFixed(1)}
                </div>
              </div>
              <div className="rounded-md border px-3 py-2" style={pressureTrend24h.surfaceStyle}>
                <div className="text-[11px] md:text-xs text-gray-400 leading-none">24h</div>
                <div className="mt-1 text-base md:text-lg font-semibold leading-none" style={pressureTrend24h.textStyle}>
                  {delta24h > 0 ? '+' : ''}{delta24h.toFixed(1)}
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};

const GasMetricCard = ({ label, value, unit, max, status, decimals = 1, compact = false }) => {
  const progress = Math.min((value / max) * 100, 100);
  const valueText = Number(value).toFixed(decimals);
  const cardPaddingClass = compact ? 'p-2.5 md:p-3' : 'p-3 md:p-4';
  const labelClass = compact
    ? 'text-[10px] md:text-[11px] uppercase tracking-wide text-gray-300 font-semibold'
    : 'text-[11px] md:text-xs uppercase tracking-wide text-gray-300 font-semibold';
  const unitTextClass = compact ? 'text-[10px] text-gray-500' : 'text-[10px] md:text-xs text-gray-500';
  const valueClass = compact ? 'text-2xl md:text-[30px] font-semibold leading-none' : 'text-3xl md:text-[34px] font-semibold leading-none';
  const progressBarClass = compact ? 'mt-2.5 h-1.5 bg-gray-700 rounded-full overflow-hidden' : 'mt-3 h-2 bg-gray-700 rounded-full overflow-hidden';
  return (
    <div className={`bg-gray-800 rounded-xl border border-gray-700/50 ${cardPaddingClass}`}>
      <div className="flex items-center justify-between">
        <div className={labelClass}>{label}</div>
        <span className={unitTextClass}>{unit}</span>
      </div>
      <div className="mt-1.5 flex items-end justify-between">
        <span className={valueClass} style={{ color: statusColors[status] }}>{valueText}</span>
        <StatusPill status={status} compact={compact} />
      </div>
      <div className={progressBarClass}>
        <div className="h-full rounded-full" style={{ width: `${progress}%`, backgroundColor: statusColors[status] }} />
      </div>
    </div>
  );
};

const trendToken = (delta, is24h = false) => {
  const absDelta = Math.abs(delta);
  const status = getStatus(absDelta, is24h ? thresholds.pressureDelta24h : thresholds.pressureDelta3h);

  let label = 'Stable';
  if (delta <= -2) label = 'Strong Fall';
  else if (delta < -0.3) label = 'Falling';
  else if (delta >= 2) label = 'Strong Rise';
  else if (delta > 0.3) label = 'Rising';

  return {
    status,
    label,
    textStyle: statusTextStyle(status),
    surfaceStyle: statusSurfaceStyle(status),
  };
};

// Chart component
const ChartSection = ({ title, data, lines, unit, color, latestValues = {} }) => {
  const fallbackPalette = ['#22c55e', '#38bdf8', '#a78bfa', '#f59e0b'];
  const lineColors = lines.map((line, index) => line.color || color || fallbackPalette[index % fallbackPalette.length]);
  const unitTrimmed = (unit || '').trim();

  const minMax = useMemo(() => {
    const values = [];
    lines.forEach((line) => {
      data.forEach((row) => {
        const raw = row?.[line.key];
        if (typeof raw === 'number' && Number.isFinite(raw)) values.push(raw);
      });
    });

    if (!values.length) return null;
    return {
      min: Math.min(...values),
      max: Math.max(...values),
    };
  }, [data, lines]);

  const latestItems = lines.map((line, index) => {
    const fromApi = latestValues[line.key];
    if (typeof fromApi === 'number' && Number.isFinite(fromApi)) {
      return { text: fromApi.toFixed(1), color: lineColors[index] };
    }

    for (let i = data.length - 1; i >= 0; i--) {
      const raw = data[i]?.[line.key];
      if (typeof raw === 'number' && Number.isFinite(raw)) {
        return { text: raw.toFixed(1), color: lineColors[index] };
      }
    }

    return { text: '-', color: '#9ca3af' };
  });

  return (
    <div className="bg-gray-800 rounded-xl p-3 border border-gray-700/50">
      <div className="flex justify-between items-center mb-3">
        <span className="text-gray-400 text-xs font-bold">{title}</span>
        <div className="flex items-center gap-2 flex-wrap justify-end">
          {latestItems.map((item, index) => (
            <span
              key={`${title}_latest_${index}`}
              className="text-xs md:text-sm font-semibold"
              style={{ color: item.color }}
            >
              {item.text}
            </span>
          ))}
        </div>
      </div>
      <div className="h-32 md:h-40 lg:h-44 w-full">
        <SvgTrendChart
          data={data}
          lines={lines}
          lineColors={lineColors}
          showGrid={true}
          unit={unit}
        />
      </div>
      <div className="mt-2 flex items-center justify-between text-[11px] md:text-xs text-gray-400">
        <span>min {formatMinMaxValue(minMax?.min, unitTrimmed)}</span>
        <span>max {formatMinMaxValue(minMax?.max, unitTrimmed)}</span>
      </div>
    </div>
  );
};

// Alert item
const AlertItem = ({ time, type, message, severity }) => {
  const severityColors = {
    warning: 'border-l-yellow-500 bg-yellow-500/10',
    danger: 'border-l-orange-500 bg-orange-500/10',
    critical: 'border-l-red-500 bg-red-500/10',
    info: 'border-l-blue-500 bg-blue-500/10',
  };
  
  return (
    <div className={`flex flex-col p-3 rounded-r-lg border-l-4 mb-2 ${severityColors[severity] || 'border-l-gray-500'}`}>
      <span className="text-white text-sm md:text-[15px] font-medium leading-snug">{message}</span>
      <div className="mt-1.5 flex items-center justify-between gap-2">
        <span className="text-xs text-gray-400 uppercase font-bold">{type}</span>
        <span className="text-cyan-200 text-[12px] md:text-[13px] font-semibold tracking-[0.02em] whitespace-nowrap">{time}</span>
      </div>
    </div>
  );
};

const HeaderPencilIcon = ({ className = "" }) => (
  <svg viewBox="0 0 24 24" className={className} aria-hidden="true">
    <path d="M4 20h4l9.7-9.7a1.7 1.7 0 0 0 0-2.4l-1.6-1.6a1.7 1.7 0 0 0-2.4 0L4 16v4z" fill="none" stroke="currentColor" strokeWidth="1.8" strokeLinecap="round" strokeLinejoin="round" />
    <path d="m12.8 7.2 4 4" fill="none" stroke="currentColor" strokeWidth="1.8" strokeLinecap="round" />
  </svg>
);

const HeaderCheckIcon = ({ className = "" }) => (
  <svg viewBox="0 0 24 24" className={className} aria-hidden="true">
    <path d="m5 12.5 4.2 4.2L19 7.8" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" />
  </svg>
);

const HeaderCloseIcon = ({ className = "" }) => (
  <svg viewBox="0 0 24 24" className={className} aria-hidden="true">
    <path d="M6 6l12 12M18 6 6 18" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" />
  </svg>
);

const TabIcon = ({ id }) => {
  const base = "w-4 h-4 md:w-[18px] md:h-[18px]";
  const stroke = {
    fill: "none",
    stroke: "currentColor",
    strokeWidth: 2.2,
    strokeLinecap: "round",
    strokeLinejoin: "round",
  };

  if (id === "sensors") {
    return (
      <svg viewBox="0 0 24 24" className={base} aria-hidden="true">
        <circle {...stroke} cx="12" cy="12" r="4" />
        <circle {...stroke} cx="12" cy="12" r="1.6" />
        <path {...stroke} d="M12 4.5v2.5M12 17v2.5M4.5 12H7M17 12h2.5" />
      </svg>
    );
  }

  if (id === "charts") {
    return (
      <svg viewBox="0 0 24 24" className={base} aria-hidden="true">
        <path {...stroke} d="M4 18h16" />
        <path {...stroke} d="M6 14.5 10 10l3 2.5 5-6" />
        <path {...stroke} d="M18 6h-2.5v2.5" />
      </svg>
    );
  }

  if (id === "events") {
    return (
      <svg viewBox="0 0 24 24" className={base} aria-hidden="true">
        <path {...stroke} d="M18 15.5V11a6 6 0 0 0-12 0v4.5L4.5 17h15L18 15.5z" />
        <path {...stroke} d="M10 18a2 2 0 0 0 4 0" />
      </svg>
    );
  }

  return (
    <svg viewBox="0 0 24 24" className={base} aria-hidden="true">
      <path {...stroke} d="M4 7h16" />
      <circle {...stroke} cx="9" cy="7" r="1.6" />
      <path {...stroke} d="M4 12h16" />
      <circle {...stroke} cx="15" cy="12" r="1.6" />
      <path {...stroke} d="M4 17h16" />
      <circle {...stroke} cx="11" cy="17" r="1.6" />
    </svg>
  );
};

// Tab navigation
const TabNav = ({ tabs, activeTab, onChange }) => {
  return (
    <div className="flex flex-wrap bg-gray-800 p-1 rounded-xl mb-4 border border-gray-700/50 gap-1">
      {tabs.map(tab => (
        <button
          key={tab.id}
          onClick={() => onChange(tab.id)}
          className={`flex items-center justify-center gap-2 flex-1 min-w-[48%] md:flex-none md:min-w-[132px] py-2 px-3 rounded-lg text-xs md:text-sm font-bold transition-all ${
            activeTab === tab.id
              ? 'bg-cyan-600 text-white shadow-lg shadow-cyan-900/40'
              : 'text-gray-400 hover:text-gray-200'
          }`}
        >
          <span className="inline-flex items-center justify-center">
            <TabIcon id={tab.id} />
          </span>
          {tab.label}
        </button>
      ))}
    </div>
  );
};

// Settings Components
const SettingGroup = ({ title, children }) => (
  <div className="bg-gray-800 rounded-xl p-4 md:p-5 border border-gray-700/50">
    <div className="text-gray-400 text-xs font-bold uppercase tracking-wider mb-3">{title}</div>
    <div className="space-y-3">{children}</div>
  </div>
);

const SettingStepper = ({ label, value, unit, stepHint, onDec, onInc, disabled = false }) => (
  <div className={`flex justify-between items-center ${disabled ? 'opacity-65' : ''}`}>
    <div>
      <div className="text-gray-300 text-sm">{label}</div>
      {stepHint && <div className="text-[11px] text-gray-500 mt-0.5">{stepHint}</div>}
    </div>
    <div className="flex items-center gap-2">
      <button 
        disabled={disabled}
        onClick={onDec}
        className={`w-8 h-8 flex items-center justify-center bg-gray-800 border border-gray-700 rounded-lg text-gray-400 transition-colors ${
          disabled ? 'cursor-not-allowed opacity-60' : 'hover:bg-gray-700 hover:text-white'
        }`}
      >
        <Minus size={14} />
      </button>
      <div className="w-24 text-center">
        <span className="text-sm md:text-base font-mono text-white">{value > 0 ? '+' : ''}{value}</span>
        <span className="ml-1 text-xs md:text-sm text-gray-400">{unit}</span>
      </div>
      <button 
        disabled={disabled}
        onClick={onInc}
        className={`w-8 h-8 flex items-center justify-center bg-gray-800 border border-gray-700 rounded-lg text-gray-400 transition-colors ${
          disabled ? 'cursor-not-allowed opacity-60' : 'hover:bg-gray-700 hover:text-white'
        }`}
      >
        <Plus size={14} />
      </button>
    </div>
  </div>
);

const SettingToggle = ({ label, enabled, onClick, icon: Icon, disabled = false }) => (
  <div 
    className={`flex justify-between items-center select-none group ${disabled ? 'opacity-65 cursor-not-allowed' : 'cursor-pointer'}`} 
    onClick={disabled ? undefined : onClick}
  >
    <div className={`flex items-center gap-2 text-gray-300 text-sm transition-colors ${disabled ? '' : 'group-hover:text-white'}`}>
      {Icon && <Icon size={16} className="text-gray-500" />}
      {label}
    </div>
    <div className={`w-10 h-5 rounded-full relative transition-colors duration-300 ${enabled ? 'bg-cyan-600' : 'bg-gray-700'}`}>
      <div className={`absolute top-1 w-3 h-3 bg-white rounded-full transition-all duration-300 shadow-md ${enabled ? 'left-6' : 'left-1'}`} />
    </div>
  </div>
);

const SettingInfoRow = ({ label, value, valueClassName = "text-white text-sm", mono = false }) => (
  <div className="flex justify-between items-center gap-3 py-1">
    <span className="text-gray-300 text-sm">{label}</span>
    <span className={`${valueClassName} ${mono ? 'font-mono' : ''}`.trim()}>{value}</span>
  </div>
);

// ============ MAIN DASHBOARD ============
function AuraDashboard() {
  const [activeTab, setActiveTab] = useState('sensors');
  const [chartRange, setChartRange] = useState('24h');
  const [chartGroup, setChartGroup] = useState('core');
  const settingsPreviewOnly = true;
  
  // Settings State
  const [settings, setSettings] = useState({
    nightMode: false,
    alertBlink: true,
    backlight: true,
    tempOffset: -1.2,
    humOffset: 2.0,
  });

  // Device Name Editing
  const [deviceName, setDeviceName] = useState(PREVIEW_HOSTNAME);
  const [isEditingName, setIsEditingName] = useState(false);
  const [tempDeviceName, setTempDeviceName] = useState(deviceName);

  const handleNameSave = () => {
    setDeviceName(tempDeviceName);
    setIsEditingName(false);
  };

  const handleNameCancel = () => {
    setTempDeviceName(deviceName);
    setIsEditingName(false);
  };

  const toggleSetting = (key) => {
    setSettings(prev => ({ ...prev, [key]: !prev[key] }));
  };

  const updateOffset = (key, delta) => {
    setSettings(prev => ({ 
      ...prev, 
      [key]: Number((prev[key] + delta).toFixed(1)) 
    }));
  };
  
  const fullData = SENSOR_PLACEHOLDER_HISTORY;
  const placeholderCurrent = fullData[fullData.length - 1];

  const [stateApi, setStateApi] = useState(null);
  const [stateApiLive, setStateApiLive] = useState(false);

  const [chartApiData, setChartApiData] = useState(null);
  const [chartApiLatest, setChartApiLatest] = useState({});
  const [chartApiLoading, setChartApiLoading] = useState(false);
  const [chartApiLive, setChartApiLive] = useState(false);
  const [sensorHistoryData, setSensorHistoryData] = useState(null);
  const [clockTickMs, setClockTickMs] = useState(Date.now());
  const [deviceClockRef, setDeviceClockRef] = useState(null);
  const [eventsApiAlerts, setEventsApiAlerts] = useState(null);
  const [eventsApiLive, setEventsApiLive] = useState(false);

  useEffect(() => {
    const intervalId = setInterval(() => setClockTickMs(Date.now()), 1000);
    return () => clearInterval(intervalId);
  }, []);

  useEffect(() => {
    if (activeTab !== 'charts') return;

    const controller = new AbortController();
    const apiGroup = chartGroup === 'core' ? 'core' : chartGroup;
    setChartApiLoading(true);

    fetch(`/api/charts?group=${encodeURIComponent(apiGroup)}&window=${encodeURIComponent(chartRange)}`, {
      cache: 'no-store',
      signal: controller.signal,
    })
      .then((response) => {
        if (!response.ok) {
          throw new Error(`HTTP ${response.status}`);
        }
        return response.json();
      })
      .then((payload) => {
        const parsed = parseChartApiPayload(payload);
        setChartApiData(parsed.points);
        setChartApiLatest(parsed.latest);
        if (typeof parsed.latestEpoch === 'number' && Number.isFinite(parsed.latestEpoch)) {
          setDeviceClockRef({
            epochMs: parsed.latestEpoch * 1000,
            capturedAtMs: Date.now(),
          });
        }
        setChartApiLive(true);
      })
      .catch((error) => {
        if (error?.name === 'AbortError') return;
        setChartApiData(null);
        setChartApiLatest({});
        setChartApiLive(false);
      })
      .finally(() => {
        if (!controller.signal.aborted) {
          setChartApiLoading(false);
        }
      });

    return () => controller.abort();
  }, [activeTab, chartRange, chartGroup]);

  useEffect(() => {
    if (activeTab !== 'sensors') return;

    let active = true;
    const loadSensorHistory = () => {
      fetch('/api/charts?group=core&window=24h', { cache: 'no-store' })
        .then((response) => {
          if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
          }
          return response.json();
        })
        .then((payload) => {
          if (!active) return;
          const parsed = parseChartApiPayload(payload);
          setSensorHistoryData(parsed.points);
          if (typeof parsed.latestEpoch === 'number' && Number.isFinite(parsed.latestEpoch)) {
            setDeviceClockRef({
              epochMs: parsed.latestEpoch * 1000,
              capturedAtMs: Date.now(),
            });
          }
        })
        .catch(() => {
          // Keep last successful history to avoid flicker.
        });
    };

    loadSensorHistory();
    const intervalId = setInterval(loadSensorHistory, 30000);
    return () => {
      active = false;
      clearInterval(intervalId);
    };
  }, [activeTab]);

  useEffect(() => {
    let active = true;

    const loadState = () => {
      fetch('/api/state', { cache: 'no-store' })
        .then((response) => {
          if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
          }
          return response.json();
        })
        .then((payload) => {
          if (!active) return;
          const parsed = parseStateApiPayload(payload);
          setStateApi(parsed);
          setStateApiLive(true);
        })
        .catch(() => {
          if (!active) return;
          setStateApiLive(false);
        });
    };

    loadState();
    const intervalId = setInterval(loadState, 10000);
    return () => {
      active = false;
      clearInterval(intervalId);
    };
  }, []);

  useEffect(() => {
    if (activeTab !== 'events') return;

    let active = true;
    const loadEvents = () => {
      fetch('/api/events', { cache: 'no-store' })
        .then((response) => {
          if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
          }
          return response.json();
        })
        .then((payload) => {
          if (!active) return;
          const parsed = parseEventsApiPayload(payload);
          setEventsApiAlerts(parsed);
          setEventsApiLive(true);
        })
        .catch(() => {
          if (!active) return;
          setEventsApiLive(false);
        });
    };

    loadEvents();
    const intervalId = setInterval(loadEvents, 10000);
    return () => {
      active = false;
      clearInterval(intervalId);
    };
  }, [activeTab]);

  const chartData = Array.isArray(chartApiData) ? chartApiData : [];
  const sensorHistory = Array.isArray(sensorHistoryData) ? sensorHistoryData : [];

  const stateCurrent = stateApi?.current || {};
  const stateDerived = stateApi?.derived || {};
  const stateConnectivity = stateApi?.connectivity || {};
  const stateSystem = stateApi?.system || {};
  const current = {
    co2: stateCurrent.co2 ?? placeholderCurrent.co2,
    temp: stateCurrent.temp ?? placeholderCurrent.temp,
    rh: stateCurrent.rh ?? placeholderCurrent.rh,
    pressure: stateCurrent.pressure ?? placeholderCurrent.pressure,
    pm05: stateCurrent.pm05 ?? placeholderCurrent.pm05,
    pm1: stateCurrent.pm1 ?? placeholderCurrent.pm1,
    pm25: stateCurrent.pm25 ?? placeholderCurrent.pm25,
    pm4: stateCurrent.pm4 ?? placeholderCurrent.pm4,
    pm10: stateCurrent.pm10 ?? placeholderCurrent.pm10,
    voc: stateCurrent.voc ?? placeholderCurrent.voc,
    nox: stateCurrent.nox ?? placeholderCurrent.nox,
    hcho: stateCurrent.hcho ?? placeholderCurrent.hcho,
    co: stateCurrent.co ?? placeholderCurrent.co,
    mold: stateDerived.mold ?? placeholderCurrent.mold,
  };
  // Top-right values in Charts should match live sensor cards.
  const chartLatestValues = {
    ...chartApiLatest,
    ...current,
  };
  
  const ah = stateDerived.ah ?? SENSOR_PLACEHOLDER_DERIVED.ah;
  const dewPoint = stateDerived.dewPoint ?? SENSOR_PLACEHOLDER_DERIVED.dewPoint;
  const delta3h = stateDerived.delta3h ?? SENSOR_PLACEHOLDER_DERIVED.delta3h;
  const delta24h = stateDerived.delta24h ?? SENSOR_PLACEHOLDER_DERIVED.delta24h;
  const co2Status = getStatus(current.co2, thresholds.co2);
  const tempStatus = getStatus(current.temp, thresholds.temp);
  const rhStatus = getStatus(current.rh, thresholds.rh);
  const pm05Status = getStatus(current.pm05, thresholds.pm05);
  const pm1Status = getStatus(current.pm1, thresholds.pm1);
  const pm25Status = getStatus(current.pm25, thresholds.pm25);
  const pm4Status = getStatus(current.pm4, thresholds.pm4);
  const pm10Status = getStatus(current.pm10, thresholds.pm10);
  const vocStatus = getStatus(current.voc, thresholds.voc);
  const noxStatus = getStatus(current.nox, thresholds.nox);
  const hchoStatus = getStatus(current.hcho, thresholds.hcho);
  const coStatus = getStatus(current.co, thresholds.co);
  const moldStatus = getStatus(current.mold, thresholds.mold);
  const pressureTrend3h = trendToken(delta3h, false);
  const pressureTrend24h = trendToken(delta24h, true);
  const uptime = stateSystem.uptime || stateDerived.uptime || SENSOR_PLACEHOLDER_DERIVED.uptime;

  const connectivity = {
    wifiSsid: stateConnectivity.wifiSsid || 'MyHome_5G',
    hostname: stateConnectivity.hostname || PREVIEW_HOSTNAME,
    ip: stateConnectivity.ip || '192.168.1.105',
    rssi: typeof stateConnectivity.rssi === 'number' ? stateConnectivity.rssi : -65,
    mqttBroker: stateConnectivity.mqttBroker || '192.168.1.200',
    mqttConnected: typeof stateConnectivity.mqttConnected === 'boolean' ? stateConnectivity.mqttConnected : true,
  };
  const firmwareVersion = stateSystem.firmware || 'v2.1.0-beta';
  const firmwareBuild = [stateSystem.buildDate, stateSystem.buildTime].filter(Boolean).join(' ') || '20240315';
  const localWebUrl = `http://${connectivity.hostname}.local`;
  const signalClass =
    connectivity.rssi > -67
      ? "text-emerald-400 text-sm font-semibold"
      : connectivity.rssi > -75
        ? "text-yellow-400 text-sm font-semibold"
        : "text-red-400 text-sm font-semibold";
  const headerNow = deviceClockRef
    ? new Date(deviceClockRef.epochMs + (clockTickMs - deviceClockRef.capturedAtMs))
    : new Date(clockTickMs);
  const headerTime = formatHeaderTime(headerNow);
  const headerDate = formatHeaderDate(headerNow);

  useEffect(() => {
    if (!isEditingName && stateConnectivity.hostname) {
      setDeviceName(stateConnectivity.hostname);
    }
  }, [isEditingName, stateConnectivity.hostname]);
  
  const fallbackAlerts = [
    { time: '14:32', type: 'CO2', message: 'Threshold exceeded (>1000 ppm)', severity: 'warning' },
    { time: '12:45', type: 'VOC', message: 'Elevated index detected', severity: 'info' },
    { time: '08:15', type: 'CO2', message: 'Critical level (>1400 ppm)', severity: 'danger' },
  ];
  const alerts = eventsApiLive && Array.isArray(eventsApiAlerts) ? eventsApiAlerts : fallbackAlerts;

  const tabs = [
    { id: 'sensors', label: 'Sensors' },
    { id: 'charts', label: 'Charts' },
    { id: 'events', label: 'Events' },
    { id: 'settings', label: 'Settings' },
  ];

  return (
    <div className="min-h-screen bg-gray-900 text-white px-4 py-4 md:px-6 lg:px-8 max-w-md md:max-w-3xl lg:max-w-6xl mx-auto font-sans">
      {/* Header */}
      <div className="flex items-center justify-between mb-5 md:mb-6 px-1">
        <div>
          <h1 className="text-xl font-bold text-white flex items-center gap-2">
            <span className="relative inline-flex w-3.5 h-3.5 items-center justify-center">
              <span className="absolute inset-0 rounded-full border border-emerald-400/70 animate-pulse shadow-[0_0_10px_#22c55e]"></span>
              <span className="w-2 h-2 rounded-full bg-emerald-400 shadow-[0_0_8px_#4ade80]"></span>
            </span>
            AURA
          </h1>
          {isEditingName ? (
            <div className="flex items-center gap-2 pl-5 mt-1">
              <input 
                type="text" 
                value={tempDeviceName}
                onChange={(e) => setTempDeviceName(e.target.value)}
                className="bg-gray-800 text-white text-[12px] font-mono border border-gray-600 rounded px-2 py-1 outline-none w-40"
                autoFocus
                onKeyDown={(e) => {
                  if (e.key === 'Enter') handleNameSave();
                  if (e.key === 'Escape') handleNameCancel();
                }}
              />
              <button onClick={handleNameSave} className="w-6 h-6 rounded-md bg-emerald-500/15 border border-emerald-500/35 text-emerald-300 hover:text-emerald-200 hover:border-emerald-400/60 transition-colors inline-flex items-center justify-center">
                <HeaderCheckIcon className="w-3.5 h-3.5" />
              </button>
              <button onClick={handleNameCancel} className="w-6 h-6 rounded-md bg-red-500/15 border border-red-500/35 text-red-300 hover:text-red-200 hover:border-red-400/60 transition-colors inline-flex items-center justify-center">
                <HeaderCloseIcon className="w-3.5 h-3.5" />
              </button>
            </div>
          ) : (
            <div 
              className="flex items-center gap-2 pl-5 mt-1 group cursor-pointer"
              onClick={() => {
                setTempDeviceName(deviceName);
                setIsEditingName(true);
              }}
            >
              <p className="text-gray-400 text-[13px] md:text-sm font-semibold tracking-[0.06em] group-hover:text-gray-200 transition-colors">{deviceName}</p>
              <HeaderPencilIcon className="w-3.5 h-3.5 text-gray-500 group-hover:text-emerald-300 transition-colors opacity-70 group-hover:opacity-100" />
            </div>
          )}
        </div>
        <div className="text-right">
          <div className="text-white text-xl md:text-2xl font-bold leading-none tracking-wide">
            {headerTime}
          </div>
          <div className="mt-1 text-gray-400 text-[11px] md:text-xs font-semibold uppercase tracking-[0.12em]">
            {headerDate}
          </div>
        </div>
      </div>

      <TabNav tabs={tabs} activeTab={activeTab} onChange={setActiveTab} />

      {activeTab === 'sensors' && (
        <div className="space-y-4 md:space-y-5 animate-in fade-in duration-300">
          <div className="grid grid-cols-1 xl:grid-cols-12 gap-4 md:gap-5">
            <div className="xl:col-span-7">
              <HeroMetric value={current.co2} status={co2Status} history={sensorHistory} />
            </div>
            <div className="xl:col-span-5">
              <ClimateOverview
                temp={current.temp}
                tempStatus={tempStatus}
                rh={current.rh}
                rhStatus={rhStatus}
                dewPoint={dewPoint}
                ah={ah}
                mold={current.mold}
                moldStatus={moldStatus}
                pressure={current.pressure}
                delta3h={delta3h}
                delta24h={delta24h}
                pressureTrend3h={pressureTrend3h}
                pressureTrend24h={pressureTrend24h}
              />
            </div>
          </div>

          <div className="grid grid-cols-1 sm:grid-cols-2 xl:grid-cols-4 gap-3 md:gap-4">
            <GasMetricCard label="CO" value={current.co} unit="ppm" max={25} status={coStatus} />
            <GasMetricCard label="VOC" value={current.voc} unit="idx" max={400} status={vocStatus} />
            <GasMetricCard label="NOx" value={current.nox} unit="idx" max={300} status={noxStatus} />
            <GasMetricCard label="HCHO" value={current.hcho} unit="ppb" max={100} status={hchoStatus} />
          </div>

          <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 xl:grid-cols-5 gap-2.5 md:gap-3">
            <GasMetricCard label="PM0.5" value={current.pm05} unit="#/cm\u00B3" max={thresholds.pm05.bad} status={pm05Status} decimals={0} compact />
            <GasMetricCard label="PM1.0" value={current.pm1} unit="\u00B5g/m\u00B3" max={thresholds.pm1.bad} status={pm1Status} compact />
            <GasMetricCard label="PM2.5" value={current.pm25} unit="\u00B5g/m\u00B3" max={thresholds.pm25.bad} status={pm25Status} compact />
            <GasMetricCard label="PM4.0" value={current.pm4} unit="\u00B5g/m\u00B3" max={thresholds.pm4.bad} status={pm4Status} compact />
            <GasMetricCard label="PM10" value={current.pm10} unit="\u00B5g/m\u00B3" max={thresholds.pm10.bad} status={pm10Status} compact />
          </div>

        </div>
      )}

      {activeTab === 'charts' && (
        <div className="space-y-4 md:space-y-5 animate-in fade-in duration-300">
          {/* Chart Controls */}
          <div className="grid grid-cols-1 xl:grid-cols-[320px_minmax(0,1fr)] gap-3 md:gap-4 items-start">
            <div className="flex bg-gray-800 p-1 rounded-lg border border-gray-700/50 md:w-[320px]">
              {['1h', '3h', '24h'].map(r => (
                <button
                  key={r}
                  onClick={() => setChartRange(r)}
                  className={`flex-1 py-1.5 text-xs md:text-sm font-bold rounded-md transition-all ${
                    chartRange === r
                      ? 'bg-gray-700 text-white shadow-sm'
                      : 'text-gray-500 hover:text-gray-300'
                  }`}
                >
                  {r}
                </button>
              ))}
            </div>

            <div className="bg-gray-800 p-1 rounded-lg border border-gray-700/50">
              <div className="grid grid-cols-3 gap-1">
                {[
                  { key: 'core', label: 'Core' },
                  { key: 'gases', label: 'Gases' },
                  { key: 'pm', label: 'PM' },
                ].map(group => (
                  <button
                    key={group.key}
                    onClick={() => setChartGroup(group.key)}
                    className={`py-1.5 px-2 text-xs md:text-sm font-bold rounded-md transition-all ${
                      chartGroup === group.key
                        ? 'bg-gray-700 text-white shadow-sm'
                        : 'text-gray-500 hover:text-gray-300'
                    }`}
                  >
                    {group.label}
                  </button>
                ))}
              </div>
            </div>
          </div>

          <div className="text-[11px] md:text-xs text-gray-500">
            {chartApiLoading
              ? 'Loading live chart history...'
              : chartApiLive
                ? 'Live history: /api/charts'
                : 'No data / Awaiting data from /api/charts'}
          </div>

          {chartGroup === 'core' && (
            <div className="grid grid-cols-1 xl:grid-cols-2 gap-3 md:gap-4">
              <ChartSection title="CO2 Concentration" data={chartData} lines={[{ key: 'co2', name: 'CO2' }]} unit="ppm" color="#10b981" latestValues={chartLatestValues} />
              <ChartSection title="Temperature" data={chartData} lines={[{ key: 'temp', name: 'Temp' }]} unit="\u00B0C" color="#f59e0b" latestValues={chartLatestValues} />
              <ChartSection title="Humidity" data={chartData} lines={[{ key: 'rh', name: 'RH' }]} unit="%" color="#3b82f6" latestValues={chartLatestValues} />
              <ChartSection title="Pressure" data={chartData} lines={[{ key: 'pressure', name: 'hPa' }]} unit="hPa" color="#0ea5e9" latestValues={chartLatestValues} />
            </div>
          )}

          {chartGroup === 'gases' && (
            <div className="grid grid-cols-1 xl:grid-cols-2 gap-3 md:gap-4">
              <ChartSection title="Carbon Monoxide (CO)" data={chartData} lines={[{ key: 'co', name: 'CO' }]} unit="ppm" color="#f97316" latestValues={chartLatestValues} />
              <ChartSection title="VOC Index" data={chartData} lines={[{ key: 'voc', name: 'VOC' }]} unit="" color="#ef4444" latestValues={chartLatestValues} />
              <ChartSection title="NOx Index" data={chartData} lines={[{ key: 'nox', name: 'NOx' }]} unit="" color="#f43f5e" latestValues={chartLatestValues} />
              <ChartSection title="Formaldehyde (HCHO)" data={chartData} lines={[{ key: 'hcho', name: 'HCHO' }]} unit="ppb" color="#d946ef" latestValues={chartLatestValues} />
            </div>
          )}

          {chartGroup === 'pm' && (
            <div className="grid grid-cols-1 xl:grid-cols-2 gap-3 md:gap-4">
              <ChartSection title="PM0.5" data={chartData} lines={[{ key: 'pm05', name: 'PM0.5' }]} unit="#/cm\u00B3" color="#14b8a6" latestValues={chartLatestValues} />
              <ChartSection title="PM1.0" data={chartData} lines={[{ key: 'pm1', name: 'PM1.0' }]} unit="\u00B5g/m\u00B3" color="#a78bfa" latestValues={chartLatestValues} />
              <ChartSection title="PM2.5" data={chartData} lines={[{ key: 'pm25', name: 'PM2.5' }]} unit="\u00B5g/m\u00B3" color="#8b5cf6" latestValues={chartLatestValues} />
              <ChartSection
                title="PM10 + PM4.0"
                data={chartData}
                lines={[
                  { key: 'pm10', name: 'PM10', color: '#6d28d9' },
                  { key: 'pm4', name: 'PM4.0', color: '#0ea5e9' },
                ]}
                unit="\u00B5g/m\u00B3"
                latestValues={chartLatestValues}
              />
            </div>
          )}
        </div>
      )}

      {activeTab === 'events' && (
        <div className="space-y-3 animate-in fade-in duration-300">
          <div className="bg-gray-800 rounded-xl p-4 md:p-5 border border-gray-700/50">
             <div className="text-gray-400 text-xs font-bold uppercase tracking-wider mb-3">System Log</div>
             <div className="text-[11px] text-gray-500 mb-2">
               {eventsApiLive ? 'Live log: /api/events' : 'Preview fallback log (API unavailable)'}
             </div>
             {alerts.length > 0 ? (
               alerts.map((alert, i) => (
                 <AlertItem key={i} {...alert} />
               ))
             ) : (
               <div className="text-sm text-gray-400 py-3">No events yet.</div>
             )}
          </div>
        </div>
      )}

      {activeTab === 'settings' && (
        <div className="space-y-3 md:space-y-4 animate-in fade-in duration-300">
          {settingsPreviewOnly && (
            <div className="rounded-xl border border-yellow-500/30 bg-yellow-500/10 px-3 py-2 text-[11px] md:text-xs text-yellow-200">
              Preview only: settings controls are disabled in this web prototype.
            </div>
          )}
          <div className="grid grid-cols-1 lg:grid-cols-2 gap-3 md:gap-4">
            <SettingGroup title="Quick Actions">
              <SettingToggle 
                label="Night Mode" 
                icon={Moon}
                enabled={settings.nightMode} 
                onClick={() => toggleSetting('nightMode')}
                disabled={settingsPreviewOnly}
              />
              <SettingToggle 
                label="Alert Blink" 
                icon={Bell}
                enabled={settings.alertBlink} 
                onClick={() => toggleSetting('alertBlink')}
                disabled={settingsPreviewOnly}
              />
              <SettingToggle 
                label="Display Backlight" 
                icon={Sun}
                enabled={settings.backlight} 
                onClick={() => toggleSetting('backlight')}
                disabled={settingsPreviewOnly}
              />
            </SettingGroup>

            <SettingGroup title="Sensor Calibration">
              <SettingStepper 
                label="Temperature Offset" 
                value={settings.tempOffset} 
                unit="\u00B0C"
                stepHint="Step: 0.1 \u00B0C"
                onDec={() => updateOffset('tempOffset', -0.1)}
                onInc={() => updateOffset('tempOffset', 0.1)}
                disabled={settingsPreviewOnly}
              />
              <SettingStepper 
                label="Humidity Offset" 
                value={settings.humOffset} 
                unit="%RH"
                stepHint="Step: 1 %"
                onDec={() => updateOffset('humOffset', -1)}
                onInc={() => updateOffset('humOffset', 1)}
                disabled={settingsPreviewOnly}
              />
            </SettingGroup>

            <SettingGroup title="Connectivity">
              <SettingInfoRow label="WiFi SSID" value={connectivity.wifiSsid} valueClassName="text-white text-sm" mono />
              <SettingInfoRow label="Hostname" value={connectivity.hostname} valueClassName="text-white text-sm" mono />
              <SettingInfoRow label="IP Address" value={connectivity.ip} valueClassName="text-white text-sm" mono />
              <SettingInfoRow label="Signal" value={`${connectivity.rssi} dBm`} valueClassName={signalClass} />
              <SettingInfoRow label="MQTT Broker" value={connectivity.mqttBroker} valueClassName="text-gray-300 text-sm" mono />
              <SettingInfoRow
                label="MQTT Status"
                value={connectivity.mqttConnected ? 'Connected' : 'Disconnected'}
                valueClassName={connectivity.mqttConnected ? "text-emerald-400 text-sm font-semibold" : "text-red-400 text-sm font-semibold"}
              />
            </SettingGroup>

            <SettingGroup title="System">
              <div className="space-y-1">
                <SettingInfoRow label="Firmware" value={firmwareVersion} valueClassName="text-gray-200 text-sm" mono />
                <SettingInfoRow label="Build" value={firmwareBuild} valueClassName="text-gray-300 text-sm" mono />
                <SettingInfoRow label="Uptime" value={uptime} valueClassName="text-gray-200 text-sm" mono />
                <SettingInfoRow label="Web URL" value={localWebUrl} valueClassName="text-cyan-300 text-sm" mono />
              </div>
              <button
                disabled={settingsPreviewOnly}
                className={`w-full mt-2 border py-2.5 rounded-lg text-sm font-semibold flex items-center justify-center gap-2 ${
                  settingsPreviewOnly
                    ? 'bg-gray-700/35 text-gray-400 border-gray-600/50 cursor-not-allowed'
                    : 'bg-red-500/10 hover:bg-red-500/20 text-red-400 border-red-500/40 transition-colors'
                }`}
              >
                <RotateCw size={14} /> Reboot Device
              </button>
            </SettingGroup>
          </div>
        </div>
      )}
    </div>
  );
}
const root = ReactDOM.createRoot(document.getElementById("root"));
root.render(<AuraDashboard />);
/*__INLINE_APP_END__*/
  </script>
</body>
</html>
)HTML_DASH";

} // namespace WebTemplates
