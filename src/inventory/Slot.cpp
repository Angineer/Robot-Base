#include "inventory/Slot.h"

Slot::Slot(){
    this->type = ItemType("<empty>");
    this->count = 0;
    this->reserved_count = 0;
}

int Slot::get_count_available() const{
    return count - reserved_count;
}
