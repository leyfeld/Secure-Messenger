#include "chatprotocol.h"
#include "messageprotocol.h"
#include "common.h"
#include "threadsend.h"
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QFileInfo>
#include <QDataStream>
#include <QBuffer>
#include <QMap>
#include <QDir>
#include <QSslCertificate>
#include <QSslSocket>
#include <QThreadPool>
#include <QMutexLocker>

ChatProtocol::ChatProtocol()
    : m_nNextBlockSize(0)
{
    chatList.resize(0);
    m_socket = new QSslSocket(this);
    m_socket->disconnectFromHost();
    QByteArray cert;
    QFile file_cert(":/cert/sc.crt");
    if(file_cert.open(QIODevice::ReadOnly))
    {
        cert = file_cert.readAll();
        file_cert.close();
    }
    QSslCertificate ssl_cert(cert);
    m_socket->addCaCertificate(ssl_cert);
    //m_socket->setLocalCertificate(ssl_cert);
    m_socket->setProtocol(QSsl::AnyProtocol);
    connect(m_socket, SIGNAL(sslErrors(const QList<QSslError>&)),
            this, SLOT(slotSslErrorOccured(const QList<QSslError>&)));

    connect(m_socket, SIGNAL(encrypted()), SIGNAL(SigConnected()));
    connect(m_socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(slotError(QAbstractSocket::SocketError)));

}

void ChatProtocol::ConnectEncrypted(const QString &strHost, int nPort)
{
    m_socket->connectToHostEncrypted(strHost, nPort);
}

void ChatProtocol::slotSslErrorOccured(const QList<QSslError> & error) {
    qDebug() << "sslErrorOccured:" << error;
    m_socket->ignoreSslErrors(error);
}

void ChatProtocol::Send(LoginAndSmsProtocol protocolCommand, const QList<QVariant> &params)
{
    ThreadSend* senderThread = new ThreadSend(protocolCommand, params, m_socket, m_mutex);
    QThreadPool::globalInstance()->start(senderThread);
}
void ChatProtocol::SendRegistrationToServer(const QString& login,const QString& name, const QString& password)
{
    qDebug() <<"SendRegistrationToServer: ";
    Send(LoginAndSmsProtocol::registration, {login, name, password});
}

void ChatProtocol::SendLoginToServer(const QString &login, const QString &password)
{
    qDebug() <<"SendLoginToServer: ";
    Send(LoginAndSmsProtocol::login, {login, password});
}

void ChatProtocol::SendMessageToClient(const QString &name,const QString &sms)
{
    qDebug() <<"SendMessageToClient: ";
    Send(LoginAndSmsProtocol::mes, {name,sms});
}
void ChatProtocol::SendRefreshChatList()
{
    qDebug() <<"SendRefreshChatList: ";
    Send(LoginAndSmsProtocol::sendChatList, {});
}
void ChatProtocol::SendFile(const QString& login, const QVariant &data)
{
    qDebug() <<"SendFile: ";
    Send(LoginAndSmsProtocol::sendFile, {login, data});
}
void ChatProtocol::WriteAndReadFile(const QString &whosend, const QVariant &data, const QDateTime &time)
{
    QMap<QString, QVariant> val;
    val = data.toMap();
    QString filename;
    QString endOrNext;
    qint64 size = 0;
    QByteArray byteArray;
    if(val.find("SIZE") != val.end())
    {
        size = val.value("SIZE").toUInt();
    }
    if(val.find("FILENAME") != val.end())
    {
        filename = val.value("FILENAME").toString();
    }
    if(val.find("PARTFILE") != val.end())
    {
        byteArray = val.value("PARTFILE").toByteArray();
    }
    if(val.find("ISEND") != val.end())
    {
        endOrNext = val.value("ISEND").toString();
    }
    QString new_dir = QFileInfo(filename).fileName();
    filename = QDir::currentPath() + "/" + new_dir;
//    qDebug() << "Current File path: " <<  filename;
    if(size > 0)
    {
        QFile file(filename);
//        qDebug() << "Current File open path: " <<  filename;
        if(!file.open(QIODevice::Append | QIODevice::Unbuffered))
        {
            qDebug() << "Can't write to file '" << filename;
        }
        file.write(byteArray.data(), size);
        file.close();
    }
    if(endOrNext == "end")
    {
        emit SigAllFile(whosend,new_dir, time);
    }
}

void ChatProtocol::ReqwestAddFile(const QString& loginSentTo, const QString &filename)
{
    qDebug() <<"ReqwestAddFile: ";
    Send(LoginAndSmsProtocol::reqwestFileInfo, {loginSentTo});
}

void ChatProtocol::AnswerOnReqwestSendFile(const QString &loginSentTo)
{
    qDebug() <<"AnswerOnReqwestSendFile: ";
    Send(LoginAndSmsProtocol::answerSendFile, {loginSentTo});
}
void ChatProtocol::slotReadyRead()
{
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_4_2);
    for (;;)
    {
        if (!m_nNextBlockSize)
        {
            if (m_socket->bytesAvailable() < static_cast<qint64>(sizeof(quint16)))
            {
                break;
            }
            in >> m_nNextBlockSize;
            qDebug() <<"read block: " << m_nNextBlockSize;
            qDebug() <<"read bytesAvailable: " << m_socket->bytesAvailable();
        }

        if (m_socket->bytesAvailable() < m_nNextBlockSize)
        {
            break;
        }
        QDateTime   time;
        QString loginProtocol;
        in >> time >> loginProtocol;
        if(static_cast<ServerError>(loginProtocol.toUInt()) == ServerError::LoginOffline)
        {
            QString filename;
            in >> filename;
            emit SigStopSendFile(filename);
            //break;
        }
        else{
        switch (static_cast<LoginAndSmsProtocol>(loginProtocol.toUInt()))
        {
            case LoginAndSmsProtocol::registration: // если int = 1
            {
                QString serAnswer;
                in >> serAnswer;
                emit SigAnswerReg(static_cast<ServerError>(serAnswer.toUInt()));
                break;
             }
            case LoginAndSmsProtocol::login:
            {
                QString serAnswer;
                in >> serAnswer;
                qDebug() <<"LoginAndSmsProtocol::login: ";
                emit SigAnswerLogin(static_cast<ServerError>(serAnswer.toUInt()));
                break;
            }
            case LoginAndSmsProtocol::sendChatList:
            {
                in >> chatList;
                qDebug() << chatList[0].m_login<<chatList[0].m_name<<chatList[0].m_online<<chatList.size();
                qDebug() <<"LoginAndSmsProtocol::sendChatList: ";
                emit SigGetClientList(chatList);
                break;
            }
            case LoginAndSmsProtocol::mes:
            {
                QString message;
                QString log;
                in>>message;
                int i=0;
                while(message[i]!=' ')
                {
                    i++;
                }
                while(message[i]!=':')
                {
                    log+=message[i];
                    i++;
                }
                emit SigGetMessage(log, message, time);
                break;
            }
            case LoginAndSmsProtocol::sendFile:
            {
                QString whosend;
                QVariant msgData;
                in >> whosend >> msgData ;
                WriteAndReadFile(whosend, msgData, time);
                break;
            }
            case LoginAndSmsProtocol::reqwestFileInfo:
            {
                QString whosend;
                //QString filename;
                in >> whosend ;
                qDebug() <<"LoginAndSmsProtocol::reqwestFileInfo:от кого запрос "<< whosend;
                emit SigGetFile(whosend);
                break;
            }
            case LoginAndSmsProtocol::answerSendFile:
            {
                QString whosend;
                in>>whosend;
                qDebug() <<"LoginAndSmsProtocol::answerSendFile:кто прислал ответ "<< whosend;
                emit SigSendFileTo(whosend);
                break;
            }
        default:
            throw std::runtime_error("not implemented switch LoginAndSmsProtocol");
            break;
        }}
        m_nNextBlockSize = 0;
        qDebug()<<"000000";
    }
 }
void ChatProtocol::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                     "The host was not found." :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     "The remote host is closed." :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     "The connection was refused." :
                     QString(m_socket->errorString()) );
    emit SigErrorHappened(strError);
}

void ChatProtocol::slotSendFile(const QString &login, const QVariant &data)
{
    SendFile(login,data);
}
