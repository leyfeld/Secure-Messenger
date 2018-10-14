#ifndef QMLCONNECT_H
#define QMLCONNECT_H

#include <QObject>
#include <QDebug>
#include "tcpclient.h"

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
    tcpClient* client;
public: 
    qmlConnect(QObject *QMLObject=nullptr);
    void SetRootObj (QObject* RObject=nullptr);


signals:
    void toMessanger();


public slots:
   void enterForm ();
   void registrationForm();
   void messageForm();
   void slotReadMessage(const QString& str);

protected:
   QObject *viewer;

};

#endif // QMLCONNECT_H
