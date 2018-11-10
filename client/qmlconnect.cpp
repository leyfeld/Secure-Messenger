#include "qmlconnect.h"
#include "chatprotocol.h"
#include "myfile.h"
#include <QDateTime>
#include <QFileDialog>
#include <QThread>
#include <QVariant>
#include <QVector>

qmlConnect::qmlConnect() : client(new ChatProtocol())
{

}
void qmlConnect::CreateConnection(const QString& ip)
{
        connect(client.get(), SIGNAL(SigGetMessage(const QString &, const QString &, const QDateTime)),this,
                SLOT(slotReadMessage(const QString &, const QString &, const QDateTime& )));
        connect(client.get(), SIGNAL(SigAllFile(const QString &, const QString &, const QDateTime)),this,
                SLOT(slotReadMessage(const QString &, const QString &, const QDateTime& )));
        connect(client.get(), SIGNAL(SigAnswerReg(ServerError)),this, SLOT(slotRegistrationError(ServerError)));
        connect(client.get(), SIGNAL(SigAnswerLogin(ServerError)),this, SLOT(slotRegistrationError(ServerError)));
        connect(client.get(), SIGNAL (SigGetClientList(const QVector <ClientList> & )),
                this, SLOT(chatListChange(const QVector <ClientList> & )));
        connect(client.get(), SIGNAL (SigGetFile(const QString &)),this, SLOT(slotGetFile(const QString &)));
        connect(client.get(), SIGNAL(SigSendFileTo(const QString&)), this, SLOT(transportFile(QString)));
        connect(client.get(), SIGNAL (SigErrorHappened(const QString& )),
                this, SLOT(slotServerError(const QString& )));
        connect(client.get(),SIGNAL(SigConnected()),this,SLOT(slotServerConnected()));

        client->ConnectEncrypted(ip, 2323);
}
void qmlConnect::SetRootObj(QObject* RObj)
{
    viewer=RObj;
}
void qmlConnect::slotServerConnected()
{
    if(fldName)
    {
        QString vName, log, password;
        vName=(fldName->property("text")).toString();
        log=(fldLogin->property("text")).toString();
        password=(fldPassword->property("text")).toString();
        qDebug()<<vName<<" "<<log<<" "<<password;
        myLogin=log;
        client->SendRegistrationToServer(log,vName,password);
    }
    if(!fldName)
    {
        QString log, password;
        log=(fldLogin->property("text")).toString();
        password=(fldPassword->property("text")).toString();
        qDebug()<<log<<password;
        myLogin=log;
        client->SendLoginToServer(log,password);
    }
}
void qmlConnect::enterForm()
{
    //console.log("We are in Enter");
    qDebug()<<"We are in Enter";
    fldIP=viewer->findChild<QObject*>("ipField");
    fldLogin=viewer->findChild<QObject*>("logField");
    fldPassword=viewer->findChild<QObject*>("pswField");

    QString ip;

    ip=(fldIP->property("text")).toString();

    qDebug()<<ip;
    CreateConnection(ip);
}

void qmlConnect::registrationForm()
{
    //console.log("We are in Enter");
    qDebug()<<"We are in Registration";

    fldIP=viewer->findChild<QObject*>("ipRegField");
    fldName=viewer->findChild<QObject*>("nameRegField");
    fldLogin=viewer->findChild<QObject*>("logRegField");
    fldPassword=viewer->findChild<QObject*>("pswRegField");

    QString ip;

    ip=(fldIP->property("text")).toString();
    CreateConnection(ip);
    qDebug()<<ip;


}

void qmlConnect::messageForm()
{
        qDebug()<<"We are in Messanger";
        btnTabBar=viewer->findChild<QObject*>("btnMessageList");
        fldText=viewer->findChild<QObject*>("field1");
        QString log, message;
        log=(btnTabBar->property("text")).toString();
        message=(fldText->property("text")).toString();
        qDebug()<<" "<<log<<" "<<message;
        if(!m_attachmentPath.isEmpty())
        {
            fileLogin.clear();
            fileLogin = log;
            QString filename = QFileInfo(m_attachmentPath).fileName();
            client->ReqwestAddFile(log, filename);
        }
        if(!message.isEmpty())
        {
            dbClient->InsertSendMessage(log, message, QDateTime::currentDateTime());
            client->SendMessageToClient(log,message);
        }
}
void qmlConnect::chooseFile(const QUrl& url)
{
    QObject* filename = viewer->findChild<QObject*>("filename");
    filename->setProperty("text", url.fileName());
    m_attachmentPath = url.path();
#ifdef _WIN32
    m_attachmentPath.remove(0,1);
#endif
}
void qmlConnect::slotServerError(const QString& errorCode)
{
    txtError=viewer->findChild<QObject*>("txtError");
    txtError->setProperty("text",errorCode);
}
void qmlConnect::cancelFile()
{
    m_attachmentPath.clear();
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
    default:
        txtError->setProperty("text", "ERROR");
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

void qmlConnect::slotGetFile(const QString & login)
{
    fileLogin.clear();
    fileLogin = login;
    emit toGetFile();
}

void qmlConnect::transportFile(const QString& login)
{
    QThread *workerThread= new QThread();
    FileSender* m_file = new FileSender(login, m_attachmentPath);
    m_file->moveToThread(workerThread);
    connect(workerThread, SIGNAL(started()), m_file, SLOT(slotTransferFile()));
    connect(client.get(), SIGNAL(SigStopSendFile(const QString &)), m_file, SLOT(slotStopTransferFile(const QString &)));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    connect(m_file, SIGNAL(SigSendFile(const QString&, const QVariant&)),
            client.get(), SLOT(slotSendFile(const QString&, const QVariant&)));
    workerThread->start();
    m_attachmentPath.clear();
}

void qmlConnect::okSendFile()
{
    client->AnswerOnReqwestSendFile(fileLogin);
}

void qmlConnect::cancelSendFile()
{
    //client->AnswerOnReqwestSendFile(fileLogin);
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
