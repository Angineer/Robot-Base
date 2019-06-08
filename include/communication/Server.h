#ifndef SERVER_H
#define SERVER_H

#include <functional>
#include <netinet/in.h>
#include <string>
#include <string.h>

#include "communication/Socket.h"

class Server: public Socket
{
    public:
        Server(std::string host, int portno);
        void serve(std::function<std::string ( std::string )> callback_func);
        void shutdown();

    private:
        struct sockaddr_in cli_addr;
        socklen_t clilen;
        pid_t pID;

        void child_serve ( int sockfd,
                           std::function<std::string ( std::string )>
                               callback_func );
};

#endif
