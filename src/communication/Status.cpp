#include "communication/Status.h"

StatusCode string_to_status(std::string input){
    if(input == "battery_low") return StatusCode::BATTERY_LOW;
    if(input == "unavailable") return StatusCode::UNAVAILABLE;
    if(input == "ready") return StatusCode::READY;
    if(input == "delivering") return StatusCode::DELIVERING;
    if(input == "waiting") return StatusCode::WAITING;
    if(input == "returning") return StatusCode::RETURNING;
    if(input == "dispensing") return StatusCode::DISPENSING;

    return StatusCode::UNKNOWN;
}

std::string status_to_string(StatusCode input){
    switch(input){
        case StatusCode::BATTERY_LOW: return "battery_low";
        case StatusCode::UNAVAILABLE: return "unavailable";
        case StatusCode::READY: return "ready";
        case StatusCode::DELIVERING: return "delivering";
        case StatusCode::WAITING: return "waiting";
        case StatusCode::RETURNING: return "returning";
        case StatusCode::DISPENSING: return "dispensing";

        default: return "unknown";
    }
}

Status::Status(StatusCode status){
    this->status = status;
}
void Status::write_serial(){
    serial = "s" + std::to_string(int(status));
}

