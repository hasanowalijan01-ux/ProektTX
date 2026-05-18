#include "clienthandler.h"
#include "protocol.h"
#include "database.h"
#include "vigenere.h"
#include "sha384.h"
#include "chord.h"
#include "steganography.h"

#include <QJsonArray>
#include <QHostAddress>
#include <QDebug>

using namespace pd::proto;

namespace pd::server {

ClientHandler::ClientHandler(QTcpSocket* s, QObject* parent)
    : QObject(parent), sock_(s)
{
    sock_->setParent(this);
    connect(sock_, &QTcpSocket::readyRead,    this, &ClientHandler::onReadyRead);
    connect(sock_, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);
}

void ClientHandler::onDisconnected() {
    if (authed_) Database::instance().log(login_, "disconnect");
    emit finished();
    deleteLater();
}

void ClientHandler::onReadyRead() {
    buffer_ += sock_->readAll();
    int idx;
    while ((idx = buffer_.indexOf('\n')) >= 0) {
        QByteArray line = buffer_.left(idx);
        buffer_.remove(0, idx+1);
        if (!line.trimmed().isEmpty())
            handleLine(line);
    }
}

void ClientHandler::handleLine(const QByteArray& line) {
    bool ok = false;
    auto req = unpack(line, &ok);
    if (!ok) { sock_->write(pack(error("bad json"))); return; }
    auto resp = dispatch(req);
    // echo correlation id if present
    if (req.contains("id")) resp["id"] = req["id"];
    sock_->write(pack(resp));
}

QJsonObject ClientHandler::dispatch(const QJsonObject& r) {
    const auto cmd = r.value("cmd").toString();
    if (cmd == "PING")        return cmdPing();
    if (cmd == "REGISTER")    return cmdRegister(r);
    if (cmd == "LOGIN")       return cmdLogin(r);
    if (cmd == "VIGENERE")    return cmdVigenere(r);
    if (cmd == "SHA384")      return cmdSha384(r);
    if (cmd == "CHORD")       return cmdChord(r);
    if (cmd == "STEG_CAP")    return cmdStegCap(r);
    if (cmd == "LIST_USERS")  return cmdListUsers();
    if (cmd == "SET_BLOCK")   return cmdSetBlock(r);
    if (cmd == "LOG_LIST")    return cmdLogList();
    return error("unknown cmd: " + cmd);
}

QJsonObject ClientHandler::cmdPing() { return ok(); }

QJsonObject ClientHandler::cmdRegister(const QJsonObject& r) {
    const auto login = r.value("login").toString();
    const auto pwd   = r.value("password").toString();
    if (login.isEmpty() || pwd.isEmpty()) return error("login/password required");
    if (Database::instance().userExists(login)) return error("user exists");
    auto h = QString::fromStdString(crypto::Sha384::hash(pwd.toStdString()));
    if (!Database::instance().registerUser(login, h, "user")) return error("db error");
    Database::instance().log(login, "register");
    return ok();
}

QJsonObject ClientHandler::cmdLogin(const QJsonObject& r) {
    const auto login = r.value("login").toString();
    const auto pwd   = r.value("password").toString();
    if (login.isEmpty() || pwd.isEmpty()) return error("login/password required");
    auto h = QString::fromStdString(crypto::Sha384::hash(pwd.toStdString()));
    QString role; bool blocked = false;
    if (!Database::instance().authenticate(login, h, &role, &blocked))
        return error("invalid credentials");
    if (blocked) return error("user blocked");
    login_ = login; role_ = role; authed_ = true;
    Database::instance().log(login, "login");
    auto o = ok();
    o["role"]  = role;
    o["token"] = QString::fromStdString(crypto::Sha384::hash((login+":"+role).toStdString())).left(32);
    return o;
}

bool ClientHandler::requireAuth(QJsonObject& err) const {
    if (!authed_) { err = error("auth required"); return false; }
    return true;
}
bool ClientHandler::requireAdmin(QJsonObject& err) const {
    if (!requireAuth(err)) return false;
    if (role_ != "admin") { err = error("admin only"); return false; }
    return true;
}

QJsonObject ClientHandler::cmdVigenere(const QJsonObject& r) {
    QJsonObject e; if (!requireAuth(e)) return e;
    const auto mode = r.value("mode").toString("enc");
    const auto text = r.value("text").toString();
    const auto key  = r.value("key").toString();
    if (key.isEmpty()) return error("empty key");
    try {
        std::string out = (mode == "dec")
            ? crypto::Vigenere::decrypt(text.toStdString(), key.toStdString())
            : crypto::Vigenere::encrypt(text.toStdString(), key.toStdString());
        Database::instance().log(login_, "vigenere", mode);
        auto o = ok(); o["result"] = QString::fromStdString(out); return o;
    } catch (const std::exception& ex) { return error(ex.what()); }
}

QJsonObject ClientHandler::cmdSha384(const QJsonObject& r) {
    QJsonObject e; if (!requireAuth(e)) return e;
    const auto t = r.value("text").toString();
    Database::instance().log(login_, "sha384");
    auto o = ok(); o["hash"] = QString::fromStdString(crypto::Sha384::hash(t.toStdString())); return o;
}

QJsonObject ClientHandler::cmdChord(const QJsonObject& r) {
    QJsonObject e; if (!requireAuth(e)) return e;
    const auto expr = r.value("expr").toString().toStdString();
    double a = r.value("a").toDouble(), b = r.value("b").toDouble();
    double eps = r.value("eps").toDouble(1e-9);
    try {
        auto res = math::chord([&](double x){ return math::evalExpr(expr, x); }, a, b, eps);
        Database::instance().log(login_, "chord", QString::fromStdString(expr));
        auto o = ok();
        o["root"] = res.root; o["iter"] = res.iterations;
        o["residual"] = res.residual; o["converged"] = res.converged;
        return o;
    } catch (const std::exception& ex) { return error(ex.what()); }
}

QJsonObject ClientHandler::cmdStegCap(const QJsonObject& r) {
    QJsonObject e; if (!requireAuth(e)) return e;
    auto cap = stego::capacity((size_t)r.value("size").toInt());
    auto o = ok(); o["capacity"] = (qint64)cap; return o;
}

QJsonObject ClientHandler::cmdListUsers() {
    QJsonObject e; if (!requireAdmin(e)) return e;
    QJsonArray arr;
    for (auto& u : Database::instance().listUsers()) {
        arr.append(QJsonObject{{"login",u.login},{"role",u.role},{"blocked",u.blocked}});
    }
    auto o = ok(); o["users"] = arr; return o;
}

QJsonObject ClientHandler::cmdSetBlock(const QJsonObject& r) {
    QJsonObject e; if (!requireAdmin(e)) return e;
    const auto login = r.value("login").toString();
    const bool b = r.value("blocked").toBool();
    if (!Database::instance().setBlocked(login, b)) return error("not found");
    Database::instance().log(login_, "set_block", login + (b?":1":":0"));
    return ok();
}

QJsonObject ClientHandler::cmdLogList() {
    QJsonObject e; if (!requireAdmin(e)) return e;
    QJsonArray arr;
    for (auto& l : Database::instance().listLogs()) {
        arr.append(QJsonObject{{"ts",l.ts},{"login",l.login},
                               {"action",l.action},{"details",l.details}});
    }
    auto o = ok(); o["logs"] = arr; return o;
}

} // namespace
