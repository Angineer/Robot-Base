#ifndef CLIENT_H
#define CLIENT_H

#include <memory>
#include <string>

#include "communication/Message.h"
#include "communication/Socket.h"

class Client
{
public:
    Client ( SocketType type, std::string connection_string );
    ~Client ();
    std::string send ( Message& message );

private:
    std::unique_ptr<Socket> socket;
};

#endif
