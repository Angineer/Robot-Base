#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

class MotorController{
    int count_motors;
    int fd;
    int connect();
    int disconnect();

    public:
        MotorController(int count_motors);
        ~MotorController();

        void dispense(int slot, int count);
};

#endif
