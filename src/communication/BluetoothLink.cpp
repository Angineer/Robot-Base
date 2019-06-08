#include "communication/BluetoothLink.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <unistd.h>

BluetoothLink::BluetoothLink() {
    std::cout << "Establishing bluetooth connection..." << std::endl;

    // Get default controller
    device = hci_get_route ( NULL );
    sock = hci_open_dev ( device );
}

BluetoothLink::~BluetoothLink(){
    std::cout << "Stopping bluetooth connection..." << std::endl;

    close ( sock );
}

int BluetoothLink::send(std::string message){
    std::cout << "Sending message over bluetooth..." << std::endl;
    return 0;
}

std::string BluetoothLink::receive(){
    std::cout << "Receiving message over bluetooth..." << std::endl;
    std::this_thread::sleep_for ( std::chrono::seconds ( 2 ) );
    return "DELIVERING";
}
