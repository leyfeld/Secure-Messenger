#include "myclient.h"

MyClient::MyClient(const QString& strHost,int nPort, QObject *myparent) : QObject(myparent), m_nNextBlockSize(0)
{

    m_pTcpSocket = new QTcpSocket(this);

    m_pTcpSocket->connectToHost(strHost, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );

//    textArea=new QObject;
//    field1=new QObject;
//    button=new QObject;

    textArea = this->parent()->findChild<QObject*>("textArea");
    field1 = this->parent()->findChild<QObject*>("field1");
    button = this->parent()->findChild<QObject*>("button");

//    connect(field1, SIGNAL(editingFinished()),
//            this,        SLOT(slotSendToServer())
//           );
    connect(button, SIGNAL(clicked()), SLOT(slotSendToServer()));

}

void MyClient::slotReadyRead()
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
        in >> time >> str;
        textArea->setProperty("text", time.toString() + " " + str);
        m_nNextBlockSize = 0;
    }
}
void MyClient::slotError(QAbstractSocket::SocketError err)
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
    textArea->setProperty("text", strError);
}
void MyClient::slotSendToServer()
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << QTime::currentTime() << (field1->property("text")).toString();

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
    field1->property("text")="";
}
void MyClient::slotConnected()
{
    textArea->setProperty("text", "Received the connected() signal");
}

