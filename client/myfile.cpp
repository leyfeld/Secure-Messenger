#include "myfile.h"
#include "common.h"
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QDebug>
#include <QDir>


FileSender::FileSender(const QString &login,  const QString &filename):m_login(login), m_filename(filename)
{

}

void FileSender::slotTransferFile()
{
    QFileInfo fi(m_filename);
    qint64 bytes_to_read = 1024;//1048576;//2*1024*102416*1024;//1*1024;//16*1024;
    qint64 bytes_read = 0;
    qint64 max_bytes = fi.size();
    qint64 full_max_bytes = max_bytes;
    qint64 read_bytes = 0;
    QFile file(m_filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Can't read from file '" << m_filename;
    }
    while (max_bytes != 0)
    {
        QMap<QString, QVariant> new_val;
        new_val.insert("FILENAME", m_filename);
        QByteArray byteArray;
        if(bytes_to_read > full_max_bytes)
        {
            bytes_to_read = full_max_bytes;
        }
        read_bytes = bytes_to_read;
        if((read_bytes + bytes_read) > full_max_bytes)
        {
            read_bytes = full_max_bytes - bytes_to_read;
        }
        byteArray = file.read(read_bytes);
        max_bytes -= byteArray.size();
        new_val.insert("SIZE", byteArray.size());
        new_val.insert("PARTFILE", byteArray);
        if(!max_bytes)
        {
            new_val.insert("ISEND", "end");
        }
        else
        {
            new_val.insert("ISEND", "next");
        }
        emit SigSendFile(m_login, new_val);
        bytes_read += byteArray.size();
        delay(10);
    }
    file.close();
}
