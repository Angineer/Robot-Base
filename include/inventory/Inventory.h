#ifndef INVENTORY_H
#define INVENTORY_H

#include "MotorController.h"

#include <map>
#include <vector>

#include "inventory/Slot.h"

class Inventory
{
public:
    Inventory ( std::string inventory_file );

    std::vector<Slot> get_slots() const;
    void set_type ( int slot, std::string type );
    void add ( int slot, int count );

    bool dispense ( int slot, int count );
    void reserve ( int slot, int count );

    std::map<std::string, int> summarize_inventory() const;
private:
    std::vector<Slot> slots;
    MotorController controller;
};

#endif
