#include "inventory/Slot.h"

#include "inventory/Snack.h"

Slot::Slot ( Snack type ){
    this->type = type;
    count = 0;
    reserved_count = 0;
}

Snack Slot::get_type() const {
    return type;
}

void Slot::set_type ( const Snack& new_type ) {
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
