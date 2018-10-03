#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlTableModel>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QString>
#include <QSqlQuery>

class ServDb
{
public:
    ServDb(const QString& dbName);
    void createConnection();
    void ServInsert(int id, const QString& name);
    void IsHasClient(const QString& name);
    ~ServDb();
private:
    static const char* m_serverDatabase;
};


#endif // DATABASE_H
