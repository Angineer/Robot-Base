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

#include "InventoryMsg.h"
#include "Locations.h"

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

std::string BaseManager::handle_input ( std::string input )
{
    char code = input[0];
    std::string response;

    std::lock_guard<std::mutex> lock ( access_mutex );

    if ( code == 'c' ) {
        Command command { input };
        response = handle_command ( command );
    } else if ( code == 'o' ) {
        Order order { input };
        bool check = handle_order ( order );
        if (check){
            response = "Order placed";
        }
        else{
            response = "Could not place order";
        }
    } else if ( code == 'u' ){
        Update update { input };
        response = handle_update ( update );
    } else{
        response = "Unrecognized input!";
    }

    process_queue();

    return response;
}

std::string BaseManager::handle_command ( const Command &command )
{
    if ( command.get_command() == "status" ) {
        std::string stateStr = stateToString ( current_state );
        return "Robie's status is " + stateStr;
    } else if ( command.get_command() == "inventory" ){
        std::map<std::string, int> existing = inventory.summarize_inventory();
        InventoryMsg response;
        response.set_items ( existing );
        return response.serialize();
    } else if ( command.get_command() == "locations" ) {
        size_t loc_count = config.getConfig<size_t> ( "location_count" );
        std::map<int, std::string> locations;
        // Locations indices start at 1 because 0 is the home base
        for ( int i { 1 };
              i < config.getConfig<int> ( "location_count" ) + 1;
              ++i ) {
            locations.emplace (
                i,
                config.getConfig<std::string> (
                    "location_" + std::to_string ( i ) + "_name" ) );
        }
        Locations response;
        response.set_locations ( locations );
        return response.serialize();
    }

    return "Command not recognized";
}

bool BaseManager::handle_order ( const Order& order )
{
    std::cout << "Processing order..." << std::endl;

    // Read in new order
    std::map<std::string, int> items = order.get_items();

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
        queue.push_back ( order );

        std::cout << "New order placed!" << std::endl;
    }

    return valid_order;
}

std::string BaseManager::handle_update ( const Update& update )
{
    int slot_id;
    std::string new_type;
    int new_quant;
    std::tie ( slot_id, new_type, new_quant ) = update.get_update();

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

        std::map<std::string, int> order_contents = curr_order.get_items();

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
        Command status_inquiry;
        status_inquiry.set_command ( "status" );
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
