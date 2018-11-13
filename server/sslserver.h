#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QTcpServer>
#include <QSslError>
#include <QList>

class QSslSocket;

class SslServer:public QTcpServer
{
    Q_OBJECT
public:
    SslServer(QObject *parent);
public slots:
    void slotError(QAbstractSocket::SocketError error);
    void slotSslErrors(const QList<QSslError> & errors);
signals:
    void SigReady();
protected:
    void incomingConnection(qintptr descriptor ) override;


};


#endif // SSLSERVER_H
