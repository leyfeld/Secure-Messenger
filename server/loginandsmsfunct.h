#ifndef LOGINANDSMSFUNCT_H
#define LOGINANDSMSFUNCT_H

#include "database.h"
#include "servererror.h"

enum class LoginAndSmsProtocol : int
{
    registration = 1,
    login = 2,
    mes = 3

};

int Registration(QTcpSocket* pClientSocket, QMap <QString, QTcpSocket*>& m_clientMap, QVector<ClientList>& chatList,
                 ServDb* m_sdb, QString& login, QString& name, QString& password);


int Login(QTcpSocket* pClientSocket, QMap <QString, QTcpSocket*>& m_clientMap, QVector<ClientList>& chatList,
          ServDb* m_sdb, QString& login, QString& name, QString& password);
#endif // LOGINANDSMSFUNCT_H
