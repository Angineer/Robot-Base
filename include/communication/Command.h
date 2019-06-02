#ifndef COMMAND_H
#define COMMAND_H

#include <string>

#include "communication/Message.h"

class Command: public Message
{
    private:
        std::string command;
    public:
        Command ( std::string command );
        void write_serial();
};

#endif
