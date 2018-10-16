#include "myserver.h"
#include "database.h"
#include "servererror.h"
#include "loginandsmsfunct.h"
#include "messageprotocol.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QTime>
#include <QList>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlDriverPlugin>
#include <QDir>
#include <QTcpServer>
#include <QTextEdit>
#include <QTcpSocket>
#include <QDataStream>



MyServer::MyServer(int nPort, QWidget* pwgt /*=0*/) : QWidget(pwgt), m_nNextBlockSize(0)
{
    QString dbPath = QDir::currentPath() + "/server.db";
    qDebug() << "Current Server Db path: " << dbPath;
    m_sdb.reset(new ServDb(dbPath));
    m_ptcpServer = new QTcpServer(this);
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
        QMessageBox::critical(0, "Server Error", "Unable to start the server:" + m_ptcpServer->errorString());
        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

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
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    m_clientList.push_back(pClientSocket);
    m_sdb->createConnection();
    connect(pClientSocket, SIGNAL(disconnected()), this, SLOT(slotDeleteMap()));
    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));

   // sendToClient("Server Response: Connected!");
}
void MyServer::slotDeleteMap()
{
   QTcpSocket* pClientSocket = (QTcpSocket*)sender();
   QString nameClient = m_clientMap.key(pClientSocket);
   m_clientMap.remove(nameClient);
}

void MyServer::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
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
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime   time;
        quint8 loginProtocol = 0;
        in >> time >> loginProtocol;
        switch (static_cast<LoginAndSmsProtocol>(loginProtocol))
        {
            case LoginAndSmsProtocol::registration: // если int = 1
            {
               QString login;
               QString name;
               QString password;
               in >> login >> name >> password;
               ServerError status = Registration(pClientSocket, m_clientMap, chatList, m_sdb.get(), login, name, password);
               sendToClient(QString::number(static_cast<int>(LoginAndSmsProtocol::registration)), QString::number(static_cast<int>(status)), pClientSocket);
               if(status == ServerError::Success)
               {
                   sendToClient(QString::number(static_cast<int>(LoginAndSmsProtocol::sendChatList)), chatList, pClientSocket);
               }
               break;
            }
            case LoginAndSmsProtocol::login:
            {
                QString login;
                QString password;
                in >> login >> password;
                ServerError status = Login(pClientSocket, m_clientMap, chatList, m_sdb.get(), login, password);
                sendToClient(QString::number(static_cast<int>(LoginAndSmsProtocol::login)),QString::number(static_cast<int>(status)), pClientSocket);
                if(status == ServerError::Success)
                {
                    sendToClient(QString::number(static_cast<int>(LoginAndSmsProtocol::sendChatList)), chatList, pClientSocket);
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
                sendToClient(QString::number(static_cast<int>(LoginAndSmsProtocol::mes)),"Login " + whosend +": " + str, m_clientMap.value(login));
                break;
            }
        case LoginAndSmsProtocol::sendFile:
            {
//                QString partOfFile;
//                QString end;
//                QString login;
//                in >> login >> partOfFile >> end;
//                const QString whosend = m_clientMap.key(pClientSocket);
//                sendToClient(QString::number(static_cast<int>(LoginAndSmsProtocol::sendFile)),whosend, partOfFile, end, m_clientMap.value(login));
            }
        case LoginAndSmsProtocol::fileInfo:
            {
//                QString filename;
//                qint64 size;
//                QString login;
//                in>> login >> filename >> size;
//                const QString whosend = m_clientMap.key(pClientSocket);
//                LoginAndSmsProtocol status = LoginAndSmsProtocol::fileInfo;
//                sendToClient(QString::number(static_cast<int>(status)), whosend, filename, size, m_clientMap.value(login));
//                break;
            }
        default:
            throw std::runtime_error("not implemented switch LoginAndSmsProtocol");

        }
        m_nNextBlockSize = 0;
    }
}
template <typename T>
void MyServer::sendToClient(const QString& str2, const T& str, QTcpSocket* pSocket)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << QTime::currentTime() << str2 << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    pSocket->write(arrBlock);

}
template <typename T>
void MyServer::sendToClient(const QString& protocol,const QString& whosend ,const QString& array, const T& str, QTcpSocket* pSocket)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << QTime::currentTime() << protocol << whosend << array << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    pSocket->write(arrBlock);
}
