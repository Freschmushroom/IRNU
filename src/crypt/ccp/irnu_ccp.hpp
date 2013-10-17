

#ifndef ccp__
#define ccp__

#include "../../IRNU.hpp"
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/md5.h>

#define ENC_PORT 9999
const unsigned char PACKAGE_CCP = 0x0F;
const unsigned char PACKAGE_CCP_ACK = 0x01;

typedef struct ccp_package_struct {
  unsigned char protocol = PROTOCOL_CCP;
  unsigned char package = PACKAGE_CCP;
  unsigned char session;
  unsigned char id;
  unsigned char checksum[16];
  unsigned char retries = 0;
  unsigned char placeholder[11];
  unsigned char blocks[14][16];
  struct sockaddr_in remote_addr;
} ccp_package;

typedef struct ccp_package_ack_struct {
  unsigned char protocol = PROTOCOL_CCP;
  unsigned char package = PACKAGE_CCP_ACK;
  unsigned char session;
  unsigned char id;
  unsigned char checksum[16];
  unsigned char retries = 0;
  unsigned char placeholder[11];
  unsigned char ack_session;
  unsigned char ack_id;
  unsigned char data[222];
  struct sockaddr_in remote_addr;
} ccp_ack_package;

void encrypt(ccp_package*, byte*); 
void decrypt(ccp_package*, byte*);
byte * get_checksum(unsigned char *);

typedef void ( * enc_handler) (ccp_package, void *);

void add_ccp_handlers(void *);

#endif
