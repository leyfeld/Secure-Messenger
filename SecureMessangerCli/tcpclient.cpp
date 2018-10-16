#include "tcpclient.h"


tcpClient::tcpClient(const QString& strHost,int nPort, QObject *myparent, QObject *ta) : QObject(myparent), m_nNextBlockSize(0)
{

    m_pTcpSocket = new QTcpSocket(this);
    textArea=ta;
    qDebug()<<strHost;
    qDebug()<<nPort;
    m_pTcpSocket->connectToHost(strHost, nPort);
    qDebug()<<m_pTcpSocket;
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );


    //connect(obj.btnEnter, SIGNAL(clicked()), SLOT(slotSendToServer()));

}

void tcpClient::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_4_2);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime   time;
        QString str;

        if(!EnterSuccess)
        {
            qDebug()<<"flagFalse";
            in >> time >> str;
            qDebug()<<str;
            if(str.toInt()>100)
            {
                emit registrationError(str.toInt());
            }
            if(str.toInt()==0)
            {
                emit registrationError(str.toInt());
                EnterSuccess=true;
            }
        }
        else
        {
            qDebug()<<"flagTrue";
            in>>time>>chatList;
            EnterSuccess=false;
            qDebug()<<chatList[0].m_login<<chatList[0].m_name<<chatList[0].m_online;
            emit getClientList(chatList);
            //qDebug()<<str.toInt();
        }
        qDebug()<<"end";

    m_nNextBlockSize=0;
    }
}
void tcpClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                     "The host was not found." :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     "The remote host is closed." :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     "The connection was refused." :
                     QString(m_pTcpSocket->errorString())
                    );
    qDebug()<<strError;
    //textArea->setProperty("text", strError);
}
void tcpClient::SendMessageToServer(const QString & login, const QString & mes)
{
    QByteArray  arrBlock;
    quint8 actionNumber=3;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    qDebug()<<login<<" "<<mes;
    out << quint16(0) << QTime::currentTime()<<actionNumber<<login<<mes;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    qDebug()<<m_pTcpSocket;
    m_pTcpSocket->write(arrBlock);
    //fldLogin->property("text")="";
   // fldPassword->property("text")="";
}
void tcpClient::SendRegistrationToServer(const QString & login,const QString & name,const QString & password )
{
    QByteArray  arrBlock;
    quint8 actionNumber=1;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    qDebug()<<login<<name<<password;
    out << quint16(0) << QTime::currentTime()<<actionNumber
        <<login<<name<<password;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    qDebug()<<m_pTcpSocket;
    m_pTcpSocket->write(arrBlock);
    //fldLogin->property("text")="";
   // fldPassword->property("text")="";
}

void tcpClient::SendLoginToServer(const QString & login,const QString & password )
{
    QByteArray  arrBlock;
    quint8 actionNumber=2;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    qDebug()<<login<<password;
    out << quint16(0) << QTime::currentTime()<<actionNumber
        <<login<<password;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    qDebug()<<m_pTcpSocket;
    m_pTcpSocket->write(arrBlock);
    //fldLogin->property("text")="";
   // fldPassword->property("text")="";
}
void tcpClient::slotConnected()
{
    qDebug()<<"OK";
    //textArea->setProperty("text", "Received the connected() signal");
}


