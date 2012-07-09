#include "crypto.h"

// keyString ��һ����Կ�����뱣֤����Ҫ���� 16
// block ��Ҫ��������ݣ�����������Ҳͬʱ����� block ����뱣֤���ĳ���Ϊ 8 ��������
// length �� block �ĳ��ȣ����뱣֤��Ϊ 8 ��������
// direction �Ǳ�ʾ�Ƿ��Ǽ��ܻ��ǽ��ܣ����Ǽ��ܣ����� CryptoPP::ENCRYPTION, ������ CryptoPP::DECRYPTION
void DES_Process(const char *keyString, byte *block, size_t length, CryptoPP::CipherDir direction){
    using namespace CryptoPP;

    byte key[DES_EDE2::KEYLENGTH];
    memcpy(key, keyString, DES_EDE2::KEYLENGTH);
    BlockTransformation *t = NULL;

    if(direction == ENCRYPTION)
        t = new DES_EDE2_Encryption(key, DES_EDE2::KEYLENGTH);
    else
        t = new DES_EDE2_Decryption(key, DES_EDE2::KEYLENGTH);

    int steps = length / t->BlockSize();
    for(int i=0; i<steps; i++){
        int offset = i * t->BlockSize();
        t->ProcessBlock(block + offset);
    }

    delete t;
}
/*
int maintest(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    byte block[1024] = "++++++++--------********12345678";

    const char *key = "http://qsanguosha.org/forum";

    printf("original text: %s\n", block);

    DES_Process(key, block, 16, CryptoPP::ENCRYPTION);

    printf("Encrypt: %s\n", block);

    DES_Process(key, block, 16, CryptoPP::DECRYPTION);

    printf("Decrypt: %s\n", block);

    return a.exec();
}
*/
char *Crypto::doCrypto(CryType type, const QString &input, const QString &output, const char *key){
    QFile file(input);
    if(file.open(QIODevice::ReadOnly)){
        QByteArray data = file.readAll();

        int oldSize = data.size();
        int remainder = oldSize % 8;

        char *buffer = data.data();
        if(remainder != 0){
            int padding = 8 - remainder;
            data.resize(data.size() + padding);
            buffer = data.data();
            memset(buffer + oldSize, 0, padding);
        }

        DES_Process(key, (byte *)buffer, data.size(),
                    type == Crypto::Jiami ? CryptoPP::ENCRYPTION : CryptoPP::DECRYPTION);

        if(output == "none")
            return buffer;
        else{
            QFile outFile(output == "default" ? input : output);
            outFile.open(QIODevice::WriteOnly);
            outFile.write(buffer, data.size());
            outFile.close();
            return buffer;
        }
    }
}
