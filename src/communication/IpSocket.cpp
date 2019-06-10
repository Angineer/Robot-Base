#include "communication/IpSocket.h"

#include <cstring>
#include <iostream>
#include <netdb.h>
#include <strings.h>

IpSocket::IpSocket ( std::string host, std::string port )
{
    // Set up the address info
    struct addrinfo hints;
    memset ( &hints, 0, sizeof ( struct addrinfo ) );
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;

    int address_success = getaddrinfo ( host.c_str(), 
                                        port.c_str(),
                                        &hints,
                                        &address );
    if ( address_success < 0 ) {
        std::cout << "ERROR finding address info!" << std::endl;
    }

    // Create the socket
    socket_fd = socket ( address->ai_family,
                         address->ai_socktype,
                         address->ai_protocol );

    if ( socket_fd < 0 ){
        std::cout << "ERROR opening socket" << std::endl;
    }
}

IpSocket::~IpSocket ()
{
    freeaddrinfo ( address );
}

