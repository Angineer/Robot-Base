#ifndef COMMAND_H
#define COMMAND_H

#include <string>

class Command: public Message{
    private:
        std::string command;
    public:
        Command(std::string command);
        void write_serial();
};

#endif
