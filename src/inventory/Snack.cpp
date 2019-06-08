#include "inventory/Snack.h"

Snack::Snack ( std::string name ) :
    name ( name ) {
}

std::string Snack::get_name() const {
    return name;
}

bool Snack::operator< ( const Snack& other ) const {
    return other.get_name() < get_name();
}
