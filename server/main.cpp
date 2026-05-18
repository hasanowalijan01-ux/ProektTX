/**
 * @file server/main.cpp
 * @brief Точка входа сервера.
 */
#include <cstdio>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>
#include "server.h"
#include "database.h"

namespace {

/** На macOS бинарник из build/ часто не видит плагины Qt без QT_PLUGIN_PATH — добавляем типичные пути Homebrew. */
void ensureQtPluginPaths()
{
#ifdef __APPLE__
    const QString env = qEnvironmentVariable("QT_PLUGIN_PATH");
    if (!env.isEmpty()) {
        const QStringList parts = env.split(QDir::listSeparator(), Qt::SkipEmptyParts);
        for (const QString& part : parts)
            QCoreApplication::addLibraryPath(part);
        return;
    }
    const QStringList candidates = {
        QStringLiteral("/opt/homebrew/opt/qt/share/qt/plugins"),
        QStringLiteral("/usr/local/opt/qt/share/qt/plugins"),
    };
    for (const QString& dir : candidates) {
        if (QDir(dir).exists()) {
            QCoreApplication::addLibraryPath(dir);
            return;
        }
    }
#endif
}

} // namespace

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ensureQtPluginPaths();
    app.setApplicationName("pd_server");
    app.setApplicationVersion("1.0");

    QCommandLineParser p;
    p.addHelpOption();
    QCommandLineOption portOpt({"p","port"}, "TCP port", "port", "5555");
    QCommandLineOption dbOpt  ({"d","db"},   "DB file",  "path", "pd_server.db");
    p.addOption(portOpt); p.addOption(dbOpt);
    p.process(app);

    const QString dbPath = p.value(dbOpt);
    const quint16 port = static_cast<quint16>(p.value(portOpt).toUShort());

    if (!pd::server::Database::instance().open(dbPath)) {
        std::fprintf(stderr,
            "pd_server: не удалось открыть БД \"%s\".\n"
            "  Частая причина на macOS/Homebrew: не найден плагин QSQLITE.\n"
            "  Попробуйте: export QT_PLUGIN_PATH=\"$(brew --prefix qt)/share/qt/plugins\"\n",
            dbPath.toUtf8().constData());
        std::fflush(stderr);
        return 1;
    }

    pd::server::Server s;
    if (!s.start(port)) {
        std::fprintf(stderr,
            "pd_server: не удалось слушать порт %u (занят или недоступен).\n",
            static_cast<unsigned>(port));
        std::fflush(stderr);
        return 2;
    }

    std::fprintf(stderr,
        "pd_server: слушаю порт %u, БД \"%s\". Оставьте это окно открытым; стоп — Ctrl+C.\n",
        static_cast<unsigned>(port), dbPath.toUtf8().constData());
    std::fflush(stderr);

    return app.exec();
}
