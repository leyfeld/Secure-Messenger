#ifndef CHATPROTOCOL_H
#define CHATPROTOCOL_H
#include "servererror.h"
#include "clientlist.h"
#include "myfile.h"
#include "messageprotocol.h"
#include <QDataStream>
#include <QTcpSocket>
#include <QSslSocket>


class ChatProtocol: public QObject
{
    Q_OBJECT
public:
    ChatProtocol();
    void ConnectEncrypted(const QString &strHost, int nPort);
    void SendRegistrationToServer(const QString& login, const QString& name, const QString& password);
    void SendLoginToServer(const QString& login,const QString& password);
    void SendRefreshChatList();
    void SendMessageToClient(const QString &name, const QString &sms);
    void SendFile(const QString &login, const QVariant &data);
    void WriteAndReadFile(const QString &whosend, const QVariant &data, const QDateTime &time);
    void ReqwestAddFile(const QString &loginSentTo, const QString &filename);
    void AnswerOnReqwestSendFile(const QString &loginSentTo);

signals:
    void SigGetMessage(const QString &login, const QString &message, const QDateTime &time);
    void SigErrorHappened(const QString& strError);
    void SigConnected();
    void SigAnswerReg(ServerError);
    void SigAnswerLogin(ServerError);
    void SigAnswerMessage(ServerError);
    void SigAnswerSendFile(ServerError);
    void SigGetClientList (const QVector <ClientList>&);
    void SigGetFile(const QString & login);
    void SigSendFileTo(const QString& login);
    void SigAllFile(const QString &whosend, const QString &filename, const QDateTime &time);
    void SigStopSendFile(const QString &filename);
    void SigSendInfo();

private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotSslErrorOccured(const QList<QSslError> & error);
    void slotSendFile(const QString &login, const QVariant &data);
private:
    void Send(LoginAndSmsProtocol protocolCommand, const QList<QVariant>& params);

    quint16     m_nNextBlockSize;
    QSslSocket* m_socket;
    QVector<ClientList> chatList;
    QMutex m_mutex;


};


#endif // CHATPROTOCOL_H
