 
#include <iostream>
#include <vector>
#include "IRNU.hpp"
#include "rcp/irnu_rcp.hpp"
#include "socket/udpsocket.h"
#include <unistd.h>
#include "variables.hpp"
#include <string.h>

std::vector<handler> l_handler;
extern UDPSocket * con = new UDPSocket;

void addHandler(handler h) {
	l_handler.push_back(h);
}

void handle(unsigned char* data, sockaddr_in addr) {
	base_package bp;
	bp.protocol = data[0];
	bp.package = data[1];
	bp.remote_addr = addr;
	int i = 0;
	for(i = 0; i < 254; ++i) {
	  bp.data[i] = data[i + 2];
	}
	for(i = 0; i < l_handler.size(); ++i) {
		l_handler[i](bp);
	}
}

void printPackage(base_package bp) {
	std::cout << "Received Package from: " << inet_ntoa(bp.remote_addr.sin_addr) << std::endl;
	if(bp.protocol == PROTOCOL_RCP) {
		std::cout << "Remote Control Protocol" << std::endl;
		print_rcp_package(bp);
	}
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
  con->start_connection();
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
  *con << bp;
  bp.protocol = PROTOCOL_IRTUTP;
  *con << bp;
  bp.package = PACKAGE_RCP_RESULT;
  bp.protocol = PROTOCOL_RCP;
  bp.data[0] = 0;
  *con << bp;
  /*rcp_package_request * req = (rcp_package_request *) &bp;
  req->u_name[0] = 'f';
  req->u_name[1] = 'e';
  req->u_name[2] = 'l';
  req->u_name[3] = 'i';
  req->u_name[4] = 'x';
  req->u_name[5] = 0;
  req->pass[0] = 't';
  req->pass[1] = 'e';
  req->pass[2] = 's';
  req->pass[3] = 't';
  req->pass[4] = 0;
  req->package = PACKAGE_RCP_REQUEST;
  *con << bp;*/
  request_rcp_login("felix", "test", addr);
  sleep(3);
  exit_session(addr);
  sleep(2);
}

bool check_login_test(char * u_name, char * pass) {
  if(strcmp(u_name, "felix") == 0 && strcmp(pass, "test") == 0) {
    return true;
  }
  return false;
}

int main() {
	std::cout << "Hello" << std::endl;
	addHandler(printPackage);
	add_rcp_handler();
	set_login_check(check_login_test);
	test();
	return 0;
}
