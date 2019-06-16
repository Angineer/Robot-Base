#include "communication/BluetoothSocket.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <strings.h>

BluetoothSocket::BluetoothSocket ( std::string device ) {
    std::cout << "Establishing bluetooth connection..." << std::endl;

    // Address to which we will connect via this socket
    sockaddr_rc* local_address = new sockaddr_rc;
    local_address->rc_family = AF_BLUETOOTH;
    str2ba ( device.c_str(), & ( local_address->rc_bdaddr ) );
    local_address->rc_channel = (uint8_t) 1;

    address = static_cast<void*> ( local_address );

    address_len = sizeof ( sockaddr_rc );

    // Create the socket
    socket_fd = socket ( AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM );

    if ( socket_fd < 0 ){
        std::cout << "ERROR opening socket" << std::endl;
    }
}

BluetoothSocket::~BluetoothSocket () {
    delete static_cast <sockaddr_rc*> ( address );
}

