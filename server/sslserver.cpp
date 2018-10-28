#include "sslserver.h"
#include <QSslSocket>
#include <QFile>
#include <QDebug>
#include <QSslKey>
#include <QSslConfiguration>



SslServer::SslServer(QObject* parent):QTcpServer(parent)
{

}

void SslServer::slotError(QAbstractSocket::SocketError error)
{
    qDebug()<<error;
}

void SslServer::slotSslErrors(const QList<QSslError> & errors)
{
    for(const QSslError& sslError : errors)
    {
        qDebug()<< sslError;
    }
}

void SslServer::incomingConnection(qintptr descriptor)
{
        QSslSocket *sslSocket = new QSslSocket(this);
        connect(sslSocket, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(slotSslErrors(const QList<QSslError>&)));
        connect(sslSocket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(slotError(QAbstractSocket::SocketError)));
        QByteArray key;
        QByteArray cert;
        QFile file_key("/Users/leyfeld/Documents/projects/tasks/testCert/sc.key");
        if(file_key.open(QIODevice::ReadOnly))
        {
            key = file_key.readAll();
            file_key.close();
        }
        else
        {
            qDebug() << file_key.errorString();
        }
        QFile file_cert("/Users/leyfeld/Documents/projects/tasks/testCert/sc.crt");
        if(file_cert.open(QIODevice::ReadOnly))
        {
            cert = file_cert.readAll();
            file_cert.close();
        }
        else
        {
            qDebug() << file_cert.errorString();
        }
        QSslKey ssl_key(key, QSsl::Rsa,QSsl::Pem,QSsl::PrivateKey);
        QSslCertificate ssl_cert(cert);
        sslSocket->setProtocol(QSsl::AnyProtocol);
        sslSocket->addCaCertificate(ssl_cert);
        sslSocket->setLocalCertificate(ssl_cert);
        sslSocket->setPrivateKey(ssl_key);
        if (sslSocket->setSocketDescriptor(descriptor))
        {
            sslSocket->startServerEncryption();
        }
        else
        {
            qDebug() << "setSocketDescription error";
            return;
        }
        this->addPendingConnection(sslSocket);

}
