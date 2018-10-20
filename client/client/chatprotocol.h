#ifndef CHATPROTOCOL_H
#define CHATPROTOCOL_H
#include <servererror.h>
#include <clientlist.h>
#include <QTcpSocket>
#include <QDataStream>


class ChatProtocol: public QObject
{
    Q_OBJECT
public:
    ChatProtocol(const QString &strHost, int nPort);
    void SendRegistrationToServer(const QString& login, const QString& name, const QString& password);
    void SendLoginToServer(const QString& login,const QString& password);
    void SendRefreshChatList();
    void SendMessageToClient(const QString &name, const QString &sms);
    void SendFileInfo(const QString& login, const QString &name, qint64 size);
    void SendFile(const QString &login, const QByteArray &array, const QString &endOrNext);
    void TransferFile(const QString &login, const QString &filename);
signals:
    void SigGetMessage(const QString &, const QString &, const QDateTime &);
    void SigErrorHappened(const QString& strError);
    void SigConnected();
    void SigAnswerReg(ServerError);
    void SigAnswerLogin(ServerError);
    void SigAnswerMessage(ServerError);
    void SigAnswerSendFile(ServerError);
    void SigGetClientList (const QVector <ClientList>&);
private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
private:
    void Send(QByteArray &arrBlock, QDataStream &streamPtr);
    quint16     m_nNextBlockSize;
    QTcpSocket* m_socket;
    QVector<ClientList> chatList;
};


#endif // CHATPROTOCOL_H
