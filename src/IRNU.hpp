
#ifndef IRNU_
#define IRNU_
#include <netinet/in.h>

const unsigned char PROTOCOL_RCP = 0x01;
const unsigned char PROTOCOL_MTP = 0x02;
const unsigned char PROTOCOL_FTP = 0x03;
const unsigned char PROTOCOL_IRTUTP = 0x04;
const unsigned char PROTOCOL_CCP = 0x05;
const unsigned char PROTOCOL_KTP = 0x06;
const unsigned char PROTOCOL_ECP = 0x07;

typedef struct base_package_struct {
	unsigned char protocol;
	unsigned char package;
	unsigned char * data;
	struct sockaddr_in remote_addr;
} base_package;

typedef void (* handler)(base_package);

void addHandler(handler);

void handle(unsigned char[]);
#endif