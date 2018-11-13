#include "qmlconnect.h"
#include "chatprotocol.h"
#include "myfile.h"
#include "common.h"
#include <QDateTime>
#include <QFileDialog>
#include <QThread>
#include <QVariant>
#include <QVector>

qmlConnect::qmlConnect() : client(new ChatProtocol())
{
    connect(client.get(),SIGNAL(SigConnected()),this,SLOT(slotServerConnected()));
    connect(client.get(), SIGNAL (SigErrorHappened(const QString& )),
            this, SLOT(slotServerError(const QString& )));
    connect( client.get(),SIGNAL(SigSuccess()), this,SLOT(slotClientConnected()));
    connect(client.get(), SIGNAL(SigSendLoginAndPassword(const QString)),this, SLOT(sendLoginAndPassword(const QString)));
    connect(client.get(), SIGNAL(SigAnswerReg(ServerError)),this, SLOT(slotRegistrationError(ServerError)));
    connect(client.get(), SIGNAL(SigAnswerLogin(ServerError)),this, SLOT(slotRegistrationError(ServerError)));
}
void qmlConnect::CreateConnection(const QString& ip)
{
            client->ConnectEncrypted(ip, 2323);

}
void qmlConnect::SetRootObj(QObject* RObj)
{
    viewer=RObj;
}
void qmlConnect::slotClientConnected()
{
    connect(client.get(), SIGNAL(SigGetMessage(const QString &, const QString &, const QDateTime)),this,
            SLOT(slotReadMessage(const QString &, const QString &, const QDateTime& )));
    connect(client.get(), SIGNAL(SigAllFile(const QString &, const QString &, const QDateTime)),this,
            SLOT(slotReadMessage(const QString &, const QString &, const QDateTime& )));
    connect(client.get(), SIGNAL (SigGetClientList(const QVector <ClientList> & )),
            this, SLOT(chatListChange(const QVector <ClientList> & )));
    connect(client.get(), SIGNAL (SigGetFile(const QString &)),this, SLOT(slotGetFile(const QString &)));
    connect(client.get(), SIGNAL(SigSendFileTo(const QString&)), this, SLOT(transportFile(QString)));
    connect(client.get(), SIGNAL (SigReturnMessage(const QString &)),this, SLOT(slotReturnMessage(const QString &)));
    connect(client.get(), SIGNAL (SigAnswerReturnMessage(const QString&, QList <QVariant> )),
            this, SLOT (slotAnswerReturnMessage(const QString& , QList <QVariant>)));
}
void qmlConnect::slotServerConnected()
{
    qDebug()<<"slotConnected";
    IsConnected=true;
    if(registration)
    {

        QString vName, password;
        vName=(fldName->property("text")).toString(); 
        password=(fldPassword->property("text")).toString();
        SecurePassword sPsw(password);
        solt=sPsw.GetRandomString();
        QByteArray psw =sPsw.GetHash();
        qDebug()<<vName<<psw<<solt;
        client->SendRegistrationToServer(myLogin,vName,psw,solt);
    }
    else
    {
        qDebug()<<"Connection";
        if(!IsSendLogin)
        {
            qDebug()<< "send to server";
            client->SendLoginToServer(myLogin);
            IsSendLogin=true;    
        }
//        QString password;
//        password=(fldPassword->property("text")).toString();
//        SecurePassword sPsw(password);
//        sPsw.SetSolt(dbClient->GetSolt(myLogin));
//        QByteArray psw =sPsw.GetHash();
//        qDebug()<<myLogin<<psw;
//        client->SendLoginToServer(myLogin,psw);
    }
}
void qmlConnect::sendLoginAndPassword(const QString solt)
{
    qDebug()<<"send login and pass";
    QString password;
    password=(fldPassword->property("text")).toString();
    SecurePassword sPsw(password);
    sPsw.SetSolt(solt);
    QByteArray psw =sPsw.GetHash();
    qDebug()<<myLogin<<psw;
    client->SendLoginAndPasswordToServer(myLogin,psw);
}

void qmlConnect::enterForm()
{
    //console.log("We are in Enter");
    qDebug()<<"We are in Enter";
    IsSendLogin=false;
    fldIP=viewer->findChild<QObject*>("ipField");
    fldLogin=viewer->findChild<QObject*>("logField");
    fldPassword=viewer->findChild<QObject*>("pswField");
    QString ip, log;
    ip=(fldIP->property("text")).toString();
    if( !IsConnected)
    {
        qDebug()<<ip;
        //OpenClientDB();
        CreateConnection(ip);
        qDebug()<<"enter form connected";
    }
    log=(fldLogin->property("text")).toString();
    myLogin=log;
    if(IsConnected)
    {
        qDebug()<<"enter form slotserverconnected";
        slotServerConnected();
    }

}

void qmlConnect::registrationForm()
{
    //console.log("We are in Enter");
    qDebug()<<"We are in Registration";
    registration = true;
    fldIP=viewer->findChild<QObject*>("ipRegField");
    fldName=viewer->findChild<QObject*>("nameRegField");
    fldLogin=viewer->findChild<QObject*>("logRegField");
    fldPassword=viewer->findChild<QObject*>("pswRegField");

    QString ip, log, passw;
    log=(fldLogin->property("text")).toString();
    ip=(fldIP->property("text")).toString();
    passw = (fldPassword->property("text")).toString();
    myLogin=log;

    if(IsLogStatusOk(log,passw) && !IsConnected)
    {

       // OpenClientDB();
        CreateConnection(ip);

    }
    if(IsConnected)
    {
        slotServerConnected();
    }
    qDebug()<<ip<<myLogin<<log;


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
            QString new_dir = QFileInfo(filename).fileName();
            dbClient->InsertSendMessage(log, new_dir, QDateTime::currentDateTime());
            //fldText->setProperty("text",new_dir);
        }
        if(!message.isEmpty())
        {
            dbClient->InsertSendMessage(log, message, QDateTime::currentDateTime());
            client->SendMessageToClient(log,message);
        }
}
void qmlConnect::chooseFile(const QUrl& url)
{
    qDebug()<<url;
    QObject* filename = viewer->findChild<QObject*>("filename");
    filename->setProperty("text", url.fileName());
    m_attachmentPath = url.path();
#ifdef _WIN32
    m_attachmentPath.remove(0,1);
#endif
    qDebug()<<m_attachmentPath;
}
void qmlConnect::slotServerError(const QString& errorCode)
{
    qDebug()<<"server error";
    if(rctError)
        rctError->setProperty("visible", true);
    //IsConnected = false;
    QObject* txtError=viewer->findChild<QObject*>("txtError");
    txtError->setProperty("text",errorCode);
}
void qmlConnect::cancelFile()
{
    m_attachmentPath.clear();
}

void qmlConnect::getKey()
{
    btnTabBar=viewer->findChild<QObject*>("btnMessageList");
    QString log;
    log=(btnTabBar->property("text")).toString();
    qDebug()<< "get key: "<< log;
    QString skey = client->GetSKey(log);
    QObject* key = viewer->findChild<QObject*>("key");
    key->setProperty("text", skey);
}
void qmlConnect::messageList(const QString & log)
{
    dbClient->GetMessage(log, mesList);
    qDebug()<<mesList.size();
    QString dTime;
    for(int i=0;i<mesList.size();i++)
    {
        dTime=mesList[i].data.toString("hh:mm ap\nddd MMMM d yy");
        emit toPrevMessageList(mesList[i].direction,mesList[i].message,dTime);
    }

}
void qmlConnect::slotRegistrationError(ServerError errorCode)
{
    QObject* txtError=viewer->findChild<QObject*>("txtError");
    switch(errorCode)
    {
        case ServerError::Success:
        {
            qDebug()<<"Success";
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
    rctError=viewer->findChild<QObject*>("rctError");
    qDebug()<<"Here we are!"<<chatList.size();
    if (dbClient->IsDBEmpty(chatList))
    {
        qDebug()<<"Empty DB!";
        client->SendMessageRequest();
    }
    for(int i=0;i<chatList.size();i++)
    {
        qDebug()<<"chat list";
        emit toChatList(chatList[i].m_login, chatList[i].m_online);
    }
}
void qmlConnect::slotReturnMessage(const QString &login)
{
    QList <QVariant> retMesList;
    dbClient->ReturnMessage(login, retMesList);
    //qDebug()<<retMesList[1].direction;
    client->ReturnMessage(login, retMesList);
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
    qDebug()<<"In transport File";
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
    client->AnswerOnReqwestSendFile(fileLogin);
}
void qmlConnect::slotReadMessage(const QString& log, const QString& mes, const QDateTime & time)
{
    tabBar=viewer->findChild<QObject*>("bar");
    btnTabBar=viewer->findChild<QObject*>("btnMessageList");
    QString lg;
    lg=(btnTabBar->property("text")).toString();
    bool tabBtn=(tabBar->property("currentIndex")).toBool();
    qDebug()<<log<<mes<<time;
    dbClient->InsertReceiveMessage(log, mes, time);
    if(tabBtn && (lg==log) )
        emit toMessageList(mes, time.toString("hh:mm ap\nddd MMMM d yy"));
}
void qmlConnect::slotAnswerReturnMessage(const QString& login, QList <QVariant> RetMessage)
{
    Messagelist tmp;
    for(int i=0;i<RetMessage.length();++i)
    {
        tmp=RetMessage[i].value<Messagelist>();
        if(tmp.direction=="to")
            dbClient->InsertReceiveMessage(login,tmp.message,tmp.data);
        if(tmp.direction=="from")
            dbClient->InsertSendMessage(login,tmp.message,tmp.data);
    }
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
bool qmlConnect::IsForbidSign(const QString &str)
{
    for(int i=0;i<str.length();i++)
    {
        if(str[i].isPunct() && str[i]!='_')
            return true;
        if(str[i].isSpace())
            return true;
    }
    return false;
}
bool qmlConnect::IsLogStatusOk(const QString & login, const QString& passw)
{
    QObject* txtError=viewer->findChild<QObject*>("txtError");
    if (login.length()<3)
    {
        txtError->setProperty("text","Логин слишком короткий");
        return false;
    }
    if (login.length()>20)
    {
        txtError->setProperty("text","Логин слишком длинный!");
        return false;
    }
    if(IsForbidSign(login))
    {
        txtError->setProperty("text","Присутствуют запрещенные символы");
        return false;
    }
    if (passw.length()<3)
    {
        txtError->setProperty("text","Пароль слишком короткий");
        return false;
    }
    if (passw.length()>20)
    {
        txtError->setProperty("text","Пароль слишком длинный!");
        return false;
    }
    if(IsForbidSign(passw))
    {
        txtError->setProperty("text","Присутствуют запрещенные символы");
        return false;
    }

    return true;
}
