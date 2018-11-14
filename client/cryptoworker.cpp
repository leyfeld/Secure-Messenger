#include "cryptoworker.h"
#include <openssl/bn.h>

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

CryptoWorker::CryptoWorker(): m_iv(16, '\0'), m_dh(DH_get_2048_256(), DH_free)
{
    DH_generate_key(m_dh.get());
}

int CryptoWorker::Encrypt(const QString &forLogin, const QString& string, QByteArray &encstring)
{
    int size =string.size()+127;
    encstring.resize(size);
    QByteArray message = string.toUtf8();
    int messize = message.size();
    unsigned char* plaintext = reinterpret_cast <unsigned char* >(message.data());
    std::unique_ptr<EVP_CIPHER_CTX, void(*)(EVP_CIPHER_CTX *)> ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
    int len=0;
    int ciphertext_len=0;
    if(ctx.get() == nullptr )
    {
        handleErrors();
    }
    unsigned char* iv = reinterpret_cast<unsigned char*>(m_iv.data());
    const unsigned char* key = reinterpret_cast <const unsigned char*> (m_secretkey.value(forLogin).data());
    if(1 != EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL,key,iv))
    {
        handleErrors();
    }
    unsigned char* ciphertext = reinterpret_cast<unsigned char*>(encstring.data());
    if(1 != EVP_EncryptUpdate(ctx.get(), ciphertext, &len, plaintext, messize))
    {
        handleErrors();
    }
    ciphertext_len = len;
    if(1 != EVP_EncryptFinal_ex(ctx.get(), ciphertext + len, &len))
    {
        handleErrors();
    }
    ciphertext_len += len;
    encstring.resize(ciphertext_len);
    return ciphertext_len;
}

int CryptoWorker::Decrypt(const QString& login, const QByteArray &encstring, QString &decrtext)
{
    std::unique_ptr<EVP_CIPHER_CTX, void(*)(EVP_CIPHER_CTX *)> ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
    int len=0;
    int plaintext_len=0;
    if(ctx.get() == nullptr )
    {
        handleErrors();
    }
    unsigned char* iv = reinterpret_cast<unsigned char*>(m_iv.data());
    const unsigned char* key = reinterpret_cast <const unsigned char*> (m_secretkey.value(login).data());
    if(1 != EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL, key, iv))
    {
        handleErrors();
    }
    QByteArray tempArray(encstring.size()+127,'\0');
    unsigned char* plaintext = reinterpret_cast <unsigned char*>(tempArray.data());
    const unsigned char* ciphertext = reinterpret_cast <const unsigned char* > (encstring.data());
    if(1 != EVP_DecryptUpdate(ctx.get(), plaintext, &len, ciphertext, encstring.size()))
    {
        handleErrors();
    }
    plaintext_len = len;
    if(1 != EVP_DecryptFinal_ex(ctx.get(), plaintext + len, &len))
    {
        handleErrors();
    }
    plaintext_len += len;
    tempArray.resize(plaintext_len);
    decrtext = QString::fromUtf8(tempArray);
    return plaintext_len;
}

void CryptoWorker::GeneratorKey(const QString& login, QByteArray& pubkey )
{
    QByteArray securekey (DH_size(m_dh.get()), '\0');
    unsigned char* pkey = reinterpret_cast<unsigned char*> (pubkey.data());
    BIGNUM* pubkeyfrom = BN_bin2bn(pkey,  pubkey.size(), NULL);
    unsigned char* skey = reinterpret_cast<unsigned char*> (securekey.data());
    DH_compute_key(skey, pubkeyfrom, m_dh.get());
    BN_clear_free(pubkeyfrom);
    m_secretkey.insert(login, securekey);
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

QByteArray CryptoWorker::GetKey(const QString &login)
{
    QByteArray skey = m_secretkey.value(login);
    return skey;
}


