#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <string>

#include <Arduino.h>

class MotorController{
public:
    MotorController ( std::string device );
    ~MotorController();

    bool dispense ( int slot, int count );

private:
    // The Arduino that runs the motors
    Arduino arduino;

    // How many motors are attached
    int count_motors;
};

#endif
