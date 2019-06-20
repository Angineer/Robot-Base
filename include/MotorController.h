#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <string>

class MotorController{
public:
    MotorController ( std::string device );
    ~MotorController();

    void dispense ( int slot, int count );

private:
    int count_motors;
    int serial_fd;

};

#endif
