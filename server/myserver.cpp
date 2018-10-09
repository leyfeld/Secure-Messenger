#include "myserver.h"
#include "database.h"
#include "servererror.h"
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
        QString str;
        in >> time >> str;
        if(str == "reg")
        {
            QString login;
            QString name;
            QString password;
            str.clear();
            in >> login >> name >> password;
            m_clientMap.insert(login, pClientSocket);
            if(!m_sdb->IsHasClient(login))
            {
                m_sdb->ServInsert(login, name, password);
            }
            else
            {
                in << static_cast<qint8>(ServerError::NameInDbError);
                break;
            }
            m_sdb->ChatList(m_clientMap, chatList);
        }
        else if(str == "mes")
        {
            QString login;
            str.clear();
            in >>login >>str;
            QString strMessage = time.toString() + " " + "Client has sended - " + str;
            m_ptxt->append(strMessage);
            if(!m_clientMap.contains(login))
            {
                in<<static_cast<qint8>(ServerError::LoginOffline);
                break;
            }
            sendToClient("Server Response: Received \"" + str + "\"", m_clientMap.value(login));
        }
        m_nNextBlockSize = 0;
    }
}
void MyServer::sendToClient(const QString& str, QTcpSocket* pSocket)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << QTime::currentTime() << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    pSocket->write(arrBlock);

}