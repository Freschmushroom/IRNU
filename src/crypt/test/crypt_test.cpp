
#include <iostream>
#include <string.h>

#include <cryptopp/modes.h>
#include <cryptopp/aes.h>

using namespace CryptoPP;



int main() {
    byte key[AES::DEFAULT_KEYLENGTH], iv[AES::BLOCKSIZE];
    AES::Encryption aesEncryption(key, AES::DEFAULT_KEYLENGTH);
    CFB_Mode_ExternalCipher::Encryption cfbEncryption(aesEncryption, iv);
    byte plaintext[100], ciphertext[100];
    char * text = "Hello, this is text. I want to tell you I finished!";
    int i;
    for(i = 0; i < 100; ++i) {
        plaintext[i] = i + 1;
    }
    for(i = 0; i < strlen(text); ++i) {
      plaintext[i] = text[i];
    }
    std::cout << "Encrypted Data:" << std::endl;
    cfbEncryption.ProcessData(ciphertext, plaintext, 100);
    for(i = 0; i < 100; ++i) {
        std::cout << ciphertext[i] << " ";
    }
    CFB_Mode<AES>::Decryption cfbDecryption(key, 16, iv);
    std::cout << std::endl << "Decrypted Data:" << std::endl;
    cfbDecryption.ProcessData(plaintext, ciphertext, 100);
    for(i = 0; i < 100; ++i) {
        std::cout << plaintext[i] << " ";
    }
    std::cout << std::endl;
}
