#ifndef MYCLIENT_H
#define MYCLIENT_H

#include "chatprotocol.h"
#include <QWidget>

class QTextEdit;
class QLineEdit;

class MyClient : public QWidget {
Q_OBJECT
private:
    QTextEdit*  m_ptxtInfo;
    QLineEdit*  m_ptxtInput;
    QLineEdit*  m_ptxtInputName;
    QLineEdit*  m_ptxtInputReg;
    std::unique_ptr<ChatProtocol> m_chatProtocol;
public:
    MyClient(const QString& strHost, int nPort, QWidget* pwgt = 0) ;
private:
    void  InitProtocol(const QString& strHost,int nPort);
private slots:
    void slotReadyRead(const QTime &time, const QString &str);
    void slotError(const QString &strError);
    void slotSendToServer();
    void slotConnected();
    void slotRegistrationClient();
};
void ParsStr2(const QString& line, QString& name, QString& sms);
void ParsStr3(const QString& line, QString& login, QString& name, QString &password);


#endif // MYCLIENT_H
