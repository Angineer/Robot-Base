#ifndef IP_SOCKET_H
#define IP_SOCKET_H

#include <netdb.h>
#include <string>

#include "communication/Socket.h"

// A TCP/IP socket
class IpSocket : public Socket
{
public:
    // Constructor
    IpSocket ( std::string host, std::string port );
    ~IpSocket ();
};

#endif
