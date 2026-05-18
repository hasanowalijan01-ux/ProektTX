#include "server.h"
#include "clienthandler.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>

namespace pd::server {

Server::Server(QObject* p) : QTcpServer(p) {}

bool Server::start(quint16 port) {
    if (!listen(QHostAddress::Any, port)) {
        qCritical() << "listen failed:" << errorString();
        return false;
    }
    qInfo() << "Server listening on port" << port;
    return true;
}

void Server::incomingConnection(qintptr handle) {
    auto* sock = new QTcpSocket();
    if (!sock->setSocketDescriptor(handle)) { delete sock; return; }
    qInfo() << "Client connected:" << sock->peerAddress().toString();
    new ClientHandler(sock, this);
}

} // namespace
