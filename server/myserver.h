#ifndef MYSERVER_H
#define MYSERVER_H

#include "database.h"
#include <QWidget>
#include <QMap>

class QTcpServer;
class QTextEdit;
class QTcpSocket;
class ServDb;


class MyServer : public QWidget {
Q_OBJECT
private:
    QTcpServer* m_ptcpServer;
    QTextEdit*  m_ptxt;
    quint16     m_nNextBlockSize;
    QList < QTcpSocket*> m_clientList;
    QMap <QString, QTcpSocket*> m_clientMap;
    std::unique_ptr<ServDb> m_sdb;
    QVector<ClientList> chatList;

private:
    template <typename T>
    void sendToClient(const QString& str2, const T& str, QTcpSocket* pSocket);
    void sendToClient(const QString& protocol,const QString& whosend ,const QVariant& msgData, QTcpSocket* pSocket);

public:
    MyServer(int nPort, QWidget* pwgt = 0);
public slots:
    virtual void slotNewConnection();
            void slotReadClient   ();
            void slotDeleteMap();
};


#endif // MYSERVER_H

