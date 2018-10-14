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
        QString num;
        in >> time >> str;
        qDebug()<<str;
        int i=0;
        while(str[i]!=' ')
        {
            num+=str[i];
            i++;
        }
        if(num=="3")
            emit getMessage(str);
        //obj.fldLogin->setProperty("text", str);
        //QObject* win=this->parent()->findChild<QObject*>("registerwin");
        //if(str=="200")
        {

//            clEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));
//            if (!(clEngine.rootObjects().isEmpty()))
//                clRoot = clEngine.rootObjects()[0];
        }

        //textArea->setProperty("text", time.toString() + " " + str);
        m_nNextBlockSize = 0;
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
void tcpClient::SendToServer(const QString & str)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    qDebug()<<str;
    out << quint16(0) << QTime::currentTime()<<str;
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


