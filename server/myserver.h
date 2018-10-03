#ifndef MYSERVER_H
#define MYSERVER_H


#include <QWidget>
#include <QTcpServer>
#include <QTextEdit>
#include <QTcpSocket>
#include "database.h"


class QTcpServer;
class QTextEdit;
class QTcpSocket;

class MyServer : public QWidget {
Q_OBJECT
private:
    QTcpServer* m_ptcpServer;
    QTextEdit*  m_ptxt;
    quint16     m_nNextBlockSize;
    QList < QTcpSocket*> m_clientList;
    std::unique_ptr<ServDb> m_sdb;

private:
    void sendToClient(const QString& str);

public:
    MyServer(int nPort, QWidget* pwgt = 0);

public slots:

    virtual void slotNewConnection();
            void slotReadClient   ();
};


#endif // MYSERVER_H
