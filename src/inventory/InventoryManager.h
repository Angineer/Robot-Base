#ifndef INVENTORY_H
#define INVENTORY_H

#include "MotorController.h"

#include <map>
#include <vector>

#include "inventory/Slot.h"

class InventoryManager
{
public:
    // Constructor
    // @param file The file containing the saved inventory data
    InventoryManager ( std::string file );

    std::vector<Slot> get_slots() const;
    void set_type ( int slot, std::string type );
    void add ( int slot, int count );

    bool dispense ( int slot, int count );
    void reserve ( int slot, int count );

    std::map<std::string, int> summarize_inventory() const;
private:
    // Helper functions to manage reading from/writing to disk
    void load_from_disk();
    void save_to_disk();

    // The on-disk inventory data
    std::string inventory_file;

    // A representation of the different types of items that are available in
    // the inventory
    std::vector<Slot> slots;

    // Controls the dispenser motors
    MotorController controller;
};

#endif
