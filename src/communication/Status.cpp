#include "communication/Status.h"

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

Status::Status(StatusCode status){
    this->status = status;
}
void Status::write_serial(){
    serial = "s" + std::to_string(int(status));
}

