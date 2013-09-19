
#ifndef IRNU_RCP_
#define IRNU_RCP_

#include <netinet/in.h>
#include "../IRNU.hpp"

const unsigned char PACKAGE_RCP_ACK = 0x01;
const unsigned char PACKAGE_RCP_EXIT = 0x02;
const unsigned char PACKAGE_RCP_REQUEST = 0x03;
const unsigned char PACKAGE_RCP_COMMAND = 0x04;
const unsigned char PACKAGE_RCP_RESULT = 0x05;

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
  unsigned char data[254];
  struct sockaddr_in remote_addr;
} rcp_package_command;

typedef struct rcp_package_result_struct {
  unsigned char protocol = PROTOCOL_RCP;
  unsigned char package = PACKAGE_RCP_RESULT;
  unsigned char err_code;
  unsigned char data[253];
  struct sockaddr_in remote_addr;
} rcp_package_result;

void rcp_ack(base_package *, rcp_package_ack *);
void rcp_exit(base_package *, rcp_package_exit *);
void rcp_request(base_package *, rcp_package_request *);
void rcp_command(base_package *, rcp_package_command *);
void rcp_result(base_package *, rcp_package_result *);

void rcp_ack(rcp_package_ack *, base_package *);
void rcp_exit(rcp_package_exit *, base_package *);
void rcp_request(rcp_package_request *, base_package *);
void rcp_command(rcp_package_command *, base_package *);
void rcp_result(rcp_package_result *, base_package *);

#endif