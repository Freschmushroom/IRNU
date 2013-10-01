
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

void addHandler ( handler h ) {
    l_handler.push_back ( h );
}

void handle ( unsigned char* data, sockaddr_in addr ) {
    base_package bp;
    bp.protocol = data[0];
    bp.package = data[1];
    bp.remote_addr = addr;
    bzero(bp.data, 254);
    int i = 0;
    for ( i = 0; i < 254; ++i ) {
        bp.data[i] = data[i + 2];
    }
    for ( i = 0; i < l_handler.size(); ++i ) {
        l_handler[i] ( bp );
    }
}

void printPackage ( base_package bp ) {
    std::cout << "Received Package from: " << inet_ntoa ( bp.remote_addr.sin_addr ) << std::endl;
    if ( bp.protocol == PROTOCOL_RCP ) {
        std::cout << "Remote Control Protocol" << std::endl;
        print_rcp_package ( bp );
    } else if ( bp.protocol == PROTOCOL_MTP )
        std::cout << "Message Transfer Protocol" << std::endl;
    else if ( bp.protocol == PROTOCOL_FTP )
        std::cout << "File Transfer Protocol" << std::endl;
    else if ( bp.protocol == PROTOCOL_IRTUTP )
        std::cout << "Infinite Reality Tag Utility Transfer Protocol" << std::endl;
    else if ( bp.protocol == PROTOCOL_CCP )
        std::cout << "Connection Control Protocol" << std::endl;
    else if ( bp.protocol == PROTOCOL_KTP )
        std::cout << "Key Transfer Protocol" << std::endl;
    else if ( bp.protocol == PROTOCOL_ECP )
        std::cout << "Encrypted Communication Protocol" << std::endl;
    std::cout << "PackageID: " << ( int ) bp.package << std::endl;
    int i;
    for ( i = 0; i < 254; i++ ) {
        std::cout << ( int ) bp.data[i] << " ";
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
    for ( i = 0; i < 254; ++i ) {
        bp.data[i] = ( char ) i;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons ( PORT );
    if ( inet_aton ( "127.0.0.1", &addr.sin_addr ) == 0 )
        std::cout << "Error: inet_aton()" << std::endl;
    bp.remote_addr = addr;
    *con << bp;
    bp.protocol = PROTOCOL_IRTUTP;
    *con << bp;
    bp.package = PACKAGE_RCP_RESULT;
    bp.protocol = PROTOCOL_RCP;
    bp.data[0] = 0;
    *con << bp;
    request_rcp_login ( "felix", "test", addr );
    sleep ( 1 );
    unsigned char cmd[6];
    cmd[0] = 'h';
    cmd[1] = 'e';
    cmd[2] = 'l';
    cmd[3] = 'l';
    cmd[4] = 'o';
    cmd[5] = 0;
    send_command ( cmd, NULL, 0, addr );
    sleep ( 1 );
    exit_session ( addr );
    sleep ( 1 );
}

bool check_login_test ( char * u_name, char * pass, sockaddr_in ) {
    if ( strcmp ( u_name, "felix" ) == 0 && strcmp ( pass, "test" ) == 0 ) {
        return true;
    }
    return false;
}

bool handle_command_test ( unsigned char * cmd, unsigned char ** args, int arg_count, sockaddr_in remote_addr ) {
    std::cout << "Handling Command: " << cmd << " with " << arg_count << " Args" << std::endl;
    send_result(0, NULL, remote_addr);
}

#ifndef _no_main
/*int main() {
    add_rcp_handler();
    set_login_check ( check_login_test );
    set_command_handle ( handle_command_test );
    test();
    cancel_rcp_timeout();
    return 0;
}*/
#endif