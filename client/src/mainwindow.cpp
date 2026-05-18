#include "mainwindow.h"
#include "clientcore.h"
#include "steganography.h"
#include <QAbstractItemView>
#include <QTableWidgetItem>

#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QImage>
#include <QJsonArray>
#include <QJsonObject>

namespace pd::client {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("PD Crypto Suite — клиент");
    resize(960, 640);

    tabs_ = new QTabWidget;
    setCentralWidget(tabs_);

    buildVigenereTab();
    buildShaTab();
    buildChordTab();
    buildStegTab();
    buildAdminTab();

    status_ = new QLabel;
    statusBar()->addPermanentWidget(status_);
    auto& c = ClientCore::instance();
    auto upd = [this,&c]{
        status_->setText(QString("Пользователь: %1 | Роль: %2")
                         .arg(c.login(), c.role()));
        updateAdminVisibility();
    };
    connect(&c, &ClientCore::loginChanged, this, upd);
    upd();
}

void MainWindow::updateAdminVisibility() {
    bool admin = ClientCore::instance().isAdmin();
    tabs_->setTabVisible(tabs_->count()-1, admin);
}

// ---------- Vigenere ----------
void MainWindow::buildVigenereTab() {
    auto w = new QWidget;
    vIn_  = new QPlainTextEdit;
    vOut_ = new QPlainTextEdit; vOut_->setReadOnly(true);
    vKey_ = new QLineEdit;
    auto enc = new QPushButton(tr("Шифровать"));
    auto dec = new QPushButton(tr("Расшифровать"));
    connect(enc, &QPushButton::clicked, this, &MainWindow::onVigenereEnc);
    connect(dec, &QPushButton::clicked, this, &MainWindow::onVigenereDec);

    auto lay = new QVBoxLayout(w);
    auto kl = new QHBoxLayout; kl->addWidget(new QLabel(tr("Ключ:"))); kl->addWidget(vKey_);
    lay->addLayout(kl);
    lay->addWidget(new QLabel(tr("Исходный текст:"))); lay->addWidget(vIn_);
    auto bl = new QHBoxLayout; bl->addWidget(enc); bl->addWidget(dec); lay->addLayout(bl);
    lay->addWidget(new QLabel(tr("Результат:"))); lay->addWidget(vOut_);
    tabs_->addTab(w, tr("Виженер"));
}

void MainWindow::onVigenereEnc() {
    auto r = ClientCore::instance().request({{"cmd","VIGENERE"},{"mode","enc"},
                                             {"text",vIn_->toPlainText()},{"key",vKey_->text()}});
    if (r.value("ok").toBool()) vOut_->setPlainText(r.value("result").toString());
    else QMessageBox::warning(this, tr("Виженер"), r.value("error").toString());
}
void MainWindow::onVigenereDec() {
    auto r = ClientCore::instance().request({{"cmd","VIGENERE"},{"mode","dec"},
                                             {"text",vIn_->toPlainText()},{"key",vKey_->text()}});
    if (r.value("ok").toBool()) vOut_->setPlainText(r.value("result").toString());
    else QMessageBox::warning(this, tr("Виженер"), r.value("error").toString());
}

// ---------- SHA ----------
void MainWindow::buildShaTab() {
    auto w = new QWidget;
    sIn_  = new QPlainTextEdit;
    sOut_ = new QPlainTextEdit; sOut_->setReadOnly(true);
    auto btn = new QPushButton(tr("Вычислить SHA-384"));
    connect(btn, &QPushButton::clicked, this, &MainWindow::onSha384);
    auto lay = new QVBoxLayout(w);
    lay->addWidget(new QLabel(tr("Текст:"))); lay->addWidget(sIn_);
    lay->addWidget(btn);
    lay->addWidget(new QLabel(tr("Хеш (hex, 96 симв.):"))); lay->addWidget(sOut_);
    tabs_->addTab(w, tr("SHA-384"));
}
void MainWindow::onSha384() {
    auto r = ClientCore::instance().request({{"cmd","SHA384"},{"text",sIn_->toPlainText()}});
    if (r.value("ok").toBool()) sOut_->setPlainText(r.value("hash").toString());
    else QMessageBox::warning(this, "SHA-384", r.value("error").toString());
}

// ---------- Chord ----------
void MainWindow::buildChordTab() {
    auto w = new QWidget;
    cExpr_ = new QLineEdit("x^3 - x - 2");
    cA_ = new QDoubleSpinBox; cA_->setRange(-1e6, 1e6); cA_->setDecimals(6); cA_->setValue(1.0);
    cB_ = new QDoubleSpinBox; cB_->setRange(-1e6, 1e6); cB_->setDecimals(6); cB_->setValue(2.0);
    cEps_ = new QDoubleSpinBox; cEps_->setRange(1e-15, 1.0); cEps_->setDecimals(12);
    cEps_->setValue(1e-9); cEps_->setSingleStep(1e-9);
    cResult_ = new QLabel("—");
    auto btn = new QPushButton(tr("Найти корень"));
    connect(btn, &QPushButton::clicked, this, &MainWindow::onChordSolve);

    auto form = new QFormLayout;
    form->addRow(tr("f(x) ="), cExpr_);
    form->addRow("a", cA_); form->addRow("b", cB_); form->addRow(tr("точность"), cEps_);
    auto lay = new QVBoxLayout(w);
    lay->addLayout(form); lay->addWidget(btn);
    lay->addWidget(new QLabel(tr("Результат:"))); lay->addWidget(cResult_);
    lay->addWidget(new QLabel(tr("Поддерживается: + - * / ^, sin/cos/exp/log/sqrt, скобки.")));
    lay->addStretch();
    tabs_->addTab(w, tr("Метод хорд"));
}
void MainWindow::onChordSolve() {
    auto r = ClientCore::instance().request({{"cmd","CHORD"},{"expr",cExpr_->text()},
                                             {"a",cA_->value()},{"b",cB_->value()},{"eps",cEps_->value()}});
    if (r.value("ok").toBool()) {
        cResult_->setText(QString("x ≈ %1   итераций: %2   |f|=%3   %4")
            .arg(r.value("root").toDouble(),0,'g',12)
            .arg(r.value("iter").toInt())
            .arg(r.value("residual").toDouble(),0,'g',6)
            .arg(r.value("converged").toBool() ? "OK" : "не сошлось"));
    } else QMessageBox::warning(this, tr("Метод хорд"), r.value("error").toString());
}

// ---------- Steg ----------
void MainWindow::buildStegTab() {
    auto w = new QWidget;
    stegImgIn_  = new QLineEdit;
    stegImgOut_ = new QLineEdit;
    auto bIn  = new QPushButton(tr("…"));
    auto bOut = new QPushButton(tr("…"));
    connect(bIn,  &QPushButton::clicked, this, [this]{
        auto f = QFileDialog::getOpenFileName(this, tr("Картинка"), {}, "Images (*.png *.bmp)");
        if (!f.isEmpty()) stegImgIn_->setText(f);
    });
    connect(bOut, &QPushButton::clicked, this, [this]{
        auto f = QFileDialog::getSaveFileName(this, tr("Сохранить как"), "out.png", "PNG (*.png)");
        if (!f.isEmpty()) stegImgOut_->setText(f);
    });

    stegMsg_ = new QPlainTextEdit;
    stegStatus_ = new QLabel("—");
    auto bEmb = new QPushButton(tr("Внедрить в картинку"));
    auto bExt = new QPushButton(tr("Извлечь сообщение"));
    connect(bEmb, &QPushButton::clicked, this, &MainWindow::onStegEmbed);
    connect(bExt, &QPushButton::clicked, this, &MainWindow::onStegExtract);

    auto form = new QFormLayout;
    auto inRow = new QHBoxLayout; inRow->addWidget(stegImgIn_); inRow->addWidget(bIn);
    auto outRow= new QHBoxLayout; outRow->addWidget(stegImgOut_);outRow->addWidget(bOut);
    form->addRow(tr("Исходная картинка:"), inRow);
    form->addRow(tr("Результат (PNG):"),    outRow);
    auto lay = new QVBoxLayout(w);
    lay->addLayout(form);
    lay->addWidget(new QLabel(tr("Сообщение:"))); lay->addWidget(stegMsg_);
    auto br = new QHBoxLayout; br->addWidget(bEmb); br->addWidget(bExt); lay->addLayout(br);
    lay->addWidget(stegStatus_);
    tabs_->addTab(w, tr("Стеганография"));
}

void MainWindow::onStegEmbed() {
    QImage img(stegImgIn_->text());
    if (img.isNull()) { QMessageBox::warning(this,"Стег","Не удалось загрузить изображение"); return; }
    img = img.convertToFormat(QImage::Format_RGBA8888);
    std::vector<uint8_t> px(img.bits(), img.bits() + img.sizeInBytes());
    auto msg = stegMsg_->toPlainText().toStdString();
    if (!stego::embed(px, msg)) {
        stegStatus_->setText(tr("Недостаточно ёмкости (нужно %1 пикселей).").arg(msg.size()*8/4 + 8));
        return;
    }
    QImage out(px.data(), img.width(), img.height(), QImage::Format_RGBA8888);
    if (!out.copy().save(stegImgOut_->text(), "PNG")) {
        QMessageBox::warning(this,"Стег","Не удалось сохранить PNG"); return;
    }
    stegStatus_->setText(tr("Сообщение внедрено."));
}

void MainWindow::onStegExtract() {
    QImage img(stegImgIn_->text());
    if (img.isNull()) { QMessageBox::warning(this,"Стег","Не удалось загрузить изображение"); return; }
    img = img.convertToFormat(QImage::Format_RGBA8888);
    std::vector<uint8_t> px(img.bits(), img.bits() + img.sizeInBytes());
    auto m = stego::extract(px);
    stegMsg_->setPlainText(QString::fromStdString(m));
    stegStatus_->setText(tr("Извлечено %1 символов.").arg((int)m.size()));
}

// ---------- Admin ----------
void MainWindow::buildAdminTab() {
    auto w = new QWidget;
    userTable_ = new QTableWidget(0, 3);
    userTable_->setHorizontalHeaderLabels({"login","role","blocked"});
    userTable_->horizontalHeader()->setStretchLastSection(true);
    userTable_->setSelectionBehavior(QAbstractItemView::SelectRows);

    logTable_ = new QTableWidget(0, 4);
    logTable_->setHorizontalHeaderLabels({"timestamp","login","action","details"});
    logTable_->horizontalHeader()->setStretchLastSection(true);

    auto bRefU = new QPushButton(tr("Обновить пользователей"));
    auto bBlk  = new QPushButton(tr("Блок/разблок выбранного"));
    auto bRefL = new QPushButton(tr("Обновить логи"));
    connect(bRefU, &QPushButton::clicked, this, &MainWindow::refreshUsers);
    connect(bBlk,  &QPushButton::clicked, this, &MainWindow::toggleBlock);
    connect(bRefL, &QPushButton::clicked, this, &MainWindow::refreshLogs);

    auto lay = new QVBoxLayout(w);
    lay->addWidget(new QLabel("<b>" + tr("Пользователи") + "</b>"));
    lay->addWidget(userTable_);
    auto br = new QHBoxLayout; br->addWidget(bRefU); br->addWidget(bBlk); lay->addLayout(br);
    lay->addWidget(new QLabel("<b>" + tr("Журнал событий") + "</b>"));
    lay->addWidget(logTable_);
    lay->addWidget(bRefL);
    tabs_->addTab(w, tr("Админ"));
}

void MainWindow::refreshUsers() {
    auto r = ClientCore::instance().request({{"cmd","LIST_USERS"}});
    if (!r.value("ok").toBool()) { QMessageBox::warning(this,"admin",r.value("error").toString()); return; }
    auto arr = r.value("users").toArray();
    userTable_->setRowCount(arr.size());
    for (int i=0;i<arr.size();++i) {
        auto o = arr[i].toObject();
        userTable_->setItem(i,0,new QTableWidgetItem(o.value("login").toString()));
        userTable_->setItem(i,1,new QTableWidgetItem(o.value("role").toString()));
        userTable_->setItem(i,2,new QTableWidgetItem(o.value("blocked").toBool()?"yes":"no"));
    }
}

void MainWindow::toggleBlock() {
    auto rows = userTable_->selectionModel()->selectedRows();
    if (rows.isEmpty()) return;
    int r = rows.first().row();
    auto login = userTable_->item(r,0)->text();
    bool blocked = userTable_->item(r,2)->text() == "yes";
    auto resp = ClientCore::instance().request({{"cmd","SET_BLOCK"},{"login",login},{"blocked",!blocked}});
    if (!resp.value("ok").toBool())
        QMessageBox::warning(this,"admin",resp.value("error").toString());
    refreshUsers();
}

void MainWindow::refreshLogs() {
    auto r = ClientCore::instance().request({{"cmd","LOG_LIST"}});
    if (!r.value("ok").toBool()) { QMessageBox::warning(this,"admin",r.value("error").toString()); return; }
    auto arr = r.value("logs").toArray();
    logTable_->setRowCount(arr.size());
    for (int i=0;i<arr.size();++i) {
        auto o = arr[i].toObject();
        logTable_->setItem(i,0,new QTableWidgetItem(o.value("ts").toString()));
        logTable_->setItem(i,1,new QTableWidgetItem(o.value("login").toString()));
        logTable_->setItem(i,2,new QTableWidgetItem(o.value("action").toString()));
        logTable_->setItem(i,3,new QTableWidgetItem(o.value("details").toString()));
    }
}

} // namespace
