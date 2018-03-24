#include "../include/communication.h"
#include "../include/inventory.h"
#include <fstream>

#define UI_SOCKET 5000
#define DP_SOCKET 5001
#define INV_SIZE 3 // How many physical slots in inventory

using namespace std;

void load_inventory(string file_path, robie_inv::Inventory& inv){
    ifstream in_file;
    string item;
    int quant;

    cout << "Loading inventory from config file..." << endl;

    in_file.open(file_path);
    if (!in_file) {
        cerr << "Error reading file " << file_path << endl;
    }
    else{
        // TODO: Check that inventory has sufficient room for config
        int i = 0;
        while(in_file >> item >> quant){
            robie_inv::ItemType curr_type(item);
            inv.change_slot_type(i, curr_type);
            inv.add(curr_type, quant);
            i++;
        }
        in_file.close();
    }
}

void save_inventory(){
    // TODO
}

// Set up initial inventory with empty slots
robie_inv::Inventory base(INV_SIZE);

// Create server
robie_comm::Server server("localhost", UI_SOCKET);

// Set up the manager
robie_inv::Manager manager(&base, &server);

void shutdown(int signum){
    manager.shutdown();
    std::cout << "\nStopping Inventory Manager" << std::endl;
    exit(0);
}

int main()
{
    std::cout << "Starting Inventory Manager" << std::endl;

    // Load initial inventory settings from file
    load_inventory("/home/andy/Desktop/git/Robot-Base/inventory.conf", base);

    // Kill manager gracefully on ctrl+c
    std::signal(SIGINT, shutdown);

    // Run forever
    manager.run();
}
