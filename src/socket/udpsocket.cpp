/*
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


#include "udpsocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

UDPSocket::UDPSocket() {
    
}

UDPSocket::~UDPSocket() {
  pthread_cancel(input_id);
}

using std::string;
using std::cout;
using std::endl;
using std::vector;

UDPSocket& UDPSocket::operator<< ( base_package bp ) {
    bzero(buf, BUFLEN);
    buf[0] = bp.protocol;
    buf[1] = bp.package;
    int i;
    for(i = 0; i < 254; i++) {
      buf[2 + i] = bp.data[i];
    }
    if(sendto(sockfd, buf, BUFLEN, 0, (struct sockaddr*) &bp.remote_addr, sizeof(bp.remote_addr)) == -1)
      cout << "Error: Sending to " << sockfd << " failed!" << endl;
}

void * fetch_input(void * arg) {
  cout << "Task: Initializing new Message Receiving Thread";
  int * sockfd = (int*) arg;
  unsigned char buff[BUFLEN];
  cout << "   [Finished]" << endl;
  while(1) {
    bzero(buff, BUFLEN);
    base_package bp;
    socklen_t slen = sizeof(bp.remote_addr);
    if(recvfrom((*sockfd), buff, BUFLEN, 0, (struct sockaddr*) &bp.remote_addr, &slen) == -1)
      cout << "Error: While Receiving" << endl;
    else
      cout << "Info: Received" << endl;
    handle(buff);
  }
}

bool UDPSocket::start_connection () {
    socklen_t slen = sizeof(local_addr);
    if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
      cout << "Error: Creating Socket" << endl;
    bzero(&local_addr, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(PORT);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sockfd, (struct sockaddr*) &local_addr, sizeof(local_addr)) == -1)
      cout << "Error: Binding Socket" << endl;
    cout << "Info: Starting new Message Receiving Thread" << endl;
    int err = pthread_create(&input_id, NULL, &fetch_input, (void*)&sockfd);
    cout << "Created Thread with code: " << err << endl;
}

unsigned int UDPSocket::get_sockfd() {
    return sockfd;
}




