
#include "irnu_rcp.hpp"
#include "../socket/udpsocket.h"
#include "../variables.hpp"
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <pthread.h>

std::vector<sockaddr_in> allowed;
check_login check;
handle_command cmd;
extern int rcp_state = RCP_STATE_NONE;
bool rcp_waiting = false;
bool end = false;
base_package * wait_pack = NULL;
pthread_t rcp_timeout;
int count;

void set_login_check ( check_login cl ) {
    if ( cl != NULL )
        check = cl;
}

void set_command_handle ( handle_command hc ) {
    if ( hc != NULL )
        cmd = hc;
}

void * handle_timeout ( void * args ) {
    //std::cout << "Started new RCP Timeout Thread: " << rcp_timeout << std::endl;
    count == true;
    while ( rcp_waiting == true ) {
        sleep ( 3 );
        if ( rcp_waiting == true )
            *con << *wait_pack;
        else {
            //std::cout << "Canceled RCP Timeout Thread: " << rcp_timeout << std::endl;
            if ( wait_pack != NULL ) {
                try {
                    //delete wait_pack;
                } catch ( ... ) {

                }
                cancel_rcp_timeout();
            }
        }
        if ( count == 10 ) {
            std::cout << "Operation timed out, please retry" << std::endl;
            cancel_rcp_timeout();
        }
        count++;
    }
}

void start_rcp_timeout() {
    pthread_create ( &rcp_timeout, NULL, handle_timeout, NULL );
}

void cancel_rcp_timeout() {
    //std::cout << "Canceling all RCP Timeout threads" << std::endl;
    pthread_cancel ( rcp_timeout );
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
        if ( check ( req->u_name, req->pass, req->remote_addr ) == true ) {
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
    if ( rcp_waiting == true )
        pthread_join ( rcp_timeout, NULL );
    if ( rcp_state != RCP_STATE_NONE )
        return;
    base_package bp = * ( new base_package );
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
    wait_pack = &bp;
    rcp_waiting = true;
    start_rcp_timeout();
}

void handle_rcp_ack ( base_package bp ) {
    if ( bp.protocol == PROTOCOL_RCP && bp.package == PACKAGE_RCP_ACK ) {
        rcp_package_ack * ack = ( rcp_package_ack * ) & bp;
        //std::cout << "Received ACK Package for Package ID: " << ( int ) ack->ack_package << std::endl;
        if ( ack->ack_package == PACKAGE_RCP_REQUEST && rcp_state == RCP_STATE_WAIT_ACK_REQUEST ) {
            rcp_state = RCP_STATE_WAIT_RESULT_REQUEST;
            //std::cout << "Received Acknowledgement for Login Request" << std::endl;
            rcp_waiting = false;
        } else if ( ack->ack_package == PACKAGE_RCP_EXIT ) {
            //std::cout << "Received ACK for Exit Package" << std::endl;
            base_package bp_;
            rcp_package_ack * ack_ = ( rcp_package_ack * ) &bp_;
            ack_->ack_package = PACKAGE_RCP_ACK;
            ack_->remote_addr = bp.remote_addr;
            ack_->package = PACKAGE_RCP_ACK;
            ack_->protocol = PROTOCOL_RCP;
	    //std::cout << "Sending ACK_ACK" << std::endl;
            *con << bp_;
            rcp_state = RCP_STATE_NONE;
            rcp_waiting = false;
	    std::cout << std::endl;
            exit(0);
        } else if ( ack->ack_package == PACKAGE_RCP_ACK ) {
            //std::cout << "Received ACK for ACK Package" << std::endl;
            int i;
            for ( i = 0; i < allowed.size(); ++i ) {
                if ( allowed[i].sin_addr.s_addr == ack->remote_addr.sin_addr.s_addr ) {
                    allowed.erase ( allowed.begin() + i );
                }
            }
            rcp_waiting = false;
        } else if ( ack->ack_package == PACKAGE_RCP_COMMAND ) {
            rcp_state = RCP_STATE_WAIT_RESULT_COMMAND;
            //std::cout << "Command was Acknowledged" << std::endl;
            rcp_waiting = false;
        } else if ( ack->ack_package == PACKAGE_RCP_RESULT ) {
            //std::cout << "Result was Acknowledged" << std::endl;
            if ( end == true ) {
                std::cout << "Shuting down Server!" << std::endl;
                exit ( 0 );
            }
            rcp_waiting = false;
        }
    }
}

void handle_rcp_result ( base_package bp ) {
    if ( bp.protocol == PROTOCOL_RCP && bp.package == PACKAGE_RCP_RESULT ) {
        rcp_package_result * res = ( rcp_package_result * ) &bp;
        if ( rcp_state == RCP_STATE_WAIT_RESULT_REQUEST ) {
            if ( res->err_code == 0 ) {
                std::cout << "[Access Granted]" << std::endl;
                rcp_state = RCP_STATE_OK;
            } else {
                std::cout << "[Access Denied]" << std::endl;
                rcp_state = RCP_STATE_NONE;
            }
            base_package bp_;
            rcp_package_ack * ack = ( rcp_package_ack * ) &bp_;
            ack->ack_package = res->package;
            ack->protocol = PROTOCOL_RCP;
            ack->package = PACKAGE_RCP_ACK;
            ack->remote_addr = res->remote_addr;
            *con << bp_;
            rcp_waiting = false;
        } else if ( rcp_state == RCP_STATE_WAIT_RESULT_COMMAND ) {
            base_package bp_;
            rcp_package_ack * ack = ( rcp_package_ack * ) &bp_;
            ack->ack_package = res->package;
            ack->protocol = PROTOCOL_RCP;
            ack->package = PACKAGE_RCP_ACK;
            ack->remote_addr = res->remote_addr;
            *con << bp_;
            std::cout << "Remote Command Exited with error code: " << ( int ) res->err_code << std::endl << "\tAdditional Information: " << res->data << std::endl;
            rcp_waiting = false;
            rcp_state = RCP_STATE_OK;
        }
    }
}

void handle_rcp_exit ( base_package bp ) {
    if ( bp.protocol == PROTOCOL_RCP && bp.package == PACKAGE_RCP_EXIT ) {
        rcp_package_exit * exit = ( rcp_package_exit * ) &bp;
        std::cout << inet_ntoa ( exit->remote_addr.sin_addr ) << " requested close of connection at " << ( int ) exit->hour << ":" << ( int ) exit->minute << ":" << ( int ) exit->second << " UTC" << std::endl;
        base_package * bp_ = new base_package;
        rcp_package_ack * ack = ( rcp_package_ack * ) bp_;
        ack->remote_addr = exit->remote_addr;
        ack->ack_package = PACKAGE_RCP_EXIT;
        ack->protocol = PROTOCOL_RCP;
        ack->package = PACKAGE_RCP_ACK;
	//std::cout << "Sending ACK_Exit" << std::endl;
        *con << *bp_;
        rcp_waiting = true;
        wait_pack = bp_;
        start_rcp_timeout();
    }
}

void exit_session ( sockaddr_in remote_addr ) {
    if ( rcp_waiting == true )
        pthread_join ( rcp_timeout, NULL );
    base_package bp = * ( new base_package );
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

    //std::cout << "Sending Exit" << std::endl;
    *con << bp;
    rcp_waiting = true;
    wait_pack = &bp;
    start_rcp_timeout();
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
        bzero ( cmd_, _pos + 1 );
        for ( i = 0; i < _pos; ++i ) {
            cmd_[i] = com->data[i];
        }
        std::cout << "User at " << inet_ntoa ( com->remote_addr.sin_addr ) << " issued command: " << cmd_ << std::endl;
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
        base_package bp_;
        rcp_package_ack * ack = ( rcp_package_ack * ) &bp_;
        ack->ack_package = com->package;
        ack->protocol = PROTOCOL_RCP;
        ack->package = PACKAGE_RCP_ACK;
        ack->remote_addr = com->remote_addr;
        *con << bp_;
        if ( strcmp ( ( const char * ) cmd_, "rs" ) == 0) {
            end = true;
        }
        if ( cmd != NULL )
            cmd ( cmd_, args, com->args , com->remote_addr );
    }
}

void send_command ( unsigned char * cmd, unsigned char ** args, unsigned int arg_count, sockaddr_in remote_addr ) {
    if ( rcp_waiting == true )
        pthread_join ( rcp_timeout, NULL );
    base_package bp = * ( new base_package );
    rcp_package_command * com = ( rcp_package_command * ) &bp;
    com->args = arg_count;
    com->protocol = PROTOCOL_RCP;
    com->package = PACKAGE_RCP_COMMAND;
    com->remote_addr = remote_addr;
    bzero ( com->data, 253 );
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
    rcp_waiting = true;
    wait_pack = &bp;
    start_rcp_timeout();
}

void send_result ( unsigned char err_code, unsigned char * other, sockaddr_in remote_addr ) {
    if ( rcp_waiting == true )
        pthread_join ( rcp_timeout, NULL );
    base_package bp = * ( new base_package );
    rcp_package_result * res = ( rcp_package_result * ) &bp;
    unsigned int len = other != NULL && _strlen ( other ) < 254 ? _strlen ( other ) : 253, i;
    res->protocol = PROTOCOL_RCP;
    res->package = PACKAGE_RCP_RESULT;
    res->err_code = err_code;
    res->remote_addr = remote_addr;
    for ( i = 0; i < len; ++i ) {
        res->data[i] = other[i];
    }
    *con << bp;
    rcp_waiting = true;
    rcp_state = RCP_STATE_WAIT_ACK_RESULT;
    wait_pack = &bp;
    start_rcp_timeout();
}

bool is_rcp_waiting() {
    return rcp_waiting;
}

unsigned char get_rcp_state() {
    return ( unsigned char ) rcp_state;
}

void add_rcp_login_handler() {
    addHandler ( handle_rcp_request );
    addHandler ( handle_rcp_ack );
    addHandler ( handle_rcp_result );
    addHandler ( handle_rcp_exit );
    addHandler ( handle_rcp_command );
}
