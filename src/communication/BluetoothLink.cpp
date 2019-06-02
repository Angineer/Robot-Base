#include "communication/BluetoothLink.h"

BluetoothLink::BluetoothLink(){
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
int BluetoothLink::receive(){
    std::cout << "Receiving message over bluetooth..." << std::endl;
    return 0;
}
