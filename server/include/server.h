#pragma once
/**
 * @file server.h
 * @brief Класс TCP-сервера, поддерживающего множество клиентов.
 */
#include <QTcpServer>

namespace pd::server {

/**
 * @class Server
 * @brief Слушает порт и для каждого подключения создаёт ClientHandler.
 */
class Server : public QTcpServer {
    Q_OBJECT
public:
    explicit Server(QObject* parent = nullptr);
    bool start(quint16 port);

protected:
    void incomingConnection(qintptr handle) override;
};

} // namespace
