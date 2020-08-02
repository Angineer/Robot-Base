#include "inventory/InventoryManager.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "Inventory.h"

InventoryManager::InventoryManager ( std::string file ) :
    inventory_file ( file ),
    controller ( "/dev/ttyACM0" )
{
    // Try to load inventory file
    std::cout << "Loading inventory from file "
              << inventory_file << "..." << std::endl;
    load_from_disk();
}

std::vector<Slot> InventoryManager::get_slots() const{
    return slots;
}

void InventoryManager::set_type ( int slot, std::string type ){
    slots[slot].set_type ( type );
}

void InventoryManager::add ( int slot, int count ){
    slots[slot].add ( count );
}

bool InventoryManager::dispense ( int slot, int count ){
    slots[slot].add ( -count );
    return controller.dispense ( slot, count );
}

void InventoryManager::reserve ( int slot, int count ){
    slots[slot].reserve ( count );
}

std::map<std::string, int> InventoryManager::summarize_inventory() const{
    std::map<std::string, int> curr_inventory;

    for ( auto it = slots.begin(); it != slots.end(); ++it ){
        // If slot type already in map, combine them
        auto existing = curr_inventory.find ( it->get_type() );
        if(existing != curr_inventory.end()){
            existing->second += it->get_count_available();
        }
        // Else, add a new entry for the type
        else{
            curr_inventory.insert (
                    std::pair<std::string, int> ( it->get_type(), it->get_count_available() ) );
        }
    }
    return curr_inventory;
}

void InventoryManager::load_from_disk() {
    std::ifstream in_file;
    in_file.open ( inventory_file );

    if ( in_file ) {
        std::stringstream buffer;
        buffer << in_file.rdbuf();
        Inventory on_disk ( buffer.str() );

        for ( const auto& item : on_disk.get_items() ) {
            std::cout << "Adding slot: " << item.first << ", "
                      << item.second << std::endl;
            slots.emplace_back ( item.first );
            slots.back().add ( item.second );
        }

        in_file.close();
    } else {
        std::ofstream new_file ( inventory_file );
        Inventory empty;

        new_file << empty.serialize();

        std::cout << "Inventory file not found, created a new empty one"
                  << std::endl;
    }
}

void InventoryManager::save_to_disk() {
    Inventory inventory;
    inventory.set_items ( summarize_inventory() );

    std::ofstream save_file ( inventory_file );
    save_file << inventory.serialize();

    std::cout << "Inventory saved to disk: " << inventory_file << std::endl;
}
