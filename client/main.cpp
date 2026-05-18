/**
 * @file client/main.cpp
 * @brief Точка входа Qt-клиента.
 */
#include <QApplication>
#include "logindialog.h"
#include "mainwindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    app.setApplicationName("pd_client");

    pd::client::LoginDialog dlg;
    if (dlg.exec() != QDialog::Accepted) return 0;

    pd::client::MainWindow w;
    w.show();
    return app.exec();
}
