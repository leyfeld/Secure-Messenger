#ifndef QMLCONNECT_H
#define QMLCONNECT_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <memory>
#include "chatprotocol.h"
#include "database.h"


class qmlConnect : public QObject
{
    Q_OBJECT
private:
    QObject* fldLogin=nullptr;
    QObject* fldPassword=nullptr;
    QObject* fldName=nullptr;
    QObject* btnEnter=nullptr;
    QObject* textArea=nullptr;
    QObject* fldText=nullptr;
    QObject* txtError=nullptr;
    QObject* btnTabBar=nullptr;
    QObject* listview=nullptr;
    QString myLogin;
    std::unique_ptr<ChatProtocol> client;
    std::unique_ptr <database> dbClient;
public: 
    qmlConnect();
    QVector <QString> mesList;
    void SetRootObj (QObject* RObject=nullptr);



signals:
    void toMessanger();
    void toChatList(const QString & log, const bool & offOn);
    void toPrevMessageList(const QString & mes, const bool & num);
    void toMessageList(const QString & mes);

public slots:
   void enterForm ();
   void registrationForm();
   void messageForm();
   void messageList(const QString& log);
   void slotReadMessage(const QString& log, const QString& me, const QDateTime & time);
   void slotRegistrationError(ServerError);
   void chatListChange(const QVector <ClientList> & chatList);

protected:
   QObject *viewer;


};

#endif // QMLCONNECT_H
