#ifndef INVENTORY_H
#define INVENTORY_H

class Inventory
{
private:
    vector<Slot> slots;
    MotorController controller;
public:
    Inventory(int count_slots);

    vector<Slot> get_slots() const;
    void set_type(int slot, ItemType type);
    void set_count(int slot, int count);

    void dispense(int slot, int count);
    void reserve(int slot, int count);

    map<ItemType, int> summarize_inventory() const;
};

#endif
