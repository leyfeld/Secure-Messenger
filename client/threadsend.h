#ifndef THREADSEND_H
#define THREADSEND_H

#include "messageprotocol.h"
#include <QByteArray>
#include <QDataStream>
#include <QRunnable>

class QAbstractSocket;

class ThreadSend: public QRunnable
{
public:
    ThreadSend(LoginAndSmsProtocol protocolCommand, const QList<QVariant> &params, QAbstractSocket* socket, QMutex& mutex);
    void run() override;

private:
    LoginAndSmsProtocol m_protocolCommand;
    QList<QVariant> m_params;
    QAbstractSocket* m_socket;
    QMutex& m_mutex;

};

#endif // THREADSEND_H
