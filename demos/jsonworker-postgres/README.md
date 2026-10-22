# SADER · JsonWorker + PostgreSQL + Web UI

Сквозной учебный проект курса C++ (SADER): self-describing Worker с PostgreSQL-хранилищем JSON-документов и веб-интерфейсом.

## Что внутри

| Файл | Назначение |
|------|------------|
| `Worker.h` | Контракт SADER: `name`, `description`, `schema`, `execute(Arguments)` |
| `JsonWorker.h/.cpp` | JSON-стор на PostgreSQL: `put` / `get` / `delete` / `list` |
| `WebServer.h/.cpp` | HTTP API + красивый UI (cpp-httplib) |
| `main.cpp` | Запуск вебки или CLI-демо |
| `CMakeLists.txt` | Сборка |
| `third_party/httplib.h` | cpp-httplib (single header) |
| `test.docx` | Методичка / постановка задачи |

## Возможности JsonWorker

| `op` | Аргументы | Результат |
|------|-----------|-----------|
| `put` | `key`, `json` | сохраняет документ в таблицу `json_documents` |
| `get` | `key`, опц. `path` | документ целиком или значение по dotted-path (`user.city`, `user.roles.0`) |
| `delete` | `key` | удаляет документ |
| `list` | — | JSON-массив ключей |

Валидация до выполнения, RAII (`pqxx::connection`/`work`), ошибки возвращаются в `Result.error`, исключения не утекают.

## Требования

- Linux (проверено на Ubuntu 24.04)
- PostgreSQL 14+
- g++ с C++17, CMake ≥ 3.16
- пакеты: `libpq-dev`, `libpqxx-dev`, `nlohmann-json3-dev`, `pkg-config`

## Установка зависимостей (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake pkg-config \
  postgresql postgresql-contrib \
  libpq-dev libpqxx-dev nlohmann-json3-dev
```

## Настройка PostgreSQL

```bash
sudo service postgresql start

sudo -u postgres psql <<'SQL'
DROP DATABASE IF EXISTS sader_demo;
DROP ROLE IF EXISTS sader;
CREATE ROLE sader LOGIN PASSWORD 'sader';
CREATE DATABASE sader_demo OWNER sader;
SQL

sudo -u postgres psql -d sader_demo <<'SQL'
CREATE TABLE json_documents (
  key TEXT PRIMARY KEY,
  data JSONB NOT NULL,
  updated_at TIMESTAMPTZ NOT NULL DEFAULT now()
);
GRANT ALL PRIVILEGES ON TABLE json_documents TO sader;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO sader;
SQL
```

> Таблицу `json_documents` воркер создаст и сам (`CREATE TABLE IF NOT EXISTS`), если у роли есть права на схему `public`.

Проверка доступа:

```bash
PGPASSWORD=sader psql -h 127.0.0.1 -U sader -d sader_demo -c "SELECT 1;"
```

DSN по умолчанию (в `main.cpp`):

```
host=127.0.0.1 port=5432 dbname=sader_demo user=sader password=sader
```

## Сборка

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Запуск

### Веб-интерфес

```bash
./build/sader_demo
```

Откройте **http://127.0.0.1:8080/**

- слева — список документов из PostgreSQL
- справа — операции `put / get / delete / list`
- `path` — dotted-path для `get` (например `user.city` или `user.roles.0`)

### CLI-демо (успех + ошибочные вызовы)

```bash
./build/sader_demo --demo
```

## HTTP API

`POST /api/call` — универсальный вызов Worker:

```bash
# сохранить
curl -s -X POST http://127.0.0.1:8080/api/call \
  -H 'Content-Type: application/json' \
  -d '{"op":"put","args":{"key":"user:ivan","json":"{\"name\":\"Ivan\",\"user\":{\"city\":\"Moscow\"}}"}}'

# получить значение по пути
curl -s -X POST http://127.0.0.1:8080/api/call \
  -H 'Content-Type: application/json' \
  -d '{"op":"get","args":{"key":"user:ivan","path":"user.city"}}'

# список ключей
curl -s -X POST http://127.0.0.1:8080/api/call \
  -H 'Content-Type: application/json' \
  -d '{"op":"list","args":{}}'

# удалить
curl -s -X POST http://127.0.0.1:8080/api/call \
  -H 'Content-Type: application/json' \
  -d '{"op":"delete","args":{"key":"user:ivan"}}'
```

Ответ: `{"ok":true,"value":...}` или `{"ok":false,"error":"..."}`.

`GET /api/health` — проверка живости.

## Как это встраивается в SADER

Worker реализует общий контракт:

```cpp
virtual std::string name() const = 0;
virtual std::string description() const = 0;
virtual Schema schema() const = 0;
virtual Result execute(const Arguments& args) = 0;
```

Executor регистрирует его через `std::unique_ptr<Worker>` и вызывает без веток `if (name == "json")` — специфика (JSON/SQL) остаётся внутри `JsonWorker`.

## Структура данных

Таблица PostgreSQL `json_documents`:

| колонка | тип | описание |
|---------|-----|----------|
| `key` | TEXT PK | имя документа |
| `data` | JSONB | тело документа |
| `updated_at` | TIMESTAMPTZ | время обновления |
