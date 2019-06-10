#ifndef SERVER_H
#define SERVER_H

#include <functional>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <string.h>

#include "communication/Socket.h"

class Server: public Socket
{
public:
    Server ( SocketType type, std::string connection_string );
    void serve ( std::function<std::string ( std::string )> callback_func );
    void shutdown();

private:
    std::unique_ptr<Socket> socket;
    pid_t pID;

    void child_serve ( int sockfd,
                       std::function<std::string ( std::string )>
                           callback_func );
};

#endif
