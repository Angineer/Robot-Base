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
    cfsetispeed(&options, B19200);
    cfsetospeed(&options, B19200);

    // 8N1
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    /* set the options */
    tcsetattr(serial_fd, TCSANOW, &options);
}

MotorController::~MotorController(){
    close ( serial_fd );
}

void MotorController::dispense ( int slot, int count ){
    if ( serial_fd > 0) {
        // Messages are 2 bytes long. The first byte is the slot number and the
        // second is the quantity.
        char c_slot ( slot );
        char c_count ( count );
        std::cout << "Motor contoller writing first byte" << std::endl;
        write ( serial_fd, &c_slot, 1 );
        std::cout << "Motor contoller writing second byte" << std::endl;
        write ( serial_fd, &c_count, 1 );

        // Read back an ack
        char ack;
        std::cout << "Motor contoller reading ack" << std::endl;
        read ( serial_fd, &ack, 1 );
        std::cout << "Motor contoller ACK received: " << ack << std::endl;
    }
    else {
        std::cout << "Motor controller disconnected!" << std::endl;
    }
}
