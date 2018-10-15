#include "myclient.h"
#include "messageprotocol.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include<QVBoxLayout>
#include <QLabel>
#include <QTime>
#include <QTextStream>

MyClient::MyClient(const QString& strHost,int nPort, QWidget* pwgt /*=0*/) : QWidget(pwgt)
{
    InitProtocol(strHost, nPort);
    m_ptxtInfo  = new QTextEdit;
    m_ptxtInput = new QLineEdit;
    m_ptxtInputName = new QLineEdit;
    m_ptxtInputReg = new QLineEdit;
    m_ptxtInfo->setStyleSheet("QTextEdit{color: red;}");
    m_ptxtInput->setStyleSheet("QLineEdit { background: rgb(0, 255, 255); selection-background-color: rgb(233, 99, 0); }");
    m_ptxtInputReg->setStyleSheet("QLineEdit { background: blue; selection-background-color:  black; }");
    m_ptxtInputName->setStyleSheet("QLineEdit { background: rgb(0, 255, 255); selection-background-color: rgb(233, 99, 0); }");
    connect(m_ptxtInputReg, SIGNAL(returnPressed()),this, SLOT(slotRegistrationClient()));
    connect(m_ptxtInput, SIGNAL(returnPressed()),this, SLOT(slotSendToServer()));
    connect(m_ptxtInputName, SIGNAL(returnPressed()),this, SLOT(slotSendToServer()));
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
    pvbxLayout->addWidget(m_ptxtInputName);
    pvbxLayout->addWidget(pcmd);
    pvbxLayout->addWidget(m_ptxtInputReg);
    pvbxLayout->addWidget(preg);
    setLayout(pvbxLayout);
}

void MyClient::InitProtocol(const QString& strHost,int nPort)
{
    m_chatProtocol.reset(new ChatProtocol(strHost, nPort));
    connect(m_chatProtocol.get(), SIGNAL(SigReadyRead(QTime,QString)), this, SLOT(slotReadyRead(QTime,QString)));
    connect(m_chatProtocol.get(), SIGNAL(SigErrorHappened(QString)), this, SLOT(slotError(QString)));
    connect(m_chatProtocol.get(), SIGNAL(SigConnected()), this, SLOT(slotConnected()));
}
void MyClient::slotReadyRead(const QTime& time, const QString& str)
{
    m_ptxtInfo->append(time.toString() + " " + str);
}
void MyClient::slotError(const QString& strError)
{
    m_ptxtInfo->append(strError);
}
void MyClient::slotSendToServer()
{
//    QString name;
//    QString sms;
//    ParsStr2(m_ptxtInput->text(), name, sms);
    m_chatProtocol->SendMessageInfo(m_ptxtInputName->text(), m_ptxtInput->text());
    m_ptxtInput->setText("");
    m_ptxtInputName->setText("");
}
void MyClient::slotRegistrationClient()
{
    QString login = "";
    QString name = "";
    QString password = "";
    ParsStr3(m_ptxtInputReg->text(), login, name, password);
    m_chatProtocol->SendRegistrationInfo(login, name, password);
    m_ptxtInputReg->setText("");
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






