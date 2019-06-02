#include "communication/Socket.h"

Socket::Socket(std::string host, int portno){
    portno = portno;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        std::cout << "ERROR opening socket" << std::endl;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
}

