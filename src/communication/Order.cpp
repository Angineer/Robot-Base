#include "communication/Order.h"

#include <sstream>

Order::Order ( std::map<Snack, int> items ){
    this->items = items;
}

std::map<Snack, int> Order::get_order(){
    return this->items;
}

void Order::write_serial(){
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

        oarchive(this->items); // Write the data to the archive
    }

    serial = "o" + ss.str();
}
