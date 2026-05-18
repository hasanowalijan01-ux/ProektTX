#include "logindialog.h"
#include "clientcore.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

namespace pd::client {

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Вход — PD Crypto Suite"));

    host_  = new QLineEdit("127.0.0.1");
    port_  = new QSpinBox; port_->setRange(1, 65535); port_->setValue(5555);
    login_ = new QLineEdit;
    pwd_   = new QLineEdit; pwd_->setEchoMode(QLineEdit::Password);

    auto form = new QFormLayout;
    form->addRow(tr("Сервер:"),  host_);
    form->addRow(tr("Порт:"),    port_);
    form->addRow(tr("Логин:"),   login_);
    form->addRow(tr("Пароль:"),  pwd_);

    auto loginBtn = new QPushButton(tr("Войти"));
    auto regBtn   = new QPushButton(tr("Регистрация"));
    auto cancel   = new QPushButton(tr("Отмена"));
    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::doLogin);
    connect(regBtn,   &QPushButton::clicked, this, &LoginDialog::doRegister);
    connect(cancel,   &QPushButton::clicked, this, &QDialog::reject);

    auto buttons = new QHBoxLayout;
    buttons->addWidget(loginBtn); buttons->addWidget(regBtn); buttons->addWidget(cancel);

    auto root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addLayout(buttons);
    root->addWidget(new QLabel(tr("Подсказка: admin / admin для администратора")));
}

bool LoginDialog::ensureConnected() {
    auto& c = ClientCore::instance();
    if (c.isConnected()) return true;
    if (!c.connectTo(host_->text(), (quint16)port_->value())) {
        QMessageBox::critical(this, tr("Ошибка"),
            tr("Не удалось подключиться к серверу.\n"
               "Сначала запустите в другом терминале:\n"
               "./build/server/pd_server --port 5555 --db pd_server.db\n\n%1")
                .arg(c.socketErrorString()));
        return false;
    }
    return true;
}

void LoginDialog::doLogin() {
    if (!ensureConnected()) return;
    QString err;
    if (ClientCore::instance().login(login_->text(), pwd_->text(), &err))
        accept();
    else
        QMessageBox::warning(this, tr("Вход"), err);
}

void LoginDialog::doRegister() {
    if (!ensureConnected()) return;
    QString err;
    if (ClientCore::instance().registerUser(login_->text(), pwd_->text(), &err))
        QMessageBox::information(this, tr("Регистрация"), tr("Аккаунт создан, войдите."));
    else
        QMessageBox::warning(this, tr("Регистрация"), err);
}

} // namespace
