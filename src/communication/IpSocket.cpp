#include "communication/IpSocket.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <strings.h>

IpSocket::IpSocket ( std::string host, int port )
{
    // Address to which we will connect via this socket
    sockaddr_in* local_address = new sockaddr_in;
    local_address->sin_family = AF_INET;
    local_address->sin_port = htons ( port );
    inet_aton ( host.c_str(), &( local_address->sin_addr ) );

    address = static_cast<void*> ( local_address );

    address_len = sizeof ( sockaddr_in );

    // Create the socket
    socket_fd = socket ( AF_INET, SOCK_STREAM, 0 );

    if ( socket_fd < 0 ){
        std::cout << "ERROR opening socket" << std::endl;
    }
}

IpSocket::IpSocket() : IpSocket::IpSocket ( "127.0.0.1", 5000 ) {}

IpSocket::~IpSocket ()
{
    delete static_cast<sockaddr_in*> ( address );
}

