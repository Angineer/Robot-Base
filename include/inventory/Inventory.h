#ifndef INVENTORY_H
#define INVENTORY_H

#include "MotorController.h"

#include <map>
#include <vector>

#include "inventory/Slot.h"
#include "inventory/Snack.h"

class Inventory
{
public:
    Inventory ( std::string inventory_file );

    std::vector<Slot> get_slots() const;
    void set_type ( int slot, Snack type );
    void add ( int slot, int count );

    void dispense ( int slot, int count );
    void reserve ( int slot, int count );

    std::map<Snack, int> summarize_inventory() const;
private:
    std::vector<Slot> slots;
    MotorController controller;
};

#endif
