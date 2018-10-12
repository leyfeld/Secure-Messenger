#include "myclient.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include<QVBoxLayout>
#include <QLabel>
#include <QTime>
#include <QTextStream>



MyClient::MyClient(const QString& strHost,int nPort, QWidget* pwgt /*=0*/) : QWidget(pwgt), m_nNextBlockSize(0)
{
    m_pTcpSocket = new QTcpSocket(this);

    m_pTcpSocket->connectToHost(strHost, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );

    m_ptxtInfo  = new QTextEdit;
    m_ptxtInput = new QLineEdit;
    m_ptxtInputName = new QLineEdit;
    m_ptxtInfo->setStyleSheet("QTextEdit{color: red;}");
    m_ptxtInput->setStyleSheet("QLineEdit { background: rgb(0, 255, 255); selection-background-color: rgb(233, 99, 0); }");
    m_ptxtInputName->setStyleSheet("QLineEdit { background: blue; selection-background-color:  black; }");
    connect(m_ptxtInputName, SIGNAL(returnPressed()),
            this,        SLOT(slotRegistrationClient())
           );
    connect(m_ptxtInput, SIGNAL(returnPressed()),
            this,        SLOT(slotSendToServer())
           );
    m_ptxtInfo->setReadOnly(true);
    QPushButton* pcmd = new QPushButton("&Send");
    connect(pcmd, SIGNAL(clicked()), SLOT(slotSendToServer()));
    pcmd->setObjectName("mes");
    QPushButton* preg = new QPushButton("&Registration");
    connect(preg, SIGNAL(clicked()), SLOT(slotRegistrationClient()));
    preg->setObjectName("reg");
    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Client</H1>"));
    pvbxLayout->addWidget(m_ptxtInfo);
    pvbxLayout->addWidget(m_ptxtInput);
    pvbxLayout->addWidget(pcmd);
    pvbxLayout->addWidget(m_ptxtInputName);
    pvbxLayout->addWidget(preg);
    setLayout(pvbxLayout);
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

        m_ptxtInfo->append(time.toString() + " " + str);
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
    m_ptxtInfo->append(strError);
}
void MyClient::slotSendToServer()
{
    QString mes = "mes";
    QString name;
    QString sms;
    ParsStr2(m_ptxtInput->text(), name, sms);
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << QTime::currentTime() << mes<< name << sms;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
    m_ptxtInput->setText("");
}
void MyClient::slotRegistrationClient()
{
    //QString reg = "reg";
    QString login = "";
    QString name = "";
    QString password = "";
    ParsStr3(m_ptxtInputName->text(), login, name, password);
    //RegistrationClient(login, name, password);
    qint8 regProtocol = 2;
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << QTime::currentTime() << regProtocol << login << name << password;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    m_pTcpSocket->write(arrBlock);
    m_ptxtInputName->setText("");
}
void ParsStr2(const QString& line, QString& name, QString& sms)
{
    QString newLine = line;
    QTextStream input(&newLine, QIODevice::ReadOnly);
    input >> name >> sms;
}
void MyClient::slotConnected()
{
    m_ptxtInfo->append("Received the connected() signal");
}
void ParsStr3(const QString& line,QString& login, QString& name, QString& password)
{
    QString newLine = line;
    QTextStream input(&newLine, QIODevice::ReadOnly);
    input >> login >> name >> password;
}

void RegistrationClient(  QString& login, QString& name, QString& password)
{
    int regProtocol = 1;
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << QTime::currentTime() << regProtocol << login << name << password;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
}

