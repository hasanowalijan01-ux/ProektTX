/**
 * @file test_core.cpp
 * @brief UnitTest-ы для алгоритмов core (Qt Test framework).
 */
#include <QtTest>
#include <vector>
#include "vigenere.h"
#include "sha384.h"
#include "chord.h"
#include "steganography.h"

class CoreTests : public QObject {
    Q_OBJECT
private slots:
    // ---- Vigenere ----
    void vigenereRoundtripLatin() {
        auto enc = pd::crypto::Vigenere::encrypt("HelloWorld", "key");
        QCOMPARE(QString::fromStdString(pd::crypto::Vigenere::decrypt(enc, "key")),
                 QStringLiteral("HelloWorld"));
    }
    void vigenereKnownVector() {
        // ATTACKATDAWN with key LEMON -> LXFOPVEFRNHR (classic textbook).
        QCOMPARE(QString::fromStdString(pd::crypto::Vigenere::encrypt("ATTACKATDAWN","LEMON")),
                 QStringLiteral("LXFOPVEFRNHR"));
    }
    void vigenerePreservesNonLetters() {
        auto enc = pd::crypto::Vigenere::encrypt("Hi, World!", "abc");
        QVERIFY(enc.find(',') != std::string::npos);
        QVERIFY(enc.find(' ') != std::string::npos);
        QVERIFY(enc.find('!') != std::string::npos);
    }
    void vigenereEmptyKey() {
        QVERIFY_EXCEPTION_THROWN(pd::crypto::Vigenere::encrypt("abc",""),
                                 std::invalid_argument);
    }

    // ---- SHA-384 ----
    void sha384EmptyVector() {
        // FIPS 180-4 test vector for empty input.
        QCOMPARE(QString::fromStdString(pd::crypto::Sha384::hash("")),
                 QStringLiteral("38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da"
                                "274edebfe76f65fbd51ad2f14898b95b"));
    }
    void sha384AbcVector() {
        QCOMPARE(QString::fromStdString(pd::crypto::Sha384::hash("abc")),
                 QStringLiteral("cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed"
                                "8086072ba1e7cc2358baeca134c825a7"));
    }
    void sha384DifferentInputsDiffer() {
        QVERIFY(pd::crypto::Sha384::hash("a") != pd::crypto::Sha384::hash("b"));
    }

    // ---- Chord method ----
    void chordCubicRoot() {
        auto r = pd::math::chord([](double x){ return x*x*x - x - 2.0; }, 1.0, 2.0, 1e-10);
        QVERIFY(r.converged);
        QVERIFY(qAbs(r.root - 1.5213797068045676) < 1e-6);
    }
    void chordSameSignThrows() {
        QVERIFY_EXCEPTION_THROWN(
            pd::math::chord([](double x){ return x*x + 1.0; }, -1.0, 1.0),
            std::invalid_argument);
    }
    void exprParserBasic() {
        QCOMPARE(pd::math::evalExpr("x^2 + 2*x + 1", 3.0), 16.0);
        QCOMPARE(pd::math::evalExpr("(1+2)*3", 0.0), 9.0);
    }

    // ---- Steganography ----
    void stegRoundtripAscii() {
        std::vector<uint8_t> buf(8000, 0xCC);
        std::string msg = "Hello, world!";
        QVERIFY(pd::stego::embed(buf, msg));
        QCOMPARE(QString::fromStdString(pd::stego::extract(buf)),
                 QString::fromStdString(msg));
    }
    void stegRoundtripUnicode() {
        std::vector<uint8_t> buf(8000, 0x00);
        std::string msg = "Привет, мир! 🎉";
        QVERIFY(pd::stego::embed(buf, msg));
        QCOMPARE(QString::fromStdString(pd::stego::extract(buf)),
                 QString::fromStdString(msg));
    }
    void stegOverflowFails() {
        std::vector<uint8_t> buf(40, 0); // capacity ~ 1 byte
        QVERIFY(!pd::stego::embed(buf, std::string(20, 'A')));
    }
    void stegCapacityMath() {
        QCOMPARE(pd::stego::capacity(0u),  size_t(0));
        QCOMPARE(pd::stego::capacity(32u), size_t(0));
        QCOMPARE(pd::stego::capacity(40u), size_t(1));
    }
};

QTEST_APPLESS_MAIN(CoreTests)
#include "test_core.moc"
