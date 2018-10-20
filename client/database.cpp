#include "database.h"

database::database(const QString& dbName)
{

    dbClient = QSqlDatabase::addDatabase("QSQLITE", cnDbSendMessage);
    dbClient.setDatabaseName(dbName);
    dbClient = QSqlDatabase::database(cnDbSendMessage);
}

void database::CreateConnection()
{

    if (!QSqlDatabase::contains(cnDbSendMessage))
    {
        qDebug() << "Can't find db connection";
        return;
    }
    QString tableName="test";
    QSqlQuery query(dbClient );

    if (!query.exec("create table if not exists SendMessage (login varchar(20), message varchar(256), smData integer)"))
    {
        qDebug() << "Can't create table: " << query.lastError().text();
    }
    if (!query.exec("create table if not exists ReceiveMessage (login varchar(20), message varchar(256), smData integer)"))
    {
        qDebug() << "Can't create table: " << query.lastError().text();
    }
}

void database:: InsertSendMessage (const QString& login, const QString& message, const QDateTime& time)
{
    if (!QSqlDatabase::contains(cnDbSendMessage))
    {
        qDebug() << "Can't find db connection";
        return;
    }
    dbClient = QSqlDatabase::database(cnDbSendMessage);
    QSqlQuery query(dbClient);
    if (!query.exec("create table if not exists "+login+" (message varchar(256), time integer, direction varchar(5))"))
    {
        qDebug() << "Can't create table: " << query.lastError().text();
    }
    query.prepare("INSERT INTO "+login+" (message, time, direction) values(:message, :time, :direction)");
    query.bindValue(":message", message);
    query.bindValue(":time", time);
    query.bindValue(":direction", "to");
    if (!query.exec())
    {
        qDebug() << "Can't insert message: " << query.lastError().text();
    }
}

void database:: InsertReceiveMessage (const QString& login, const QString& message, const QDateTime& time)
{
    if (!QSqlDatabase::contains(cnDbSendMessage))
    {
        qDebug() << "Can't find db connection";
        return;
    }
    qDebug()<<"we are in database";
    dbClient = QSqlDatabase::database(cnDbSendMessage);
    QSqlQuery query(dbClient);
    if (!query.exec("create table if not exists "+login+" (message varchar(256), time integer, direction varchar(5))"))
    {
        qDebug() << "Can't create table: " << query.lastError().text();
    }
    query.prepare("INSERT INTO "+login+" (message, time, direction) values(:message, :time, :direction)");
    query.bindValue(":message", message);
    query.bindValue(":time", time);
    query.bindValue(":direction", "from");
    if (!query.exec())
    {
        qDebug() << "Can't insert message: " << query.lastError().text();
    }
}

void database::GetMessage(const QString& login, QVector<Messagelist> & mesList)
{
    mesList.resize(0);
    qDebug()<<"We want old message";
    qDebug()<<login;
    QString dir, mes;
    QSqlQuery query(dbClient);
    if (!query.exec("create table if not exists "+login+" (message varchar(256), time integer, direction varchar(5))"))
    {
        qDebug() << "Can't create table: " << query.lastError().text();
    }

    if (!query.exec("select message, direction from "+login))
    {
        qDebug() << query.lastError().text();
        return;
    }

    while(query.next())
    {
        dir=query.value(1).toString();
        mes=query.value(0).toString();
        mesList.push_back({dir, mes});
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
