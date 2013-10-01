
#define _no_main

#include <iostream>
#include <stdio.h>
#include "../IRNU.hpp"
#include "../rcp/irnu_rcp.hpp"
#include "../socket/udpsocket.h"
#include <src/variables.hpp>
#include <string.h>

using std::cout;
using std::endl;

bool login (char * u_name, char * pass, sockaddr_in remote_address) {
  if(strcmp(u_name, "felix") == 0 && strcmp(pass, "test") == 0)
    return true;
  return false;
}

bool cmd_handler (unsigned char * cmd, unsigned char **, int, sockaddr_in remote_address) {
  unsigned char empty[1];
  empty[0] = 0;
  if(cmd[0] == 'h' && cmd[1] == 'i' && cmd[2] == 0) {
    unsigned char other[3];
    other[0] = 'h';
    other[1] = 'i';
    other[2] = 0;
    send_result(0, other, remote_address);
  } else if (cmd[0] == 'i' && cmd[1] == 'n' && cmd[2] == 'f' && cmd[3] == 0) {
    send_result(0, (unsigned char *) "RCP Version 0.1b, (c) Felix Resch, 2013", remote_address);
  } else if (cmd[0] == 'r' && cmd[1] == 's' && cmd[2] == 0) {
    send_result(0, (unsigned char * )"Server will go down for reboot now! Please relogon!", remote_address);
    cout << endl << "Server will go down for restart now!" << endl << "Client will have to relogin and all transaction will be aborted!" << endl;
    exit(0);
  } else if (cmd[0] == 's' && cmd[1] == 't' && cmd[2] == 'a' && cmd[3] == 't' && cmd[4] == 0) {
    send_result(0, (unsigned char * ) "Server Running normally | No errors", remote_address);
    cout << "User at " << inet_ntoa(remote_address.sin_addr) << " requested Status Report" << endl;
  } else if (strcmp((const char *) cmd, "mods") == 0) {
    send_result(0, (unsigned char * ) "No Modifications installed", remote_address);
  } else {
    send_result(1, (unsigned char *) "No command!", remote_address);
  }
}

int main() {
  try {
  set_command_handle(cmd_handler);
  set_login_check(login);
  add_rcp_handler();
  con->start_connection();
  sleep(1);
  cout << "All services online!" << endl;
  con->join();
  } catch (...) {
   cout << "There was some error!" << endl << "Server will go down for restart now!" << endl << "Client will have to relogin and all transaction will be aborted!" << endl;
   system("./server&");
  }
}