#include "communication/Client.h"

Client::Client(std::string host, int portno) : Socket(host, portno)
{
    server = gethostbyname(host.c_str());
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
}
int Client::connect(){

    int connect_success = ::connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (connect_success < 0){
        std::cout << "ERROR connecting to server!" << std::endl;
    }

    return connect_success;
}
std::string Client::send(Message& message){
    message.write_serial();
    std::string content = message.get_serial();

    // First send the message length
    int len = content.length();
    n = ::write(sockfd, &len, sizeof(len));
    if (n < 0)
         std::cout << "ERROR writing to socket" << std::endl;

    // Then send the actual message
    n = ::write(sockfd, content.c_str(), len);
    if (n < 0)
         std::cout << "ERROR writing to socket" << std::endl;

    // Read response length
    n = read(sockfd, &len, sizeof(len));
    if (n < 0){
        std::cout << "ERROR reading from socket!" << std::endl;
    }
    else if (n > 0){
        // Prep the buffer
        buffer = new char[len];
        memset(buffer, 0, len);

        // Then read message
        n = read(sockfd, buffer, len);
        if (n < 0){
            std::cout << "ERROR reading from socket!" << std::endl;
        }
        else if (n > 0){
            std::string response(buffer, len);
            return response;
        }

        // Clean up buffer
        delete[] buffer;
    }

    return "ERROR";
}
void Client::disconnect(){
    close(sockfd);
}

