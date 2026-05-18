#pragma once
/**
 * @file clientcore.h
 * @brief Singleton-фасад клиентского сетевого слоя.
 */
#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QQueue>

namespace pd::client {

/**
 * @class ClientCore
 * @brief Singleton: соединение с сервером, отправка команд, парсинг ответов.
 */
class ClientCore : public QObject {
    Q_OBJECT
public:
    static ClientCore& instance();
    ClientCore(const ClientCore&)            = delete;
    ClientCore& operator=(const ClientCore&) = delete;

    bool connectTo(const QString& host, quint16 port, int timeoutMs = 3000);
    void disconnectFromServer();
    bool isConnected() const;
    /** Последняя ошибка сокета после неудачного connectTo / обрыва. */
    QString socketErrorString() const { return sock_.errorString(); }

    // Synchronous request/response helper for simplicity.
    QJsonObject request(const QJsonObject& cmd, int timeoutMs = 5000);

    // Auth state
    QString login() const { return login_; }
    QString role () const { return role_;  }
    bool    isAdmin() const { return role_ == "admin"; }

    bool login(const QString& user, const QString& password, QString* err = nullptr);
    bool registerUser(const QString& user, const QString& password, QString* err = nullptr);
    void logout();

signals:
    void connectionLost();
    void loginChanged();

private:
    ClientCore();
    QTcpSocket sock_;
    QString    login_;
    QString    role_;
    int        nextId_ = 1;
};

} // namespace
