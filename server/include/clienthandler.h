#pragma once
/**
 * @file clienthandler.h
 * @brief Обработчик одного TCP-клиента: парсинг команд и маршрутизация.
 */
#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

namespace pd::server {

/**
 * @class ClientHandler
 * @brief Обслуживает одного клиента: чтение строк, обработка JSON-команд.
 *
 * Поддерживает аутентификацию, ограничение по ролям (user/admin) и набор
 * криптографических команд. Часть тяжёлых операций — заглушки (см. protocol.h).
 */
class ClientHandler : public QObject {
    Q_OBJECT
public:
    explicit ClientHandler(QTcpSocket* socket, QObject* parent = nullptr);

signals:
    void finished();

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    void handleLine(const QByteArray& line);
    QJsonObject dispatch(const QJsonObject& req);

    // command handlers
    QJsonObject cmdRegister(const QJsonObject&);
    QJsonObject cmdLogin   (const QJsonObject&);
    QJsonObject cmdVigenere(const QJsonObject&);
    QJsonObject cmdSha384  (const QJsonObject&);
    QJsonObject cmdChord   (const QJsonObject&);
    QJsonObject cmdStegCap (const QJsonObject&);
    QJsonObject cmdListUsers();
    QJsonObject cmdSetBlock(const QJsonObject&);
    QJsonObject cmdLogList ();
    QJsonObject cmdPing();

    bool requireAuth(QJsonObject& err) const;
    bool requireAdmin(QJsonObject& err) const;

    QTcpSocket* sock_;
    QByteArray  buffer_;
    QString     login_;
    QString     role_;     // "", "user", "admin"
    bool        authed_ = false;
};

} // namespace
