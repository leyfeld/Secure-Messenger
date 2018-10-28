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



MyServer::MyServer(int nPort, QWidget* pwgt /*=0*/) : QWidget(pwgt), m_nNextBlockSize(0)
{
    QString dbPath = QDir::currentPath() + "/server.db";
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

    qDebug()<<"slotnewconnection!";
}
void MyServer::slotDeleteMap()
{
   QSslSocket* pClientSocket = (QSslSocket*)sender();
   QString nameClient = m_clientMap.key(pClientSocket);
   m_clientMap.remove(nameClient);
}

void MyServer::slotEncryptedReady()
{
    qDebug()<<"slotEncryptedReady!";
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
            in >> login >> name >> password;
            ServerError status = Registration(pClientSocket, m_clientMap, chatList, m_sdb.get(), login, name, password);
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
            QString login;
            QString password;
            in >> login >> password;
            ServerError status = Login(pClientSocket, m_clientMap, chatList, m_sdb.get(), login, password);
            sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::login)),QString::number(static_cast<int>(status)), pClientSocket);
            if(status == ServerError::Success)
            {
                sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::sendChatList)), chatList, pClientSocket);
                chatList.clear();
            }
            break;
        }
        case LoginAndSmsProtocol::mes:
        {
            QString login;
            QString str;
            in >>login >> str;
            QString strMessage = time.toString() + " " + "Client has sended - " + str;
            m_ptxt->append(strMessage);
            if(!m_clientMap.contains(login))
            {
                in<<static_cast<qint8>(ServerError::LoginOffline);
                break;
            }
            const QString whosend = m_clientMap.key(pClientSocket);
            sendToClient(QString::number(static_cast<quint8>(LoginAndSmsProtocol::mes)),"From " + whosend + ":" +str, m_clientMap.value(login));
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
