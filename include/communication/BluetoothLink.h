#ifndef BLUETOOTH_LINK_H
#define BLUETOOTH_LINK_H

class BluetoothLink{
    public:
        BluetoothLink();
        
        void connect();
        void disconnect();
        int send(std::string message);
        int receive();
};

#endif
