#include "../include/inventory.h"
#include "../include/communication.h"

#include "cereal/cereal.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/map.hpp"

using robie_comm::StatusCode;

namespace robie_inv{

    // Item type
    ItemType::ItemType(){
        // Default constructor for Cereal
    }
    ItemType::ItemType(const string& name){
        this->name = name;
    }
    const string& ItemType::get_name() const{
        return this->name;
    }
    bool ItemType::operator<(const ItemType &item) const{
        return this->name < item.name;
    }
    bool ItemType::operator==(const ItemType &item) const{
        return this->name == item.name;
    }

    Order::Order(map<ItemType, int> items){
        this->items = items;
    }
    map<ItemType, int> Order::get_order(){
        return this->items;
    }
    void Order::write_serial(){
        stringstream ss;
        {
            cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

            oarchive(this->items); // Write the data to the archive
        }

        string serial_str = ss.str();

        this->serial = "o" + serial_str;
    }

// Slot
Slot::Slot(){
    this->type = ItemType("<empty>");
    this->count = 0;
    this->reserved_count = 0;
}
void Slot::add_items(int quantity){
    this->count += quantity;
}
void Slot::change_type(ItemType new_type){
    if (this->type == ItemType("Empty")){ // If slot has not been ininitialized
        cout << "Assigning item type " << new_type.get_name() << endl;
        this->type = new_type;
    }
    else if (this->count == 0){
        cout << "Changing item type from " + this->type.get_name() + " to " + new_type.get_name() << endl;
        this->type = new_type;
    }
    else{
        cout << "Cannot change type when there are items remaining!" << endl;
    }
}
ItemType Slot::get_type() const{
    return this->type;
}
int Slot::get_count_available() const{
    return this->count - this->reserved_count;
}
int Slot::get_count_total() const{
    return this->count;
}
void Slot::remove_items(int quantity){
    if (this->count >= quantity){
        this->count -= quantity;
    }
    else{
        cout << "Not enough items in slot!" << endl;
    }
}
void Slot::reserve(int quantity){
    if (this->count - this->reserved_count >= quantity){
        this->reserved_count += quantity;
    }
    else{
        cout << "Not enough items in slot!" << endl;
    }
}

// Inventory
Inventory::Inventory(int count_slots){
    slots.resize(count_slots);
}
ItemType Inventory::get_slot_type(int slot) const{
    return slots[slot].get_type();
}
int Inventory::change_slot_type(int slot, ItemType new_type){
    slots[slot].change_type(new_type);
}
int Inventory::get_matching_slot(ItemType type) const{
    for(vector<Slot>::const_iterator it = slots.begin(); it != slots.end(); ++it){
        if(it->get_type() == type) return it - slots.begin();
    }
    return -1;
}
map<ItemType, int> Inventory::get_current_inventory() const{
    map<ItemType, int> curr_inventory;

    for(vector<Slot>::const_iterator it = slots.begin(); it != slots.end(); ++it){
        // If slot type already in map, combine them
        map<ItemType, int>::iterator existing = curr_inventory.find(it->get_type());
        if(existing != curr_inventory.end()){
            existing->second += it->get_count_available();
        }
        // Else, add a new entry for the type
        else{
            curr_inventory.insert(pair<ItemType, int>(it->get_type(), it->get_count_available()));
        }
    }
    return curr_inventory;
};
void Inventory::add(ItemType type, int count){
    int slot_idx = get_matching_slot(type);

    if(slot_idx >= 0){
        slots[slot_idx].add_items(count);
    }
}
void Inventory::remove(ItemType type, int count){
    int slot_idx = get_matching_slot(type);

    if(slot_idx >= 0){
        slots[slot_idx].remove_items(count);
    }
}
void Inventory::reserve(ItemType type, int count){
    int slot_idx = get_matching_slot(type);

    if(slot_idx >= 0){
        slots[slot_idx].reserve(count);
    }
}

// Manager
Manager::Manager(Inventory* inventory, Server* server){
    // Set up inventory
    this->inventory = inventory;
    this->server = server;
}
void Manager::dispense_item(ItemType item, float quantity){
    cout << "Dispensing item!" << endl;
}
int Manager::handle_input(string input, string& response){
    char code = input[0];
    string message = input.substr(1, string::npos);

    if (code == 'c'){
        // Command
        response = handle_command(message);
    }
    else if (code == 'o'){
        // Order
        handle_order(message);
        response = "Order received";
    }
    else{
        response = "Unrecognized input!";
        return 1;
    }

    process_queue();

    return 0;
}
string Manager::handle_command(string input){
    if (input == "status"){
        switch(status){
            case(StatusCode::ready): return "Ready"; break;
            default: return "Not ready";
        }
    }
    else if (input == "inv"){
        map<ItemType, int> existing = inventory->get_current_inventory();
        stringstream inv_ss;

        for(auto it = existing.begin(); it != existing.end(); ++it){
            inv_ss << it->first.get_name() << ": " << it->second;
            if(it != --existing.end()) inv_ss << "\n";
        }
        return inv_ss.str();
    }

    return "Command not recognized";
}
void Manager::handle_order(string input){
    cout << "Processing order..." << endl;

    // Read in new order
    stringstream ss(input);
    map<ItemType, int> items;

    {
        cereal::BinaryInputArchive iarchive(ss); // Create an input archive

        iarchive(items); // Read the data from the archive
    }

    // Double check order validity
    bool valid_order = true;
    map<ItemType, int> existing = this->inventory->get_current_inventory();

    // Make sure items match inventory
    for(map<ItemType, int>::iterator it = items.begin(); it != items.end(); ++it){

        map<ItemType, int>::iterator item_in_inv = existing.find(it->first);

        if(item_in_inv == existing.end()){
            cout << "Order contains item not in inventory: " << it->first.get_name() << "!" << endl;
            valid_order = false;
            break;
        }
        if(item_in_inv->second < it->second){
            cout << "Insufficient quantity available: " + it->first.get_name() + "!" << endl;
            cout << "Asked for " << it->second << ", but inventory has " << item_in_inv->second << endl;
            valid_order = false;
            break;
        }
    }

    // If order is valid, reserve it
    if (valid_order){
        for (map<ItemType, int>::iterator it = items.begin(); it != items.end(); ++it){
            this->inventory->reserve(it->first, it->second);
        }

        // Add order to queue
        this->queue.emplace_back(items);

        cout << "New order placed!" << endl;
    }
}
StatusCode Manager::get_status(){
    return status;
}
void Manager::process_queue(){
    // First, check current status
    // If robot is occupied, do nothing
    // If robot is ready to go and queue has orders, start processing them
    if (this->status == StatusCode::ready && this->queue.size() > 0){

        cout << "Processing queue with size " << this->queue.size() << "..." << endl;

        // Pop first order off queue
        Order curr_order = this->queue.front();
        this->queue.pop_front();

        map<ItemType, int> order = curr_order.get_order();

        for (map<ItemType, int>::iterator it = order.begin(); it != order.end(); ++it){

            // Dispense items
            dispense_item(it->first, it->second);

            // Subtract inventory
            this->inventory->remove(it->first, it->second);

            // Unreserve quantities
            this->inventory->reserve(it->first, -it->second);
        }
    }

    map<ItemType, int> inventory = this->inventory->get_current_inventory();

    cout << "After processing queue, the inventory status is:" << endl;
    for (map<ItemType, int>::iterator it = inventory.begin(); it != inventory.end(); ++it){
        cout << it->first.get_name() << " has " << it->second << endl;
    }
}
void Manager::run(){

    // Create callback function that can be passed as argument
    function<int(string, string&)> callback_func(bind(&Manager::handle_input, this, placeholders::_1, placeholders::_2));

    // Run server and process callbacks
    server->serve(callback_func);
}
void Manager::shutdown(){
    // TODO: save inventory to file

    this->server->shutdown();
}

}
