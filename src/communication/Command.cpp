#include "communication/Command.h"

Command::Command(std::string command) {
    this->command = command;
}
void Command::write_serial(){
    serial = "c" + command;
}

