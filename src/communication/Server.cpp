#include "communication/Server.h"

#include <iostream>
#include <thread>
#include <unistd.h>

Server::Server(std::string host, int portno) : Socket(host, portno)
{
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    int bind_success = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (bind_success < 0){
        std::cout << "ERROR binding to socket!" << std::endl;
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
        n = read(sockfd, &len, sizeof(len));
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
void Server::serve(std::function<std::string ( std::string )> callback_func){
    std::cout << "Listening for connections" << std::endl;
    
    // Server runs forever
    while (true){
        listen(sockfd, 5);
        clilen = sizeof(cli_addr);
        int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd > 0){
            std::cout << "Connection " << newsockfd << " established!" << std::endl;

            std::thread t(&Server::child_serve, this, newsockfd, callback_func); // Create child thread
            t.detach();
        }
    }
}
void Server::shutdown(){
    close(sockfd);
}

