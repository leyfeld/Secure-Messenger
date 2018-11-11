#ifndef CRYPTOWORKER_H
#define CRYPTOWORKER_H

#include <QDebug>
#include <QString>
#include <QByteArray>
#include <openssl/dh.h>
#include <openssl/engine.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

class CryptoWorker
{
public:
    CryptoWorker();
    int Encrypt(const QString & forLogin,const QString &string, QByteArray &encstring);
    int Decrypt(const QString& login, const QByteArray &encstring, QString &decrtext);
    void GeneratorKey(const QString &login, QByteArray &pubkey);
    QByteArray GetPublicKey();
    void DeleteNameFromMap(const QString& name);
    QByteArray GetKey(const QString& login);
private:
    QByteArray m_iv;
    std::unique_ptr<DH, void(*)(DH*)> m_dh;
    QMap <QString, QByteArray > m_secretkey;
};

#endif // CRYPTOWORKER_H
