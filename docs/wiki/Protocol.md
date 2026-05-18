# Протокол клиент-сервер

Каждое сообщение — строка JSON, оканчивающаяся `\n`.
Опциональное поле `id` сервер возвращает в ответе для корреляции.

## Команды

### REGISTER
```json
{"cmd":"REGISTER","login":"u","password":"p"}
```
Ответ: `{"ok":true}` или `{"ok":false,"error":"..."}`.

### LOGIN
```json
{"cmd":"LOGIN","login":"u","password":"p"}
```
Ответ: `{"ok":true,"role":"user|admin","token":"<short>"}`.

### VIGENERE
```json
{"cmd":"VIGENERE","mode":"enc|dec","text":"...","key":"..."}
```
Ответ: `{"ok":true,"result":"..."}`.

### SHA384
```json
{"cmd":"SHA384","text":"..."}
```
Ответ: `{"ok":true,"hash":"<hex 96 chars>"}`.

### CHORD
```json
{"cmd":"CHORD","expr":"x^3 - x - 2","a":1,"b":2,"eps":1e-9}
```
Ответ: `{"ok":true,"root":...,"iter":N,"residual":...,"converged":true}`.

### STEG_CAP
```json
{"cmd":"STEG_CAP","size":12345}
```
Ответ: `{"ok":true,"capacity":N}` — сколько байт сообщения влезет.

### LIST_USERS (admin)
Ответ: `{"ok":true,"users":[{"login":"u","role":"user","blocked":false}, ...]}`.

### SET_BLOCK (admin)
```json
{"cmd":"SET_BLOCK","login":"u","blocked":true}
```

### LOG_LIST (admin)
Ответ: `{"ok":true,"logs":[{"ts":"...","login":"...","action":"...","details":"..."}, ...]}`.

### PING
Ответ: `{"ok":true}`.
