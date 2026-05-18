#include "database.h"
#include "sha384.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QMutexLocker>
#include <QDebug>

namespace pd::server {

Database& Database::instance() {
    static Database inst;
    return inst;
}

Database::Database() = default;
Database::~Database() { close(); }

bool Database::open(const QString& path) {
    QMutexLocker lk(&mtx_);
    db_ = QSqlDatabase::addDatabase("QSQLITE", "pd_main");
    db_.setDatabaseName(path);
    if (!db_.open()) {
        qWarning() << "DB open failed:" << db_.lastError().text();
        return false;
    }
    ensureSchema();
    seedAdmin();
    return true;
}

void Database::close() {
    QMutexLocker lk(&mtx_);
    if (db_.isOpen()) db_.close();
}

void Database::ensureSchema() {
    QSqlQuery q(db_);
    q.exec("CREATE TABLE IF NOT EXISTS users ("
           " login TEXT PRIMARY KEY,"
           " pwd_hash TEXT NOT NULL,"
           " role TEXT NOT NULL DEFAULT 'user',"
           " blocked INTEGER NOT NULL DEFAULT 0)");
    q.exec("CREATE TABLE IF NOT EXISTS logs ("
           " id INTEGER PRIMARY KEY AUTOINCREMENT,"
           " ts TEXT NOT NULL,"
           " login TEXT,"
           " action TEXT NOT NULL,"
           " details TEXT)");
}

void Database::seedAdmin() {
    if (userExists("admin")) return;
    auto h = QString::fromStdString(pd::crypto::Sha384::hash("admin"));
    QSqlQuery q(db_);
    q.prepare("INSERT INTO users(login,pwd_hash,role,blocked) VALUES(?,?,?,0)");
    q.addBindValue("admin"); q.addBindValue(h); q.addBindValue("admin");
    q.exec();
    log("system", "seed_admin", "default admin/admin created");
}

bool Database::userExists(const QString& login) {
    QMutexLocker lk(&mtx_);
    QSqlQuery q(db_);
    q.prepare("SELECT 1 FROM users WHERE login=?");
    q.addBindValue(login);
    return q.exec() && q.next();
}

bool Database::registerUser(const QString& login, const QString& hash, const QString& role) {
    QMutexLocker lk(&mtx_);
    QSqlQuery q(db_);
    q.prepare("INSERT INTO users(login,pwd_hash,role,blocked) VALUES(?,?,?,0)");
    q.addBindValue(login); q.addBindValue(hash); q.addBindValue(role);
    return q.exec();
}

bool Database::authenticate(const QString& login, const QString& hash, QString* role, bool* blocked) {
    QMutexLocker lk(&mtx_);
    QSqlQuery q(db_);
    q.prepare("SELECT pwd_hash, role, blocked FROM users WHERE login=?");
    q.addBindValue(login);
    if (!q.exec() || !q.next()) return false;
    if (q.value(0).toString() != hash) return false;
    if (role)    *role    = q.value(1).toString();
    if (blocked) *blocked = q.value(2).toInt() != 0;
    return true;
}

bool Database::setBlocked(const QString& login, bool b) {
    QMutexLocker lk(&mtx_);
    QSqlQuery q(db_);
    q.prepare("UPDATE users SET blocked=? WHERE login=?");
    q.addBindValue(b ? 1 : 0); q.addBindValue(login);
    return q.exec() && q.numRowsAffected() > 0;
}

QVector<Database::UserRow> Database::listUsers() {
    QMutexLocker lk(&mtx_);
    QVector<UserRow> r;
    QSqlQuery q(db_);
    if (!q.exec("SELECT login, role, blocked FROM users ORDER BY login")) return r;
    while (q.next())
        r.push_back({q.value(0).toString(), q.value(1).toString(), q.value(2).toInt()!=0});
    return r;
}

void Database::log(const QString& login, const QString& action, const QString& details) {
    QMutexLocker lk(&mtx_);
    QSqlQuery q(db_);
    q.prepare("INSERT INTO logs(ts,login,action,details) VALUES(?,?,?,?)");
    q.addBindValue(QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    q.addBindValue(login); q.addBindValue(action); q.addBindValue(details);
    q.exec();
}

QVector<Database::LogRow> Database::listLogs(int limit) {
    QMutexLocker lk(&mtx_);
    QVector<LogRow> r;
    QSqlQuery q(db_);
    q.prepare("SELECT ts,login,action,details FROM logs ORDER BY id DESC LIMIT ?");
    q.addBindValue(limit);
    if (!q.exec()) return r;
    while (q.next())
        r.push_back({q.value(0).toString(), q.value(1).toString(),
                     q.value(2).toString(), q.value(3).toString()});
    return r;
}

} // namespace
