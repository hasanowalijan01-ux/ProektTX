# Архитектура

```
+----------------+        TCP/JSON         +----------------+
|   pd_client    |  <------------------>   |   pd_server    |
| (Qt Widgets)   |   line-delimited json   |  (QTcpServer)  |
| ClientCore* (S)|                         | ClientHandler  |
+----------------+                         +-------+--------+
                                                   |
                                                   v
                                           +----------------+
                                           |  Database  (S) |
                                           |   SQLite       |
                                           +----------------+
   (S) = singleton
```

## Компоненты

### core (`core/`)
Чистый C++ без Qt. Содержит:
- [`pd::crypto::Vigenere`](../../core/include/vigenere.h:1)
- [`pd::crypto::Sha384`](../../core/include/sha384.h:1)
- [`pd::math::chord`](../../core/include/chord.h:1) и парсер выражений
- [`pd::stego::embed`](../../core/include/steganography.h:1)/`extract`

### server (`server/`)
- [`pd::server::Server`](../../server/include/server.h:1) — `QTcpServer`,
  поддерживает множество клиентов, на каждое подключение создаёт
  [`pd::server::ClientHandler`](../../server/include/clienthandler.h:1).
- [`pd::server::Database`](../../server/include/database.h:1) — singleton поверх SQLite.
- Парсинг команд + заглушки/полные реализации в [`server/src/clienthandler.cpp`](../../server/src/clienthandler.cpp:1).

### client (`client/`)
- [`pd::client::ClientCore`](../../client/include/clientcore.h:1) — singleton TCP-клиента.
- [`pd::client::LoginDialog`](../../client/include/logindialog.h:1) — авторизация.
- [`pd::client::MainWindow`](../../client/include/mainwindow.h:1) — вкладки и админ-панель.

## Потоки данных
1. Клиент шлёт `{"cmd": "...", ...}\n`.
2. Сервер парсит JSON, проверяет аутентификацию/роль.
3. Передаёт в core или БД, формирует ответ `{"ok": true|false, ...}\n`.
4. Клиент читает строку и обновляет UI.

См. диаграмму классов: [`docs/uml/class_server.puml`](../uml/class_server.puml).
