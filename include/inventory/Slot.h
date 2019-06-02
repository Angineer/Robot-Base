#ifndef SLOT_H
#define SLOT_H

#include "inventory/Snack.h"

class Slot
{
public:
    Slot ( Snack type );
    int get_count_available() const;
    Snack get_type() const;
    void set_type ( const Snack& new_type );
    int get_count() const;
    void add ( int quantity );
    bool reserve ( int quantity );

private:
    Snack type;
    int count;
    int reserved_count;
};

#endif
