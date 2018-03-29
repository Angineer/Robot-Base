#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <csignal>
#include <functional>
#include <netinet/in.h>
#include <string>
#include <string.h>

namespace robie_comm{

    enum class StatusCode{
        unknown = -99,
        battery_low = -2,
        unavailable = -1,
        ready = 0,
        delivering = 1,
        waiting = 2,
        returning = 3,
        dispensing = 4
    };

    class Message{
        protected:
            std::string serial;
        public:
            virtual void write_serial() = 0;
            std::string get_serial() const;
    };

    class Command: public Message{
        private:
            std::string command;
        public:
            Command(std::string command);
            void write_serial();
    };

    class Status: public Message{
        private:
            StatusCode status;
        public:
            Status(StatusCode status);
            void write_serial();
    };

    class Socket{
        protected:
            int sockfd, portno, n;
            struct sockaddr_in serv_addr;
            struct hostent *server;
            char* buffer; // Message buffer
        public:
            Socket(std::string host, int portno);
    };

    class Client: public Socket{
        private:
            struct hostent *server;
        public:
            Client(std::string host, int portno);
            void connect();
            void disconnect();
            std::string send(Message& message);
    };

    class Server: public Socket{
        private:
            struct sockaddr_in cli_addr;
            socklen_t clilen;
            pid_t pID;

            void child_serve(int sockfd, std::function<int(std::string, std::string&)> callback_func);
        public:
            Server(std::string host, int portno);
            void serve(std::function<int(std::string, std::string&)> callback_func);
            void shutdown();
    };

} // robie_comm namespace

#endif
