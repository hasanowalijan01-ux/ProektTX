# Сборка и запуск

## Требования
- CMake ≥ 3.16
- Qt 6 (Core, Network, Sql, Widgets, Gui, Test)
- Компилятор с поддержкой C++17

## Локальная сборка
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```
Бинарники появятся в `build/server/pd_server` и `build/client/pd_client`.

## Запуск
```bash
# терминал 1
./build/server/pd_server --port 5555 --db pd_server.db
# терминал 2 / N
./build/client/pd_client
```
Учётные данные администратора по умолчанию: **admin / admin**.

## Тесты
```bash
ctest --test-dir build/tests --output-on-failure
```

## Docker
```bash
docker compose up --build
```
Сервер слушает на `localhost:5555`. Базу хранит в volume `pd_data`.

## Doxygen
```bash
doxygen Doxyfile
xdg-open docs/doxygen/html/index.html
```
