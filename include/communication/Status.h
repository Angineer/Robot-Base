#ifndef STATUS_H
#define STATUS_H

#include <string>

#include "communication/Message.h"

enum class StatusCode
{
    UNKNOWN = -99,
    BATTERY_LOW = -2,
    UNAVAILABLE = -1,
    READY = 0,
    DELIVERING = 1,
    WAITING = 2,
    RETURNING = 3,
    DISPENSING = 4
};

StatusCode string_to_status(std::string input);
std::string status_to_string(StatusCode input);

class Status: public Message {
    private:
        StatusCode status;
    public:
        Status(StatusCode status);
        void write_serial();
};

#endif
