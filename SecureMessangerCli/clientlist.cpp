#include "clientlist.h"
#include <QDataStream>

QDataStream &operator<< (QDataStream& out, const ClientList& list)
{
   out << list.m_login << list.m_name << list.m_online;
   return out;
}

QDataStream &operator>> (QDataStream& in, ClientList& list)
{
    in >> list.m_login >> list.m_name >> list.m_online;
    return in;
}
