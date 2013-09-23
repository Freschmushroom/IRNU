
#include "irnu_rcp.hpp"
#include "../socket/udpsocket.h"
#include "../variables.hpp"
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <vector>

std::vector<sockaddr_in> allowed;
check_login check;
handle_command cmd;
extern int rcp_state = RCP_STATE_NONE;

void set_login_check ( check_login cl ) {
    if ( cl != NULL )
        check = cl;
}

void set_command_handle ( handle_command hc ) {
    if ( hc != NULL )
        cmd = hc;
}

void handle_rcp_request ( base_package bp ) {
    if ( bp.protocol == PROTOCOL_RCP && bp.package == PACKAGE_RCP_REQUEST ) {
        rcp_package_request * req = ( rcp_package_request * ) &bp;
        rcp_package_ack ack;
        ack.ack_package = req->package;
        ack.remote_addr = bp.remote_addr;
        base_package * bp_ = ( base_package * ) & ack;
        *con << ( *bp_ );
        std::cout << "User " << req->u_name << "@" << inet_ntoa ( req->remote_addr.sin_addr ) << " Requested Purrrmission for RCP Session" << std::endl;
        if ( check ( req->u_name, req->pass ) == true ) {
            rcp_package_result res;
            res.err_code = 0;
            res.remote_addr = bp.remote_addr;
            bp_ = ( base_package * ) &res;
            std::cout << "Access Granted" << std::endl;
            allowed.push_back ( req->remote_addr );
            *con << ( *bp_ );
        } else {
            rcp_package_result res;
            res.err_code = 1;
            res.remote_addr = bp.remote_addr;
            bp_ = ( base_package * ) &res;
            std::cout << "Access Denied" << std::endl;
            *con << ( *bp_ );
        }
    }
}

void request_rcp_login ( char * u_name, char * pass, sockaddr_in remote_addr ) {
    if ( rcp_state != RCP_STATE_NONE )
        return;
    base_package bp;
    rcp_package_request * req = ( rcp_package_request * ) &bp;
    req->protocol = PROTOCOL_RCP;
    req->package = PACKAGE_RCP_REQUEST;
    int i;
    for ( i = 0; i < 10; ++i ) {
        if ( u_name[i] == 0 ) {
            req->u_name[i] = 0;
            break;
        }
        req->u_name[i] = u_name[i];
    }
    for ( i = 0; i < 10; ++i ) {
        if ( pass[i] == 0 ) {
            req->pass[i] = 0;
            break;
        }
        req->pass[i] = pass[i];
    }
    req->remote_addr = remote_addr;
    *con << bp;
    rcp_state = RCP_STATE_WAIT_ACK_REQUEST;
}

void handle_rcp_ack ( base_package bp ) {
    if ( bp.protocol == PROTOCOL_RCP && bp.package == PACKAGE_RCP_ACK ) {
        rcp_package_ack * ack = ( rcp_package_ack * ) & bp;
        if ( ack->ack_package == PACKAGE_RCP_REQUEST && rcp_state == RCP_STATE_WAIT_ACK_REQUEST ) {
            rcp_state = RCP_STATE_WAIT_RESULT_REQUEST;
            std::cout << "Received Acknowledgement for Login Request" << std::endl;
        } else if ( ack->ack_package == PACKAGE_RCP_EXIT && rcp_state == RCP_STATE_OK ) {
            base_package bp_;
            rcp_package_ack * ack_ = ( rcp_package_ack * ) &bp_;
            ack_->ack_package = PACKAGE_RCP_ACK;
            ack_->remote_addr = bp.remote_addr;
            ack_->package = PACKAGE_RCP_ACK;
            ack_->protocol = PROTOCOL_RCP;
            *con << bp_;
            rcp_state = RCP_STATE_NONE;
        } else if ( ack->ack_package == PACKAGE_RCP_ACK ) {
            int i;
            for ( i = 0; i < allowed.size(); ++i ) {
                if ( allowed[i].sin_addr.s_addr == ack->remote_addr.sin_addr.s_addr ) {
                    allowed.erase ( allowed.begin() + i );
                }
            }
        } else if ( ack->ack_package == PACKAGE_RCP_COMMAND ) {
            rcp_state = RCP_STATE_WAIT_RESULT_COMMAND;
            std::cout << "Command was Acknowledged" << std::endl;
        } else if ( ack->ack_package == PACKAGE_RCP_RESULT ) {
            std::cout << "Result was Acknowledged" << std::endl;
        }
    }
}

void handle_rcp_result ( base_package bp ) {
    if ( bp.protocol == PROTOCOL_RCP && bp.package == PACKAGE_RCP_RESULT ) {
        rcp_package_result * res = ( rcp_package_result * ) &bp;
        if ( rcp_state == RCP_STATE_WAIT_RESULT_REQUEST ) {
            if ( res->err_code == 0 ) {
                std::cout << "Access granted" << std::endl;
                rcp_state = RCP_STATE_OK;
            } else {
                std::cout << "Authentification failed" << std::endl;
                rcp_state = RCP_STATE_NONE;
            }
            base_package bp_;
            rcp_package_ack * ack = ( rcp_package_ack * ) &bp_;
            ack->ack_package = res->package;
            ack->protocol = PROTOCOL_RCP;
            ack->package = PACKAGE_RCP_ACK;
            ack->remote_addr = res->remote_addr;
            *con << bp_;
        } else if ( rcp_state == RCP_STATE_WAIT_RESULT_COMMAND ) {
            base_package bp_;
            rcp_package_ack * ack = ( rcp_package_ack * ) &bp_;
            ack->ack_package = res->package;
            ack->protocol = PROTOCOL_RCP;
            ack->package = PACKAGE_RCP_ACK;
            ack->remote_addr = res->remote_addr;
            *con << bp_;
        }
    }
}

void handle_rcp_exit ( base_package bp ) {
    if ( bp.protocol == PROTOCOL_RCP && bp.package == PACKAGE_RCP_EXIT ) {
        rcp_package_exit * exit = ( rcp_package_exit * ) &bp;
        std::cout << inet_ntoa ( exit->remote_addr.sin_addr ) << " requested close of connection at " << ( int ) exit->hour << ":" << ( int ) exit->minute << ":" << ( int ) exit->second << " UTC" << std::endl;
        base_package bp_;
        rcp_package_ack * ack = ( rcp_package_ack * ) &bp_;
        ack->remote_addr = exit->remote_addr;
        ack->ack_package = exit->package;
        ack->protocol = PROTOCOL_RCP;
        ack->package = PACKAGE_RCP_ACK;
        *con << bp_;
    }
}

void exit_session ( sockaddr_in remote_addr ) {
    base_package bp;
    rcp_package_exit * exit = ( rcp_package_exit * ) & bp;
    exit->protocol = PROTOCOL_RCP;
    exit->package = PACKAGE_RCP_EXIT;
    exit->remote_addr = remote_addr;
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = gmtime ( &rawtime );

    exit->hour = timeinfo->tm_hour;
    exit->minute = timeinfo->tm_min;
    exit->second = timeinfo->tm_sec;

    *con << bp;
}

unsigned int _strlen ( unsigned char * str ) {
    unsigned int i;
    for ( i = 0; i < 254; ++i ) {
        if ( str[i] == 0 ) {
            return i;
        }
    }
}

void handle_rcp_command ( base_package bp ) {
    if ( bp.protocol == PROTOCOL_RCP && bp.package == PACKAGE_RCP_COMMAND ) {
        bool _allowed = false;
        unsigned int _pos, i;
        for ( i = 0; i < allowed.size(); ++i ) {
            if ( allowed[i].sin_addr.s_addr == bp.remote_addr.sin_addr.s_addr ) {
                _allowed = true;
                break;
            }
        }
        if ( _allowed == false )
            return;
        rcp_package_command * com = ( rcp_package_command * ) &bp;

        _pos = _strlen ( com->data );
        unsigned char cmd_[_pos];
        for ( i = 0; i < _pos; ++i ) {
            cmd_[i] = com->data[i];
        }
        unsigned char * args[com->args];
        unsigned char * pos = com->data;
        pos += _strlen ( com->data );
        for ( i = 0; i < com->args; ++i ) {
            int len = _strlen ( pos ), j;
            unsigned char tmp[len];
            for ( j = 0; j < len; j++ ) {
                tmp[i] = pos[i];
            }
            args[i] = tmp;
        }
        if ( cmd != NULL )
            cmd ( cmd_, args, com->args );
        base_package bp_;
        rcp_package_ack * ack = ( rcp_package_ack * ) &bp_;
        ack->ack_package = com->package;
        ack->protocol = PROTOCOL_RCP;
        ack->package = PACKAGE_RCP_ACK;
        ack->remote_addr = com->remote_addr;
        *con << bp_;
    }
}

void send_command ( unsigned char * cmd, unsigned char ** args, unsigned int arg_count, sockaddr_in remote_addr ) {
    base_package bp;
    rcp_package_command * com = ( rcp_package_command * ) &bp;
    com->args = arg_count;
    com->protocol = PROTOCOL_RCP;
    com->package = PACKAGE_RCP_COMMAND;
    com->remote_addr = remote_addr;
    unsigned int i, _pos;
    _pos = _strlen ( cmd );
    for ( i = 0; i < _pos; ++i ) {
        com->data[i] = cmd[i];
    }
    com->data[_pos] = 0;
    unsigned char * pos = com->data + _pos;
    for ( i = 0; i < arg_count; ++i ) {
        _pos = _strlen ( pos );
        int j;
        if ( pos - com->data + _pos >= 255 )
            break;
        for ( j = 0; j < _pos; ++i ) {
            pos[j] = args[i][j];
        }
        pos[_pos] = 0;
        pos += _pos;
    }
    *con << bp;
    rcp_state = RCP_STATE_WAIT_ACK_COMMAND;
}

void add_rcp_login_handler() {
    addHandler ( handle_rcp_request );
    addHandler ( handle_rcp_ack );
    addHandler ( handle_rcp_result );
    addHandler ( handle_rcp_exit );
    addHandler ( handle_rcp_command );
}
