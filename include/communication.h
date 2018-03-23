#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <csignal>
#include <functional>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MSG_LENGTH 256

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
            virtual void serialize() = 0;
            std::string get_serial() const;
    };

    class Command: public Message{
        private:
            std::string command;
        public:
            Command(std::string command);
            void serialize();
    };

    class Status: public Message{
        private:
            StatusCode status;
        public:
            Status(StatusCode status);
            void serialize();
    };

    class Socket{
        protected:
            int sockfd, portno, n;
            struct sockaddr_in serv_addr;
            struct hostent *server;
            char buffer[MSG_LENGTH]; // Message buffer
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
            void send(const Message& message);
    };

    class Server: public Socket{
        private:
            struct sockaddr_in cli_addr;
            socklen_t clilen;
            pid_t pID;
        public:
            Server(std::string host, int portno);
            void serve(std::function<void(char*, int)> callback_func);
            void shutdown();
    };

} // robot_comm namespace

#endif
