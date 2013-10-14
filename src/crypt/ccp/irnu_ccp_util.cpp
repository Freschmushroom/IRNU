
#include "../../IRNU.hpp"
#include "irnu_ccp.hpp"
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>

using namespace CryptoPP;

void encrypt(ccp_package * pack, byte * key) {
  byte iv[AES::BLOCKSIZE];
  unsigned char tmp[16];
  int i, j;
  AES::Encryption aesEncryption(key, AES::DEFAULT_KEYLENGTH);
  CFB_Mode_ExternalCipher::Encryption cfbEncryption(aesEncryption, iv);
  for(i = 0; i < 15; ++i) {
    cfbEncryption.ProcessData(tmp, pack->blocks[i], 16);
    for(j = 0; j < 16; ++j) {
      pack->blocks[i][j] = tmp[j];
    }
  }
}

void decrypt(ccp_package * pack, byte * key) {
  byte iv[AES::BLOCKSIZE];
  int i, j;
  unsigned char tmp[16];
  CFB_Mode<AES>::Decryption cfbDecryption(key, 16, iv);
  for(i = 0; i < 15; ++i) {
    cfbDecryption.ProcessData(tmp, pack->blocks[i], 16);
    for(j = 0; j < 16; ++j) {
      pack->blocks[i][j] = tmp[j];
    }
  }
}
