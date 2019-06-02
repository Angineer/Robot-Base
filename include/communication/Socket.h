#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <string>

class Socket{
    protected:
        int sockfd, portno, n;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        char* buffer; // Message buffer
    public:
        Socket(std::string host, int portno);
};

#endif
