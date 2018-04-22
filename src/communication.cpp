#include <iostream>
#include <netdb.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "../include/communication.h"

namespace robie_comm{

    StatusCode string_to_status(std::string input){
        if(input == "battery_low") return StatusCode::battery_low;
        if(input == "unavailable") return StatusCode::unavailable;
        if(input == "ready") return StatusCode::ready;
        if(input == "delivering") return StatusCode::delivering;
        if(input == "waiting") return StatusCode::waiting;
        if(input == "returning") return StatusCode::returning;
        if(input == "dispensing") return StatusCode::dispensing;

        return StatusCode::unknown;
    }

    std::string status_to_string(StatusCode input){
        switch(input){
            case StatusCode::battery_low: return "battery_low";
            case StatusCode::unavailable: return "unavailable";
            case StatusCode::ready: return "ready";
            case StatusCode::delivering: return "delivering";
            case StatusCode::waiting: return "waiting";
            case StatusCode::returning: return "returning";
            case StatusCode::dispensing: return "dispensing";

            default: return "unknown";
        }
    }

    BluetoothLink::BluetoothLink(){
        //foo
    }
    void BluetoothLink::connect(){
        std::cout << "Establishing bluetooth connection..." << std::endl;
    }
    void BluetoothLink::disconnect(){
        std::cout << "Stopping bluetooth connection..." << std::endl;
    }
    int BluetoothLink::send(std::string message){
        std::cout << "Sending message over bluetooth..." << std::endl;
        return 0;
    }
    int BluetoothLink::receive(){
        std::cout << "Receiving message over bluetooth..." << std::endl;
        return 0;
    }

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

    Server::Server(std::string host, int portno) : Socket(host, portno)
    {
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        int bind_success = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

        if (bind_success < 0){
            std::cout << "ERROR binding to socket!" << std::endl;
            exit(1);
        }
    }
    void Server::child_serve(int sockfd, std::function<int(std::string, std::string&)> callback_func){
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
                    std::string message(buffer, len);
                    std::string response;
                    callback_func(message, response);

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
    void Server::serve(std::function<int(std::string, std::string&)> callback_func){
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

    std::string Message::get_serial() const{
        return serial;
    }

    Command::Command(std::string command){
        this->command = command;
    }
    void Command::write_serial(){
        serial = "c" + command;
    }

    Status::Status(StatusCode status){
        this->status = status;
    }
    void Status::write_serial(){
        serial = "s" + std::to_string(int(status));
    }

} // robot_comm namespace
