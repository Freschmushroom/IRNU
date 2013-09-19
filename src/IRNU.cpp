 
#include <iostream>
#include <vector>
#include "IRNU.hpp"
#include "rcp/irnu_rcp.hpp"
#include "socket/udpsocket.h"
#include <unistd.h>

std::vector<handler> l_handler;

void addHandler(handler h) {
	l_handler.push_back(h);
}

void handle(unsigned char* data) {
	base_package bp;
	bp.protocol = data[0];
	bp.package = data[1];
	int i = 0;
	for(i = 0; i < 254; ++i) {
	  bp.data[i] = data[i + 2];
	}
	for(i = 0; i < l_handler.size(); ++i) {
		l_handler[i](bp);
	}
}

void printPackage(base_package bp) {
	if(bp.protocol == PROTOCOL_RCP)
		std::cout << "Remote Control Protocol" << std::endl;
	else if (bp.protocol == PROTOCOL_MTP)
		std::cout << "Message Transfer Protocol" << std::endl;
	else if (bp.protocol == PROTOCOL_FTP)
		std::cout << "File Transfer Protocol" << std::endl;
	else if (bp.protocol == PROTOCOL_IRTUTP)
		std::cout << "Infinite Reality Tag Utility Transfer Protocol" << std::endl;
	else if (bp.protocol == PROTOCOL_CCP)
		std::cout << "Connection Control Protocol" << std::endl;
	else if (bp.protocol == PROTOCOL_KTP)
		std::cout << "Key Transfer Protocol" << std::endl;
	else if (bp.protocol == PROTOCOL_ECP)
		std::cout << "Encrypted Communication Protocol" << std::endl;
	std::cout << "PackageID: " << (int) bp.package << std::endl;
	int i;
	for(i = 0; i < 254; i++) {
		std::cout << (int) bp.data[i] << " ";
	}
	std::cout << std::endl;
}

void test() {
  UDPSocket udp;
  udp.start_connection();
  base_package bp;
  bp.protocol = PROTOCOL_RCP;
  bp.package = 1;
  int i;
  //unsigned char buff[254];
  for(i = 0; i < 254; ++i) {
    bp.data[i] = (char)i;
  }
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  if (inet_aton("127.0.0.1", &addr.sin_addr) == 0)
    std::cout << "Error: inet_aton()" << std::endl;
  bp.remote_addr = addr;
  udp << bp;
  bp.protocol = PROTOCOL_IRTUTP;
  udp << bp;
  sleep(3);
  base_package bp_;
  bp_.data[0] = 0x02;
  bp_.protocol = PROTOCOL_RCP;
  bp_.package = PACKAGE_RCP_ACK;
  rcp_package_ack * ack = (rcp_package_ack *) &bp_;
  std::cout << "ACK for pack:" << (int)(ack->ack_package) << std::endl;
}

int main() {
	std::cout << "Hello" << std::endl;
	addHandler(printPackage);
	unsigned char msg[256];
	msg[0] = PROTOCOL_CCP;
	msg[1] = 1;
	handle(msg);
	test();
	return 0;
}