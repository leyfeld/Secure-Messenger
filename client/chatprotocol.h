#ifndef CHATPROTOCOL_H
#define CHATPROTOCOL_H
#include <QTcpSocket>
#include <QDataStream>

class ChatProtocol: public QObject
{
    Q_OBJECT
public:
    ChatProtocol(const QString &strHost, int nPort);
    void SendRegistrationInfo(const QString& login, const QString& name, const QString& password);
    void SendLoginInfo(const QString& login,const QString& password);
    void SendMessageInfo(const QString &name, const QString &sms);
    void SendFileInfo(const QString &name, qint64 size);
    void SendFile(const QByteArray &array, const QString &endOrNext);
    void TransferFile(const QString &filename);
signals:
    void SigReadyRead(const QTime &time, const QString &str);
    void SigErrorHappened(const QString& strError);
    void SigConnected();
private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
private:
    void Send(QByteArray &arrBlock, QDataStream &streamPtr);
    quint16     m_nNextBlockSize;
    QTcpSocket* m_socket;
};


#endif // CHATPROTOCOL_H
