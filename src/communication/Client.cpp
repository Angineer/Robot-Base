#include "communication/Client.h"

#include <iostream>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include "communication/IpSocket.h"

Client::Client ( SocketType type, std::string connection_string ) {
    if ( type == SocketType::IP ) {
        // Parse connection string
        size_t delim = connection_string.find ( ":" );
        std::string host = connection_string.substr ( 0, delim );
        std::string port = connection_string.substr ( delim + 1 );

        socket.reset ( new IpSocket ( host, port ) );
    } else if ( type == SocketType::BLUETOOTH ) {
        // TODO
    }

    int connect_success = connect ( socket->socket_fd,
                                    socket->address->ai_addr,
                                    sizeof ( struct addrinfo ) );

    if (connect_success < 0){
        std::cout << "ERROR connecting to server!" << std::endl;
    }
}

Client::~Client() {
    close ( socket->socket_fd );
}

std::string Client::send(Message& message){
    message.write_serial();
    std::string content = message.get_serial();

    // First send the message length
    int len = content.length();
    int n = write(socket->socket_fd, &len, sizeof(len));
    if (n < 0)
         std::cout << "ERROR writing to socket" << std::endl;

    // Then send the actual message
    n = write(socket->socket_fd, content.c_str(), len);
    if (n < 0)
         std::cout << "ERROR writing to socket" << std::endl;

    // Read response length
    n = read(socket->socket_fd, &len, sizeof(len));
    if (n < 0){
        std::cout << "ERROR reading from socket!" << std::endl;
    }
    else if (n > 0){
        // Prep the buffer
        char buffer [len] = {};

        // Then read message
        n = read(socket->socket_fd, buffer, len);
        if (n < 0){
            std::cout << "ERROR reading from socket!" << std::endl;
        }
        else if (n > 0){
            std::string response(buffer, len);
            return response;
        }
    }

    return "ERROR";
}

