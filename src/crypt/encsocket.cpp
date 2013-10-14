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
    encrypt(&ccp, key);
    UDPSocket::operator<<(*((base_package*) & ccp));
}

encsocket& encsocket::operator<<(char* msg)
{
    if(strlen(msg) > 224) {
        int i, j, count;
        count = (int)(strlen(msg) / 224) + 1;
        char * p = msg;
        for(j = 0; j < count; ++j) {
            unsigned char * pos;
            ccp_package ccp;
            pos = ccp.blocks[0] + 16;
            for(i = 0; i < strlen(msg); ++i) {
                pos[i] = (unsigned char) msg[i];
            }
            this->operator<<(ccp);
            p += 224;
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
}

void encsocket::fetch_enc_input(void* arg)
{
    std::cout << "Task: Initializing new Message Receiving Thread";
    int * sockfd = ( int* ) arg;
    unsigned char buff[BUFLEN];
    std::cout << "   [Finished]" << std::endl;
    while ( 1 ) {
        std::cout << "Starting loop" << std::endl;
        bzero ( buff, BUFLEN );
        std::cout << "Step_0x00" << std::endl;
        struct sockaddr_in addr;
        std::cout << "Step_0x01" << std::endl;
        socklen_t slen = sizeof ( addr );
        std::cout << "Step_0x02" << std::endl;
        try {
            if ( recvfrom ( ( *sockfd ), buff, BUFLEN, 0, ( struct sockaddr* ) &addr, &slen ) == -1 )
                std::cout << "Error: While Receiving" << std::endl;
            else
                std::cout << "Received" << std::endl;
        } catch (...) {
            std::cout << "Step_0x03" << std::endl;
        }
        handle_enc ( buff, addr );
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
    local_addr.sin_port = htons ( PORT );
    local_addr.sin_addr.s_addr = htonl ( INADDR_ANY );
    if ( bind ( sockfd, ( struct sockaddr* ) &local_addr, sizeof ( local_addr ) ) == -1 )
        std::cout << "Error: Binding Socket" << std::endl;
    //cout << "Info: Starting new Message Receiving Thread" << endl;
    int err = pthread_create ( &input_id, NULL,(void* (*) (void*)) &encsocket::fetch_enc_input, ( void* ) &sockfd );
    //cout << "Created Thread with code: " << err << endl;
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
    bp.remote_addr = addr;
    bzero(bp.data, 254);
    int i;
    for(i = 0; i < 254; ++i) {
        bp.data[i] = data[i + 2];
    }
    ccp_package * ccp = (ccp_package *) &bp;
    decrypt(ccp, key);
    for(i = 0; i < l_handler.size(); ++i) {
        l_handler[i] (*ccp);
    }
}






