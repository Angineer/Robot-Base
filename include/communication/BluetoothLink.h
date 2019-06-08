#ifndef BLUETOOTH_LINK_H
#define BLUETOOTH_LINK_H

#include <string>

class BluetoothLink{
    public:
        BluetoothLink();
        
        void connect();
        void disconnect();
        int send ( std::string message );
        std::string receive();
};

#endif
