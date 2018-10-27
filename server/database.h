#ifndef DATABASE_H
#define DATABASE_H

#include "clientlist.h"
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QString>
#include <QSqlQuery>
#include <QMap>

class QAbstractSocket;

class ServDb
{
public:
    ServDb(const QString& dbName);
    void createConnection();
    void ServInsert(const QString &login, const QString &name, const QString &hashPassword);
    bool IsHasClient(const QString& name);
    bool LoginCheck(const QString& login, const QString& password);
    void ChatList(const QMap<QString, QAbstractSocket *> &serverMap, QVector<ClientList>& chatList);
    ~ServDb();
private:
    static const char* m_serverDatabase;
};


#endif // DATABASE_H
