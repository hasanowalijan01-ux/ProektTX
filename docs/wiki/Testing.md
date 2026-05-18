# Тестирование

## Уровни
| Уровень | Что покрывает | Инструмент |
|---------|---------------|------------|
| Unit    | Алгоритмы core (Vigenere, SHA-384, Chord, Steg) | QtTest |
| Integration | Команды протокола end-to-end | ручные сценарии |
| UI/Smoke | GUI клиента | чек-лист |

## Запуск unit-тестов
```bash
cmake --build build --target test_core
ctest --test-dir build/tests --output-on-failure
```

## Артефакты
| Файл | Что |
|------|-----|
| [`docs/testing/test-plan.md`](../testing/test-plan.md) | Тест-план |
| [`docs/testing/checklist.csv`](../testing/checklist.csv) | Чек-лист |
| [`docs/testing/test-cases.csv`](../testing/test-cases.csv) | Тест-кейсы |
| [`docs/testing/defects.csv`](../testing/defects.csv) | Журнал дефектов |
| [`tests/test_core.cpp`](../../tests/test_core.cpp) | UnitTest |

> CSV открывается в Excel/LibreOffice как `*.xls` — кодировка UTF-8, разделитель «;».
