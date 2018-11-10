#include "cryptoworker.h"
#include <openssl/bn.h>

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

CryptoWorker::CryptoWorker():m_iv((unsigned char *)"0000000000000000")
{
    srand(time(NULL));
    m_dh.reset(DH_get_2048_256());
    DH_generate_key(m_dh.get());
}

int CryptoWorker::Encrypt(const QString &forLogin, const QString& string, QByteArray &encstring)
{
    int size =string.size()+127;
    encstring.resize(size);
    unsigned char* plaintext = (unsigned char*)string.toStdString().c_str();
    EVP_CIPHER_CTX *ctx = nullptr;
    int len=0;
    int ciphertext_len=0;
    if(!(ctx = EVP_CIPHER_CTX_new()))
    {
        handleErrors();
    }
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, m_secretkey.value(forLogin), m_iv))
    {
        handleErrors();
    }
    unsigned char* ciphertext = reinterpret_cast<unsigned char*>(encstring.data());
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, string.size()))
    {
        handleErrors();
    }
    ciphertext_len = len;
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
    {
        handleErrors();
    }
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    encstring.resize(ciphertext_len);
    return ciphertext_len;
}

int CryptoWorker::Decrypt(const QString& login, const QByteArray &encstring, QString &decrtext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    if(!(ctx = EVP_CIPHER_CTX_new()))
    {
        handleErrors();
    }
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, m_secretkey.value(login), m_iv))
    {
        handleErrors();
    }
    QByteArray tempArray(encstring.size()+127,'\0');
    unsigned char* plaintext = reinterpret_cast <unsigned char*>(tempArray.data());
    const unsigned char* ciphertext = reinterpret_cast <const unsigned char* > (encstring.data());
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, encstring.size()))
    {
        handleErrors();
    }
    plaintext_len = len;
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
    {
        handleErrors();
    }
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    tempArray.resize(plaintext_len);
    decrtext = tempArray;
    return plaintext_len;
}

void CryptoWorker::GeneratorKey(const QString& login, QByteArray& pubkey )
{
    unsigned char* skey = new unsigned char[DH_size(m_dh.get())];
    memset(skey, 0, DH_size(m_dh.get()));
    unsigned char* pkey = reinterpret_cast<unsigned char*> (pubkey.data());
    BIGNUM* pubkeyfrom = BN_bin2bn(pkey,  pubkey.size(), NULL);
    DH_compute_key(skey, pubkeyfrom, m_dh.get());
    m_secretkey.insert(login, skey);
}

QByteArray CryptoWorker::GetPublicKey()
{
   BIGNUM* key = m_dh.get()->pub_key;
   QByteArray pubKey(256,'\0');
   unsigned char* to = reinterpret_cast <unsigned char*> (pubKey.data());
   int t = BN_bn2bin(key, to);
   pubKey.resize(t);
   return pubKey;
}

void CryptoWorker::DeleteNameFromMap(const QString &name)
{
    m_secretkey.remove(name);
}

