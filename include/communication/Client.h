#ifndef CLIENT_H
#define CLIENT_H

#include <string>

class Client: public Socket{
    private:
        struct hostent *server;
    public:
        Client(std::string host, int portno);
        int connect();
        void disconnect();
        std::string send(Message& message);
};

#endif
