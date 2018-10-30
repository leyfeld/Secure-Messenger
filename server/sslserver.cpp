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
        QByteArray cert;
        QFile file_cert(":/cert/sc.crt");
        if(file_cert.open(QIODevice::ReadOnly))
        {
            cert = file_cert.readAll();
            file_cert.close();
        }
        else
        {
            qDebug() << file_cert.errorString();
        }
        char m_key[] = "-----BEGIN RSA PRIVATE KEY-----\n"\
                "MIIEoQIBAAKCAQEAuGyF4Hiy3OUk32TXHDp9mVWyKoE6OhnuUNFRbb/PRhGS4v4h\n"
                "OpYC83/sdWB3xM9Cue+CHC15O97SnxV+2+dU7FkcxqRwqeUSkT6yRvL1S784izXA\n"
                "dsEj6g6yojILCwpdokMR5psplg7jWQkqImMfUuzbpincNotsan5EKDu+CFfKE9Fo\n"
                "2lRStud0aE8uCXfzDJpF6NtYQhtFhOy3V1aClWsQoqVXzn1/9cuaj4jz3xXvnk+6\n"
                "Vqk5zxCay55q7hyT1lGG/pQqjFux4dr64NVibefUjm6oXU6ca2XvdSVdu4u/QSXb\n"
                "tacsrv797gROxguIXeFYDmQnJSRcHUPl1HPeHQIDAQABAoH/ZwQ3xCunvbxB3Id3\n"
                "oU0K25wC6btFyy6w8HP8CFIcdgdbBXDvHsqs21eZvfck78rWewZOp26/SbIt9K8N\n"
                "LckKpvsgF9XDGSPcO0XKT9dnLeiirSBZYfyBEz0ii3dos8IWi4zrcuU5VZj7ZcC4\n"
                "yHyWArxL9YVsfQtkhBgD+nVksUNl1FjT0nmm92DXoCqf8+S0K/qONMku3X2RdF0N\n"
                "atO85ERRnSYfgCNoySrGh+M5078nnakv+dXq+klJeqj6zeRvsv1xns7g7aNZyON0\n"
                "QQpm9bhqxJ0ZRwglE9HXSNn6H/rXxa2hn+mh2qNEBKSZgdLOiUBFvy2fGRaAx8f9\n"
                "OuElAoGBAN02oNzrFcI8at0WMCgW6kxuDOCKj95cBKhCYXDoAfVcjpkBYkAN0hL9\n"
                "7ZZ9UNhqIPI/uIcR092N9xH9hu5Tc6yxBEg4YmJ2aXk3Ma2bm2qkortAVsqam4KG\n"
                "QdtkNonsRn5vXfLCwkdKjiwsP2FZAkPqolfgeJv6Se/+wk5IuZkLAoGBANVs3QVd\n"
                "shQp32UwF2n8Xe4cb3Nb5LzC3leKRlbwcOrRBUPtZ14yxnCvOhoYMWHJJVuZ/2J2\n"
                "6why3DGBNRlPszalbwDQfBO/F1D5AJg0OpVS5YsBDydaoju2qLM1SajjCjvkilIH\n"
                "Zx8YxL6uPY5GrzHngZTFR/mqencYoHAkIW53AoGAekbyEGuZ8W34HBFhuSdBh9Hl\n"
                "KFEvSVhP8roOppx4SKpK4GSVbJa3ImxeYisO/zFa2M1EMNU36WXJxpX/ecmDTVfj\n"
                "6A/Swl512t0Zhr9W31H16c+cqg1gHHwd9ymjxAVoygbBRalGPqYFhIFI34SV7fjK\n"
                "mAyqXn2xVQnbWe51ORUCgYA0GTUC4FJeNbKAwvKUPrS3MV3LbT0zDuazu99bzani\n"
                "yE9eu48mIYLrcvrd79XuJunlqHv9D0Zkc5VoyD+sBBadWlj0e/At6dHJRF9uyCEc\n"
                "3nWGLyYB11AO/5HC3VkzzuhqomlZiNEVeDhey63fE0YlCnxJUrc1LPZkKDDR7x/y\n"
                "FQKBgQCo4L7GNLFLIr+NTDk7H6Y4jOgkYSyvH6R97l10ovbU5D8U+x3tBMDeXRfg\n"
                "qzo7JVjNLXKScae66+15J1T+gvgygihzgvoWHW3gKHZIea4xgmsLwQ/Z4mxQgIWK\n"
                "O/EEpcQI/JFiIZAKMjX6yOe1+ku6GotV8TCrW6af5AjSiTyHdw==\n"
                "-----END RSA PRIVATE KEY----- \n";
        QSslKey ssl_key(m_key, QSsl::Rsa,QSsl::Pem,QSsl::PrivateKey);
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
