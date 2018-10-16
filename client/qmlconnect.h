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
    std::unique_ptr<ChatProtocol> client;
    std::unique_ptr <database> dbClient;
public: 
    qmlConnect();
    void SetRootObj (QObject* RObject=nullptr);


signals:
    void toMessanger();


public slots:
   void enterForm ();
   void registrationForm();
   void messageForm();
   void slotReadMessage(const QString& str);
   void slotRegistrationError(ServerError);
   void chatListChange(const QVector <ClientList> & chatList);

protected:
   QObject *viewer;

};

#endif // QMLCONNECT_H
