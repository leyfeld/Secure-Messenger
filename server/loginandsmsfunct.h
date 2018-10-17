#ifndef LOGINANDSMSFUNCT_H
#define LOGINANDSMSFUNCT_H

#include "database.h"
#include "servererror.h"


ServerError Registration(QTcpSocket* pClientSocket, QMap <QString, QTcpSocket*>& m_clientMap, QVector<ClientList>& chatList,
                 ServDb* m_sdb, QString& login, QString& name, QString& password);


ServerError Login(QTcpSocket* pClientSocket, QMap <QString, QTcpSocket*>& m_clientMap, QVector<ClientList>& chatList,
          ServDb* m_sdb, QString& login, QString& password);

#endif // LOGINANDSMSFUNCT_H
