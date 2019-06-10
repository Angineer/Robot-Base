#ifndef BLUETOOTH_SOCKET_H
#define BLUETOOTH_SOCKET_H

#include <netinet/in.h>
#include <string>

#include "communication/Socket.h"

// A Bluetooth socket
class BluetoothSocket : public Socket
{
public:
    // Constructor
    BluetoothSocket ( std::string device );

private:
    // BT device id
    int device;
};

#endif
