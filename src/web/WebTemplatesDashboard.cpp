// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#include "web/WebTemplates.h"

namespace WebTemplates {

const char kDashboardPageTemplate[] PROGMEM = R"HTML_DASH(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Project Aura</title>
  <style>
    :root {
      --bg: #0f172a;
      --panel: rgba(30, 41, 59, 0.75);
      --border: rgba(255, 255, 255, 0.12);
      --text: #f1f5f9;
      --text-dim: #94a3b8;
      --accent: #22d3ee;
    }
    * { box-sizing: border-box; }
    html, body {
      margin: 0;
      padding: 0;
      min-height: 100%;
      color: var(--text);
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      background: radial-gradient(900px 500px at -20% -20%, rgba(34, 211, 238, 0.18), transparent 60%), var(--bg);
    }
    body {
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 16px;
    }
    .card {
      width: 100%;
      max-width: 560px;
      background: var(--panel);
      border: 1px solid var(--border);
      border-radius: 16px;
      padding: 20px;
      text-align: center;
      backdrop-filter: blur(8px);
      -webkit-backdrop-filter: blur(8px);
    }
    h1 {
      margin: 0 0 10px;
      font-size: 24px;
      letter-spacing: -0.02em;
    }
    p {
      margin: 0 0 12px;
      color: var(--text-dim);
      line-height: 1.5;
    }
    a {
      color: var(--accent);
      font-weight: 700;
      text-decoration: none;
    }
    a:hover { text-decoration: underline; }
  </style>
</head>
<body>
  <div class="card">
    <h1>Project Aura Dashboard</h1>
    <p>
      This compatibility page is fully local and CDN-free.
      Use the active dashboard endpoint below.
    </p>
    <p><a href="/dashboard">Open /dashboard</a></p>
  </div>
</body>
</html>
)HTML_DASH";

} // namespace WebTemplates
