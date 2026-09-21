# SADER

**SADER — Self-Describing Agent Discovery & Execution Runtime**

Учебный проект по разработке self-describing runtime для регистрации, обнаружения и выполнения возможностей (`capabilities`), предоставляемых независимыми Worker-модулями.

Проект разрабатывается в рамках курса **MlSecDevOps**.

---

## 1. Идея проекта

SADER представляет собой runtime, в котором каждая доступная операция реализуется отдельным `Worker`.

Каждый Worker самостоятельно описывает предоставляемую capability и в дальнейшем должен содержать всю информацию, необходимую для:

- обнаружения capability;
- получения её описания;
- проверки входных аргументов;
- выполнения операции;
- формирования структурированного результата.

Основной принцип архитектуры:

> **Worker является источником истины для своей capability.**

`Executor` не должен содержать специальную логику для конкретных Worker'ов.

Например, внутри `Executor` не должно появляться:

```cpp
if (workerName == "hash")
{
    // HashWorker-specific logic
}
else if (workerName == "json")
{
    // JsonWorker-specific logic
}
```

Новый Worker должен подключаться через общий интерфейс `Worker` и регистрироваться без изменения внутренней логики `Executor`.

---

## 2. Текущий статус

На текущем этапе реализован базовый каркас SADER:

- базовый интерфейс `Worker`;
- `Executor`;
- обработка команд;
- `HashWorker`;
- `FileWorker`;
- заглушки остальных Worker'ов;
- регистрация Worker'ов через общий интерфейс;
- CMake-сборка проекта;
- структура проекта разделена на Core и независимые Worker-модули.

Интерфейс Worker на текущем этапе:

```cpp
class Worker
{
public:
    virtual std::string name() const = 0;
    virtual std::string description() const = 0;

    virtual ~Worker() = default;
};
```

Контракт будет расширяться по мере реализации `Schema`, `Result`, валидации аргументов и выполнения операций.

---

## 3. Архитектура

Упрощённо архитектура выглядит следующим образом:

```text
                        SADER
                          |
                      Executor
                          |
                          v
                        Worker
                          ^
          +---------------+---------------+
          |               |               |
          |               |               |
     HashWorker       FileWorker      JsonWorker
          |                               |
          |                              ...
          |
     concrete capability
```

`Worker` является абстрактным базовым классом.

Все конкретные Worker'ы наследуются от него:

```text
Worker
 |
 +-- HashWorker
 +-- FileWorker
 +-- JsonWorker
 +-- HttpWorker
 +-- TextWorker
 +-- CsvWorker
 +-- ProcessWorker
 +-- ModelWorker
```

`Executor` работает с Worker'ами через базовый интерфейс:

```cpp
std::unique_ptr<Worker>
```

Это позволяет добавлять новые реализации без привязки `Executor` к конкретному типу Worker.

---

## 4. Структура репозитория

```text
sader/
|
+-- include/
|   +-- sader/
|       +-- Command.h
|       +-- Worker.h
|       +-- Executor.h
|
+-- src/
|   +-- main.cpp
|   +-- Worker.cpp
|   +-- Executor.cpp
|   +-- stdafx.h
|
+-- workers/
|   |
|   +-- hash/
|   |   +-- HashWorker.h
|   |   +-- HashWorker.cpp
|   |
|   +-- file/
|   |   +-- FileWorker.h
|   |   +-- FileWorker.cpp
|   |
|   +-- json/
|   |   +-- JsonWorker.h
|   |   +-- JsonWorker.cpp
|   |
|   +-- http/
|   |   +-- HttpWorker.h
|   |   +-- HttpWorker.cpp
|   |
|   +-- text/
|   |   +-- TextWorker.h
|   |   +-- TextWorker.cpp
|   |
|   +-- csv/
|   |   +-- CsvWorker.h
|   |   +-- CsvWorker.cpp
|   |
|   +-- process/
|   |   +-- ProcessWorker.h
|   |   +-- ProcessWorker.cpp
|   |
|   +-- model/
|       +-- ModelWorker.h
|       +-- ModelWorker.cpp
|
+-- CMakeLists.txt
+-- CMakePresets.json
+-- README.md
```

### `include/sader`

Публичный интерфейс Core-компонентов SADER.

Здесь находятся:

- `Command`;
- `Worker`;
- `Executor`.

### `src`

Реализация Core и точка входа приложения.

### `workers`

Каждая capability располагается в собственной директории.

Принцип:

```text
one Worker = one directory
```

Это позволяет разрабатывать Worker'ы независимо друг от друга и уменьшает количество конфликтов между командами.

---

# 5. Worker'ы

Проект содержит восемь Worker-модулей.

## HashWorker

Директория:

```text
workers/hash/
```

Назначение:

- вычисление хеша строки;
- вычисление хеша файла;
- поддержка SHA-256;
- поддержка MD5.

---

## FileWorker

Директория:

```text
workers/file/
```

Назначение:

- безопасное чтение текстовых файлов;
- получение метаданных файла.

---

## JsonWorker

Директория:

```text
workers/json/
```

Назначение:

- разбор JSON;
- извлечение данных по заданному пути.

---

## HttpWorker

Директория:

```text
workers/http/
```

Назначение:

- выполнение разрешённых HTTP GET-запросов;
- получение HTTP status;
- получение body;
- получение headers.

---

## TextWorker

Директория:

```text
workers/text/
```

Назначение:

- определение длины текста;
- подсчёт слов;
- подсчёт строк;
- поиск;
- замена текста.

---

## CsvWorker

Директория:

```text
workers/csv/
```

Назначение:

- чтение CSV;
- подсчёт элементов;
- вычисление `sum`;
- вычисление `mean`;
- поиск `min`;
- поиск `max`.

---

## ProcessWorker

Директория:

```text
workers/process/
```

Назначение:

- запуск разрешённых локальных команд;
- получение `stdout`;
- получение exit code.

Команды должны выполняться только в рамках определённого разрешённого списка (`whitelist`).

---

## ModelWorker

Директория:

```text
workers/model/
```

Назначение:

- нормализация векторов;
- cosine similarity;
- threshold classification.

Операции должны быть детерминированными.

---

# 6. Требования к каждому Worker

Каждая команда должна реализовать свой Worker согласно единому контракту.

## 6.1. Имя и описание

Worker должен иметь:

- уникальное имя;
- понятное текстовое описание capability.

Например:

```cpp
std::string JsonWorker::name() const
{
    return "json";
}

std::string JsonWorker::description() const
{
    return "Parse JSON and extract data by path";
}
```

Имя Worker должно быть уникальным в рамках SADER.

---

## 6.2. Описание входных аргументов

Worker должен объявлять входные аргументы своей операции.

Для каждого аргумента необходимо определить:

- имя;
- тип;
- является ли аргумент обязательным;
- ограничения.

Конкретная структура описания аргументов будет определяться общим контрактом SADER.

Worker должен быть источником этой информации.

То есть описание аргументов не должно дублироваться внутри `Executor`.

---

## 6.3. Валидация

Worker обязан проверять входные данные **до выполнения операции**.

Если аргументы нарушают контракт:

- операция не выполняется;
- Worker возвращает корректную информацию об ошибке;
- внутреннее состояние Worker не должно повреждаться.

Примеры ошибок:

```text
required argument is missing
invalid argument type
unsupported algorithm
file does not exist
invalid JSON
command is not allowed
```

---

## 6.4. Result

Результат выполнения Worker должен быть структурированным.

Он должен позволять представить как успешное выполнение:

```text
success
result
```

так и ошибку:

```text
failure
error description
```

Не следует использовать обычный вывод в консоль как основной способ возврата результата из Worker.

Консольный вывод может использоваться демонстрационным приложением, но Worker должен возвращать результат через общий контракт SADER.

---

## 6.5. Управление ресурсами

Worker должен корректно управлять всеми используемыми ресурсами.

Используем принцип:

> **RAII — Resource Acquisition Is Initialization**

Необходимо избегать ручного управления ресурсами там, где существуют стандартные RAII-абстракции.

Предпочтительно использовать:

```cpp
std::unique_ptr
std::shared_ptr       // только когда действительно требуется shared ownership
std::vector
std::string
std::fstream
```

вместо ручного управления:

```cpp
new
delete
malloc
free
```

---

## 6.6. Exception safety

Исключение не должно оставлять:

- открытые ресурсы;
- некорректное внутреннее состояние;
- частично изменённые данные, нарушающие контракт объекта.

Используем RAII и стандартные C++-контейнеры для автоматического управления временем жизни ресурсов.

---

## 6.7. Демонстрация / тестирование

Каждый Worker должен иметь небольшой демонстрационный тест.

Минимальный набор:

```text
1 successful call
2 invalid calls
```

Например:

```text
HashWorker

[OK]
SHA-256("hello")

[ERROR]
unknown hash algorithm

[ERROR]
missing input
```

Ошибочные вызовы должны проверять реальную валидацию Worker, а не искусственно генерировать ошибку в тесте.

---

## 6.8. Регистрация

Worker должен быть готов к регистрации через общий Worker Registry.

Добавление Worker не должно требовать изменения внутренней реализации `Executor`.

Концептуально регистрация выглядит следующим образом:

```cpp
Executor executor;

executor.addWorker(std::make_unique<HashWorker>());
executor.addWorker(std::make_unique<FileWorker>());
executor.addWorker(std::make_unique<JsonWorker>());
```

В дальнейшем регистрация будет вынесена в отдельный Worker Registry.

---

# 7. Добавление нового Worker

Каждый новый Worker создаётся в собственной директории:

```text
workers/<worker-name>/
```

Например:

```text
workers/json/
├── JsonWorker.h
└── JsonWorker.cpp
```

Класс наследуется от:

```cpp
Worker
```

Пример минимального header:

```cpp
#pragma once

#include <sader/Worker.h>

class JsonWorker final : public Worker
{
public:
    std::string name() const override;
    std::string description() const override;
};
```

Минимальная реализация:

```cpp
#include "stdafx.h"

#include "JsonWorker.h"

std::string JsonWorker::name() const
{
    return "json";
}

std::string JsonWorker::description() const
{
    return "Parse JSON and extract data by path";
}
```

После реализации `.cpp` необходимо добавить в соответствующий CMake target.

Например:

```cmake
"${CMAKE_SOURCE_DIR}/workers/json/JsonWorker.cpp"
```

После этого Worker должен успешно собираться вместе с проектом.

---

# 8. Правила работы с заголовочными файлами

Core headers подключаются через:

```cpp
#include <sader/Worker.h>
#include <sader/Executor.h>
#include <sader/Command.h>
```

Worker-модули:

```cpp
#include <workers/hash/HashWorker.h>
#include <workers/file/FileWorker.h>
```

Каждый header должен самостоятельно объявлять необходимые ему зависимости.

Не следует рассчитывать на то, что необходимый стандартный header был случайно подключён другим файлом.

Например, если header непосредственно использует:

```cpp
std::string
```

он должен иметь:

```cpp
#include <string>
```

---

# 9. Precompiled Headers

Проект использует:

```text
src/stdafx.h
```

для precompiled headers.

PCH предназначен преимущественно для стабильных и часто используемых заголовков стандартной библиотеки.

Например:

```cpp
#pragma once

#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
```

Не следует помещать в `stdafx.h` Worker-specific headers:

```cpp
#include <workers/hash/HashWorker.h> // не рекомендуется
```

или Core API только ради получения транзитивных зависимостей.

Собственные `.h` должны оставаться максимально самодостаточными.

---

# 10. Сборка проекта

Проект использует **CMake** и требует поддержку **C++20**.

Общий процесс:

```bash
cmake -S . -B build
cmake --build build
```

При работе через Visual Studio CMake-проект может быть сконфигурирован и собран непосредственно из IDE.

После любых изменений необходимо убедиться, что:

```text
CMake Configure    OK
Compilation        OK
Linking            OK
Application Run    OK
```

---

# 11. Правила командной разработки

В проекте одновременно работают несколько команд. Чтобы изменения разных команд не конфликтовали между собой, необходимо соблюдать единый Git workflow.

## 11.1. Главное правило

> **Ветка `main` является общей стабильной веткой проекта. Напрямую работать и делать commit в `main` запрещено.**

Каждая команда:

1. клонирует репозиторий;
2. получает актуальное состояние `main`;
3. переключается на заранее подготовленную ветку своей команды;
4. работает только над Worker своей команды;
5. не изменяет Worker других команд и Core без согласования;
6. регулярно синхронизирует свою ветку с `main`;
7. после завершения работы создаёт Pull Request в `main`.

Рабочие ветки команд создаются и публикуются владельцем проекта заранее.

Напрямую создавать изменения и выполнять push в `main` запрещено.
Слияние изменений выполняется только через Pull Request.

---

# 12. Распределение Worker'ов между командами

Каждая команда отвечает за свой Worker и работает в заранее подготовленной ветке.

| Команда | Worker | Рабочая директория | Ветка |
|---|---|---|---|
| Team-1 | HashWorker | `workers/hash/` | `feature/hash-worker` |
| Team-2 | FileWorker | `workers/file/` | `feature/file-worker` |
| Team-3 | JsonWorker | `workers/json/` | `feature/json-worker` |
| Team-4 | HttpWorker | `workers/http/` | `feature/http-worker` |
| Team-5 | TextWorker | `workers/text/` | `feature/text-worker` |
| Team-6 | CsvWorker | `workers/csv/` | `feature/csv-worker` |
| Team-7 | ProcessWorker | `workers/process/` | `feature/process-worker` |
| Team-8 | ModelWorker | `workers/model/` | `feature/model-worker` |

Например, команда `Team-3` работает с:

```text
workers/json/
├── JsonWorker.h
└── JsonWorker.cpp
```

и не должна изменять:

```text
workers/hash/
workers/file/
workers/http/
workers/text/
workers/csv/
workers/process/
workers/model/
```

без предварительного согласования.

---

# 13. Начало работы

Рабочие ветки для всех команд уже созданы владельцем проекта от актуальной версии `main`.

Самостоятельно создавать отдельную ветку для реализации Worker не требуется.

## 13.1. Клонирование репозитория

При первом получении проекта:

```bash
git clone <repository-url>
cd sader
git fetch origin
```

После клонирования локально будет доступен `main`, а рабочие ветки команд находятся в remote `origin`.

Проверить доступные ветки:

```bash
git branch -a
```

Например:

```text
* main
  remotes/origin/main
  remotes/origin/feature/hash-worker
  remotes/origin/feature/file-worker
  remotes/origin/feature/json-worker
  ...
```

---

## 13.2. Переключение на ветку своей команды

Каждая команда переключается **только на назначенную ей рабочую ветку**.

Например, для Team-3:

```bash
git switch --track origin/feature/json-worker
```

После этого:

```bash
git branch --show-current
```

должно вывести:

```text
feature/json-worker
```

Для остальных команд используется соответствующая ветка из таблицы выше.

Например:

```bash
# Team-1
git switch --track origin/feature/hash-worker

# Team-4
git switch --track origin/feature/http-worker

# Team-8
git switch --track origin/feature/model-worker
```

Команда не должна выполнять разработку в:

```text
main
```

и не должна использовать рабочую ветку другой команды.

---

# 14. Ежедневная работа

Перед началом работы убедитесь, что используется правильная ветка:

```bash
git branch --show-current
```

Например, для Team-3:

```text
feature/json-worker
```

Получить последние изменения своей команды:

```bash
git pull
```

После этого можно продолжать разработку Worker.

Например, Team-3 работает преимущественно с:

```text
workers/json/
├── JsonWorker.h
└── JsonWorker.cpp
```

После внесения изменений:

```bash
git status
git diff
```

Затем:

```bash
git add .
git commit -m "feat: implement JsonWorker"
git push
```

Таким образом:

```text
                GitHub
                  |
             main (protected)
                  |
       +----------+----------+
       |          |          |
       v          v          v
   Team-1      Team-2      Team-3
     |            |           |
feature/       feature/     feature/
hash-worker   file-worker   json-worker
     |            |           |
 HashWorker    FileWorker   JsonWorker
```

Все изменения команды отправляются в её рабочую ветку.

---

# 15. Важное ограничение

Команда работает только в назначенной ей области проекта.

Например:

```text
Team-3
  |
  +-- feature/json-worker
          |
          +-- workers/json/
```

Team-3 не должна без согласования изменять:

```text
workers/hash/
workers/file/
workers/http/
workers/text/
workers/csv/
workers/process/
workers/model/

include/sader/
src/
```

Если для реализации Worker требуется изменение общего контракта `Worker`, `Executor`, `Command`, CMake или другого Core-компонента, изменение сначала необходимо согласовать с владельцем проекта.

Это необходимо потому, что изменение Core может одновременно повлиять на все восемь команд.

---

# 16. Что команда должна реализовать

Каждый Worker должен:

- иметь уникальное имя;
- иметь понятное описание capability;
- объявлять входные аргументы;
- описывать типы аргументов;
- определять обязательные и необязательные аргументы;
- определять ограничения аргументов;
- проверять входные данные до выполнения операции;
- не выполнять операцию при нарушении контракта;
- возвращать структурированный `Result`;
- корректно сообщать об ошибках;
- использовать RAII для управления ресурсами;
- оставаться в корректном состоянии при исключениях;
- иметь демонстрацию успешного вызова;
- иметь минимум два ошибочных вызова;
- регистрироваться через общий Worker Registry без изменения `Executor`.

---

# 17. Что команда НЕ должна изменять

Без предварительного согласования не следует изменять Core:

```text
include/sader/
src/
```

В частности:

```text
Worker.h
Executor.h
Command.h
Worker.cpp
Executor.cpp
main.cpp
```

Также команда не должна изменять Worker другой команды.

Например, при разработке `CsvWorker` не следует одновременно исправлять:

```text
workers/http/HttpWorker.cpp
```

даже если там была замечена проблема.

Такое изменение необходимо вынести в отдельную задачу / Issue / Pull Request либо согласовать с владельцем соответствующего Worker.

Это позволяет сохранить Pull Request небольшим и избежать конфликтов между командами.

---

# 18. Commit изменений

После реализации необходимо проверить изменения:

```bash
git status
```

Посмотреть diff:

```bash
git diff
```

После проверки:

```bash
git add .
```

Создать commit.

Например:

```bash
git commit -m "feat: implement JsonWorker"
```

Рекомендуемые типы commit:

```text
feat:     новая функциональность
fix:      исправление ошибки
refactor: изменение структуры без изменения поведения
test:     добавление или изменение тестов
docs:     документация
chore:    служебные изменения
```

Примеры:

```text
feat: implement CsvWorker
fix: validate empty file path
test: add HashWorker invalid input tests
docs: update JsonWorker documentation
```

---

# 19. Push рабочей ветки

Первый push новой ветки:

```bash
git push -u origin feature/json-worker
```

После этого последующие изменения можно отправлять:

```bash
git push
```

Изменения отправляются в рабочую ветку команды, а не непосредственно в `main`.

---

# 20. Синхронизация с main

Пока команда работает над Worker, `main` может измениться из-за Pull Request других команд.

Перед созданием своего Pull Request необходимо получить актуальный `main`.

Сначала убедитесь, что все собственные изменения сохранены в commit.

Затем:

```bash
git switch main
git pull origin main
```

Вернуться в рабочую ветку:

```bash
git switch feature/json-worker
```

После этого необходимо синхронизировать рабочую ветку с `main`.

На текущем этапе проекта рекомендуется:

```bash
git merge main
```

Если Git обнаружит конфликты, их необходимо разрешить локально, после чего снова собрать и протестировать проект.

После успешного merge:

```bash
git push
```

Синхронизацию с `main` выполняет один из участников команды осознанно, чтобы несколько разработчиков одновременно не создавали разные merge-коммиты.

Перед Pull Request рабочая ветка обязательно должна быть синхронизирована с актуальным `main`.

```
origin/main
    │
    │ git pull
    ▼
local main
    │
    │ git merge main
    ▼
feature/json-worker
    │
    │ build + tests
    ▼
origin/feature/json-worker
```

---

# 21. Проверка перед Pull Request

Перед созданием Pull Request команда должна убедиться, что:

- используется актуальный `main`;
- рабочая ветка успешно синхронизирована с `main`;
- CMake Configure выполняется успешно;
- проект полностью компилируется;
- отсутствуют linker errors;
- приложение запускается;
- Worker реализует требуемый контракт;
- Worker имеет уникальное имя;
- Worker имеет понятное описание;
- входные данные валидируются;
- реализован успешный тестовый вызов;
- реализовано минимум два ошибочных вызова;
- ресурсы управляются через RAII;
- Worker не требует специальной логики внутри `Executor`;
- случайно не изменены файлы других команд.

Последний пункт удобно проверить:

```bash
git diff main...HEAD
```

В diff должны находиться только изменения, относящиеся к задаче команды.

---

# 22. Pull Request

После завершения работы создаётся Pull Request:

```text
feature/json-worker
        |
        | Pull Request
        v
       main
```

Название Pull Request должно кратко описывать изменение.

Например:

```text
feat: implement JsonWorker
```

В описании PR необходимо указать:

```text
## Changes

- Implemented JsonWorker.
- Added argument validation.
- Added structured result handling.
- Added demonstration/tests.

## Tests

- Successful call: passed.
- Invalid call #1: passed.
- Invalid call #2: passed.
- Full project build: passed.
```

После проверки Pull Request изменения объединяются с `main`.

---

# 23. После слияния Pull Request

После merge старую рабочую ветку больше не следует использовать для новой задачи.

Локально:

```bash
git switch main
git pull origin main
```

После этого старую ветку можно удалить:

```bash
git branch -d feature/json-worker
```

Для следующей задачи от актуального `main` создаётся новая ветка:

```bash
git switch -c feature/new-task
```

То есть жизненный цикл задачи выглядит так:

```text
                 git pull
                    |
                    v
                  main
                    |
                    | git switch -c
                    v
           feature/json-worker
                    |
                 development
                    |
                  commit
                    |
                   push
                    |
              Pull Request
                    |
                    v
                  main
```

---

# 24. Краткая памятка

Для повседневной работы достаточно помнить следующий сценарий:

```bash
# 1. Получить актуальный main
git switch main
git pull origin main

# 2. Создать рабочую ветку
git switch -c feature/json-worker

# 3. Разработать свой Worker

# 4. Проверить изменения
git status
git diff

# 5. Commit
git add .
git commit -m "feat: implement JsonWorker"

# 6. Синхронизироваться с актуальным main
git switch main
git pull origin main
git switch feature/json-worker
git merge main

# 7. Повторно собрать и протестировать проект

# 8. Push
git push -u origin feature/json-worker

# 9. Создать Pull Request
# feature/json-worker -> main
```

> **Никогда не выполняйте разработку непосредственно в `main`.**

> **Каждая команда работает только над своим Worker.**

> **Изменения Core и файлов других команд выполняются только после согласования.**

> **Любые изменения попадают в `main` только через Pull Request.**