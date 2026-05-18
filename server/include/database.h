#pragma once
/**
 * @file database.h
 * @brief Singleton-обёртка над SQLite (Qt SQL).
 *
 * Хранит пользователей (login, password_hash, role, blocked) и журнал событий.
 */
#include <QString>
#include <QSqlDatabase>
#include <QVector>
#include <QRecursiveMutex>

namespace pd::server {

/**
 * @class Database
 * @brief Singleton, обеспечивающий доступ к SQLite-БД сервера.
 */
class Database {
public:
    struct UserRow { QString login; QString role; bool blocked; };
    struct LogRow  { QString ts; QString login; QString action; QString details; };

    /// Получить экземпляр (потокобезопасно).
    static Database& instance();

    Database(const Database&)            = delete;
    Database& operator=(const Database&) = delete;

    bool open(const QString& path);
    void close();

    // ---- users ----
    bool registerUser(const QString& login, const QString& passwordHash, const QString& role = "user");
    bool authenticate (const QString& login, const QString& passwordHash, QString* role, bool* blocked);
    bool setBlocked   (const QString& login, bool blocked);
    QVector<UserRow> listUsers();
    bool userExists(const QString& login);

    // ---- logs ----
    void log(const QString& login, const QString& action, const QString& details = {});
    QVector<LogRow> listLogs(int limit = 200);

private:
    Database();
    ~Database();
    void ensureSchema();
    void seedAdmin();

    QSqlDatabase      db_;
    QRecursiveMutex   mtx_;
};

} // namespace pd::server
