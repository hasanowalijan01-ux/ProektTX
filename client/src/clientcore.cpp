#include "clientcore.h"
#include <QJsonDocument>
#include <QEventLoop>
#include <QTimer>
#include <QElapsedTimer>
#include <QDebug>

namespace pd::client {

ClientCore& ClientCore::instance() { static ClientCore c; return c; }

ClientCore::ClientCore() {
    connect(&sock_, &QTcpSocket::disconnected, this, &ClientCore::connectionLost);
}

bool ClientCore::connectTo(const QString& host, quint16 port, int timeoutMs) {
    sock_.abort();
    sock_.connectToHost(host, port);
    return sock_.waitForConnected(timeoutMs);
}

void ClientCore::disconnectFromServer() { sock_.disconnectFromHost(); }
bool ClientCore::isConnected() const { return sock_.state() == QAbstractSocket::ConnectedState; }

QJsonObject ClientCore::request(const QJsonObject& cmd, int timeoutMs) {
    if (!isConnected()) return {{"ok",false},{"error","not connected"}};
    QJsonObject c = cmd; c["id"] = nextId_++;
    auto data = QJsonDocument(c).toJson(QJsonDocument::Compact) + '\n';
    sock_.write(data); sock_.flush();

    QByteArray buf;
    QElapsedTimer t; t.start();
    while (t.elapsed() < timeoutMs) {
        if (!sock_.waitForReadyRead(timeoutMs - (int)t.elapsed())) break;
        buf += sock_.readAll();
        int nl = buf.indexOf('\n');
        if (nl >= 0) {
            auto line = buf.left(nl);
            return QJsonDocument::fromJson(line).object();
        }
    }
    return {{"ok",false},{"error","timeout"}};
}

bool ClientCore::login(const QString& u, const QString& p, QString* err) {
    auto r = request({{"cmd","LOGIN"},{"login",u},{"password",p}});
    if (!r.value("ok").toBool()) { if (err) *err = r.value("error").toString(); return false; }
    login_ = u; role_ = r.value("role").toString();
    emit loginChanged();
    return true;
}

bool ClientCore::registerUser(const QString& u, const QString& p, QString* err) {
    auto r = request({{"cmd","REGISTER"},{"login",u},{"password",p}});
    if (!r.value("ok").toBool()) { if (err) *err = r.value("error").toString(); return false; }
    return true;
}

void ClientCore::logout() {
    login_.clear(); role_.clear();
    disconnectFromServer();
    emit loginChanged();
}

} // namespace
