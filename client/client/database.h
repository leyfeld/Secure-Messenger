#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QDebug>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include "messagelist.h"

class database
{
private:
    QSqlDatabase dbClient;
    const char* cnDbSendMessage="dbClient";


public:
     database(const QString&);
    void CreateConnection();
    void InsertSendMessage(const QString&, const QString&, const QDateTime& );
    void InsertReceiveMessage(const QString&, const QString&, const QDateTime& );
    void GetMessage(const QString &, QVector<Messagelist> &);
     ~database();


};

#endif // DATABASE_H
