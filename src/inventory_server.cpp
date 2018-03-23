#include "../include/communication.h"
#include "../include/inventory.h"

#define MESSAGE_SIZE 8
#define UI_SOCKET 5000
#define DP_SOCKET 5001

// Items that will be available for selection,
// read in from config file
robie_inv::ItemType apple("apple");
robie_inv::ItemType cracker("cracker");
robie_inv::ItemType granola("granola bar");
robie_inv::ItemType gummy("gummy bears");

// Create server
robie_comm::Server server("localhost", UI_SOCKET);

// Set up initial inventory with empty slots
const int inventory_size = 3;
robie_inv::Inventory base(inventory_size);

// Set up the manager
robie_inv::Manager manager(&base, &server);

void check_slot_quant(unsigned char slot){
    map<robie_inv::ItemType, int> inv = base.get_current_inventory();
    std::cout << "Slot " << std::to_string(slot) << " now has this many:" << inv.size() << std::endl;
}

void shutdown(int signum){
    manager.shutdown();
    std::cout << "\nStopping Inventory Manager" << std::endl;
    exit(0);
}

int main()
{
    std::cout << "Starting Inventory Manager" << std::endl;

    // Set up slots with assigned items
    base.change_slot_type(0, &apple);
    base.change_slot_type(1, &cracker);
    base.change_slot_type(2, &granola);

    base.add(apple, 5);
    base.add(cracker, 5);
    base.add(granola, 5);

    // Kill manager gracefully on ctrl+c
    std::signal(SIGINT, shutdown);

    // Run forever
    manager.run();
}
