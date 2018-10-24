#include "qmlconnect.h"
#include "chatprotocol.h"
qmlConnect::qmlConnect()
{
    client.reset(new ChatProtocol ("localhost", 2323));
    connect(client.get(), SIGNAL(SigGetMessage(const QString &, const QString &, const QDateTime)),this,
            SLOT(slotReadMessage(const QString &, const QString &, const QDateTime& )));
    connect(client.get(), SIGNAL(SigAnswerReg(ServerError)),this, SLOT(slotRegistrationError(ServerError)));
    connect(client.get(), SIGNAL(SigAnswerLogin(ServerError)),this, SLOT(slotRegistrationError(ServerError)));
    connect(client.get(), SIGNAL (SigGetClientList(const QVector <ClientList> & )),
            this, SLOT(chatListChange(const QVector <ClientList> & )));
    connect(client.get(), SIGNAL (SigErrorHappened(const QString& )),
            this, SLOT(slotServerError(const QString& )));


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
    myLogin=log;
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
    myLogin=log;
    client->SendRegistrationToServer(log,vName,password);

}

void qmlConnect::messageForm()
{
    qDebug()<<"We are in Messanger";
    btnTabBar=viewer->findChild<QObject*>("btnMessageList");
    fldText=viewer->findChild<QObject*>("field1");
    QString log, message;
    log=(btnTabBar->property("text")).toString();
    message=(fldText->property("text")).toString();
    dbClient->InsertSendMessage(log, message, QDateTime::currentDateTime());
    qDebug()<<" "<<log<<" "<<message;
    client->SendMessageToClient(log,message);
}

void qmlConnect::messageList(const QString & log)
{
    dbClient->GetMessage(log, mesList);
    qDebug()<<mesList.size();
    for(int i=0;i<mesList.size();i++)
    {
        emit toPrevMessageList(mesList[i].direction,mesList[i].message);
    }

}
void qmlConnect::slotServerError(const QString& errorCode)
{
    txtError=viewer->findChild<QObject*>("txtError");
    txtError->setProperty("text",errorCode);
}
void qmlConnect::slotRegistrationError(ServerError errorCode)
{
    txtError=viewer->findChild<QObject*>("txtError");
    switch(errorCode)
    {
        case ServerError::Success:
        {
            OpenClientDB();
            emit toMessanger();
            break;
        }
        case ServerError::NameInDbError: txtError->setProperty("text","К сожалению, такой логин уже существует!");
        break;
        case ServerError::IncorrectLogin: txtError->setProperty("text","Неправильный логин или пароль!");
        break;
        case ServerError::IncorrectRegistration: txtError->setProperty("text","Ошибка регистрации!");
        break;
    }
}
void qmlConnect::chatListChange(const QVector <ClientList>& chatList)
{
    qDebug()<<"Here we are!"<<chatList.size();
    for(int i=0;i<chatList.size();i++)
    {
        emit toChatList(chatList[i].m_login, chatList[i].m_online);
    }
//    listview=viewer->findChild<QObject*>("listClient");
//    listview->setProperty("text", " ");
}
void qmlConnect::slotReadMessage(const QString& log, const QString& mes, const QDateTime & time)
{
    qDebug()<<log<<mes<<time;
    dbClient->InsertReceiveMessage(log, mes, time);
    emit toMessageList(mes);
}
void qmlConnect::OpenClientDB ()
{
    QString dbPath = QDir::currentPath() + "/dbClient"+myLogin+".db";
    qDebug() << "Current Server Db path: " << dbPath;
    dbClient.reset(new database(dbPath));
    dbClient->CreateConnection();
}

void qmlConnect::refreshChatList()
{
    client->SendRefreshChatList();
}
