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
    query.prepare("INSERT INTO SendMessage (Login, Message, smData) values(:login, :message, :smdata)");
    query.bindValue(":login", login);
    query.bindValue(":message", message);
    query.bindValue(":smdata", time);
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
    query.prepare("INSERT INTO ReceiveMessage (Login, Message, smData) values(:login, :message, :smdata)");
    query.bindValue(":login", login);
    query.bindValue(":message", message);
    query.bindValue(":smdata", time);
    if (!query.exec())
    {
        qDebug() << "Can't insert message: " << query.lastError().text();
    }
}

void database::GetMessage(const QString& login, QVector<QString> & messageList)
{
    messageList.resize(0);
    qDebug()<<"We want old message";
    dbClient = QSqlDatabase::database(cnDbSendMessage);
    QSqlQuery smQuery(dbClient);
    QSqlQuery rmQuery(dbClient);
    smQuery.prepare("select message, smData from SendMessage where login=:login");
    smQuery.bindValue(":login", login);
    if (!smQuery.exec())
    {
        qDebug() << smQuery.lastError().text();
        return;
    }
    rmQuery.prepare("select message, smData from ReceiveMessage where login=:login");
    rmQuery.bindValue(":login", login);
    if (!rmQuery.exec())
    {
        qDebug() << rmQuery.lastError().text();
        return;
    }

    while(rmQuery.next() || smQuery.next())
    {
        QDateTime smTime, rmTime;
        if(!rmQuery.next())
        {
            messageList.push_back(smQuery.value(1).toString());
            return;
        }
        if(!smQuery.next())
        {
            messageList.push_back(rmQuery.value(1).toString());
            return;
        }
        smTime=smQuery.value(2).toDateTime();
        rmTime=rmQuery.value(2).toDateTime();

        if(smTime>=rmTime)
        {
            messageList.push_back("0");
            messageList.push_back(rmQuery.value(1).toString());
            messageList.push_back(smQuery.value(1).toString());
        }
        else
        {
            messageList.push_back("1");
            messageList.push_back(smQuery.value(1).toString());
            messageList.push_back(rmQuery.value(1).toString());
        }

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
