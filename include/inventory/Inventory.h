#ifndef INVENTORY_H
#define INVENTORY_H

#include "MotorController.h"

#include <map>
#include <vector>

#include "inventory/Slot.h"

class Inventory
{
public:
    // Constructor
    // @param file The file containing the saved inventory data
    Inventory ( std::string file );

    // Destructor. Save the current inventory out to disk.
    ~Inventory();

    std::vector<Slot> get_slots() const;
    void set_type ( int slot, std::string type );
    void add ( int slot, int count );

    bool dispense ( int slot, int count );
    void reserve ( int slot, int count );

    std::map<std::string, int> summarize_inventory() const;
private:
    // The on-disk inventory data
    std::string inventory_file;

    // A representation of the different types of items that are available in
    // the inventory
    std::vector<Slot> slots;

    // Controls the dispenser motors
    MotorController controller;
};

#endif
