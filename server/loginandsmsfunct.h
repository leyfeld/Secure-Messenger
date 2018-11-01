#ifndef LOGINANDSMSFUNCT_H
#define LOGINANDSMSFUNCT_H

#include "database.h"
#include "servererror.h"

class QSslSocket;

ServerError Registration(QSslSocket* pClientSocket, QMap<QString, QAbstractSocket *> &m_clientMap, QVector<ClientList>& chatList,
                 ServDb* m_sdb, QString& login, QString& name, QString& password);


ServerError Login(QSslSocket* pClientSocket, QMap <QString, QAbstractSocket*>& m_clientMap, QVector<ClientList>& chatList,
          ServDb* m_sdb, QString& login, QString& password);

#endif // LOGINANDSMSFUNCT_H
