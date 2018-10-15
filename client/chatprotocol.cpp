#include "chatprotocol.h"
#include "messageprotocol.h"
#include <QByteArray>
#include <QTime>
#include <QTcpSocket>
#include <QFileInfo>
#include <QDataStream>
#include <QBuffer>

ChatProtocol::ChatProtocol(const QString& strHost, int nPort)
    : m_nNextBlockSize(0)
{
    m_socket = new QTcpSocket(this);
    m_socket->connectToHost(strHost, nPort);
    connect(m_socket, SIGNAL(connected()), SIGNAL(SigConnected()));
    connect(m_socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );
}
void ChatProtocol::Send(QByteArray& arrBlock, QDataStream& streamPtr)
{
    streamPtr.device()->seek(0);
    streamPtr << quint16(arrBlock.size() - sizeof(quint16));
    m_socket->write(arrBlock);
}
void ChatProtocol::SendRegistrationInfo(const QString& login,const QString& name, const QString& password)
{
    QByteArray arrBlock;
    QDataStream outStream(&arrBlock, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_4_2);
    outStream << quint16(0) << QTime::currentTime() << static_cast<quint8>(LoginAndSmsProtocol::registration) << login << name << password;
    Send(arrBlock, outStream);
}

void ChatProtocol::SendLoginInfo(const QString &login, const QString &password)
{
    QByteArray arrBlock;
    QDataStream outStream(&arrBlock, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_4_2);
    outStream << quint16(0) << QTime::currentTime() << static_cast<quint8>(LoginAndSmsProtocol::login) << login << password;
    Send(arrBlock, outStream);
}

void ChatProtocol::SendMessageInfo(const QString &name,const QString &sms)
{
    QByteArray arrBlock;
    QDataStream outStream(&arrBlock, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_4_2);
    outStream << quint16(0) << QTime::currentTime() << static_cast<quint8>(LoginAndSmsProtocol::mes) << name << sms;
    Send(arrBlock, outStream);
}

void ChatProtocol::SendFileInfo(const QString& name, qint64 size)
{
    QByteArray arrBlock;
    QDataStream outStream(&arrBlock, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_4_2);
    outStream << quint16(0) << QTime::currentTime() << static_cast<quint8>(LoginAndSmsProtocol::fileInfo) << name << size;
    Send(arrBlock, outStream);
}

void ChatProtocol::SendFile(const QByteArray& array,const QString &endOrNext)
{
    QByteArray arrBlock;
    QDataStream outStream(&arrBlock, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_4_2);
    outStream << quint16(0) << QTime::currentTime() << static_cast<quint8>(LoginAndSmsProtocol::sendFile) << array << endOrNext;
    Send(arrBlock, outStream);
}

void ChatProtocol::TransferFile(const QString &filename)
{
    QFileInfo fi(filename);
    qint64 bytes_to_read = 1024;//1048576;//2*1024*102416*1024;//1*1024;//16*1024;
    qint64 bytes_read = 0;
    qint64 max_bytes = fi.size();
    qint64 full_max_bytes = max_bytes;
    qint64 read_bytes = 0;
    QString str = "next";
    SendFileInfo(filename, max_bytes);
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Can't read from file '" << filename;
    }
    while (max_bytes != 0)
    {
        QByteArray byteArray;
        if(bytes_to_read > full_max_bytes)
        {
            bytes_to_read = full_max_bytes;
        }
        read_bytes = bytes_to_read;
        if((read_bytes + bytes_read) > full_max_bytes)
        {
            read_bytes = full_max_bytes - bytes_to_read;
        }
        byteArray = file.read(read_bytes);
        max_bytes -= byteArray.size();

        if(m_socket->state()!=QAbstractSocket::ConnectedState)
        {
            qDebug()<<"Socket disconnected error\n";
            break;
        }
        SendFile(byteArray, str);
        bytes_read += byteArray.size();
        if(!max_bytes)
        {
            QString str = "end";
            SendFile(byteArray, str);
        }
    }
    file.close();
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
        QTime   time;
        QString str;
        in >> time >> str;
        emit SigReadyRead(time, str);
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
