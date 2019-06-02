#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <string>

class MotorController{
    int count_motors;
    int fd;
    int connect ( std::string device );
    int disconnect();

    public:
        MotorController();
        ~MotorController();

        void dispense(int slot, int count);
};

#endif
