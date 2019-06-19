#include "inventory/Slot.h"

Slot::Slot ( std::string type ){
    this->type = type;
    count = 0;
    reserved_count = 0;
}

std::string Slot::get_type() const {
    return type;
}

void Slot::set_type ( const std::string& new_type ) {
    type = new_type;
}

int Slot::get_count() const {
    return count;
}

void Slot::add ( int quantity ) {
    count += quantity;
}

bool Slot::reserve ( int quantity ) {
    reserved_count += quantity;
}

int Slot::get_count_available() const{
    return count - reserved_count;
}
