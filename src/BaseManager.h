#ifndef BASE_MANAGER_H
#define BASE_MANAGER_H

#include <deque>
#include <mutex>
#include <string>

#include "BaseConfig.h"
#include "Command.h"
#include "Client.h"
#include "Order.h"
#include "Server.h"
#include "inventory/InventoryManager.h"
#include "State.h"
#include "Update.h"

class BaseManager
{
public:
    // Constructor. Creates inventory and server objects.
    // @param inventory_file The file that stores the current inventory
    //        contents
    BaseManager ( std::string inventory_file );

    // Start the inventory server
    void run();

private:
    // Callback function that the inventory server will use for processing new
    // messages on its socket. Depending on the message type, it will call one
    // of the below specialized handle_* functions.
    std::string handle_input ( std::string input );

    // Handle a command
    std::string handle_command ( const Command& command );

    // Add a new order to the queue
    bool handle_order ( const Order& order );

    // Update the inventory levels
    std::string handle_update ( const Update& update );

    // Process all items currently in the queue; dispense product as necessary.
    // Must be called with access_mutex held.
    void process_queue();

    // Listen for a heartbeat from the mobile platform and set an error if 
    // anything goes wrong
    void listen_heartbeat();

    // Mutex for guarding the inventory, queue, and state variables
    std::mutex access_mutex;

    // The current inventory
    InventoryManager inventory;

    // The inventory server. It will listen for incoming requests and add them
    // to the queue for later processing.
    Server server;

    // The bluetooth connection to the mobile base
    Client mobile_client;

    // The queue of current orders. Should not be accessed unless holding
    // access_mutex.
    std::deque<Order> queue;

    // Mobile platform state. current_state is what is being reported by robie.
    // expected_state is what the base is expecting. Should not be accessed
    // unless holding access_mutex.
    State current_state;
    State expected_state;

    // User configuration
    BaseConfig config;
};

#endif
