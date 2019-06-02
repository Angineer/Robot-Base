#ifndef BASE_MANAGER_H
#define BASE_MANAGER_H

#include <deque>
#include <string>

#include "communication/BluetoothLink.h"
#include "communication/Order.h"
#include "communication/Server.h"
#include "communication/Status.h"
#include "inventory/Inventory.h"

class BaseManager
{
public:
    // Constructor
    // @param inventory_file The file that stores the current inventory
    //        contents
    // @post Creates inventory and server objects
    BaseManager ( std::string inventory_file );

    // Start the inventory server
    void run();

    // Stop the inventory server and save inventory contents to disk
    void shutdown();

private:
    int handle_input ( std::string input, std::string& response);
    std::string handle_command ( std::string input );
    bool handle_order ( std::string input );
    std::string handle_update ( std::string input );
    StatusCode get_status();
    void process_queue();
    void listen_heartbeat();

    Inventory inventory;
    Server server;
    BluetoothLink bl_link;
    std::deque<Order> queue;
    StatusCode status;
};

#endif
