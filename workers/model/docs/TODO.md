# TODO — замечания из ревью PR в `feature/model-worker`

Собрано из ревью-комментариев к PR #19, #20, #21, #22 после их мерджа.
Отмечено, что уже адресовано при разрешении конфликтов мерджа.

## PR #19 — model-4: cosine simalarity между двумя векторами

- [x] **Тесты не проверяли результат** (`main` всегда возвращал 0, проверки
  `name()`/`description()`/`schema()`/`normalize()` были удалены) —
  исправлено при мердже: восстановлены проверки контракта, добавлены проверки
  реальных значений cosine (ортогональные = 0, совпадающие = 1,
  противоположные = -1).
- [x] **Исключения не верифицировались** (отсутствие исключения не считалось
  ошибкой) — исправлено при мердже: хелпер `throwsInvalidArgument`.
- [x] **No newline at end of file** в `ModelWorker.cpp/.h/Test.cpp` —
  исправлено при мердже.
- [ ] **Research по cosine отсутствует**: выбор метрики (cosine similarity),
  диапазон [-1, 1] и граничные случаи не задокументированы. Добавить
  `docs/model-cosine-research.md` по образцу normalize/classify.
- [ ] **Coverage для cosine не измерялся** — перемерить после объединения
  (см. общий пункт ниже).

## PR #20 — model-3: normalize — нормализация вектора

- [ ] **CMakeLists.txt:18** — coverage-блок не ограничен компилятором: флаги
  `--coverage`/`-fprofile-arcs` невалидны под MSVC. Обернуть в
  `if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")`
  и убрать избыточный `-lgcov` из `add_link_options` (`--coverage` уже
  подтягивает gcov runtime).
- [ ] **ModelWorker.cpp: normalize** — накопление `value * value`
  переполняется на компонентах ~1e200 (результат — нулевой вектор вместо
  unit vector) и underflow'ит на денормалях. Перейти на масштабирование
  через `max|v_i|` или `std::hypot`.
- [ ] **ModelWorker.cpp:56** — связано: при underflow `norm == 0.0` даст
  ложный "Cannot normalize zero vector" для ненулевого вектора.
- [ ] **Документация** — `docs/model-normalize-research.md` лежит в корневом
  `docs/`, а остальные доки воркера — в `workers/model/docs/`. Перенести
  или договориться о едином месте (с учётом `docs/model-classify-research.md`).
- [ ] **.gitignore** — вернуть перевод строки в конце файла.

## PR #21 — model-1: Research векторизации текста

- Замечаний в ревью не было.
- [ ] Согласовать с командой выводы research и следующие шаги (выбор
  подхода к векторизации для ModelWorker).

## PR #22 — model-5: Threshold classification

- Замечаний в ревью не было. Открытые вопросы из описания PR:
- [ ] **Дублирование cosine**: `classify()` использует локальный хелпер
  `cosineSimilarity()` в anonymous namespace, хотя теперь есть публичный
  `cosine()` из #19. Переиспользовать `cosine()` (учесть: у них разная
  обработка — clamp и проверка finite есть только у хелпера).
- [ ] **Формат меток** `"similar"`/`"dissimilar"` — согласовать с командой
  (альтернативы: `"1"`/`"0"`, `"positive"`/`"negative"`).
- [ ] **execute() поддерживает только classify** — после стабилизации
  normalize/cosine добавить их в dispatch `execute()`.

## Общее

- [ ] **Перемерить coverage** объединённого `ModelWorker` (llvm-cov),
  обновить цифры (на момент PR #22: 100% lines/functions по ModelWorker.cpp,
  но мерж добавил cosine/normalize).
- [ ] **CMake-таргет для `ModelWorkerTest`** — сейчас тест собирается вручную,
  добавить в CMakeLists.txt по образцу `TestProcessWorker`.
