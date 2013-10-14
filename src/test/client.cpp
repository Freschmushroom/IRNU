#define _no_main

#include <iostream>
#include <stdio.h>
#include "../IRNU.hpp"
#include "../rcp/irnu_rcp.hpp"
#include "../socket/udpsocket.h"
#include <src/variables.hpp>
#include <string.h>
#include <string>

using std::cout;
using std::endl;
using std::cin;
using std::string;

void shutdown() {
  cout << endl << "Server requested shutdown!" << endl;
  cout << "Please restart application!" << endl;
  cout << "Bye!" << endl;;
  exit(0);
}

int main() {
    int count;
    add_rcp_handler();
    set_client_end_connection(shutdown);
    con->start_connection();
    sleep ( 1 );
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons ( 9980 );
    if ( inet_aton ( "127.0.0.1", &addr.sin_addr ) == 0 )
        std::cout << "Error: inet_aton()" << std::endl;
    cout << "Starting Client Module" << endl;
    cout << "Please enter username: ";
    char u_name[10], pass[10];
    bzero(u_name, 10);
    bzero(pass, 10);
    cin >> u_name;
    cout << "Please enter password: ";
    cin >> pass;
    request_rcp_login (u_name, pass, addr );
    while ( is_rcp_waiting() == true ) {
        sleep ( 1 );
        if ( count == 10 ) break;
        count++;
        cout << "Connection timed out!" << endl;
    }
    if ( get_rcp_state() == RCP_STATE_OK ) {
        cout << "Login successfull!" << endl;
        cout << endl << "You can now use the Remote Control Protcol Shell" << endl << endl;
        while ( 1 ) {
            unsigned char buff[254];
	    bzero(buff, 254);
            cout << "\t> ";
            cin >> buff;
	    if(buff[0] == 'x') {
	      exit_session(addr);
	    } else {
	      send_command ( buff, NULL, 0, addr );
	    }
            while ( get_rcp_state() != RCP_STATE_OK ) {}
        }
    } else {
        cout << "Login failed!" << endl;
        cout << "Exiting program!" << endl;
    }
}
