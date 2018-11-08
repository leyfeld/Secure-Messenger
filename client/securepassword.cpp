#include "securepassword.h"

SecurePassword::SecurePassword(const QString psw, QObject *parent) : QObject(parent)
{
    cleanPassword=psw;
    //solt=GetRandomString();
}

QString SecurePassword::GetRandomString()
{
   QString slt;
   for(int i=0; i<soltLength; ++i)
      {
          int index = qrand() % possibleCharacters.length();
          QChar nextChar = possibleCharacters.at(index);
          slt.append(nextChar);
      }
   solt=slt;
   return slt;
}

QByteArray SecurePassword::GetHash()
{
    QString sltPsw=cleanPassword+solt;
    return QCryptographicHash::hash(sltPsw.toLocal8Bit(), QCryptographicHash::Sha256);
}

void SecurePassword::SetSolt(const QString & slt)
{
    solt=slt;
}
