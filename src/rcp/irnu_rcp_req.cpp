
#include "irnu_rcp.hpp"
#include "../socket/udpsocket.h"
#include "../variables.hpp"
#include <iostream>
#include <time.h>
#include <stdio.h>

check_login check;
extern int state = RCP_STATE_NONE;

void set_login_check ( check_login cl ) {
    if ( cl != NULL )
        check = cl;
}

void handle_rcp_request( base_package bp ) {
    if ( bp.protocol == PROTOCOL_RCP && bp.package == PACKAGE_RCP_REQUEST ) {
        rcp_package_request * req = ( rcp_package_request * ) &bp;
        rcp_package_ack ack;
        ack.ack_package = req->package;
        ack.remote_addr = bp.remote_addr;
        base_package * bp_ = ( base_package * ) & ack;
        *con << ( *bp_ );
        if ( check ( req->u_name, req->pass ) == true ) {
            rcp_package_result res;
            res.err_code = 0;
            res.remote_addr = bp.remote_addr;
            bp_ = ( base_package * ) &res;
            *con << ( *bp_ );
        } else {
            rcp_package_result res;
            res.err_code = 1;
            res.remote_addr = bp.remote_addr;
            bp_ = ( base_package * ) &res;
	    *con << ( *bp_ );
        }
    }
}

void request_rcp_login(char * u_name, char * pass, sockaddr_in remote_addr) {
  if (state != RCP_STATE_NONE)
    return;
  base_package bp;
  rcp_package_request * req = (rcp_package_request *) &bp;
  req->protocol = PROTOCOL_RCP;
  req->package = PACKAGE_RCP_REQUEST;
  int i;
  for(i = 0; i < 10; ++i) {
    if(u_name[i] == 0) {
      req->u_name[i] = 0;
      break;
    }
    req->u_name[i] = u_name[i];
  }
  for(i = 0; i < 10; ++i) {
    if(pass[i] == 0) {
      req->pass[i] = 0;
      break;
    }
    req->pass[i] = pass[i];
  }
  req->remote_addr = remote_addr;
  *con << bp;
  state = RCP_STATE_WAIT_ACK_REQUEST;
}

void handle_rcp_ack(base_package bp) {
  if(bp.protocol == PROTOCOL_RCP && bp.package == PACKAGE_RCP_ACK) {
    rcp_package_ack * ack = (rcp_package_ack * ) & bp;
    if(ack->ack_package == PACKAGE_RCP_REQUEST && state == RCP_STATE_WAIT_ACK_REQUEST) {
      state = RCP_STATE_WAIT_RESULT_REQUEST;
      std::cout << "Received Acknowledgement for Login Request" << std::endl;
    } else if (ack->ack_package == PACKAGE_RCP_EXIT && state == RCP_STATE_OK) {
      base_package bp_;
      rcp_package_ack * ack_ = (rcp_package_ack *) &bp_;
      ack_->ack_package = PACKAGE_RCP_ACK;
      ack_->remote_addr = bp.remote_addr;
      ack_->package = PACKAGE_RCP_ACK;
      ack_->protocol = PROTOCOL_RCP;
      *con << bp_;
      state = RCP_STATE_NONE;
    } else if (ack->ack_package == PACKAGE_RCP_ACK) {
      //TODO Remove authentificated address from list
    }
  }
}

void handle_rcp_result(base_package bp) {
  if(bp.protocol == PROTOCOL_RCP && bp.package == PACKAGE_RCP_RESULT) {
    rcp_package_result * res = (rcp_package_result *) &bp;
    if(state == RCP_STATE_WAIT_RESULT_REQUEST) {
      if(res->err_code == 0) {
	std::cout << "Access granted" << std::endl;
	state = RCP_STATE_OK;
      } else {
	std::cout << "Authentification failed" << std::endl;
	state = RCP_STATE_NONE;
      }
    }
  }
}

void handle_rcp_exit(base_package bp) {
  if(bp.protocol == PROTOCOL_RCP && bp.package == PACKAGE_RCP_EXIT) {
    rcp_package_exit * exit = (rcp_package_exit *) &bp;
    std::cout << inet_ntoa(exit->remote_addr.sin_addr) << " requested close of connection at " << (int)exit->hour << ":" << (int)exit->minute << ":" << (int)exit->second << " UTC" << std::endl;
    base_package bp_;
    rcp_package_ack * ack = (rcp_package_ack * ) &bp_;
    ack->remote_addr = exit->remote_addr;
    ack->ack_package = exit->package;
    ack->protocol = PROTOCOL_RCP;
    ack->package = PACKAGE_RCP_ACK;
    *con << bp_;
  }
}

void exit_session(sockaddr_in remote_addr) {
  base_package bp;
  rcp_package_exit * exit = (rcp_package_exit * ) & bp;
  exit->protocol = PROTOCOL_RCP;
  exit->package = PACKAGE_RCP_EXIT;
  exit->remote_addr = remote_addr;
  time_t rawtime;
  struct tm * timeinfo;
  
  time(&rawtime);
  timeinfo = gmtime(&rawtime);
  
  exit->hour = timeinfo->tm_hour;
  exit->minute = timeinfo->tm_min;
  exit->second = timeinfo->tm_sec;
  
  *con << bp;
}

void add_rcp_login_handler() {
    addHandler ( handle_rcp_request );
    addHandler( handle_rcp_ack );
    addHandler( handle_rcp_result );
    addHandler( handle_rcp_exit );
}
