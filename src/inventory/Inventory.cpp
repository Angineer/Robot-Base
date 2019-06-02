#include "inventory/Inventory.h"

Inventory::Inventory(int count_slots): controller(count_slots){
    slots.resize(count_slots);
}
vector<Slot> Inventory::get_slots() const{
    return slots;
}
void Inventory::set_type(int slot, ItemType type){
    slots[slot].type = type;
}
void Inventory::set_count(int slot, int count){
    slots[slot].count = count;
    slots[slot].reserved_count = 0;
}
void Inventory::dispense(int slot, int count){
    slots[slot].count -= count;
    controller.dispense(slot, count);
}
void Inventory::reserve(int slot, int count){
    slots[slot].reserved_count += count;
}
map<ItemType, int> Inventory::summarize_inventory() const{
    map<ItemType, int> curr_inventory;

    for(vector<Slot>::const_iterator it = slots.begin(); it != slots.end(); ++it){
        // If slot type already in map, combine them
        map<ItemType, int>::iterator existing = curr_inventory.find(it->type);
        if(existing != curr_inventory.end()){
            existing->second += it->get_count_available();
        }
        // Else, add a new entry for the type
        else{
            curr_inventory.insert(pair<ItemType, int>(it->type, it->get_count_available()));
        }
    }
    return curr_inventory;
};
