#include "database.h"

database::database(const QString& dbName)
{

    dbClient = QSqlDatabase::addDatabase("QSQLITE", cnDbSendMessage);
    dbClient.setDatabaseName(dbName);
}

void database::CreateConnection()
{
    dbClient = QSqlDatabase::database(cnDbSendMessage);
    if (!QSqlDatabase::contains(cnDbSendMessage))
    {
        qDebug() << "Can't find db connection";
        return;
    }
    QSqlQuery query(dbClient );
    if (!query.exec("create table if not exists SendMessage (login varchar(20), message varchar(256))"))
    {
        qDebug() << "Can't create table: " << query.lastError().text();
    }
}

void database:: InsertMessage (const QString& login, const QString& message)
{
    if (!QSqlDatabase::contains(cnDbSendMessage))
    {
        qDebug() << "Can't find db connection";
        return;
    }
    dbClient = QSqlDatabase::database(cnDbSendMessage);
    QSqlQuery query(dbClient);
    query.prepare("INSERT INTO SendMessage (Login, Message) values(:login, :message)");
    query.bindValue(":login", login);
    query.bindValue(":message", message);
    if (!query.exec())
    {
        qDebug() << "Can't insert person: " << query.lastError().text();
    }
}

database::~database()
{
    if (!QSqlDatabase::contains(cnDbSendMessage))
    {
        qDebug() << "Can't find db connection";
        return;
    }
    QSqlDatabase db = QSqlDatabase::database(cnDbSendMessage);
    db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(cnDbSendMessage);
}
