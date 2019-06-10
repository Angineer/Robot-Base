#include "BaseManager.h"
#include "communication/Server.h"
#include "inventory/Inventory.h"

using namespace std;

int main ( int argc, char *argv[] )
{
    std::string inv_file;

    if ( argc > 1 ){
        inv_file = argv[1];
    }
    else{
        inv_file = "/home/pi/robie.inv";
    } 

    std::cout << "Starting Base Manager" << std::endl;

    // Create the manager
    BaseManager manager ( inv_file );

    std::cout << "Starting Inventory Manager" << std::endl;

    // Run forever
    manager.run();
}
