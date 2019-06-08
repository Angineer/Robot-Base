#include "communication/BluetoothLink.h"

#include <bluetooth/bluetooth.h>
#include <chrono>
#include <iostream>
#include <thread>

BluetoothLink::BluetoothLink() {
    //foo
}
void BluetoothLink::connect(){
    std::cout << "Establishing bluetooth connection..." << std::endl;
}
void BluetoothLink::disconnect(){
    std::cout << "Stopping bluetooth connection..." << std::endl;
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
