#include "loginandsmsfunct.h"
#include "clientlist.h"
#include <QSslSocket>


ServerError Registration(QSslSocket* pClientSocket, QMap <QString, QAbstractSocket*>& m_clientMap, QVector<ClientList> &chatList,
                                ServDb* m_sdb, QString& login, QString& name, QString& password, QString& solt)
{
    if((login.isNull())||(name.isNull())||(password.isNull()))
    {
        return ServerError::IncorrectProtocol;
    }
    if((login.isEmpty())||(name.isEmpty())||(password.isEmpty()))
    {
        return ServerError::IncorrectRegistration;
    }
    if(m_sdb->IsHasClient(login))
    {
        return ServerError::NameInDbError;
    }
    m_sdb->ServInsert(login, name, password, solt);
    m_clientMap.insert(login, pClientSocket);
    m_sdb->ChatList(m_clientMap, chatList);
    return ServerError::Success;
}

ServerError Login(QSslSocket* pClientSocket, QMap<QString, QAbstractSocket *> &m_clientMap,
//                                QVector<ClientList> &chatList,
                                ServDb* m_sdb, QString& login, QString& solt)
{
    if(login.isNull()/*||(password.isNull())*/)
    {
        return ServerError::IncorrectProtocol;
    }
    if(login.isEmpty()/*||(password.isEmpty())*/)
    {
        return ServerError::IncorrectLogin;
    }
    if(!m_sdb->LoginCheck(login, solt))
    {
        return (ServerError::IncorrectLogin);
    }
    if(m_clientMap.contains(login))
    {
        return (ServerError::LoginExists);
    }
    m_clientMap.insert(login, pClientSocket);
    //m_sdb->ChatList(m_clientMap, chatList);
    return (ServerError::Success);
}

ServerError LoginAndPassword(/*QSslSocket* pClientSocket,*/ QMap <QString, QAbstractSocket*>& m_clientMap, QVector<ClientList>& chatList,
                             ServDb* m_sdb, QString& login, QString& password)
{
    if((login.isNull())||(password.isNull()))
    {
        return ServerError::IncorrectProtocol;
    }
    if((login.isEmpty())||(password.isEmpty()))
    {
        return ServerError::IncorrectLogin;
    }
    if(!m_sdb->LoginAndPasswordCheck(login, password))
    {
        return (ServerError::IncorrectLogin);
    }
//    if(m_clientMap.contains(login))
//    {
//        return (ServerError::LoginExists);
//    }
    //m_clientMap.insert(login, pClientSocket);
    m_sdb->ChatList(m_clientMap, chatList);
    return (ServerError::Success);
}
