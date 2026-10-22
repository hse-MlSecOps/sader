#include "WebServer.h"

#include <httplib.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <optional>

namespace {

const char* kIndexHtml = R"html(<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>JSON Store · SADER</title>
<style>
  :root {
    --bg: #0f1419;
    --panel: #171d25;
    --panel-2: #1c2430;
    --border: #2a3544;
    --text: #e7eef7;
    --muted: #8b9bb0;
    --accent: #5b9dff;
    --accent-2: #7c5cff;
    --ok: #3dd68c;
    --err: #ff6b7a;
    --warn: #ffc857;
    --mono: "JetBrains Mono", "Fira Code", ui-monospace, monospace;
    --sans: "Segoe UI", system-ui, sans-serif;
    --radius: 14px;
  }
  * { box-sizing: border-box; }
  body {
    margin: 0;
    font-family: var(--sans);
    background:
      radial-gradient(1200px 600px at 10% -10%, #1a2740 0%, transparent 55%),
      radial-gradient(900px 500px at 100% 0%, #241a40 0%, transparent 50%),
      var(--bg);
    color: var(--text);
    min-height: 100vh;
  }
  header {
    display: flex; align-items: center; justify-content: space-between;
    padding: 22px 28px; border-bottom: 1px solid var(--border);
    backdrop-filter: blur(10px); position: sticky; top: 0; z-index: 5;
    background: rgba(15,20,25,.72);
  }
  .brand { display: flex; align-items: center; gap: 14px; }
  .logo {
    width: 42px; height: 42px; border-radius: 12px;
    background: linear-gradient(135deg, var(--accent), var(--accent-2));
    display: grid; place-items: center; font-weight: 800; letter-spacing: .5px;
    box-shadow: 0 10px 30px rgba(91,157,255,.25);
  }
  h1 { font-size: 18px; margin: 0; font-weight: 650; }
  .sub { color: var(--muted); font-size: 12px; margin-top: 2px; }
  .badge {
    padding: 6px 10px; border-radius: 999px; font-size: 12px;
    background: rgba(61,214,140,.12); color: var(--ok); border: 1px solid rgba(61,214,140,.25);
  }
  .layout {
    display: grid; grid-template-columns: 300px 1fr; gap: 18px;
    padding: 22px 28px 40px; max-width: 1280px; margin: 0 auto;
  }
  @media (max-width: 920px) { .layout { grid-template-columns: 1fr; } }
  .panel {
    background: linear-gradient(180deg, rgba(255,255,255,.03), transparent 40%), var(--panel);
    border: 1px solid var(--border); border-radius: var(--radius);
    padding: 18px; box-shadow: 0 16px 40px rgba(0,0,0,.22);
  }
  .panel h2 {
    margin: 0 0 12px; font-size: 13px; text-transform: uppercase;
    letter-spacing: .08em; color: var(--muted); font-weight: 700;
  }
  .doc-list { display: flex; flex-direction: column; gap: 8px; max-height: 65vh; overflow: auto; }
  .doc-item {
    width: 100%; text-align: left; cursor: pointer;
    background: var(--panel-2); border: 1px solid var(--border);
    color: var(--text); border-radius: 10px; padding: 12px 12px;
    font-family: var(--mono); font-size: 13px;
    transition: .15s ease;
  }
  .doc-item:hover { border-color: var(--accent); transform: translateY(-1px); }
  .doc-item.active {
    border-color: var(--accent);
    background: linear-gradient(180deg, rgba(91,157,255,.16), rgba(91,157,255,.05));
    box-shadow: 0 0 0 1px rgba(91,157,255,.2);
  }
  .empty { color: var(--muted); font-size: 13px; padding: 8px 2px; }
  .actions { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-top: 14px; }
  button, .file-btn {
    border: 1px solid var(--border); background: var(--panel-2); color: var(--text);
    border-radius: 10px; padding: 11px 14px; font-weight: 600; cursor: pointer;
    transition: .15s ease; font-size: 13px;
  }
  button:hover { border-color: var(--accent); }
  button.primary {
    background: linear-gradient(135deg, var(--accent), var(--accent-2));
    border-color: transparent; color: white;
    box-shadow: 0 10px 24px rgba(91,157,255,.28);
  }
  button.danger { color: var(--err); }
  button.danger:hover { border-color: var(--err); }
  .grid { display: grid; gap: 14px; grid-template-columns: 1fr 1fr; }
  @media (max-width: 800px) { .grid { grid-template-columns: 1fr; } }
  label { display: block; font-size: 12px; color: var(--muted); margin-bottom: 6px; font-weight: 600; }
  input, textarea, select {
    width: 100%; background: #0d1218; color: var(--text);
    border: 1px solid var(--border); border-radius: 10px;
    padding: 11px 12px; font-family: var(--mono); font-size: 13px; outline: none;
    transition: .15s ease;
  }
  input:focus, textarea:focus, select:focus { border-color: var(--accent); box-shadow: 0 0 0 3px rgba(91,157,255,.15); }
  textarea { min-height: 170px; resize: vertical; line-height: 1.5; }
  .field { margin-bottom: 12px; }
  .row { display: flex; gap: 10px; flex-wrap: wrap; }
  .row > * { flex: 1; min-width: 140px; }
  .result {
    margin-top: 14px; border-radius: 12px; border: 1px solid var(--border);
    background: #0b1016; padding: 14px; min-height: 110px;
    font-family: var(--mono); font-size: 13px; white-space: pre-wrap; word-break: break-word;
  }
  .result.ok { border-color: rgba(61,214,140,.35); }
  .result.err { border-color: rgba(255,107,122,.35); color: #ffb4bc; }
  .status { display: flex; gap: 10px; align-items: center; margin-bottom: 10px; font-size: 12px; color: var(--muted); }
  .dot { width: 8px; height: 8px; border-radius: 50%; background: var(--muted); }
  .dot.ok { background: var(--ok); box-shadow: 0 0 10px rgba(61,214,140,.6); }
  .dot.err { background: var(--err); box-shadow: 0 0 10px rgba(255,107,122,.55); }
  .toast {
    position: fixed; right: 18px; bottom: 18px; z-index: 20;
    background: var(--panel); border: 1px solid var(--border); color: var(--text);
    padding: 12px 14px; border-radius: 12px; min-width: 220px;
    box-shadow: 0 18px 40px rgba(0,0,0,.35); opacity: 0; transform: translateY(10px);
    transition: .2s ease; pointer-events: none;
  }
  .toast.show { opacity: 1; transform: translateY(0); }
  .toast.ok { border-color: rgba(61,214,140,.4); }
  .toast.err { border-color: rgba(255,107,122,.4); }
  .hint { color: var(--muted); font-size: 12px; line-height: 1.5; }
  code.inline {
    background: rgba(91,157,255,.12); color: #cfe2ff;
    padding: 2px 6px; border-radius: 6px; font-family: var(--mono); font-size: 12px;
  }
  .footer-note { margin-top: 18px; color: var(--muted); font-size: 12px; }
</style>
</head>
<body>
<header>
  <div class="brand">
    <div class="logo">J</div>
    <div>
      <h1>JSON Store · SADER JsonWorker</h1>
      <div class="sub">PostgreSQL-backed document store · put / get / delete / list</div>
    </div>
  </div>
  <div class="badge">worker: json</div>
</header>

<div class="layout">
  <aside class="panel">
    <h2>Документы</h2>
    <div id="docList" class="doc-list"><div class="empty">Загрузка…</div></div>
    <div class="actions">
      <button class="primary" onclick="focusPut()">＋ Новый</button>
      <button onclick="refreshList()">⟳ Обновить</button>
    </div>
    <div class="footer-note">
      Выберите документ, чтобы подставить его в форму.
    </div>
  </aside>

  <main>
    <section class="panel">
      <h2>Операция</h2>
      <div class="grid">
        <div>
          <div class="field">
            <label>op</label>
            <select id="op">
              <option value="put">put — сохранить документ</option>
              <option value="get">get — получить документ / значение</option>
              <option value="delete">delete — удалить документ</option>
              <option value="list">list — список ключей</option>
            </select>
          </div>
          <div class="field">
            <label>key</label>
            <input id="key" placeholder="например: user:ivan" autocomplete="off">
          </div>
          <div class="field">
            <label>path <span class="hint">(только для get, dotted)</span></label>
            <input id="path" placeholder="например: user.city" autocomplete="off">
          </div>
          <div class="row">
            <button class="primary" onclick="runOp()">Выполнить</button>
            <button onclick="clearForm()">Очистить</button>
            <button class="danger" onclick="quickDelete()">Удалить ключ</button>
          </div>
        </div>
        <div>
          <div class="field">
            <label>json <span class="hint">(для put)</span></label>
            <textarea id="json" spellcheck="false">{
  "name": "Ivan",
  "age": 27,
  "user": { "city": "Moscow", "roles": ["admin", "dev"] }
}</textarea>
          </div>
          <div class="row">
            <button onclick="formatJson()">Форматировать JSON</button>
            <button onclick="loadSample()">Пример</button>
          </div>
        </div>
      </div>

      <div class="status">
        <span id="dot" class="dot"></span>
        <span id="statusText">Готово к вызову</span>
      </div>
      <div id="result" class="result">// результат появится здесь</div>
    </section>

    <section class="panel" style="margin-top:18px">
      <h2>Как это работает</h2>
      <div class="hint">
        UI вызывает тот же контракт Worker, что и Executor в SADER:
        <code class="inline">op + arguments → Result</code>.
        Данные лежат в PostgreSQL-таблице <code class="inline">json_documents</code>,
        извлечение значения — по dotted-path вроде <code class="inline">user.city</code>.
      </div>
    </section>
  </main>
</div>

<div id="toast" class="toast"></div>

<script>
const $ = (id) => document.getElementById(id);

function toast(msg, ok = true) {
  const el = $("toast");
  el.textContent = msg;
  el.className = "toast show " + (ok ? "ok" : "err");
  setTimeout(() => el.className = "toast", 1800);
}

function focusPut() {
  $("op").value = "put";
  $("key").focus();
}

function clearForm() {
  $("key").value = "";
  $("path").value = "";
  $("json").value = "";
  setStatus("ready", "Готово к вызову");
  $("result").className = "result";
  $("result").textContent = "// результат появится здесь";
}

function loadSample() {
  $("op").value = "put";
  $("json").value = JSON.stringify({
    name: "Ivan",
    age: 27,
    user: { city: "Moscow", roles: ["admin", "dev"] }
  }, null, 2);
  if (!$("key").value) $("key").value = "user:ivan";
}

function formatJson() {
  try {
    const raw = $("json").value.trim();
    if (!raw) return;
    $("json").value = JSON.stringify(JSON.parse(raw), null, 2);
    toast("JSON отформатирован");
  } catch (e) {
    toast("Невалидный JSON", false);
  }
}

function setStatus(kind, text) {
  $("dot").className = "dot" + (kind === "ok" ? " ok" : kind === "err" ? " err" : "");
  $("statusText").textContent = text;
}

function setResult(kind, payload) {
  const el = $("result");
  el.className = "result " + (kind === "ok" ? "ok" : kind === "err" ? "err" : "");
  if (typeof payload === "string") {
    el.textContent = payload;
  } else {
    el.textContent = JSON.stringify(payload, null, 2);
  }
}

async function refreshList() {
  try {
    const res = await fetch("/api/call", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ op: "list", args: {} })
    });
    const data = await res.json();
    const box = $("docList");
    if (!data.ok) {
      box.innerHTML = `<div class="empty">${data.error}</div>`;
      return;
    }
    const keys = data.value;
    if (!keys.length) {
      box.innerHTML = `<div class="empty">Пока пусто — сохраните документ</div>`;
      return;
    }
    box.innerHTML = keys.map(k =>
      `<button class="doc-item" data-key="${encodeURIComponent(k)}">${k}</button>`
    ).join("");
    box.querySelectorAll(".doc-item").forEach(btn => {
      btn.addEventListener("click", async () => {
        box.querySelectorAll(".doc-item").forEach(b => b.classList.remove("active"));
        btn.classList.add("active");
        const key = decodeURIComponent(btn.dataset.key);
        $("key").value = key;
        $("op").value = "get";
        await runOp();
      });
    });
  } catch (e) {
    toast("Не удалось загрузить список", false);
  }
}

async function runOp() {
  const op = $("op").value;
  const args = {
    key: $("key").value,
    path: $("path").value,
    json: $("json").value
  };
  if (op === "list") {
    delete args.key; delete args.path; delete args.json;
  }

  setStatus("busy", "Выполнение…");
  try {
    const res = await fetch("/api/call", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ op, args })
    });
    const data = await res.json();
    if (data.ok) {
      setStatus("ok", "Успешно");
      let payload = data.value;
      try { payload = JSON.parse(data.value); } catch (_) {}
      setResult("ok", payload);
      toast("Готово");
      if (op === "put" || op === "delete") refreshList();
      if (op === "put") {
        $("op").value = "get";
      }
    } else {
      setStatus("err", "Ошибка");
      setResult("err", data.error);
      toast(data.error || "Ошибка вызова", false);
    }
  } catch (e) {
    setStatus("err", "Сбой запроса");
    setResult("err", String(e));
    toast("Сбой запроса", false);
  }
}

async function quickDelete() {
  const key = $("key").value.trim();
  if (!key) {
    toast("Укажите key", false);
    return;
  }
  $("op").value = "delete";
  await runOp();
}

refreshList();
</script>
</body>
</html>
)html";

std::string toApiResult(const Result& result) {
    nlohmann::json response;
    response["ok"] = result.ok;
    if (result.ok) {
        nlohmann::json value;
        try {
            value = nlohmann::json::parse(result.value);
        } catch (...) {
            value = result.value;
        }
        response["value"] = value;
        response["raw"] = result.value;
    } else {
        response["error"] = result.error;
    }
    return response.dump();
}

std::optional<Result> callWorker(JsonWorker& worker,
                                 const std::string& op,
                                 const nlohmann::json& argsJson) {
    Arguments args;
    if (argsJson.is_object()) {
        for (auto it = argsJson.begin(); it != argsJson.end(); ++it) {
            if (it.value().is_string()) {
                args[it.key()] = it.value().get<std::string>();
            } else if (it.value().is_null()) {
                // skip
            } else {
                args[it.key()] = it.value().dump();
            }
        }
    }
    args["op"] = op;
    return worker.execute(args);
}

}  // namespace

WebServer::WebServer(JsonWorker& worker, std::string host, int port)
    : worker_(worker), host_(std::move(host)), port_(port) {}

int WebServer::run() {
    httplib::Server server;

    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(kIndexHtml, "text/html; charset=utf-8");
    });

    server.Post("/api/call", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            const auto body = nlohmann::json::parse(req.body.empty() ? "{}" : req.body);
            const std::string op = body.value("op", "");
            const auto args = body.contains("args") ? body["args"] : nlohmann::json::object();

            const Result result = callWorker(worker_, op, args).value();
            res.set_content(toApiResult(result), "application/json");
        } catch (const std::exception& e) {
            nlohmann::json response;
            response["ok"] = false;
            response["error"] = std::string("bad request: ") + e.what();
            res.status = 400;
            res.set_content(response.dump(), "application/json");
        }
    });

    server.Get("/api/health", [this](const httplib::Request&, httplib::Response& res) {
        nlohmann::json response;
        response["ok"] = true;
        response["worker"] = worker_.name();
        res.set_content(response.dump(), "application/json");
    });

    std::cout << "SADER JsonWorker UI → http://" << host_ << ":" << port_ << "\n";
    if (!server.listen(host_, port_)) {
        std::cerr << "Failed to listen on " << host_ << ":" << port_ << "\n";
        return 1;
    }
    return 0;
}
