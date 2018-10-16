#include "qmlconnect.h"
qmlConnect::qmlConnect()
{
    client=new tcpClient ("localhost", 2323);
    connect(client, SIGNAL(getMessage(const QString &)),this, SLOT(slotReadMessage(const QString &)));
    connect(client, SIGNAL(registrationError(int)),this, SLOT(slotRegistrationError(int)));
    connect(client, SIGNAL (getClientList(const QVector<ClientList>&)),
            this, SLOT(chatListChange(const QVector<ClientList>&)));
    QString dbPath = QDir::currentPath() + "/dbClient.db";
    qDebug() << "Current Server Db path: " << dbPath;
    dbClient.reset(new database(dbPath));
    dbClient->CreateConnection();
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

    QString log, password;

    log=(fldLogin->property("text")).toString();
    password=(fldPassword->property("text")).toString();
    qDebug()<<log<<" "<<password;
    client->SendLoginToServer(log,password);
}

void qmlConnect::registrationForm()
{
    //console.log("We are in Enter");
    qDebug()<<"We are in Registration";

    fldName=viewer->findChild<QObject*>("nameRegField");
    fldLogin=viewer->findChild<QObject*>("logRegField");
    fldPassword=viewer->findChild<QObject*>("pswRegField");

    QString vName, log, password;

    vName=(fldName->property("text")).toString();
    log=(fldLogin->property("text")).toString();
    password=(fldPassword->property("text")).toString();
    qDebug()<<vName<<" "<<log<<" "<<password;
    client->SendRegistrationToServer(log,vName,password);

}

void qmlConnect::messageForm()
{
    qDebug()<<"We are in Messanger";
    btnTabBar=viewer->findChild<QObject*>("btnMessageList");
    fldText=viewer->findChild<QObject*>("field1");
    textArea=viewer->findChild<QObject*>("textArea");


    QString log, message;

    log=(btnTabBar->property("text")).toString();
    message=(fldText->property("text")).toString();
    dbClient->InsertMessage(log, message);
    qDebug()<<" "<<log<<" "<<message;
    client->SendMessageToServer(log,message);
}
void qmlConnect::slotRegistrationError(int errorCode)
{
    txtError=viewer->findChild<QObject*>("txtError");
    switch(errorCode)
    {
    case 0:
        //qDebug()<<"Everything is ok";
        emit toMessanger();
        break;
    case 101: txtError->setProperty("text","К сожалению, такой логин уже существует!");
        break;
    case 102: txtError->setProperty("text","Неправильный логин или пароль!");
        break;
    case 109: txtError->setProperty("text","Ошибка регистрации!");
        break;
    }
}
void qmlConnect::chatListChange(const QVector <ClientList>& chList)
{
    qDebug()<<"Here we are!";
    listview=viewer->findChild<QObject*>("listClient");
    listview->setProperty("text", " ");
}
void qmlConnect::slotReadMessage(const QString& str)
{
    textArea=viewer->findChild<QObject*>("textArea");
    textArea->setProperty("text",str);
}
