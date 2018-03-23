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
    this->type = NULL;
    this->count = 0;
    this->reserved_count = 0;
}
void Slot::add_items(int quantity){
    this->count += quantity;
}
void Slot::change_type(const ItemType* new_type){
    if (this->type == NULL){ // If slot has not been ininitialized
        cout << "Assigning item type " + new_type->get_name() << endl;
        this->type = new_type;
    }
    else if (this->count == 0){
        cout << "Changing item type from " + this->type->get_name() + " to " + new_type->get_name() << endl;
        this->type = new_type;
    }
    else{
        cout << "Cannot change type when there are items remaining!" << endl;
    }
}
const ItemType* Slot::get_type() const{
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
const ItemType* Inventory::get_slot_type(int slot) const{
    return slots[slot].get_type();
}
int Inventory::change_slot_type(int slot, const ItemType* new_type){
    slots[slot].change_type(new_type);
}
map<ItemType, int> Inventory::get_current_inventory() const{
    //TODO
};
void Inventory::add(ItemType type, int count){
    slots[slot_map[type]].add_items(count);
}
void Inventory::remove(ItemType type, int count){
    slots[slot_map[type]].remove_items(count);
}
void Inventory::reserve(ItemType type, int count){
    slots[slot_map[type]].reserve(count);
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
void Manager::handle_input(char* input, int len){
    if (len > 0){

        if (input[0] == 'c'){
            // Command
            cout << "Command received" << endl;
            handle_command(input, len);
        }
        else if (input[0] == 'o'){
            // Order
            cout << "Order received" << endl;
            handle_order(input, len);
        }
        else if (input[0] == 's'){
            // Status
            cout << "Status update" << endl;
            handle_status(input, len);
        }

        process_queue();
    }
}
void Manager::handle_command(char* input, int len){
    string command(input);
    command = command.substr(1, string::npos);

    cout << command << endl;

    if (command == "status"){
        cout << "Current Status: ";
        switch(this->status){
            case(StatusCode::ready): cout << "Ready"; break;
            default: cout << "Not ready";
        }
        cout << endl;
    }
}
void Manager::handle_order(char* input, int len){
    cout << "Processing order..." << endl;

    // Read in new order
    stringstream ss;
    map<ItemType, int> items;

    //cout << len << endl;

    for (int i = 1; i < len; i++){
        ss << input[i];
    }

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
void Manager::handle_status(char* input, int len){
    // TODO: Unserialize status codes
    // string new_status(input);
    // new_status = new_status.substr(1, string::npos);

    // this->status = stoi(new_status);
}
void Manager::process_queue(){
    cout << "Processing queue with size " + to_string(this->queue.size()) + "..." << endl;
    // First, check current status
    // If robot is occupied, do nothing
    // If robot is ready to go and queue has orders, start processing them
    if (this->status == StatusCode::ready && this->queue.size() > 0){
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
            this->inventory->reserve(it->first, it->second);
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
    function<void(char*, int)> callback_func(bind(&Manager::handle_input, this, placeholders::_1, placeholders::_2));

    // Run server and process callbacks
    server->serve(callback_func);
}
void Manager::shutdown(){
    this->server->shutdown();
}

}
