
#include <iostream>
#include <string.h>

#include <cryptopp/modes.h>
#include <cryptopp/aes.h>

#include "../ccp/irnu_ccp.hpp"
#include "../encsocket.h"

using namespace CryptoPP;

void enc_handle(ccp_package ccp, void *) {
  unsigned char * pos = ccp.blocks[0] + 16;
  int i;
  std::cout << "Received Message: " << (int) ccp.session << "_" << (int) ccp.id << " with checksum: " << std::endl;
  for(i = 0; i < 16; ++i) {
    std::cout << (int) ccp.checksum[i] << "\t";
  }
  std::cout << std::endl; 
  for(i = 0; i < strlen((char *) pos); ++i) {
    std::cout << (char) pos[i];
  }
  std::cout << std::endl;
}

int main() {
    byte key[AES::DEFAULT_KEYLENGTH];
    struct sockaddr_in addr = *(new sockaddr_in);
    addr.sin_family = AF_INET;
    addr.sin_port = htons ( 9999 );
    if ( inet_aton ( "127.0.0.1", &addr.sin_addr ) == 0 )
        std::cout << "Error: inet_aton()" << std::endl;
    encsocket * enc = new encsocket;
    add_ccp_handlers(enc);
    *enc += key;
    std::cout << "Starting Connection" << std::endl;
    enc->start_connection();
    sleep(1);
    std::cout << "Activating Handler" << std::endl;
    sleep(2);
    enc->add_enc_handler(enc_handle);
    std::cout << "Sending Message" << std::endl;
    ccp_package * ccp = new ccp_package;
    ccp->remote_addr = addr;
    byte * pos = ccp->blocks[0] + 16;
    char * text = "Hello World!";
    int i;
    for(i = 0; i < strlen(text); ++i) {
      pos[i] = text[i];
    }
    *enc << *ccp;
    text = "This is a encrypted Test if you can read this you are either the receiver or this api haz some bug...";
    for(i = 0; i < strlen(text); ++i) {
      pos[i] = text[i];
    }
    (*enc) << *ccp;
    sleep(3);
}
