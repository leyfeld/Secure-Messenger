#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QVariant>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDataStream>
#include <QTime>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "clientlist.h"


class tcpClient : public QObject
{
    Q_OBJECT
private:
    QTcpSocket* m_pTcpSocket;
    quint16     m_nNextBlockSize;
    QObject *   textArea;
    QVector<ClientList> chatList;
    bool EnterSuccess=false;
    //qmlConnect obj(myparent);

public:
    tcpClient(const QString& strHost, int nPort, QObject *myparent = nullptr, QObject* ta=nullptr);
    void SendMessageToServer(const QString &, const QString&            );
    void SendRegistrationToServer(const QString &, const QString&, const QString& );
    void SendLoginToServer(const QString &, const QString&);

signals:
     void getMessage(const QString &);
     void registrationError(int);
     void getClientList (const QVector <ClientList>&);


public slots:
    void slotReadyRead   (                            );
    void slotError       (QAbstractSocket::SocketError);
    void slotConnected   (                 );
};

#endif // TCPCLIENT_H
