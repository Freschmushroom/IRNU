 
#include <iostream>
#include <vector>
#include "IRNU.hpp"

std::vector<handler> l_handler;

void addHandler(handler h) {
	l_handler.push_back(h);
}

void handle(unsigned char* data) {
	base_package bp;
	bp.protocol = data[0];
	bp.package = data[1];
	bp.data = & data[2];
	int i = 0;
	for(;i < l_handler.size(); ++i) {
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

int main() {
	std::cout << "Hello" << std::endl;
	addHandler(printPackage);
	unsigned char msg[256];
	msg[0] = PROTOCOL_IRTUTP;
	msg[1] = 1;
	handle(msg);
	return 0;
}