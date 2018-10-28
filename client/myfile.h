#ifndef MYFILE_H
#define MYFILE_H


#include <QString>
#include <QVariant>


class FileSender:public QObject
{
    Q_OBJECT
public:
    FileSender(const QString& login,  const QString& filename);

signals:
    void SigSendFile(const QString &login, const QVariant &data);

public slots:
    void slotTransferFile();

private:
    QString m_login;
    QString m_filename;
};

#endif // MYFILE_H
