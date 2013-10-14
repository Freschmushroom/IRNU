
#ifndef IRNU_RCP_
#define IRNU_RCP_

#include <netinet/in.h>
#include "../IRNU.hpp"

const unsigned char PACKAGE_RCP_ACK = 0x01;
const unsigned char PACKAGE_RCP_EXIT = 0x02;
const unsigned char PACKAGE_RCP_REQUEST = 0x03;
const unsigned char PACKAGE_RCP_COMMAND = 0x04;
const unsigned char PACKAGE_RCP_RESULT = 0x05;

const unsigned char RCP_STATE_NONE = 0x00;
const unsigned char RCP_STATE_OK = 0x01;
const unsigned char RCP_STATE_WAIT_ACK_REQUEST = 0x02;
const unsigned char RCP_STATE_WAIT_RESULT_REQUEST = 0x03;
const unsigned char RCP_STATE_WAIT_ACK_EXIT = 0x04;
const unsigned char RCP_STATE_WAIT_ACK_COMMAND = 0x05;
const unsigned char RCP_STATE_WAIT_RESULT_COMMAND = 0x06;
const unsigned char RCP_STATE_WAIT_ACK_RESULT = 0x07;


typedef struct rcp_package_ack_struct {
    unsigned char protocol = PROTOCOL_RCP;
    unsigned char package = PACKAGE_RCP_ACK;
    unsigned char ack_package;
    unsigned char data[253];
    struct sockaddr_in remote_addr;
} rcp_package_ack;

typedef struct rcp_package_exit_struct {
    unsigned char protocol = PROTOCOL_RCP;
    unsigned char package = PACKAGE_RCP_EXIT;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
    unsigned char data[251];
    struct sockaddr_in remote_addr;
} rcp_package_exit;

typedef struct rcp_package_request_struct {
    unsigned char protocol = PROTOCOL_RCP;
    unsigned char package = PACKAGE_RCP_REQUEST;
    char u_name[10];
    char pass[10];
    unsigned char data[234];
    struct sockaddr_in remote_addr;
} rcp_package_request;

typedef struct rcp_package_command_struct {
    unsigned char protocol = PROTOCOL_RCP;
    unsigned char package = PACKAGE_RCP_COMMAND;
    unsigned char args;
    unsigned char data[253];
    struct sockaddr_in remote_addr;
} rcp_package_command;

typedef struct rcp_package_result_struct {
    unsigned char protocol = PROTOCOL_RCP;
    unsigned char package = PACKAGE_RCP_RESULT;
    unsigned char err_code;
    unsigned char data[253];
    struct sockaddr_in remote_addr;
} rcp_package_result;

void rcp_ack ( base_package *, rcp_package_ack * );	//Convert base_package to rcp_package_ack
void rcp_exit ( base_package *, rcp_package_exit * );	//Convert base_package to rcp_package_exit
void rcp_request ( base_package *, rcp_package_request * ); //Convert base_package to rcp_package_request
void rcp_command ( base_package *, rcp_package_command * ); //Convert base_package to rcp_package_command
void rcp_result ( base_package *, rcp_package_result * );	//Convert base_package to rcp_package_result

void rcp_ack ( rcp_package_ack *, base_package * );	//Convert rcp_package_ack to base_package
void rcp_exit ( rcp_package_exit *, base_package * );	//Convert rcp_package_exit to base_package
void rcp_request ( rcp_package_request *, base_package * ); //Convert rcp_package_request to base_package
void rcp_command ( rcp_package_command *, base_package * ); //Convert rcp_package_command to base_package
void rcp_result ( rcp_package_result *, base_package * );	//Convert rcp_package_result to base_package

void print_rcp_package ( base_package );

typedef bool ( * check_login ) ( char *, char *, sockaddr_in );
typedef bool ( * handle_command ) ( unsigned char *, unsigned char **, int, sockaddr_in );
typedef void ( * server_shutdown ) ( sockaddr_in );
typedef void ( * client_end_connection ) ();

void set_login_check ( check_login );
void set_command_handle ( handle_command );
void set_server_shutdown_handle ( server_shutdown );
void set_client_end_connection ( client_end_connection );
void add_rcp_login_handler();
void add_rcp_handler();

void request_rcp_login ( char *, char *, sockaddr_in );
void exit_session ( sockaddr_in );
void send_command ( unsigned char *, unsigned char **, unsigned int, sockaddr_in );
void send_result ( unsigned char, unsigned char *, sockaddr_in );
void cancel_rcp_timeout();

void shutdown_server(sockaddr_in);

bool is_rcp_waiting();
unsigned char get_rcp_state();
#endif
