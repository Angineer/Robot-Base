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

#include <Command.h>

BaseManager::BaseManager ( std::string inventory_file ) :
    inventory ( inventory_file ),
    server ( SocketType::IP ),
    mobile_client ( SocketType::BLUETOOTH, "B8:27:EB:E4:1C:3C" ),
    current_state ( State::IDLE ),
    expected_state ( State::IDLE )
{
    // Start heartbeat monitor thread
    std::thread t ( std::bind ( &BaseManager::listen_heartbeat, this ) );
    t.detach();
}

std::string BaseManager::handle_input ( std::string input ) {
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
                   << ": " << it->get_type()
                   << ", " << it->get_count()
                   << ", " << it->get_count_available();
            if ( it != --existing.end() ) inv_ss << "\n";
        }
        return inv_ss.str();
    } else if (input == "summary"){
        std::map<std::string, int> existing = inventory.summarize_inventory();
        std::stringstream inv_ss;

        for ( auto it = existing.begin(); it != existing.end(); ++it ){
            inv_ss << it->second << " "
                   << it->first;
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
    std::string location;
    std::map<std::string, int> items;

    {
        cereal::BinaryInputArchive iarchive(ss); // Create an input archive

        iarchive(location, items); // Read the data from the archive
    }

    // Double check order validity
    bool valid_order = true;
    std::map<std::string, int> existing = inventory.summarize_inventory();

    // Make sure items match inventory
    for ( auto it = items.begin(); it != items.end(); ++it ){

        auto item_in_inv = existing.find ( it->first );

        if ( item_in_inv == existing.end() ){
            std::cout << "Order contains item not in inventory: "
                      << it->first << "!" << std::endl;
            valid_order = false;
            break;
        }
        if ( it->second <= 0 ) {
            std::cout << "Unreasonable quantity requested: "
                      << it->second << " x " << it->first
                      << "!" << std::endl;
            valid_order = false;
            break;
        }
        if(item_in_inv->second < it->second){
            std::cout << "Insufficient quantity available: "
                      << it->first
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
                    if ( slots[i].get_type() == it->first ){
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
        queue.emplace_back ( location, items );

        std::cout << "New order placed!" << std::endl;
    }

    return valid_order;
}

std::string BaseManager::handle_update(std::string input){
    // Read in new order
    std::stringstream ss(input);
    int slot_id;
    std::string new_type;
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
    if (current_state == State::IDLE && queue.size() > 0){

        std::cout << "Processing queue with size " << queue.size()
                  << "..." << std::endl;

        // Pop first order off queue
        Order curr_order = queue.front();
        queue.pop_front();

        std::map<std::string, int> order_contents = curr_order.get_order();

        std::vector<Slot> slots = inventory.get_slots();

        int remaining, available;
        int start;
        bool success { true }; // Were we able to dispense the order?

        // Loop over each component of the order
        for (auto it = order_contents.begin(); it != order_contents.end(); ++it){
            remaining = it->second;

            // Loop through slots until we find enough reserved items to fulfill
            // this component
            for ( int i { 0 };
                  remaining > 0 && i < slots.size();
                  ++i ) {

                // If we find a slot that matches, try to dispense from there
                if ( slots[i].get_type() == it->first ){
                    available = slots[i].get_count_available();

                    if ( available >= remaining ) {
                        // If sufficient items available, dispense remaining component from here
                        if ( inventory.dispense(i, remaining) ) {
                            inventory.reserve(i, -remaining);
                            remaining = 0;
                        } else {
                            success = false;
                            break;
                        }
                    } else {
                        // Otherwise, dispense as much as we can and go to next slot
                        if ( inventory.dispense(i, available) ) {
                            inventory.reserve(i, -available);
                            remaining -= available;
                        } else {
                            success = false;
                            break;
                        }
                    }
                }
            }
            if ( !success ) break;
        }

        // The order has been dispensed, so Robie should be ready to go. Send
        // him the destination info and wait for him to get rolling.
        if ( success ) {
            expected_state = State::DELIVER;
            mobile_client.send ( curr_order );

            std::map<std::string, int> curr_inv = inventory.summarize_inventory();

            std::cout << "After processing queue, the inventory status is:" << std::endl;
            for (auto it = curr_inv.begin(); it != curr_inv.end(); ++it){
                std::cout << it->second << " x " << it->first << std::endl;
            }
        } else {
            std::cout << "An error occurred when processing the queue" << std::endl;
            current_state = State::ERROR;
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

void BaseManager::listen_heartbeat(){
    // Query for updates from the mobile robot every 2 seconds
    while ( true ) {
        std::this_thread::sleep_for ( std::chrono::seconds ( 2 ) );
        Command status_inquiry ( "status" );
        std::string msg = mobile_client.send ( status_inquiry );

        std::lock_guard<std::mutex> lock ( access_mutex );

        current_state = stringToState ( msg );

        //std::cout << "Current robot state: " << msg << std::endl;

        // If current state doesn't match expected, determine what action to
        // take
        if ( current_state != expected_state ) {
            if ( current_state == State::IDLE
                        && expected_state == State::DELIVER ) {
                // If Robie is lagging behind, just wait for him to transition
                // to the next state
                std::cout << "Waiting for transition..." << std::endl;
            } else if ( current_state == State::ERROR ) {
                // TODO: Deal with error states
            } else {
                // Update our current state based on what Robie reported
                std::cout << "State change detected: "
                          << stateToString ( expected_state ) 
                          << "->" 
                          << stateToString ( current_state )
                          << std::endl;
                expected_state = current_state;
                process_queue();
            }
        }
    }
}
