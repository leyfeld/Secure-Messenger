#ifndef MYSERVER_H
#define MYSERVER_H

#include "database.h"
#include "sslserver.h"
#include <QWidget>
#include <QMap>

class QTextEdit;
class QSslSocket;
class ServDb;


class MyServer : public QWidget {
Q_OBJECT
private:
    SslServer* m_socketServer;
    QTextEdit*  m_ptxt;
    quint16     m_nNextBlockSize;
    QMap <QString, QAbstractSocket*> m_clientMap;
    std::unique_ptr<ServDb> m_sdb;
    QVector<ClientList> chatList;
    QMap <QString, QByteArray> m_clientKey;

private:
    template <typename T>
    void sendToClient(const QString& str2, const T& str, QAbstractSocket* pSocket);
    void sendToClient(const QString& protocol, const QString& whosend , const QVariant& msgData, QAbstractSocket *pSocket);

public:
    MyServer(int nPort, QWidget* pwgt = 0);
signals:
            void SigNameDelete(const QString& name);
public slots:
    virtual void slotNewConnection();
            void slotReadClient   ();
            void slotDeleteMap();
            void slotEncryptedReady();
            void slotSendSig(const QString&name);
};


#endif // MYSERVER_H

