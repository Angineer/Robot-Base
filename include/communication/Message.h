#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

class Message{
    protected:
        std::string serial;
    public:
        virtual void write_serial() = 0;
        std::string get_serial() const;
};

#endif
