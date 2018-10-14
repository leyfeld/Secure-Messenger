#include "qmlconnect.h"
qmlConnect::qmlConnect(QObject* QMLObject)
{
    client=new tcpClient ("localhost", 2323);
    connect(client, SIGNAL(getMessage(const QString &)),this, SLOT(slotReadMessage(const QString &)));
}
void qmlConnect::SetRootObj(QObject* RObj)
{
    viewer=RObj;
}
void qmlConnect::enterForm()
{
    //console.log("We are in Enter");
    qDebug()<<"We are in Enter";
    fldLogin=viewer->findChild<QObject*>("logField");
    fldPassword=viewer->findChild<QObject*>("pswField");
    btnEnter=viewer->findChild<QObject*>("btEntry");

    QString str="2 ";
    str.append((fldLogin->property("text")).toString());
    str.append(" ");
    str.append((fldPassword->property("text")).toString());
    qDebug()<<str;
    client->SendToServer(str);
    emit toMessanger();
}

void qmlConnect::registrationForm()
{
    //console.log("We are in Enter");
    qDebug()<<"We are in Registration";
    fldName=viewer->findChild<QObject*>("nameRegField");
    fldLogin=viewer->findChild<QObject*>("logRegField");
    fldPassword=viewer->findChild<QObject*>("pswRegField");

    QString str="1 ";
    str.append((fldName->property("text")).toString());
    str.append(" ");
    str.append((fldLogin->property("text")).toString());
    str.append(" ");
    str.append((fldPassword->property("text")).toString());
    qDebug()<<str;
    client->SendToServer(str);
    emit toMessanger();
}

void qmlConnect::messageForm()
{
    qDebug()<<"We are in Messanger";
    fldText=viewer->findChild<QObject*>("field1");
    textArea=viewer->findChild<QObject*>("textArea");
    QString str="3 ";
    str.append((fldText->property("text")).toString());
    qDebug()<<str;
    client->SendToServer(str);
}

void qmlConnect::slotReadMessage(const QString& str)
{
    textArea=viewer->findChild<QObject*>("textArea");
    textArea->setProperty("text",str);
}
