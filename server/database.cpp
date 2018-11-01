#include "database.h"
#include "clientlist.h"
#include <QSslSocket>



const char* ServDb::m_serverDatabase = "ServerDatabase";

ServDb::ServDb(const QString &dbName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_serverDatabase);
    db.setDatabaseName(dbName);
}

void ServDb:: createConnection()
{
    qDebug()<<"start";
    QSqlDatabase db = QSqlDatabase::database(m_serverDatabase);
    if (!QSqlDatabase::contains(m_serverDatabase))
    {
        qDebug() << "Can't find db connection";
        return;
    }
    QSqlQuery query(db);
    if (!query.exec("create table if not exists person (id integer primary key autoincrement, login varchar(20), name varchar(20), hashPassword varchar(20))"))
    {
        qDebug() << "Can't create table: " << query.lastError().text();
    }
}
bool ServDb:: IsHasClient(const QString& name)
{
    QSqlDatabase db = QSqlDatabase::database(m_serverDatabase);
    QSqlQuery query(db);
    query.exec(QString("select * from person where login = '%1'").arg(name));
    if (!query.exec())
    {
        qDebug() << "Can't select person: " << query.lastError().text();
    }
    return query.next();
}
bool ServDb:: LoginCheck(const QString& login, const QString& password)
{
    QSqlDatabase db = QSqlDatabase::database(m_serverDatabase);
    QSqlQuery query(db);
    query.exec(QString("select * from person where login = '%1'").arg(login));
    if (!query.exec())
    {
        qDebug() << "Can't select person: " << query.lastError().text();
        return false;
    }
    if (query.next())
    {
       if(password != query.value(3).toString())
       {
           return false;
       }
       return true;
    }
    return false;
}
void ServDb:: ServInsert(const QString &login, const QString &name, const QString &hashPassword)
{
    if (!QSqlDatabase::contains(m_serverDatabase))
    {
        qDebug() << "Can't find db connection";
        return;
    }
    QSqlDatabase db = QSqlDatabase::database(m_serverDatabase);
    QSqlQuery query(db);
    query.prepare("INSERT INTO person (id, login, name, hashPassword) values(null, :login, :name, :hashPassword)");
    query.bindValue(":login", login);
    query.bindValue(":name", name);
    query.bindValue(":hashPassword", hashPassword);
    if (!query.exec())
    {
        qDebug() << "Can't insert person: " << query.lastError().text();
    }
}

void ServDb::ChatList(const QMap<QString, QAbstractSocket*> &serverMap, QVector<ClientList>& chatList)
{

    QSqlDatabase db = QSqlDatabase::database(m_serverDatabase);
    QSqlQuery query(db);
    query.exec("select login, name from person");
    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return;
    }
    while(query.next())
    {
        QString login = query.value(0).toString();
        QString name = query.value(1).toString();
        bool online = serverMap.contains(login);
        chatList.push_back({login, name, online});
    }
}


ServDb::~ServDb()
{
    if (!QSqlDatabase::contains(m_serverDatabase))
    {
        qDebug() << "Can't find db connection";
        return;
    }
    QSqlDatabase db = QSqlDatabase::database(m_serverDatabase);
    db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(m_serverDatabase);
}

