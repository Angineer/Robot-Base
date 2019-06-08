#ifndef ERROR_H
#define ERROR_H

#include <string>

#include "communication/Message.h"

enum class ErrorCode
{
    UNKNOWN = 0,
    BATTERY_LOW = 1
};

ErrorCode string_to_code ( std::string input );
std::string code_to_string ( ErrorCode input );

class Error: public Message {
    private:
        ErrorCode code;
    public:
        Error ( ErrorCode status );
        void write_serial();
};

#endif
