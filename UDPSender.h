//
//  UDPSender.h
//  TabletServer
//
//  Created by Timothy Kukulski on 10/22/12.
//
//

#ifndef __TabletServer__UDPSender__
#define __TabletServer__UDPSender__



#include <netdb.h>

class UDPSender {
public:
    UDPSender(const char *host, int port);
 //   virtual ~UDPSender();
    void send(void *data, size_t count);
private:
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
};

class UDPListener {
public:
    UDPListener(int port);
  //  virtual ~UDPListener();
    size_t listen(void *data, size_t maxcount);
private:
    int sock;
    struct sockaddr_in server;
    struct sockaddr_in from;
};


#endif /* defined(__TabletServer__UDPSender__) */
