#include "../include/communication.h"
#include "../include/inventory.h"

#include <csignal>
#include <fstream>
#include <chrono>

#define UI_SOCKET 5000
#define INV_SIZE 3 // How many physical slots in inventory

using namespace std;

string inv_file;

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
            inv.set_type(i, curr_type);
            inv.set_count(i, quant);
            i++;
        }
        in_file.close();
    }
}

void save_inventory(string file_path, robie_inv::Inventory& inv){
    ofstream out_file;

    cout << "Saving inventory to config file..." << endl;

    vector<robie_inv::Slot> curr_inv = inv.get_slots();

    out_file.open(file_path);
    if (!out_file) {
        cerr << "Error writing file " << file_path << endl;
    }
    else{
        for (auto it = curr_inv.begin(); it != curr_inv.end(); ++it){
            out_file << it->type.get_name() << " " << it->count << endl;
        }
        out_file.close();
    }
}

// Set up initial inventory with empty slots
robie_inv::Inventory base(INV_SIZE);

// Create server
robie_comm::Server server("localhost", UI_SOCKET);

// Set up the manager
robie_inv::Manager manager(&base, &server);

void shutdown(int signum){
    std::cout << "Stopping Inventory Manager" << std::endl;

    // Save inventory to file
    save_inventory(inv_file, base);

    manager.shutdown();

    exit(0);
}

int main(int argc, char *argv[])
{
    if(argc > 1){
        inv_file = argv[1];
    }
    else{
        inv_file = "/home/andy/robie.inv";
    } 

    std::cout << "Starting Inventory Manager" << std::endl;

    // Load initial inventory settings from file
    load_inventory(inv_file, base);

    // Kill manager gracefully on ctrl+c
    std::signal(SIGINT, shutdown);

    // Run forever
    manager.run();
}
