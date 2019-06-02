#ifndef SERVER_H
#define SERVER_H

#include <functional>
#include <netinet/in.h>
#include <string>
#include <string.h>

class Server: public Socket{
    private:
        struct sockaddr_in cli_addr;
        socklen_t clilen;
        pid_t pID;

        void child_serve ( int sockfd,
                           std::function<int(std::string, std::string&)>
                               callback_func );
    public:
        Server(std::string host, int portno);
        void serve(std::function<int(std::string, std::string&)> callback_func);
        void shutdown();
};

#endif
