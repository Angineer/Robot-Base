#include "MotorController.h"

#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

MotorController::MotorController ( std::string device ) :
    arduino ( device ),
    count_motors ( 2 )
{
}

MotorController::~MotorController()
{
}

bool MotorController::dispense ( int slot, int count ){
    // Request a dispense
    std::cout << "Requesting dispense..." << std::endl;
    char response { 'n' };
    size_t attempts { 0 };
    while ( response != 'a' ) {
        if ( !arduino.sendByte ( 'd' ) ) { // "dispense"
            std::cout << "Motor controller send failure" << std::endl;
            return false;
        }
        response = arduino.receiveByte();

        if ( response != 'a' && ++attempts >= 10 ) {
            std::cout << "Motor controller receive failure"
                      << std::endl;
            return false;
        }
    }

    // Send the message payload
    arduino.sendByte ( slot );
    arduino.sendByte ( count );

    // When the dispense is complete, we should get another ACK
    if ( arduino.receiveByte() != 'a' ) {
        std::cout << "Motor controller finalize failure"
                  << std::endl;
        return false;
    }

    return true;
}
