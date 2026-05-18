#pragma once
/**
 * @file protocol.h
 * @brief Простой текстовый JSON-протокол между клиентом и сервером.
 *
 * Каждое сообщение — одна строка JSON, оканчивающаяся '\n'.
 * Поля: { "cmd": "<command>", "id": <int>, ... }
 *
 * Команды:
 *  - REGISTER  { login, password }                     -> { ok, error? }
 *  - LOGIN     { login, password }                     -> { ok, role, token, error? }
 *  - VIGENERE  { mode: "enc"|"dec", text, key }        -> { ok, result }
 *  - SHA384    { text }                                -> { ok, hash }
 *  - CHORD     { expr, a, b, eps }                     -> { ok, root, iter }
 *  - STEG_CAP  { size }                                -> { ok, capacity }
 *  - LIST_USERS (admin)                                -> { ok, users:[{login,role,blocked}] }
 *  - SET_BLOCK (admin) { login, blocked }              -> { ok }
 *  - LOG_LIST  (admin)                                 -> { ok, logs:[...] }
 *  - PING                                              -> { ok }
 */
#include <QByteArray>
#include <QJsonObject>

namespace pd::proto {
QByteArray pack(const QJsonObject& o);
QJsonObject unpack(const QByteArray& line, bool* ok = nullptr);
QJsonObject error(const QString& msg);
QJsonObject ok();
} // namespace
