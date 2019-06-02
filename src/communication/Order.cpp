#include "communication/Order.h"

Order::Order(map<ItemType, int> items){
    this->items = items;
}

map<ItemType, int> Order::get_order(){
    return this->items;
}

void Order::write_serial(){
    stringstream ss;
    {
        cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

        oarchive(this->items); // Write the data to the archive
    }

    serial = "o" + ss.str();
}
