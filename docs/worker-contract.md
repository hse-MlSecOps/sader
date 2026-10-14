# Контракт Worker

Источник интерфейса — [include/sader/Worker.h](../include/sader/Worker.h). **IMPLEMENTED:** типы объявлены в глобальном пространстве имён:

```cpp
using Arguments = std::unordered_map<std::string, std::string>;
struct Result { bool success; std::string output; std::string error; };
struct ArgDescriptor {
    std::string name, type;
    bool required;
    std::string description, constraint;
};
struct Schema { std::vector<ArgDescriptor> args; std::string result_description; };
class Worker {
public:
    virtual std::string name() const = 0;
    virtual std::string description() const = 0;
    virtual Schema schema() const;             // по умолчанию бросает runtime_error
    virtual Result execute(const Arguments&); // по умолчанию бросает runtime_error
    virtual ~Worker() = default;
};
```

`Arguments` передаются по `const&`, значения всегда строки. `Result` несёт флаг успеха, строку результата или ошибки; `Schema` описывает аргументы и результат, но автоматического применения `required`, `type` и `constraint` в [Executor.cpp](../src/Executor.cpp) нет. Проверка выполняется конкретным Worker; `schema()` не гарантирует возможность `CALL`: у `hash` и `csv` оба метода оставлены базовыми. Исключения из `schema()` и `execute()` ловятся как `std::exception` в цикле [main.cpp](../src/main.cpp); ошибки, возвращённые через `Result`, печатает `Executor`.

## Состояние конкретных Worker

| Класс (исходник) | `schema()` | `execute(const Arguments&)` |
| --- | --- | --- |
| [HashWorker](../workers/hash/HashWorker.cpp) | базовое исключение | базовое исключение; описание MD5/SHA-256 не означает реализацию хеширования |
| [CsvWorker](../workers/csv/CsvWorker.cpp) | базовое исключение | базовое исключение |
| [FileWorker](../workers/file/FileWorker.cpp) | обязательный `path` | обычный файл ≤ 1 MiB; неизвестные ключи отвергаются, результат — текст с содержимым и метаданными |
| [HttpWorker](../workers/http/HttpWorker.cpp) | базовое исключение | обязательный `url`, GET с таймаутом 10 с; допускает схемы http/https, возвращает body при статусе 2xx; отдельно есть `execute(const HttpArguments&) const`, возвращающий `HttpResult` со статусом |
| [TextWorker](../workers/text/TextWorker.cpp) | `operation`, `text`, `find`, `replace` | `length`, `words`, `lines`, `find`, `replace`; дополнительные ключи не проверяются |
| [ProcessWorker](../workers/process/ProcessWorker.cpp) | `command`, `args` | список разрешённых: `ls`, `echo`, `date`, `whoami`, `uname`, `cat`, `wc`, `pwd`, `hostname`; `args` ≤ 1024 символов, часть shell-метасимволов запрещена; строка результата конструируется вручную и не обязана быть корректным JSON |
| [ModelWorker](../workers/model/ModelWorker.cpp) | `operation`, `vector`, `other`, `threshold` | только `classify`; два вектора через запятую, `threshold` по умолчанию 0.5; `normalize()` и `cosine()` реализованы как прямые методы, но через CLI не вызываются |

`HttpWorker` проверяет только префикс URL, allowlist хостов нет. У `ModelWorker` схема перечисляет `normalize`, `cosine`, однако `execute()` их отвергает и требует `other` для `classify`. В `ProcessWorker` проверка ошибок процесса не сводится к `exit_code`: успешный запуск канала возвращает `success=true` даже при ненулевом коде команды. Это факты реализации, а не обещания безопасности/валидности результатов.

## Регистрация и вызов

[WorkerFactory::create(WorkersName)](../src/WorkerFactory.cpp) возвращает `std::unique_ptr<Worker>` для семи перечисленных значений; неизвестное значение вызывает `std::invalid_argument`. [WorkerRegistry::registerWorker](../src/WorkerRegistry.cpp) принимает владение, отвергает нулевой указатель и дубликат `name()`. В [main.cpp](../src/main.cpp) объекты создаются фабрикой и регистрируются до цикла ввода. `Executor` получает `Worker*` от реестра и синхронно вызывает `execute(args)`; владение не передаётся. `DESCRIBE` вызывает `schema()`, `DISCOVER` вызывает `description()`. Объявленная `const`-перегрузка `findByName` не определена в `.cpp`; `Executor` использует неконстантную — см. [архитектурные вопросы](architecture.md).

## Добавление Worker (инструкция для разработки)

1. Создать класс в `workers/<name>/`, наследовать `Worker` и реализовать `name()`, `description()`, `schema()`, `execute(const Arguments&)`. Для вызываемого через CLI Worker оба последних метода нужно переопределить, несмотря на их непустую базовую реализацию.
2. Описать все используемые ключи через `ArgDescriptor` и проверять входные данные в `execute()`: сейчас универсального валидатора нет. Возвращать `Result{false, "", сообщение}` при ожидаемой ошибке; определить собственную политику исключений и тестировать её. Не рассчитывать на разбор кавычек в CLI.
3. Согласовать добавление значения в `WorkersName` и ветки в `WorkerFactory::create`, затем регистрацию в `main()`; эти изменения затрагивают публичный контракт/общий код и требуют решения команды. Добавить `.cpp` в целевые списки [Sader/CMakeLists.txt](../Sader/CMakeLists.txt) и нужных тестов при реализации (текущая задача документацию не расширяет исходниками).
4. Покрыть схему, успешный вызов и ошибки содержательными тестами. Существующие примеры: [FileWorkerTest.cpp](../tests/FileWorkerTest.cpp), [ModelWorkerTest.cpp](../workers/model/ModelWorkerTest.cpp).

**TESTED (тестовые сценарии существуют, результат запуска неизвестен):** файл проверяется на успешное чтение и несколько ошибок; модель — на численные операции и `execute`; HTTP — на ошибочный сетевой запрос; фабрика — на `hash`; реестр — на поиск `hash` (функция проверки дубликата содержит только заготовку); процесс — на примеры разрешённой/запрещённой команды. См. [development.md](development.md).

**PLANNED:** правила для асинхронного `execute` и ThreadPool ещё не оформлены реализацией. **UNKNOWN:** единый формат `Result.output`, единая обработка неизвестных ключей и гарантии потокобезопасности требуют решения до параллельного исполнения.
