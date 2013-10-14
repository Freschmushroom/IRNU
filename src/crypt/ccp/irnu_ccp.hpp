
#include "../../IRNU.hpp"
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>

#ifndef ccp__
#define ccp__

#define ENC_PORT 9999
const unsigned char PACKAGE_CCP = 0x0F;

typedef struct ccp_package_struct {
  unsigned char protocol = PROTOCOL_CCP;
  unsigned char package = PACKAGE_CCP;
  unsigned long id;
  unsigned long checksum;
  unsigned char placeholder[6];
  unsigned char blocks[15][16];
  struct sockaddr_in remote_addr;
} ccp_package;

void encrypt(ccp_package*, byte*); 
void decrypt(ccp_package*, byte*);

typedef void ( * enc_handler) (ccp_package);

#endif