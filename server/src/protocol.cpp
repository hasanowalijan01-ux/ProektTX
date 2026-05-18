#include "protocol.h"
#include <QJsonDocument>

namespace pd::proto {

QByteArray pack(const QJsonObject& o) {
    return QJsonDocument(o).toJson(QJsonDocument::Compact) + '\n';
}

QJsonObject unpack(const QByteArray& line, bool* ok) {
    QJsonParseError e;
    auto d = QJsonDocument::fromJson(line, &e);
    if (ok) *ok = (e.error == QJsonParseError::NoError && d.isObject());
    return d.object();
}

QJsonObject error(const QString& m) { return {{"ok", false}, {"error", m}}; }
QJsonObject ok() { return {{"ok", true}}; }

} // namespace
