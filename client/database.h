#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

class database
{
private:
    QSqlDatabase dbClient;
    const char* cnDbSendMessage="dbClient";

public:
     database(const QString&);
    void CreateConnection();
    void InsertMessage(const QString&, const QString& );
     ~database();


};

#endif // DATABASE_H
