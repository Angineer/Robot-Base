#ifndef BLUETOOTH_LINK_H
#define BLUETOOTH_LINK_H

#include <string>

class BluetoothLink{
public:
    BluetoothLink();
    ~BluetoothLink();
    
    int send ( std::string message );
    std::string receive();

private:
    // BT device id
    int device;

    // BT socket
    int sock;
};

#endif
