#include "inventory/Inventory.h"

#include <fstream>

Inventory::Inventory ( std::string inventory_file )
{
    // Try to load inventory file
    std::cout << "Loading inventory from config file "
              << inventory_file << "..." << std::endl;

    std::ifstream in_file;
    in_file.open ( inventory_file );

    if ( in_file ) {
        std::string snack_type;
        int quant;
        while ( in_file >> snack_type >> quant ) {
            std::cout << "Adding slot: " << snack_type << ", " << quant << std::endl;
            slots.emplace_back ( snack_type );
            slots.back().add ( quant );
        }
        in_file.close();
    } else {
        // TODO: If it doesn't exist, create a new one
        std::cout << "Inventory file not found, created a new one"
                  << std::endl;
    }
}

std::vector<Slot> Inventory::get_slots() const{
    return slots;
}

void Inventory::set_type ( int slot, Snack type ){
    slots[slot].set_type ( type );
}

void Inventory::add ( int slot, int count ){
    slots[slot].add ( count );
}

void Inventory::dispense ( int slot, int count ){
    slots[slot].add ( -count );
    controller.dispense ( slot, count );
}

void Inventory::reserve ( int slot, int count ){
    slots[slot].reserve ( count );
}

std::map<Snack, int> Inventory::summarize_inventory() const{
    std::map<Snack, int> curr_inventory;

    for ( auto it = slots.begin(); it != slots.end(); ++it ){
        // If slot type already in map, combine them
        auto existing = curr_inventory.find ( it->get_type() );
        if(existing != curr_inventory.end()){
            existing->second += it->get_count_available();
        }
        // Else, add a new entry for the type
        else{
            curr_inventory.insert (
                    std::pair<Snack, int> ( it->get_type(), it->get_count_available() ) );
        }
    }
    return curr_inventory;
}
