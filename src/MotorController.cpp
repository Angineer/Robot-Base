#include "MotorController.h"

#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

MotorController::MotorController() :
    count_motors ( 0 )
{
    int fd; // Serial port file descriptor
}

MotorController::~MotorController(){
    disconnect();
}

int MotorController::connect ( std::string device ){
    int fd = open ( device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
    if (fd == -1){
        std::cout << "Unable to connect to motor controller" << std::endl;
    }
    else{
        fcntl(fd, F_SETFL, 0);
        std::cout << "Motor controller connected" << std::endl;
    }

    // Set serial port options
    struct termios options;

    // Get current options
    tcgetattr(fd, &options);

    // Set baud rate
    cfsetispeed(&options, B19200);
    cfsetospeed(&options, B19200);

    // 8N1
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    /* set the options */
    tcsetattr(fd, TCSANOW, &options);

    return (fd);
}

int MotorController::disconnect(){
    close(fd);
}

void MotorController::dispense ( int slot, int count ){
    if(fd > 0){
        // Messages are 2 bytes long. The first byte is the slot number and the
        // second is the quantity.
        char c_slot ( slot );
        char c_count ( count );
        write ( fd, &c_slot, 1 );
        write ( fd, &c_count, 1 );

        // Read back an ack
        char ack;
        read ( fd, &ack, 1 );
    }
    else{
        std::cout << "Motor controller disconnected!" << std::endl;
    }
}
