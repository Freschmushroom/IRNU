
#include <iostream>
#include <string.h>

#include <cryptopp/modes.h>
#include <cryptopp/aes.h>

#include "../ccp/irnu_ccp.hpp"
#include "../encsocket.h"

using namespace CryptoPP;

void enc_handle(ccp_package ccp) {
  std::cout << "Received Message from " << inet_ntoa(ccp.remote_addr.sin_addr) << ": ";
  unsigned char * pos = ccp.blocks[0];
  int i;
  for(i = 0; i < strlen((char *) pos); ++i) {
    std::cout << (char) pos[i];
  }
  std::cout << std::endl;
}

int main() {
    byte key[AES::DEFAULT_KEYLENGTH];
    /*ccp_package ccp;
    ccp.package = PACKAGE_CCP;
    ccp.protocol = 0x05;
    ccp.id = 0;
    ccp.checksum = 0;
    char * text = "Hello World!";
    unsigned char * pos = ccp.blocks[0] + 16;
    int i, j;
    for(i = 0; i <  strlen(text); ++i) {
      pos[i] = (unsigned char) text[i];
    }
    for(i = 0; i < 15; ++i) {
      std::cout << i + 1 << ":\t";
      for(j = 0; j < 16; ++j) {
	std::cout << (int) ccp.blocks[i][j] << "\t";
      }
      std::cout << std::endl;
    }
    encrypt(&ccp, key);
    std::cout << "Encrypted Data:" << std::endl;
    for(i = 0; i < 15; ++i) {
      std::cout << i + 1 << ":\t";
      for(j = 0; j < 16; ++j) {
	std::cout << (int) ccp.blocks[i][j] << "\t";
      }
      std::cout << std::endl;
    }
    decrypt(&ccp, key);
    std::cout << std::endl << "Decrypted Data:" << std::endl;
    for(i = 0; i < 15; ++i) {
      std::cout << i + 1 << ":\t";
      for(j = 0; j < 16; ++j) {
	std::cout << (int) ccp.blocks[i][j] << "\t";
      }
      std::cout << std::endl;
    }*/
    encsocket enc;
    enc += key;
    std::cout << "Starting Connection" << std::endl;
    enc.start_connection();
    std::cout << "Activating Handler" << std::endl;
    sleep(2);
    enc.add_enc_handler(enc_handle);
    std::cout << "Sending Message" << std::endl;
    enc << "Hello World!";
}
