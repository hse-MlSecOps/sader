# Разработка и запуск

## Окружение

**IMPLEMENTED (конфигурация):** [CMakeLists.txt](../CMakeLists.txt) задаёт минимум CMake 3.10, но [Sader/CMakeLists.txt](../Sader/CMakeLists.txt) требует **3.20**, устанавливает C++20 без расширений и использует `find_package(CURL REQUIRED)` + `CURL::libcurl` для приложения, фабричного и HTTP-теста. Нужны компилятор с C++20, CMake ≥ 3.20, заголовки и библиотека libcurl; для [CMakePresets.json](../CMakePresets.json) — Ninja. Системные библиотеки C++ и POSIX `popen`/`pclose` используются [ProcessWorker.cpp](../workers/process/ProcessWorker.cpp); портируемость этого Worker на Windows не подтверждена. Отдельного менеджера зависимостей в CMake нет.

## Сборка Linux / macOS

Используйте команды из корня репозитория после установки CMake, компилятора и dev-пакета libcurl средствами своей ОС. Прямой вариант с системным генератором:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/Sader/Sader
```

Или presets (Ninja):

```bash
# Linux
cmake --preset linux-debug
cmake --build out/build/linux-debug
./out/build/linux-debug/Sader/Sader

# macOS
cmake --preset macos-debug
cmake --build out/build/macos-debug
./out/build/macos-debug/Sader/Sader
```

Если CMake не обнаруживает libcurl, проверьте пути установки и настройку `CMAKE_PREFIX_PATH` / `CURL_ROOT` в своей среде. Приведённые выше общие команды и presets не являются отчётом об их запуске; фактически предоставленный результат проверки macOS приведён ниже.

## Проверка на macOS (предоставленный вывод команд)

**TESTED:** пользователь предоставил вывод чистой конфигурации, сборки и CTest на macOS. CMake определил компиляторы C и C++ как **AppleClang 17.0.0.17000013** и нашёл **libcurl 8.7.1** по пути `/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib/libcurl.tbd`. Версия самого исполняемого файла CMake в предоставленном выводе **не указана (UNKNOWN)**; успешная конфигурация подтверждает лишь совместимость с требованиями текущего проекта, а не точную версию CMake.

Выполненные команды из корня репозитория:

```bash
cmake -S . -B build-macos -DCMAKE_BUILD_TYPE=Debug
cmake --build build-macos --parallel 2
ctest --test-dir build-macos --output-on-failure
```

Конфигурация и генерация завершились (`Configuring done`, `Generating done`); сборка дошла до 100%, собраны `Sader`, `TestProcessWorker`, `TestFileWorker` и пять тестовых целей подпроекта. CTest сообщил **5/5 Passed, 0 Failed** за **1.52 sec**: `WorkerRegistryTest` (0.46 sec), `WorkerFabricTest` (0.21 sec), `HttpWorkerTest` (0.45 sec), `FileWorkerTest` (0.20 sec), `ModelWorkerTest` (0.20 sec).

**Известные ограничения этого результата:** `TestProcessWorker` и верхнеуровневый `TestFileWorker` собраны, но не зарегистрированы в CTest и их отдельный запуск не подтверждён; запуск интерактивного `Sader` не показан; `HttpWorkerTest` использует сетевой адрес `https://nonexistent.invalid`, поэтому успех одного прогона не подтверждает стабильность в другом окружении. Версия CMake, результаты Linux и других конфигураций сборки остаются **UNKNOWN**.

## CLI и тесты

После запуска можно ввести `DISCOVER text`, `DESCRIBE file`, `CALL text operation=words text=hello`, затем `EXIT`. Команды построчные, чувствительны к регистру и пробелам; синтаксис разбора описан в [README.md](../README.md) и [main.cpp](../src/main.cpp). `CALL file path=README.md` использует относительный путь от **текущего рабочего каталога** процесса.

После сборки:

```bash
ctest --test-dir build --output-on-failure
# Для preset Linux: ctest --test-dir out/build/linux-debug --output-on-failure
# Для preset macOS: ctest --test-dir out/build/macos-debug --output-on-failure
```

**TESTED (только наличие проверок в коде, не результат запуска):** в [Sader/CMakeLists.txt](../Sader/CMakeLists.txt) зарегистрированы `WorkerRegistryTest`, `WorkerFabricTest`, `HttpWorkerTest`, `FileWorkerTest`, `ModelWorkerTest`. Верхний [CMakeLists.txt](../CMakeLists.txt) дополнительно создаёт `TestProcessWorker` и `TestFileWorker`, но не регистрирует их через `add_test`; запуск вручную возможен как `./build/TestProcessWorker` и `./build/TestFileWorker` после успешной сборки. При включённом `assert` тесты реестра, фабрики, HTTP, файла и процесса проверяют условия; `ModelWorkerTest` возвращает ошибку сам при расхождении. `HttpWorkerTest` обращается к `https://nonexistent.invalid`, поэтому условия исполнения зависят от сети/окружения. Проверка дублирующей регистрации в `test_worker_registry.cpp` пока лишь комментарий.

**UNKNOWN:** результаты сборки и CTest на Linux и в других конфигурациях не подтверждены предоставленным отчётом о macOS. Объявленная `WorkerRegistry::findByName(...) const` не определена в [WorkerRegistry.cpp](../src/WorkerRegistry.cpp); текущий `Executor` вызывает неконстантную перегрузку, но потребитель `const WorkerRegistry` столкнётся с проблемой линковки.

## Отладка

Собирайте с `-DCMAKE_BUILD_TYPE=Debug` или debug preset; проверяйте отдельный Worker прямым тестом, затем путь через интерактивный `CALL`. При ошибке `DESCRIBE` учитывайте базовую реализацию `schema()` с исключением; при ошибке `CALL` отличайте `Result{success=false}` (`Ошибка: ...` в stdout) от исключения (`ERROR: ...` в stderr). [Executor.cpp](../src/Executor.cpp) выводит результат непосредственно в stdout, поэтому тестирование формата CLI требует отдельной проверки. Для файла проверьте рабочий каталог и лимит 1 MiB, для HTTP — доступность сети и установленный libcurl, для процесса — поддержку `popen` и формат собираемой команды.

**PLANNED:** ThreadPool/асинхронный запуск, Docker-образ и GitHub Actions CI. **UNKNOWN / открытые вопросы:** какие платформы официально поддерживаются, нужно ли регистрировать верхнеуровневые тесты в CTest и как фиксировать сетевые зависимости HTTP-тестов для CI — см. [architecture.md](architecture.md).
