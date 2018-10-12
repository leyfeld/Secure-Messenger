#ifndef LOGINANDSMSFUNCT_H
#define LOGINANDSMSFUNCT_H

#include "database.h"
#include "servererror.h"

enum class LoginAndSmsProtocol
{
    registration = 1,
    login = 2,
    mes = 3

};

ServerError Registration(QTcpSocket* pClientSocket, QMap <QString, QTcpSocket*>& m_clientMap, QVector<ClientList>& chatList,
                 ServDb* m_sdb, QString& login, QString& name, QString& password);


ServerError Login(QTcpSocket* pClientSocket, QMap <QString, QTcpSocket*>& m_clientMap, QVector<ClientList>& chatList,
          ServDb* m_sdb, QString& login, QString& name, QString& password);
#endif // LOGINANDSMSFUNCT_H
