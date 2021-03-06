#include <algorithm>
#include <csignal>
#include <iostream>
#include <map>

#include "Client.h"
#include "Command.h"
#include "Inventory.h"
#include "Order.h"
#include "Update.h"

void shutdown(int signum){
    std::cout << "Stopping admin client" << std::endl;
    exit(0);
}

void send_command ( Client& client, std::string command_str )
{
    Command command;
    if ( command_str == "inv" ) {
        command.set_command ( "inventory" );
        Inventory inventory { client.send ( command ) };

        for ( const auto& item : inventory.get_items() ) {
            std::cout << item.second << " x " << item.first << std::endl;
        }
    } else {
        command.set_command ( command_str );
        std::cout << client.send ( command ) << std::endl;
    }
}

void send_order ( Client& client )
{
    std::map<std::string, int> items;

    std::cout << "-----New order-----" << std::endl;
    std::cout << "Leave item type empty to finish" << std::endl;
    while ( true ) {
        std::string name { "None" };
        std::string quant;

        std::cout << "Item type: ";
        getline(std::cin, name);

        if ( name.empty() ) {
            break;
        }

        std::cout << "Quantity: ";
        getline(std::cin, quant);

        items.insert( { name, stoi ( quant ) } );
    }

    if ( !items.empty() ) {
        Order order;
        order.set_location ( 0 );
        order.set_items ( items );

        std::cout << client.send ( order ) << std::endl;
    } else {
        std::cout << "(Ignoring empty order)" << std::endl;
    }
}

void send_update ( Client& client )
{
    std::string slot_id;
    std::string new_type;
    std::string new_quant;

    std::cout << "Which slot do you want to edit? ";
    getline(std::cin, slot_id);
    std::cout << "What will the slot's type be? ";
    getline(std::cin, new_type);
    std::cout << "How many items do you want to add? ";
    getline(std::cin, new_quant);

    Update update;
    update.set_update ( stoi ( slot_id ), new_type, stoi ( new_quant ) );

    std::cout << client.send ( update ) << std::endl;
}

int main ( int argc, char *argv[] )
{
    // Kill client gracefully on ctrl+c
    signal(SIGINT, shutdown);

    std::string user_input;
    std::string last_input;

    std::cout << "Connecting to inventory server..." << std::endl;
    Client client ( SocketType::IP, "localhost:5000" );

    if( true ){
        std::cout << "===================\n";
        std::cout << "Robie Admin Console\n";
        std::cout << "===================\n";
        while (true){
            std::cout << "admin@robie: ";

            getline(std::cin, user_input);

            if(user_input == "") user_input = last_input;
            last_input = user_input;

            if ( user_input == "status" || user_input == "inv" )
                send_command ( client, user_input );
            else if (user_input == "order") send_order( client );
            else if (user_input == "update") send_update( client );
            else if (user_input == "help") {
                std::cout << "Available commands (enter key will repeat last command):" << std::endl;
                std::cout << "status    Get robot status" << std::endl;
                std::cout << "inv       Get current inventory" << std::endl;
                std::cout << "order     Place a new order" << std::endl;
                std::cout << "update    Update an individual slot" << std::endl;
                std::cout << "help      Print this message" << std::endl;
                std::cout << "exit/quit Quit" << std::endl;
            }
            else if (user_input == "exit") shutdown(0);
            else if (user_input == "quit") shutdown(0);
            else std::cout << "Not recognized (type 'help' for commands)" << std::endl;
        }
    }
}
