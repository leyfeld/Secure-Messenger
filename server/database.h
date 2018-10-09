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
#include <QTcpSocket>

class ServDb
{
public:
    ServDb(const QString& dbName);
    void createConnection();
    void ServInsert(const QString &login, const QString &name, const QString &hashPassword);
    bool IsHasClient(const QString& name);
    void ChatList(const QMap<QString, QTcpSocket *> &serverMap, QVector<ClientList>& chatList);
    ~ServDb();
private:
    static const char* m_serverDatabase;
};


#endif // DATABASE_H
