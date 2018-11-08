#ifndef SECUREPASSWORD_H
#define SECUREPASSWORD_H

#include <QObject>
#include <QCryptographicHash>

class SecurePassword : public QObject
{
    Q_OBJECT

private:
    const QString possibleCharacters="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    const quint16 soltLength=20;
    QString cleanPassword;
    QString solt;

public:
    SecurePassword(const QString password, QObject *parent = nullptr);
    QString GetRandomString();
    void SetSolt(const QString & slt);
    QByteArray GetHash ();


signals:

public slots:
};

#endif // SECUREPASSWORD_H
