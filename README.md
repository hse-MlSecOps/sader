# SADER

SADER (Self-Describing Agent Discovery & Execution Runtime) — учебный C++20 runtime для регистрации, поиска, описания и вызова `Worker`. Точка входа — интерактивная программа [src/main.cpp](src/main.cpp); реализация пока синхронная.

## Статус

**IMPLEMENTED:** `WorkerRegistry` хранит зарегистрированные объекты, `WorkerFactory` создаёт семь конкретных `Worker`, `Executor` обрабатывает `DISCOVER`, `DESCRIBE`, `CALL` и печатает ответ в консоль. Парсер находится в `main.cpp`. В `main()` регистрируются `hash`, `file`, `csv`, `http`, `text`, `process`, `model`. Подробнее: [контракт Worker](docs/worker-contract.md).

| Worker | Текущая реализация |
| --- | --- |
| `hash`, `csv` | Только `name()` и `description()`; `schema()` и `execute()` бросают исключения базового класса. |
| `file` | `path`: чтение обычного файла размером до 1 MiB, текстовый вывод с метаданными. |
| `http` | GET по `url` через libcurl; в CLI возвращает body для HTTP 2xx. `schema()` не переопределён. |
| `text` | `operation=length|words|lines|find|replace`, строковый результат. |
| `process` | `command` из списка разрешённых команд и необязательный `args`; запуск через `popen`, возврат строки с `stdout` и `exit_code`. |
| `model` | Через `CALL` доступен `operation=classify` для двух векторов; `normalize()` и `cosine()` доступны как методы C++, но не через `CALL`. |

**TESTED (по исходникам тестов, без утверждения об их успешном запуске):** предусмотрены проверки реестра, фабрики, HTTP, файла, процесса и модели; список CTest и ограничения — в [руководстве разработчика](docs/development.md). Проверки парсера и `Executor` отсутствуют.

**PLANNED:** ThreadPool и асинхронное выполнение Worker, Docker-упаковка, GitHub Actions CI. Их реализации в текущем дереве нет.

**UNKNOWN:** воспроизводимость сборки/тестов на Linux и macOS и политика параллельного доступа к Worker не подтверждены. Вопросы — в [архитектуре](docs/architecture.md).

## Устройство

`main()` создаёт `CurlGlobal`, затем `WorkerRegistry`, заполняет его результатами `WorkerFactory::create()` и передаёт ссылку в `Executor`. `parseCommand()` преобразует введённую строку в `Command`; `Executor` обращается к реестру и методам Worker. Заголовки Core — [include/sader](include/sader), реализации — [src](src), конкретные Worker — [workers](workers), тесты — [tests](tests) и [workers/model/ModelWorkerTest.cpp](workers/model/ModelWorkerTest.cpp). См. [диаграмму и жизненный цикл](docs/architecture.md).

## Требования и сборка

Нужны компилятор с C++20, CMake **не ниже 3.20** (требование [Sader/CMakeLists.txt](Sader/CMakeLists.txt)), libcurl с заголовками для `find_package(CURL REQUIRED)`. Для preset нужен Ninja. Команды из корня репозитория:

```bash
cmake -S . -B build
cmake --build build
./build/Sader/Sader
```

На Linux/macOS доступны платформенные presets `linux-debug` / `macos-debug` с Ninja; подробные инструкции и тесты — [docs/development.md](docs/development.md). Эти команды приведены как инструкции, а не как отчёт об успешной сборке.

## Интерактивный CLI

Программа читает одну команду на строку, выводит приглашение `SADER> `. Например:

```text
DISCOVER text
DESCRIBE file
CALL text operation=length text=hello
CALL model operation=classify vector=1,0 other=1,0 threshold=0.5
CALL file path=README.md
EXIT
```

`DISCOVER <query>` ищет **подстроку в description** (регистр учитывается), `DESCRIBE <workerName>` и `CALL <workerName> key=value ...` ищут точное имя. `DISCOVER` и `DESCRIBE` берут весь остаток строки без удаления пробелов; аргументы `CALL` разбиваются по пробельным символам, кавычки не обрабатываются, повторяющиеся ключи перезаписываются. `EXIT`, `exit`, `QUIT`, `quit` завершают сеанс. Ошибки парсинга и исключения при выполнении выводятся как `ERROR: ...` в stderr. См. [src/main.cpp](src/main.cpp) и [src/Executor.cpp](src/Executor.cpp).

## Документация

- [Архитектура, последовательность CALL, владение и открытые вопросы](docs/architecture.md)
- [Контракт Worker и добавление модуля](docs/worker-contract.md)
- [Окружение, сборка, тесты и отладка](docs/development.md)
