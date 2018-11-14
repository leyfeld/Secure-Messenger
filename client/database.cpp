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
    //QSqlQuery query(dbClient );

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
    query.finish();
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
    query.finish();
}

void database::GetMessageW(const QString& login, QVector<Messagelist> & mesList)
{
    mesList.resize(0);
    qDebug()<<"We want old message";
    qDebug()<<login;
    QString dir, mes;
    QDateTime mesDate;
    QSqlQuery query(dbClient);
    if (!query.exec("create table if not exists "+login+" (message varchar(256), time integer, direction varchar(5))"))
    {
        qDebug() << "Can't create table: " << query.lastError().text();
    }

    if (!query.exec("select message, time, direction from "+login))
    {
        qDebug() << query.lastError().text();
        return;
    }

    while(query.next())
    {
        dir=query.value(2).toString();
        mesDate=query.value(1).toDateTime();
        mes=query.value(0).toString();
        mesList.push_back({dir, mesDate, mes});
    }
    query.finish();
}

void database::ReturnMessage(const QString& login, QList <QVariant> & mesList)
{
    mesList.clear();
    qRegisterMetaTypeStreamOperators<Messagelist>("Messagelist");
    Messagelist tmp;
    qDebug()<<"We want to return our message";
    qDebug()<<login;
    QString dir, mes;
    QDateTime mesDate;
    QSqlQuery queryTable(dbClient), queryMessage(dbClient);

    if(!queryTable.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='"+login+"'"))
    {
         qDebug() << queryTable.lastError().text();
    }

    if(queryTable.next())
    {
        if (!queryMessage.exec("select message, time, direction from "+login))
        {
            qDebug() << queryMessage.lastError().text();
            return;
        }
        while(queryMessage.next())
        {
            tmp.direction=queryMessage.value(2).toString();
            tmp.data=queryMessage.value(1).toDateTime();
            tmp.message=queryMessage.value(0).toString();

            QVariant vMessageList;
            vMessageList.setValue(tmp);
            mesList.push_back(vMessageList);
        }
    }
    queryTable.finish();
    queryMessage.finish();
}
bool database::IsDBEmpty(const QVector<ClientList>& chatList)
{
    QSqlQuery query(dbClient);
    for(int i=0;i<chatList.length();i++)
    {
        if(!query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='"+chatList[i].m_login+"'"))
        {
            qDebug() << query.lastError().text();
        }

        if(query.next())
        {
            query.finish();
            return false;
        }
    }
     query.finish();
     return true;
}

void database::CreateSoltTable(const QString& login,const QString & solt)
{
    dbClient = QSqlDatabase::database(cnDbSendMessage);
    QSqlQuery query(dbClient);
    QString tableName;
    tableName=login+"Init";
    if (!query.exec("create table if not exists "+tableName+" (solt varchar(25))"))
    {
        qDebug() << "Can't create table: " << query.lastError().text();
    }
    query.prepare("INSERT INTO "+tableName+" (solt) values(:solt)");
    query.bindValue(":solt", solt);

    if (!query.exec())
    {
        qDebug() << "Can't insert message: " << query.lastError().text();
    }
    query.finish();
}

QString database::GetSolt(const QString& login)
{
    dbClient = QSqlDatabase::database(cnDbSendMessage);
    QSqlQuery query(dbClient);
    QString tableName;
    tableName=login+"Init";
    if(!query.exec("SELECT solt FROM "+tableName))
    {
        qDebug() << query.lastError().text();
    }

    if(query.next())
    {
        QString str=query.value(0).toString();
        query.finish();
        return str;
    }
    return "Solt not exist";
}

database::~database()
{

    QSqlDatabase db;
    if (!QSqlDatabase::contains(cnDbSendMessage))
    {
        qDebug() << "Can't find db connection";
        return;
    }

        //db = QSqlDatabase::database(cnDbSendMessage);
        db.close();


    //db = QSqlDatabase();
    QSqlDatabase::removeDatabase(cnDbSendMessage);
}
