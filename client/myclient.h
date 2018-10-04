#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QObject>
#include <QVariant>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDataStream>
#include <QTime>

class MyClient : public QObject
{
    Q_OBJECT
private:
    QTcpSocket* m_pTcpSocket;
    quint16     m_nNextBlockSize;
    QObject* textArea;
    QObject* field1;
    QObject* button;
public:
    MyClient(const QString& strHost, int nPort, QObject *myparent = nullptr);

signals:

private slots:
    void slotReadyRead   (                            );
    void slotError       (QAbstractSocket::SocketError);
    void slotSendToServer(                            );
    void slotConnected   (                            );
};

#endif // MYCLIENT_H
