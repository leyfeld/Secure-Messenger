#include "loginandsmsfunct.h"
#include "clientlist.h"


int Registration(QTcpSocket* pClientSocket, QMap <QString, QTcpSocket*>& m_clientMap, QVector<ClientList> &chatList,
                                ServDb* m_sdb, QString& login, QString& name, QString& password)
{
  if(!m_sdb->IsHasClient(login))
  {
    m_sdb->ServInsert(login, name, password);
    m_clientMap.insert(login, pClientSocket);
  }
  else
  {
    return static_cast<int>(ServerError::NameInDbError);
  }
  m_sdb->ChatList(m_clientMap, chatList);
  return static_cast<int>(ServerError::Success);
}

int Login(QTcpSocket* pClientSocket, QMap <QString, QTcpSocket*>& m_clientMap,
                                QVector<ClientList> &chatList,
                                ServDb* m_sdb, QString& login, QString& name, QString& password)
{
    if(m_sdb->LoginCheck(login, name, password))
    {
        if(m_clientMap.contains(login))
        {
            return static_cast<int>(ServerError::LoginExists);
        }
        m_clientMap.insert(login, pClientSocket);
    }
    else
    {
        return static_cast<int>(ServerError::IncorrectLogin);
    }
    m_sdb->ChatList(m_clientMap, chatList);
    return static_cast<int>(ServerError::Success);
}

