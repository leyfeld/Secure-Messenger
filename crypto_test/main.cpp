#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QByteArray>
#include <QtTest/QtTest>
#include <QMap>
#include "cryptoworker.h"

#include <openssl/dh.h>
#include <openssl/engine.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

unsigned char key[]={0x7e, 0x8b, 0x9a, 0x14, 0x54, 0x3e, 0xe0, 0x2, 0xb4, 0x5c, 0x85, 0x45, 0x4f, 0x32, 0x48, 0x73, 0xd7, 0xca, 0x34, 0x31, 0x21, 0xf4, 0x55, 0x42, 0x89, 0x6e, 0x31, 0x3d, 0x6a, 0x3f, 0xa8, 0xd0};

class TestCrypt: public QObject
{
    Q_OBJECT
private slots:
    void TestCryptoTest();
    void TestCryptoTest_data();
};
void TestCrypt::TestCryptoTest_data()
{
    QTest::addColumn<QString>("PlainText");
    QTest::newRow("long string") << "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
    QTest::newRow("empty string") << "" ;
    QTest::newRow("string with symbols") << "hello,eee,eee:!" ;
    QTest::newRow("short string with numbers") << "111";
    QTest::newRow("long string with numbers") << "12345678901234567890" ;
    QTest::newRow("string with space") << "f ff fff ";
    QTest::newRow("short string") << "QQQ";
    QTest::newRow("short string with symbols") << "hello__hi";
    QTest::newRow("string with russian letters") << "привет";
    QTest::newRow("string with other letters") << "ハイàहाय";

}
void TestCrypt::TestCryptoTest()
{
        QFETCH(QString, PlainText);
        CryptoWorker example;
        QString login = "login";
        QByteArray mykey(reinterpret_cast<const char*>(key), sizeof(key));
        QMap <QString, QByteArray > secretkey;
        secretkey.insert(login, mykey);
        qDebug()<<"Size key: " << mykey.size();
        example.SetMapForTest(secretkey);
        QByteArray encstring;
        QString Decrypted;
        int e = example.Encrypt(login, PlainText,encstring);
        int d = example.Decrypt(login, encstring, Decrypted);
        QCOMPARE(PlainText, Decrypted);
}
QTEST_MAIN(TestCrypt)
#include "main.moc"



