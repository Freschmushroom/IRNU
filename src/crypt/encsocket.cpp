/*
    Copyright (c) 2013, <copyright holder> <email>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the <organization> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY <copyright holder> <email> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL <copyright holder> <email> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "encsocket.h"
#include "ccp/irnu_ccp.hpp"
#include "ccp/irnu_ccp.hpp"
#include "ccp/irnu_ccp.hpp"
#include "../socket/udpsocket.h"

encsocket::encsocket() : UDPSocket()
{
}

encsocket& encsocket::operator+=(byte * key_)
{
    if(sizeof(key) == sizeof(key_)) {
        int i;
        for(i = 0; i < 16; ++i) {
            key[i] = key_[i];
        }
    }
    return *this;
}

encsocket& encsocket::operator<<(ccp_package ccp)
{
    send_pack(ccp);
    current_id = 0;
    current_session++;
}

void encsocket::send_pack(ccp_package ccp)
{
    encrypt(&ccp, key);
    int i;
    byte * check = get_checksum(ccp.blocks[0]);
    for(i = 0; i < 16; i++) {
        ccp.checksum[i] = check[i];
    }
    free(check);
    if(ccp.retries == 0 && ccp.package != PACKAGE_CCP_ACK) {
        ccp.id = current_id;
        ccp.session = current_session;
        sent_enc_packs[ccp.session][ccp.id] = &ccp;
        //std::cout << "Pushing package with HW Addr: " << &ccp << " to " << (int)ccp.session << ", " << (int)ccp.id << std::endl;
        check_sent_packs[ccp.session][ccp.id] = true;
        remote_addr[ccp.session][ccp.id] = *(ccp.remote_addr);
        //std::cout << "IP Address: " << inet_ntoa(sent_enc_packs[current_session][current_id]->remote_addr->sin_addr) << std::endl;
        //std::cout << "Address Pointer: " << &(sent_enc_packs[current_session][current_id]->remote_addr) << std::endl;
    } else if (ccp.package == PACKAGE_CCP_ACK) {
        //std::cout << "Sending ACK package" << std::endl;
    } else {
        ccp.remote_addr = &(remote_addr[ccp.session][ccp.id]);
        ccp.remote_addr->sin_port = htons(9999);
        ccp.remote_addr->sin_family = AF_INET;
    }
    base_package * bp = (base_package *) &ccp;
    bzero ( buf, BUFLEN );
    buf[0] = bp->protocol;
    buf[1] = bp->package;

    for ( i = 0; i < 254; i++ ) {
        buf[2 + i] = bp->data[i];
    }
    if ( sendto ( sockfd, buf, BUFLEN, 0, ( struct sockaddr* ) &ccp.remote_addr, sizeof ( &ccp.remote_addr ) ) == -1 ) {
        //std::cout << "Error: Sending to " << sockfd << " failed!" << std::endl;
        //std::cout << strerror(errno) << std::endl;
        //std::cout << (int) bp->protocol << " " << (int) bp->package << " "  << inet_ntoa(bp->remote_addr->sin_addr) << std::endl;
    } else {
        //std::cout << "Sending " << (int) bp->protocol << " " << (int) bp->package << " "  << inet_ntoa(bp->remote_addr->sin_addr) << std::endl;;
        //std::cout << "[Finished]" << std::endl;
    }
}


encsocket& encsocket::operator<<(char* msg)
{
    if(strlen(msg) > 208) {
        int i, j, count;
        count = (int)(strlen(msg) / 208) + 1;
        char * p = msg;
        for(j = 0; j < count; ++j) {
            unsigned char * pos;
            ccp_package ccp;
            pos = ccp.blocks[0] + 16;
            for(i = 0; i < strlen(msg); ++i) {
                pos[i] = (unsigned char) msg[i];
            }
            send_pack(ccp);
            current_id++;
            p += 208;
        }
    } else {
        int i;
        unsigned char * pos;
        ccp_package ccp;
        pos = ccp.blocks[0] + 16;
        for(i = 0; i < strlen(msg); ++i) {
            pos[i] = (unsigned char) msg[i];
        }
        this->operator<<(ccp);
    }
    current_session++;
}

void fetch_enc_input(void* arg)
{
    std::cout << "Task: Initializing new Message Receiving Thread";
    encsocket * enc = (encsocket *) arg;
    int sockfd_ = (((UDPSocket *)(enc))->get_sockfd());
    int * sockfd = &sockfd_;
    unsigned char buff[BUFLEN];
    std::cout << "   [Finished]" << std::endl;
    while ( 1 ) {
        bzero ( buff, BUFLEN );
        struct sockaddr_in addr;
        socklen_t slen = sizeof ( addr );
        if ( recvfrom ( ( *sockfd ), buff, BUFLEN, 0, ( struct sockaddr* ) &addr, &slen ) == -1 ) {
            std::cout << "Error: While Receiving" << std::endl;
            continue;
        }
        else
            std::cout << "[INFO] Read package" << std::endl;

        enc->handle_enc ( buff, addr );
    }
    std::cout << "[FATAL] Code shouldn't reach that point" << std::endl;
}

void resend_enc_packs(void * arg) {
    //std::cout << "Initializing new Packet Resend Thread" << std::endl;
    encsocket * enc = (encsocket *) arg;
    //std::cout << "   [Finished]" << std::endl;
    while(1) {
        int i, j, count;
        count = 0;
        for(i = 0; i < 256; ++i) {
            for(j = 0; j < 256; ++j) {
                if(enc->check_sent_packs[i][j] == true) {
                    void * addr = &enc->sent_enc_packs[i][j]->remote_addr;
                    /*std::cout << "Resending Package[session = " << i << ", id = " << j << "] with " << (enc->check_sent_packs[i][j] == true ? "true" : "false") << " to " << inet_ntoa(enc->sent_enc_packs[i][j]->remote_addr->sin_addr) << std::endl;
                    std::cout << "Resending package with HW Addr: " << &enc->sent_enc_packs[i][j] << " on " << i << ", " << j << std::endl;
                    std::cout << "Address Pointer: " << &(enc->sent_enc_packs[i][j]->remote_addr) << std::endl;*/
                    enc->send_pack(*(enc->sent_enc_packs[i][j]));
                    enc->sent_enc_packs[i][j]->retries++;
                    if(enc->sent_enc_packs[i][j]->retries >= 10) {
                        enc->check_sent_packs[i][j] = false;
                    }
                    count++;
                } else {
                    enc->sent_enc_packs[i][j] = NULL;
                }
            }
        }
        sleep(1);
        //std::cout << "Full Check. Resent " << count << " Packages" << std::endl;
    }
}

bool encsocket::start_connection()
{
    socklen_t slen = sizeof ( local_addr );
    if ( ( sockfd = socket ( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) == -1 )
        std::cout << "Error: Creating Socket" << std::endl;
    bzero ( &local_addr, sizeof ( local_addr ) );
    std::cout << "Listening on " << ENC_PORT << std::endl;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons ( ENC_PORT );
    local_addr.sin_addr.s_addr = htonl ( INADDR_ANY );
    if ( bind ( sockfd, ( struct sockaddr* ) &local_addr, sizeof ( local_addr ) ) == -1 )
        std::cout << "Error: Binding Socket" << std::endl;
    std::cout << "Setting bool pointers" << std::endl;
    int i, j;
    for(i = 0; i < 256; ++i) {
        for(j = 0; j < 256; ++j) {
            check_sent_packs[i][j] = false;
            sent_enc_packs[i][j] == NULL;
        }
    }
    //cout << "Info: Starting new Message Receiving Thread" << endl;
    int err = pthread_create ( &input_id, NULL,(void* (*) (void*)) &fetch_enc_input, ( void* ) this );
    err = pthread_create( &resend_id, NULL, (void *(*)(void*)) &resend_enc_packs, (void *) this);
    //cout << "Created Thread with code: " << err << endl,
}


void encsocket::add_enc_handler(enc_handler enc)
{
    l_handler.push_back(enc);
}

void encsocket::handle_enc(unsigned char* data, sockaddr_in addr)
{
    base_package bp;
    bp.protocol = data[0];
    bp.package = data[1];
    bp.remote_addr = &addr;
    bzero(bp.data, 254);
    std::cout << "Received Package: " << inet_ntoa(addr.sin_addr) << std::endl;
    int i;
    for(i = 0; i < 254; ++i) {
        bp.data[i] = data[i + 2];
    }
    ccp_package * ccp = (ccp_package *) &bp;
    byte * check = get_checksum(ccp->blocks[0]);
    for(i = 0; i < 16; ++i) {
        if(check[i] != ccp->checksum[i]) {
            std::cout << "Checksum Error with package@[session = " << (int)ccp->session << ", id = " << (int)ccp->id << "]" << std::endl;
            break;
        }
    }
    int j;
    decrypt(ccp, key);
    std::cout << "[INFO] Found " << l_handler.size() << " handlers" << std::endl;
    for(i = 0; i < l_handler.size(); ++i) {
        l_handler[i] (*ccp, (void *) this);
    }
}






