#include "database.h"

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
    if (!query.exec("create table if not exists person (id integer primary key autoincrement, firstname varchar(20))"))
    {
        qDebug() << "Can't create table: " << query.lastError().text();
    }
}
void ServDb:: IsHasClient(const QString& name)
{
    QSqlDatabase db = QSqlDatabase::database(m_serverDatabase);
    QSqlQuery query(db);
    query.exec(QString("select * from person where firstname = '%1'").arg(name));
    if (!query.exec())
    {
        qDebug() << "Can't select person: " << query.lastError().text();
    }
    if (!query.next())
    {
        ServInsert(NULL, name);
    }
}

void ServDb:: ServInsert(int id, const QString &name)
{
    if (!QSqlDatabase::contains(m_serverDatabase))
    {
        qDebug() << "Can't find db connection";
        return;
    }
    QSqlDatabase db = QSqlDatabase::database(m_serverDatabase);
    QSqlQuery query(db);
    query.prepare("INSERT INTO person (id, firstname) values(:id, :firstname)");
    query.bindValue(":firstname", name);
    if (!query.exec())
    {
        qDebug() << "Can't insert person: " << query.lastError().text();
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

