#include "communication/Server.h"

#include <iostream>
#include <thread>
#include <unistd.h>

#include "communication/BluetoothSocket.h"
#include "communication/IpSocket.h"

Server::Server ( SocketType type, std::string connection_string )
{
    //serv_addr.sin_addr.s_addr = INADDR_ANY;
    if ( type == SocketType::IP ) {
        // Parse connection string
        size_t delim = connection_string.find ( ":" );
        std::string host = connection_string.substr ( 0, delim );
        std::string port = connection_string.substr ( delim + 1 );

        socket.reset ( new IpSocket ( host, atoi ( port.c_str() ) ) );
    } else if ( type == SocketType::BLUETOOTH ) {
        socket.reset ( new BluetoothSocket ( connection_string ) );
    }

    // If socket not configured, exit
    if ( !socket ) {
        std::cout << "Server socket not configured!" << std::endl;
        return;
    }

    int bind_success = bind ( socket->socket_fd, 
                              static_cast<sockaddr*> ( socket->address ),
                              socket->address_len );

    if ( bind_success < 0 ) {
        std::cout << "ERROR binding to socket! " << errno << std::endl;
        exit(1);
    }
}

void Server::child_serve(int sockfd, std::function<std::string ( std::string )> callback_func){
    int len;

    std::cout << "Child thread started!" << std::endl;

    // Process data until disconnect
    bool connect = true;
    while(connect){
        // Read length of message from socket
        int n = read(sockfd, &len, sizeof(len));
        if (n < 0){
            std::cout << "ERROR reading from socket!" << std::endl;
        }
        else if(n == 0){
            close(sockfd);
            std::cout << "Connection " << sockfd << " disconnected!" << std::endl;
            connect = false;
        }
        else{
            // Prep the buffer
            buffer = new char[len];
            memset(buffer, 0, len);

            // Then read message
            n = read(sockfd, buffer, len);
            if (n < 0){
                std::cout << "ERROR reading from socket!" << std::endl;
            }
            else if (n == 0){
                close(sockfd);
                std::cout << "Connection " << sockfd << " disconnected!" << std::endl;
                connect = false;
            }
            else{
                // Call callback using input
                std::string message ( buffer, len );
                std::string response = callback_func ( message );

                // Send response length
                len = response.length();
                n = ::write(sockfd, &len, sizeof(len));
                if (n < 0)
                     std::cout << "ERROR writing to socket" << std::endl;

                // Then send the actual message
                n = ::write(sockfd, response.c_str(), len);
                if (n < 0)
                     std::cout << "ERROR writing to socket" << std::endl;
            }

            // Clean up buffer
            delete[] buffer;
        }
    }
}

void Server::serve ( std::function<std::string ( std::string )> callback_func ) {
    std::cout << "Listening for connections" << std::endl;
    
    // Server runs forever
    while ( true ) {
        listen ( socket->socket_fd, 5 );
        sockaddr client;
        socklen_t client_len;
        int newsockfd = accept ( socket->socket_fd,
                                 &client,
                                 &client_len );

        if ( newsockfd > 0 ) {
            std::cout << "Connection " << newsockfd
                      << " established!" << std::endl;

            // Create child thread
            std::thread t ( &Server::child_serve,
                            this, newsockfd, callback_func );
            t.detach();
        }
    }
}

void Server::shutdown() {
    close ( socket->socket_fd );
}

