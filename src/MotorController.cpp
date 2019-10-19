#include "MotorController.h"

#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

MotorController::MotorController ( std::string device ) :
    count_motors ( 2 ),
    serial_fd ( -1 )
{
    serial_fd = open ( device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
    if ( serial_fd == -1 ) {
        std::cout << "Unable to connect to motor controller" << std::endl;
    } else {
        fcntl(serial_fd, F_SETFL, 0);
        std::cout << "Motor controller connected" << std::endl;
    }

    // Set serial port options
    struct termios options;

    // Get current options
    tcgetattr(serial_fd, &options);

    // Set baud rate
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    // 8N1
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Timeout of 20 sec. This is necessary because we wait for a response
    // after a dispense to let us know the hardware is done.
    options.c_cc[VTIME] = 200;
    options.c_cc[VMIN] = 0; 

    /* set the options */
    tcsetattr(serial_fd, TCSANOW, &options);
}

MotorController::~MotorController(){
    close ( serial_fd );
}

bool MotorController::dispense ( int slot, int count ){
    if ( serial_fd > 0) {
        // Request a dispense
        std::cout << "Requesting dispense..." << std::endl;
        char directive { 'd' };
        char response { 'n' };
        size_t attempts { 0 };
        while ( response != 'a' ) {
            write ( serial_fd, &directive, 1 );
            read ( serial_fd, &response, 1 );

            if ( response != 'a' && ++attempts >= 10 ) {
                std::cout << "Motor controller communication failure"
                          << std::endl;
                return false;
            }
        }

        // Messages are 2 bytes long. The first byte is the slot number and the
        // second is the quantity.
        char c_slot = static_cast<char> ( slot );
        char c_count = static_cast<char> ( count );
        write ( serial_fd, &c_slot, 1 );
        write ( serial_fd, &c_count, 1 );

        // When the dispense is complete, we should get another ACK
        response = 'n';
        read ( serial_fd, &response, 1 );

        if ( response != 'a' ) {
            return false;
        }
    } else {
        std::cout << "Motor controller disconnected!" << std::endl;
        return false;
    }
    return true;
}
