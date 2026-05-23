# PD Project — Crypto Suite (Vigenère + SHA‑384 + Chord Method + Steganography)

Учебный курсовой проект на **C++ / Qt 6**: клиент-серверное приложение, реализующее
четыре алгоритма (шифр Виженера, хеш SHA‑384, метод хорд для нахождения корня
уравнения, LSB-стеганография — внедрение сообщения в картинку), с авторизацией,
ролями (user/admin), БД (SQLite, синглтон), несколькими клиентами и Docker-инфраструктурой.

![architecture](docs/uml/architecture.puml)

## Возможности

| № | Функция | Где используется |
|---|---------|------------------|
| 1 | Шифр Виженера (encrypt/decrypt) | клиент ↔ сервер |
| 2 | SHA-384 (hash сообщения / хеш паролей) | сервер (auth) + клиент |
| 3 | Метод хорд — численное решение уравнений | клиент (UI ввода f(x)) |
| 4 | LSB-стеганография — скрытие сообщения в PNG | клиент |
| 5 | Авторизация / регистрация | сервер |
| 6 | Роли user / admin | сервер + клиент |
| 7 | Админ-функции (список пользователей, блокировка) | сервер + клиент |
| 8 | Логи операций в таблице | клиент |

## Структура репозитория

```
pd/
├── core/         — общая библиотека алгоритмов (Vigenere, SHA384, Chord, Steg)
├── server/       — Qt TCP-сервер + SQLite (singleton)
├── client/       — Qt Widgets GUI (singleton)
├── tests/        — UnitTest (QtTest)
├── docs/
│   ├── wiki/     — страницы для GitHub Wiki
│   ├── uml/      — диаграммы (PlantUML + PNG)
│   ├── requirements.md
│   └── testing/  — тест-план, чек-лист, тест-кейсы, дефекты (CSV/XLS)
├── docker-compose.yml
├── Doxyfile
└── CMakeLists.txt
```

## Ветки git

| Ветка | Назначение |
|-------|------------|
| `main` | стабильный релиз |
| `develop` | основная разработка |
| `feature/core-algorithms` | алгоритмы шифрования/хеширования |
| `feature/server` | TCP-сервер и БД |
| `feature/client` | GUI-клиент |
| `feature/docker` | контейнеризация |
| `feature/tests` | модульные тесты |

Создание веток: `bash scripts/setup_branches.sh`

## Сборка

### Локально (требуется Qt6, CMake ≥ 3.16)
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build/tests
```

### Docker
```bash
docker compose up --build
```

## Документация

- **Doxygen**: `doxygen Doxyfile` → `docs/doxygen/html/index.html`
- **Wiki**: см. `docs/wiki/Home.md`
- **Требования**: `docs/requirements.md`
- **UML**: `docs/uml/`

## Лицензия
MIT
