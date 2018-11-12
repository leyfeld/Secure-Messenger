#include "threadsend.h"

#include "common.h"
#include <QAbstractSocket>
#include <QDataStream>
#include <QMutexLocker>
#include <QDateTime>
#include <QVariant>

ThreadSend::ThreadSend(LoginAndSmsProtocol protocolCommand, const QList<QVariant> &params, QAbstractSocket *socket, QMutex& mutex)
    :m_protocolCommand(protocolCommand), m_params(params),m_socket(socket), m_mutex(mutex)
{

}

void ThreadSend::run()
{

    QByteArray arrBlock;
    QDataStream outStream(&arrBlock, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_4_2);
    outStream << quint16(0) << QDateTime::currentDateTime() << static_cast<quint8>(m_protocolCommand);
    for (const QVariant& param : m_params)
    {
        if (param.type() == QVariant::String)
        {
            outStream << param.toString();
        }
        else if(param.type() == QVariant::ByteArray)
        {
            outStream << param.toByteArray();
        }
        else if (typeid(param) == typeid(QVariant))
        {
            outStream << param;
        }
        else
        {
            qDebug() << "Trying to send invalid param type";
        }
    }
    outStream.device()->seek(0);
    outStream << quint16(arrBlock.size() - sizeof(quint16));
    qDebug() <<"Send: "<<arrBlock.size();
    QMutexLocker locker(&m_mutex);
    m_socket->write(arrBlock);
    //delay(10);
}
