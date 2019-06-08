#include "BaseManager.h"

#include <chrono>
#include <iostream>
#include <functional>
#include <sstream>
#include <thread>

#include "cereal/cereal.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/map.hpp"

BaseManager::BaseManager ( std::string inventory_file ) :
    inventory ( inventory_file ),
    server ( "localhost", 5000),
    state ( State::IDLE )
{
    // Establish bluetooth link
    bl_link.connect();

    // Start heartbeat monitor thread
    std::thread t ( std::bind ( &BaseManager::listen_heartbeat, this ) );
    t.detach();
}

std::string BaseManager::handle_input ( std::string input ){
    char code = input[0];
    std::string message = input.substr(1, std::string::npos);
    std::string response;

    std::lock_guard<std::mutex> lock ( access_mutex );

    if (code == 'c'){
        response = handle_command(message);
    } else if (code == 'o'){
        bool check = handle_order(message);
        if (check){
            response = "Order placed";
        }
        else{
            response = "Could not place order";
        }
    } else if (code == 'u'){
        response = handle_update(message);
    } else{
        response = "Unrecognized input!";
    }

    process_queue();

    return response;
}

std::string BaseManager::handle_command ( std::string input ) {
    if ( input == "status" ){
        return "Not implemented yet";
    } else if (input == "inv"){
        std::vector<Slot> existing = inventory.get_slots();
        std::stringstream inv_ss;

        for ( auto it = existing.begin(); it != existing.end(); ++it ){
            inv_ss << it - existing.begin()
                   << ": " << it->get_type().get_name()
                   << ", " << it->get_count()
                   << ", " << it->get_count_available();
            if ( it != --existing.end() ) inv_ss << "\n";
        }
        return inv_ss.str();
    } else if (input == "summary"){
        std::map<Snack, int> existing = inventory.summarize_inventory();
        std::stringstream inv_ss;

        for ( auto it = existing.begin(); it != existing.end(); ++it ){
            inv_ss << it->second << " "
                   << it->first.get_name();
            if(it != --existing.end()) inv_ss << "\n";
        }
        return inv_ss.str();
    }

    return "Command not recognized";
}

bool BaseManager::handle_order ( std::string input ){
    std::cout << "Processing order..." << std::endl;

    // Read in new order
    std::stringstream ss(input);
    std::map<Snack, int> items;

    {
        cereal::BinaryInputArchive iarchive(ss); // Create an input archive

        iarchive(items); // Read the data from the archive
    }

    // Double check order validity
    bool valid_order = true;
    std::map<Snack, int> existing = inventory.summarize_inventory();

    // Make sure items match inventory
    for ( auto it = items.begin(); it != items.end(); ++it ){

        auto item_in_inv = existing.find ( it->first );

        if ( item_in_inv == existing.end() ){
            std::cout << "Order contains item not in inventory: "
                      << it->first.get_name() << "!" << std::endl;
            valid_order = false;
            break;
        }
        if(item_in_inv->second < it->second){
            std::cout << "Insufficient quantity available: "
                      << it->first.get_name()
                      << "!" << std::endl;
            std::cout << "Asked for " << it->second
                      << ", but inventory has " << item_in_inv->second
                      << std::endl;
            valid_order = false;
            break;
        }
    }

    // If order is valid, reserve it
    if (valid_order){
        std::vector<Slot> slots = inventory.get_slots();

        int remaining, available;
        int start;
        for (auto it = items.begin(); it != items.end(); ++it){

            remaining = it->second;
            start = 0;

            // Loop through slots until this part of the order is fully reserved
            while(remaining > 0){
                for(int i = start; i < slots.size(); ++i){

                    // If we find a slot that matches, try to reserve there
                    if ( slots[i].get_type().get_name() == it->first.get_name() ){
                        start = i + 1;
                        available = slots[i].get_count_available();

                        // If sufficient items available, reserve remaining component here
                        if(available >= remaining){
                            inventory.reserve(i, remaining);
                            remaining = 0;
                        }
                        // Otherwise, reserve as much as we can and go to next slot
                        else{
                            inventory.reserve(i, available);
                            remaining -= available;
                        }
                        break;
                    }
                }
            }
        }

        // Add order to queue
        queue.emplace_back(items);

        std::cout << "New order placed!" << std::endl;
    }

    return valid_order;
}

std::string BaseManager::handle_update(std::string input){
    // Read in new order
    std::stringstream ss(input);
    int slot_id;
    Snack new_type;
    int new_quant;

    {
        cereal::BinaryInputArchive iarchive(ss); // Create an input archive

        iarchive(slot_id, new_type, new_quant); // Read the data from the archive
    }

    inventory.set_type ( slot_id, new_type );
    inventory.add ( slot_id, new_quant );

    return "Inventory updated";
}

void BaseManager::process_queue(){
    // First, check current status
    // If robot is occupied, do nothing
    // If robot is ready to go and queue has orders, start processing them
    if (state == State::IDLE && queue.size() > 0){

        std::cout << "Processing queue with size " << queue.size() << "..." << std::endl;

        state = State::DISPENSE;
        bl_link.send ( "order" );

        // Pop first order off queue
        Order curr_order = queue.front();
        queue.pop_front();

        std::map<Snack, int> order = curr_order.get_order();

        std::vector<Slot> slots = inventory.get_slots();

        int remaining, available;
        int start;
        for (auto it = order.begin(); it != order.end(); ++it){

            remaining = it->second;
            start = 0;

            // Loop through slots until we find enough reserved items to fulfill this component
            while(remaining > 0){
                for(int i = start; i < slots.size(); ++i){

                    // If we find a slot that matches, try to dispense from there
                    if ( slots[i].get_type().get_name() == it->first.get_name() ){
                        start = i + 1;
                        available = slots[i].get_count_available();

                        // If sufficient items available, dispense remaining component from here
                        if(available >= remaining){
                            inventory.dispense(i, remaining);
                            inventory.reserve(i, -remaining);
                            remaining = 0;
                        }
                        // Otherwise, dispense as much as we can and go to next slot
                        else{
                            inventory.dispense(i, available);
                            inventory.reserve(i, -available);
                            remaining -= available;
                        }
                        break;
                    }
                }
            }
        }
        std::map<Snack, int> curr_inv = inventory.summarize_inventory();

        std::cout << "After processing queue, the inventory status is:" << std::endl;
        for (auto it = curr_inv.begin(); it != curr_inv.end(); ++it){
            std::cout << it->second << " x " << it->first.get_name() << std::endl;
        }
    }
}

void BaseManager::run(){

    // Create callback function that can be passed as argument
    std::function<std::string ( std::string )> callback_func (
            bind ( &BaseManager::handle_input, this, std::placeholders::_1 ) );

    // Run server and process callbacks
    server.serve ( callback_func );
}

void BaseManager::shutdown(){
    bl_link.disconnect();
    server.shutdown();
}

void BaseManager::listen_heartbeat(){
    // Listen for updates from the mobile robot
    while ( true ) {
        std::string msg = bl_link.receive();

        // If status changed, process queue
        if ( msg == "HOME" ) {
            std::lock_guard<std::mutex> lock ( access_mutex );
            state = State::IDLE;
            process_queue();
        }
    }
}
