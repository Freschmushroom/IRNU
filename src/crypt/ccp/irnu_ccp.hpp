
#include "../../IRNU.hpp"

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

void encrypt(ccp_package, byte[]); 