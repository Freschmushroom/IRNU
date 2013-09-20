
#include "irnu_rcp.hpp"

#include <iostream>

void rcp_ack(base_package * bp, rcp_package_ack * ack) {
  ack->package = bp->package;
  ack->protocol = bp->protocol;
  ack->ack_package = bp->data[0];
  int i;
  for(i = 0; i < 253; ++i) {
    ack->data[i] = bp->data[i + 1];
  }
  ack->remote_addr = bp->remote_addr;
}

void rcp_ack(rcp_package_ack * ack, base_package * bp) {
  bp->protocol = ack->protocol;
  bp->package = ack->package;
  bp->data[0] = ack->ack_package;
  int i;
  for(i = 0; i < 253; ++i) {
    bp->data[i + 1] = ack->data[i];
  }
  bp->remote_addr = ack->remote_addr;
}

void rcp_exit(base_package * bp, rcp_package_exit * exit) {
  exit->protocol = bp->protocol;
  exit->package = bp->package;
  exit->hour = bp->data[0];
  exit->minute = bp->data[1];
  exit->second = bp->data[2];
  int i;
  for(i = 0; i < 251; ++i) {
    exit->data[i] = bp->data[i + 3];
  }
  exit->remote_addr = bp->remote_addr;
}

void rcp_exit(rcp_package_exit * exit, base_package * bp) {
  bp->protocol = exit->protocol;
  bp->package = exit->package;
  bp->data[0] = exit->hour;
  bp->data[1] = exit->minute;
  bp->data[2] = exit->second;
  int i;
  for(i = 0; i < 251; ++i) {
    bp->data[i + 3] = exit->data[i];
  }
  bp->remote_addr = exit->remote_addr;
}

void rcp_request(base_package * bp, rcp_package_request * req) {
  req->protocol = bp->protocol;
  req->package = bp->package;
  int i;
  for(i = 0; i < 10; ++i) {
    req->u_name[i] = bp->data[i];
  }
  for(i = 0; i < 10; ++i) {
    req->pass[i] = bp->data[i + 10];
  }
  for(i = 0; i < 234; ++i) {
    req->data[i] = bp->data[i + 20];
  }
  req->remote_addr = bp->remote_addr;
}

void rcp_request(rcp_package_request * req, base_package * bp) {
  bp->protocol = req->protocol;
  bp->package = req->package;
  int i;
  for(i = 0; i < 10; ++i) {
    bp->data[i] = req->u_name[i];
  }
  for(i = 0; i < 10; ++i) {
    bp->data[i + 10] = req->pass[i];
  }
  for(i = 0; i < 234; ++i) {
    bp->data[i + 20] = req->data[i];
  }
  bp->remote_addr = req->remote_addr;
}

void rcp_command(base_package * bp, rcp_package_command * com) {
  com->protocol = bp->protocol;
  com->package = bp->package;
  int i;
  for(i = 0; i < 254; ++i) {
    com->data[i] = bp->data[i];
  }
  com->remote_addr = bp->remote_addr;
}

void rcp_command(rcp_package_command * com, base_package * bp) {
  bp->protocol = com->protocol;
  bp->package = com->package;
  int i;
  for(i = 0; i < 254; ++i) {
    bp->data[i] = com->data[i];
  }
  bp->remote_addr = com->remote_addr;
}

void rcp_result(base_package * bp, rcp_package_result * res) {
  res->protocol = bp->protocol;
  res->package = bp->package;
  res->err_code = bp->data[0];
  int i;
  for(i = 0; i < 253; ++i) {
    res->data[i] = bp->data[i + 1];
  }
  res->remote_addr = bp->remote_addr;
}

void rcp_result(rcp_package_result * res, base_package * bp) {
  bp->protocol = res->protocol;
  bp->package = res->package;
  bp->data[0] = res->err_code;
  int i;
  for(i = 0; i < 253; ++i) {
     bp->data[i + 1] = res->data[i];
  }
  bp->remote_addr = res->remote_addr;
}

void print_rcp_package(base_package bp) {
  if(bp.protocol != PROTOCOL_RCP)
    return;
  if(bp.package == PACKAGE_RCP_ACK) {
    rcp_package_ack * ack = (rcp_package_ack *) & bp;
    std::cout << "Acknoledgement Package" << std::endl << "\tAcknoledged Package: " << (int)ack->ack_package << std::endl;
  } else if (bp.package == PACKAGE_RCP_EXIT) {
    rcp_package_exit * exit = (rcp_package_exit *) & bp;
    std::cout << "Exit Package" << std::endl << "\tHour: " << (int)exit->hour << std::endl << "\tMinute: " << (int)exit->minute << std::endl << "\tSecond: " << (int)exit->second << std::endl;
  } else if (bp.package == PACKAGE_RCP_REQUEST) {
    rcp_package_request * req = (rcp_package_request * ) & bp;
    std::cout << "Request Package" << std::endl << "\tUsername: " << req->u_name << std::endl << "\tPassword: " << req->pass << std::endl;
  } else if (bp.package == PACKAGE_RCP_COMMAND) {
    rcp_package_command * com = (rcp_package_command * ) & bp;
    std::cout << "Command Package" << std::endl << "\tData: " << com->data << std::endl;
  } else if (bp.package == PACKAGE_RCP_RESULT) {
    rcp_package_result * res = (rcp_package_result * ) & bp;
    std::cout << "Result Package" << std::endl << "\tError Code: " << (int)res->err_code << std::endl << "\tOther Data: " << res->data << std::endl;
  }
}

void add_rcp_handler() {
  add_rcp_login_handler();
}