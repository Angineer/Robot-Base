#include "communication/BluetoothSocket.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <strings.h>

BluetoothSocket::BluetoothSocket ( std::string device ) {
    std::cout << "Establishing bluetooth connection..." << std::endl;

    // Get default controller
    int dev_id = hci_get_route ( NULL );
    int sock = hci_open_dev ( dev_id );
}

