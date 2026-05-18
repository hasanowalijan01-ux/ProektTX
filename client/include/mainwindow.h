#pragma once
/**
 * @file mainwindow.h
 * @brief Главное окно клиента: вкладки с функциями + админ-панель.
 */
#include <QMainWindow>
class QTabWidget;
class QTableWidget;
class QPlainTextEdit;
class QLineEdit;
class QLabel;
class QDoubleSpinBox;
class QSpinBox;

namespace pd::client {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    // crypto tabs
    void onVigenereEnc();
    void onVigenereDec();
    void onSha384();
    void onChordSolve();
    void onStegEmbed();
    void onStegExtract();
    // admin
    void refreshUsers();
    void toggleBlock();
    void refreshLogs();

private:
    QTabWidget* tabs_;
    QLabel*     status_;

    // Vigenere
    QPlainTextEdit *vIn_, *vOut_;
    QLineEdit      *vKey_;
    // SHA
    QPlainTextEdit *sIn_, *sOut_;
    // Chord
    QLineEdit      *cExpr_;
    QDoubleSpinBox *cA_, *cB_, *cEps_;
    QLabel         *cResult_;
    // Steg
    QLineEdit      *stegImgIn_, *stegImgOut_;
    QPlainTextEdit *stegMsg_;
    QLabel         *stegStatus_;
    // Admin
    QTableWidget   *userTable_;
    QTableWidget   *logTable_;

    void buildVigenereTab();
    void buildShaTab();
    void buildChordTab();
    void buildStegTab();
    void buildAdminTab();
    void updateAdminVisibility();
};

} // namespace
