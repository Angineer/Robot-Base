#ifndef STATUS_H
#define STATUS_H

#include <string>
#include <string.h>

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

StatusCode string_to_status(std::string input);
std::string status_to_string(StatusCode input);

class Status: public Message{
    private:
        StatusCode status;
    public:
        Status(StatusCode status);
        void write_serial();
};

#endif
