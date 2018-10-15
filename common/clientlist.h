#ifndef CLIENTLIST_H
#define CLIENTLIST_H

#include <QString>

struct ClientList
{
    QString m_login;
    QString m_name;
    bool m_online;
};

QDataStream &operator << (QDataStream& out,const ClientList& list);
QDataStream &operator >> (QDataStream& in, ClientList& list);

#endif // CLIENTLIST_H
