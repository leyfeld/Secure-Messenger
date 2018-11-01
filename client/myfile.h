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
    void SigStop();

public slots:
    void slotTransferFile();
    void slotStopTransferFile(const QString &filename);

private:
    QString m_login;
    QString m_filename;
    QString m_stopFile;
    //bool m_stop;
};

#endif // MYFILE_H
