#include "chatprotocol.h"
#include "messageprotocol.h"
#include "common.h"
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QTcpSocket>
#include <QFileInfo>
#include <QDataStream>
#include <QBuffer>
#include <QMap>
#include <QDir>

ChatProtocol::ChatProtocol(const QString& strHost, int nPort)
    : m_nNextBlockSize(0)
{
    chatList.resize(0);
    //m_file = new MyFile();
    m_socket = new QTcpSocket(this);
    m_socket->connectToHost(strHost, nPort);
    connect(m_socket, SIGNAL(connected()), SIGNAL(SigConnected()));
    connect(m_socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );
    //connect(m_file, SIGNAL(SigSendFile(QString,QVariant)), this, SLOT(slotSendFile(QString,QVariant)));
}
void ChatProtocol::Send(QByteArray& arrBlock, QDataStream& streamPtr)
{
    streamPtr.device()->seek(0);
    streamPtr << quint16(arrBlock.size() - sizeof(quint16));
    m_socket->write(arrBlock);
}
void ChatProtocol::SendRegistrationToServer(const QString& login,const QString& name, const QString& password)
{
    QByteArray arrBlock;
    QDataStream outStream(&arrBlock, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_4_2);
    outStream << quint16(0) <<QDateTime::currentDateTime() << static_cast<quint8>(LoginAndSmsProtocol::registration) << login << name << password;
    Send(arrBlock, outStream);
}

void ChatProtocol::SendLoginToServer(const QString &login, const QString &password)
{
    QByteArray arrBlock;
    QDataStream outStream(&arrBlock, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_4_2);
    outStream << quint16(0) << QDateTime::currentDateTime() << static_cast<quint8>(LoginAndSmsProtocol::login) << login << password;
    Send(arrBlock, outStream);
}

void ChatProtocol::SendMessageToClient(const QString &name,const QString &sms)
{
    QByteArray arrBlock;
    QDataStream outStream(&arrBlock, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_4_2);
    outStream << quint16(0) << QDateTime::currentDateTime() << static_cast<quint8>(LoginAndSmsProtocol::mes) << name << sms;
    Send(arrBlock, outStream);
}
void ChatProtocol::SendFile(const QString& login, const QVariant &data)
{
    QByteArray arrBlock;
    QDataStream outStream(&arrBlock, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_4_2);
    outStream << quint16(0) << QDateTime::currentDateTime() << static_cast<quint8>(LoginAndSmsProtocol::sendFile)
              << login << data;
    Send(arrBlock, outStream);
}
void ChatProtocol::WriteAndReadFile(const QString &whosend, const QVariant &data)
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
        //emit SigGetFile(whosend, filename);
    }
}
void ChatProtocol::slotReadyRead()
{
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_4_2);
    for (;;)
    {
        if (!m_nNextBlockSize)
        {
            if (static_cast<quint16>(m_socket->bytesAvailable()) < static_cast<qint64>(sizeof(quint16)))
            {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_socket->bytesAvailable() < m_nNextBlockSize)
        {
            break;
        }
        QDateTime   time;
        QString loginProtocol;
        in >> time >> loginProtocol;
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
                emit SigAnswerLogin(static_cast<ServerError>(serAnswer.toUInt()));
                break;
            }
            case LoginAndSmsProtocol::sendChatList:
            {
                in >> chatList;
                qDebug() << chatList[0].m_login<<chatList[0].m_name<<chatList[0].m_online<<chatList.size();
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
                WriteAndReadFile(whosend, msgData);
                break;
            }
        default:
            throw std::runtime_error("not implemented switch LoginAndSmsProtocol");
            break;
        }
        m_nNextBlockSize = 0;
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

