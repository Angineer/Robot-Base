#include "BaseManager.h"

#include <csignal>
#include <iostream>

void shutdown ( int signum ){
    std::cout << "Stopping base server" << std::endl;
    exit(0);
}

int main ( int argc, char *argv[] )
{
    // Kill server gracefully on ctrl+c
    signal ( SIGINT, shutdown );

    std::string inv_file;

    if ( argc > 1 ){
        inv_file = argv[1];
    }
    else{
        BaseConfig config;
        inv_file = config.getConfig<std::string> ( "inventory_file" );
    } 

    std::cout << "Starting Base Manager" << std::endl;

    // Create the manager
    BaseManager manager ( inv_file );

    // Run forever
    manager.run();
}
