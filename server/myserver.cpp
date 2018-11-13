#include "myserver.h"
#include "database.h"
#include "servererror.h"
#include "loginandsmsfunct.h"
#include "messageprotocol.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QList>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlDriverPlugin>
#include <QDir>
#include <QTextEdit>
#include <QDataStream>
#include <QTcpServer>
#include <QSslSocket>
#include <QCoreApplication>



MyServer::MyServer(int nPort, QWidget* pwgt /*=0*/) : QWidget(pwgt), m_nNextBlockSize(0)
{
    QString dbPath = QCoreApplication::applicationDirPath() + "/server.db";
    qDebug() << "Current Server Db path: " << dbPath;
    m_sdb.reset(new ServDb(dbPath));
    m_socketServer = new SslServer(this);
    if (!m_socketServer->listen(QHostAddress::Any, nPort)) {
        QMessageBox::critical(0, "Server Error", "Unable to start the server:" + m_socketServer->errorString());
        m_socketServer->close();
        return;
    }

    connect(m_socketServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

    m_ptxt = new QTextEdit;
    m_ptxt->setReadOnly(true);

    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(m_ptxt);
    setLayout(pvbxLayout);

}
/*virtual*/ void MyServer::slotNewConnection()
{
   QSslSocket* pClientSocket = qobject_cast<QSslSocket*>(m_socketServer->nextPendingConnection());
    m_sdb->createConnection();

    connect(pClientSocket, SIGNAL(disconnected()), this, SLOT(slotDeleteMap()));
    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
//    connect(this, SIGNAL(SigNameDelete(cosnt QString&)), this, SLOT(slotSendSig(const QString&)));


    qDebug()<<"slotnewconnection!";
}
void MyServer::slotDeleteMap()
{
   QSslSocket* pClientSocket = (QSslSocket*)sender();
   QString nameClient = m_clientMap.key(pClientSocket);
   m_clientMap.remove(nameClient);
   m_clientKey.remove(nameClient);
   slotSendSig(nameClient);
}

void MyServer::slotEncryptedReady()
{
    qDebug()<<"slotEncryptedReady!";
}

void MyServer::slotSendSig(const QString &name)
{
    foreach (QAbstractSocket* nameFor, m_clientMap)
    {
        sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::deleteClient)),name, nameFor);
    }
}

void MyServer::slotReadClient()
{
    QSslSocket* pClientSocket = (QSslSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_4_2);
    for (;;)
    {
        if (!m_nNextBlockSize)
        {
            if (pClientSocket->bytesAvailable() < static_cast<qint64>(sizeof(quint16)))
            {
                break;
            }
            in >> m_nNextBlockSize;
            qDebug() <<"read block server: " << m_nNextBlockSize;
        }
        if (pClientSocket->bytesAvailable() < m_nNextBlockSize)
        {
            break;
        }
        QDateTime   time;
        quint8 loginProtocol = 0;
        in >> time >> loginProtocol;
        qDebug() << "Server get loginProtocol: " << loginProtocol;
        switch (static_cast<LoginAndSmsProtocol>(loginProtocol))
        {
        case LoginAndSmsProtocol::registration: // если int = 1
        {
            QString login;
            QString name;
            QString password;
            QString solt;
            in >> login >> name >> password>>solt;
            ServerError status = Registration(pClientSocket, m_clientMap, chatList, m_sdb.get(), login, name, password, solt);
            sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::registration)), QString::number(static_cast<int>(status)), pClientSocket);
            if(status == ServerError::Success)
            {
                sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::sendChatList)), chatList, pClientSocket);
                chatList.clear();
            }
            break;
        }
        case LoginAndSmsProtocol::login:
        {
            QString login, solt;
            //QString password;
            in >> login;
            ServerError status = Login(pClientSocket, m_clientMap, /*chatList,*/ m_sdb.get(), login, solt);
            qDebug()<<"Status"<<QString::number(static_cast<int>(status));
            sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::login)),QString::number(static_cast<int>(status)),solt,pClientSocket);
//            if(status == ServerError::Success)
//            {
//                sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::login)), solt, pClientSocket);
//                chatList.clear();
//            }
            break;
        }
        case LoginAndSmsProtocol::mes:
        {
            QString login;
            QByteArray str;
            in >>login >> str;
            QVariant mes(str);
            QString strMessage = time.toString() + " " + "Client has sended - " + str;
            m_ptxt->append(strMessage);
            if(!m_clientMap.contains(login))
            {
                in<<static_cast<qint8>(ServerError::LoginOffline);
                break;
            }
            const QString whosend = m_clientMap.key(pClientSocket);
            sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::mes)), whosend , mes, m_clientMap.value(login));
            break;
        }
        case LoginAndSmsProtocol::sendChatList:
        {
            m_sdb->ChatList(m_clientMap, chatList);
            sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::sendChatList)), chatList, pClientSocket);
            chatList.clear();
            break;
        }
        case LoginAndSmsProtocol::sendFile:
        {
            QVariant msgData;
            QString login;
            in >> login >> msgData;
            const QString whosend = m_clientMap.key(pClientSocket);
            if(!m_clientMap.contains(login))
            {
                QMap<QString, QVariant> val;
                val = msgData.toMap();
                QString filename;
                filename = val.value("FILENAME").toString();
                sendToClient(QString::number(static_cast<int>(ServerError::LoginOffline)), filename, m_clientMap.value(whosend));
                //in<<static_cast<qint8>(ServerError::LoginOffline);
                break;
            }
            sendToClient(QString::number(static_cast<int>(LoginAndSmsProtocol::sendFile)),whosend, msgData, m_clientMap.value(login));
            break;
        }
        case LoginAndSmsProtocol::reqwestFileInfo:
        {
            QString name;
            //QString filename;
            in >>name ;
            if(!m_clientMap.contains(name))
            {
                in<<static_cast<qint8>(ServerError::LoginOffline);
                break;
            }
            const QString whosend = m_clientMap.key(pClientSocket);
            sendToClient(QString::number(static_cast<int>(LoginAndSmsProtocol::reqwestFileInfo)),whosend, m_clientMap.value(name));
            break;
        }
        case LoginAndSmsProtocol::answerSendFile:
        {
            QString name;
            in >> name;
            if(!m_clientMap.contains(name))
            {
                in<<static_cast<qint8>(ServerError::LoginOffline);
                break;
            }
            const QString whosend = m_clientMap.key(pClientSocket);
            sendToClient(QString::number(static_cast<int>(LoginAndSmsProtocol::answerSendFile)),whosend, m_clientMap.value(name));
            break;
        }
        case LoginAndSmsProtocol::sendPublicKey:
        {
            QByteArray pbKey;
            in >> pbKey;
            const QString whosend = m_clientMap.key(pClientSocket);
            m_clientKey.insert(whosend, pbKey);
            foreach (QAbstractSocket* name, m_clientMap)
            {
                QVariant key(pbKey);
                sendToClient(QString::number(static_cast<int>(LoginAndSmsProtocol::sendPublicKey)), whosend,key,name);
            }
            foreach (QByteArray pubKey, m_clientKey)
            {
                QVariant key(pubKey);
                QString name = m_clientKey.key(pubKey);
                sendToClient(QString::number(static_cast<int>(LoginAndSmsProtocol::sendPublicKey)),name, key, pClientSocket);
            }
            break;
        }
        case LoginAndSmsProtocol::requestMessage:
        {
            const QString whosend = m_clientMap.key(pClientSocket);
            m_sdb->ChatList(m_clientMap, chatList);
            for(auto it=chatList.begin(); it!=chatList.end();++it)
            {
               // qDebug()<<it->m_login;
                if(m_clientMap.contains(it->m_login) && (it->m_login!=whosend))
                {
                    sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::requestMessage)),whosend,
                                 m_clientMap.value(it->m_login));
                    qDebug()<<it->m_login;
                }
            }
            chatList.clear();
            break;
        }
        case LoginAndSmsProtocol::answerMessage:
        {
            const QString whosend = m_clientMap.key(pClientSocket);
            qRegisterMetaTypeStreamOperators<Messagelist>("Messagelist");
            QString name, date;
          //  QList <Messagelist> lmesList;
            QList<QVariant> mesList;
            QVariant vTmp;
            in>>name>>vTmp;
            mesList=vTmp.value <QList<QVariant>>();
            //Messagelist tmp=mesList[0].value<Messagelist>();
           // lmesList=mesList.value<QList <Messagelist>>();
            if(mesList.isEmpty())
            {
                qDebug()<<"List id empty";
            }
            //qDebug()<<"name"<<name<<tmp.message<<"time"<<tmp.data<<"direction"<<tmp.direction;
            sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::answerMessage)),whosend, mesList, m_clientMap.value(name));
            break;
        }
        case LoginAndSmsProtocol::loginAndPassword:
        {
            QString login;
            QString password;
            in >> login>>password;
            ServerError status = LoginAndPassword( pClientSocket, m_clientMap, chatList, m_sdb.get(), login, password);
            sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::loginAndPassword)),QString::number(static_cast<int>(status)), pClientSocket);
            if(status == ServerError::Success)
            {
                sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::sendChatList)), chatList, pClientSocket);
                chatList.clear();
                qDebug() << "chatlist send!";
            }
            break;
        }
        default:
        throw std::runtime_error("not implemented switch LoginAndSmsProtocol");
        }
        m_nNextBlockSize = 0;
    }
}

template <typename T>
void MyServer::sendToClient(const QString& str2, const T& str, QAbstractSocket *pSocket)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << QDateTime::currentDateTime() << str2 << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    qDebug()<<"send arrblock from server: "<< arrBlock.size();
    pSocket->write(arrBlock);

}

void MyServer::sendToClient(const QString& protocol,const QString& whosend ,const QVariant& msgData, QAbstractSocket* pSocket)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << QDateTime::currentDateTime() << protocol << whosend << msgData;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    qDebug()<<"send arrblock from server: "<< arrBlock.size();
    pSocket->write(arrBlock);
}

void MyServer::sendToClient(const QString& protocol,const QString& whosend, const QString& mes, const QDateTime& dateTime,
                            const QString& dir, QAbstractSocket* pSocket)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << QDateTime::currentDateTime() << protocol << whosend << mes<<dateTime<<dir;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    qDebug()<<"send arrblock from server: "<< arrBlock.size();
    pSocket->write(arrBlock);
}
