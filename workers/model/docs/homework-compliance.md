# Соответствие ModelWorker постановке

Сверка кода с домашним заданием из `sader/SADER_architecture_and_homework_02.docx` (разделы 5, 7, 8 и 10).

По пункту 8 у `ModelWorker` три операции: нормализация вектора, cosine similarity и классификация по порогу. Сейчас это заглушки: методы сразу возвращают константу, без проверки аргументов и без вычисления.

`schema()` объявлен на `ModelWorker` и описывает аргументы `operation`, `vector`, `other` и `threshold`. В базовом `Worker` этого метода пока нет, поэтому он не виртуальный. `execute()` в контракте тоже ещё нет. `Executor` не содержит ветки для `model`.

## Что сделано

| Требование | Статус | Где в коде |
|---|---|---|
| Уникальное имя и описание capability | Выполнено: имя `model` | `ModelWorker.cpp` |
| Каталог `workers/model/` и наследование от `Worker` | Выполнено | `ModelWorker.h` |
| Операции | Заглушки: `normalize` возвращает `{}`, `cosine` — `0`, `classify` — `{}` | `ModelWorker.cpp` |
| Схема аргументов | `schema()` возвращает имя, тип и обязательность четырёх аргументов | `ModelWorker::schema` |
| Регистрация без изменения логики `Executor` | `executor.addWorker(std::make_unique<ModelWorker>())` | `sader/src/main.cpp` |
| Исходник в CMake | Выполнено | `sader/Sader/CMakeLists.txt` |

## Что остаётся за рамками этого каталога

В базовом `Worker` нет виртуальных `schema()` и `execute(Arguments)`, поэтому `CALL` через `Executor` этот воркер не вызывает.
