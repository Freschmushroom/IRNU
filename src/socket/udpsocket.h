/*
 *
    Copyright 2013 Felix Resch <email>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include "../IRNU.hpp"
#include <string>
#include <iostream>
#include <vector>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFLEN 256
#define PORT 9980

class UDPSocket {
private:
    struct sockaddr_in local_addr;
    unsigned int sockfd;
    char buf[BUFLEN];
    pthread_t input_id;
public:
    UDPSocket();
    virtual ~UDPSocket();
    UDPSocket& operator<< ( base_package bp );
    bool start_connection();
    unsigned int get_sockfd();
};

void * fetch_input ( void * arg );
#endif // UDPSOCKET_H
