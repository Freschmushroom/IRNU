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


#ifndef ENCSOCKET_H
#define ENCSOCKET_H

#include "../socket/udpsocket.h"
#include "ccp/irnu_ccp.hpp"
#include <cryptopp/config.h>
#include <cryptopp/aes.h>
#include <cryptopp/md5.h>
#include <vector>

class encsocket : UDPSocket
{
private:
  byte key[CryptoPP::AES::DEFAULT_KEYLENGTH];
  std::vector<enc_handler> l_handler;
  unsigned char current_session;
  unsigned char current_id;
  pthread_t resend_id;
public:
    encsocket();
    virtual encsocket& operator+=(byte[]);
    virtual encsocket& operator<<(ccp_package);
    virtual encsocket& operator<<(char *);
    void send_pack(ccp_package);
    void handle_enc(unsigned char* data, sockaddr_in addr);    
    void add_enc_handler(enc_handler);
    bool start_connection();
    ccp_package* sent_enc_packs[256][256];
    bool check_sent_packs[256][256];
};

#endif // ENCSOCKET_H
