#pragma once
/**
 * @file logindialog.h
 * @brief Диалог входа/регистрации.
 */
#include <QDialog>
class QLineEdit;
class QSpinBox;

namespace pd::client {

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget* parent = nullptr);

private slots:
    void doLogin();
    void doRegister();

private:
    QLineEdit *host_, *login_, *pwd_;
    QSpinBox  *port_;
    bool ensureConnected();
};

} // namespace
