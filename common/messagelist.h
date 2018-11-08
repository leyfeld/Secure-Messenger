#ifndef MESSAGELIST_H
#define MESSAGELIST_H

#include <QString>
#include <QDateTime>
#include <QMetaType>

class Messagelist
{
    public:
        QString direction;
        QDateTime data;
        QString message;
        friend QDataStream &operator << (QDataStream& out,const Messagelist& list)
        {
           out << list.message << list.data << list.direction;
           return out;
        }
        friend QDataStream &operator >> (QDataStream& in, Messagelist& list)
        {
            in >> list.message >> list.data >> list.direction;
            return in;
        }
};



Q_DECLARE_METATYPE(Messagelist);
#endif // MESSAGELIST_H
