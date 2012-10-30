//
//  UDPSender.cpp
//  TabletServer
//
//  Created by Timothy Kukulski on 10/22/12.
//
//

#include "UDPSender.h"
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

UDPSender::UDPSender(const char *host, int port) {
int rv;


memset(&hints, 0, sizeof hints);
hints.ai_family = AF_INET;//  AF_UNSPEC;
hints.ai_socktype = SOCK_DGRAM;
hints.ai_flags = NI_NUMERICSERV;

    char portString[128];
    sprintf(portString,"%d",port);
    
if ((rv = getaddrinfo(host, portString, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
}

// loop through all the results and make a socket
for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
                         p->ai_protocol)) == -1) {
        perror("talker: socket");
        continue;
    }
    
    break;
}
}

//UDPSender::~UDPSender() {
//    freeaddrinfo(servinfo);
//    close(sockfd);
//}

void UDPSender::send(void *data, size_t count) {
    
    size_t numbytes;
    if ((numbytes = sendto(sockfd, data,count, 0,
						   p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

UDPListener::UDPListener(int port) {
    sock = socket(AF_INET, SOCK_DGRAM,0);
    if (sock < 0) error("Opening socket");

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(port);
    
    if (bind(sock,(struct sockaddr *)&server,sizeof(server))<0)
        error("binding");
}

//UDPListener::~UDPListener() {
//    close(sock);
//}
size_t UDPListener::listen(void *data, size_t maxcount) {
    socklen_t fromlen = sizeof(from);
    return recvfrom(sock,data,maxcount,0,(struct sockaddr *)&from,&fromlen);
}

