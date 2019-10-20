#include "Client.h"
#include "Command.h"
#include "Order.h"
#include "Update.h"

#include <algorithm>
#include <csignal>
#include <iostream>
#include <map>


void shutdown(int signum){
    std::cout << "Stopping Admin Client" << std::endl;
    exit(0);
}

void send_command( Client& client, std::string command_str){
    Command command(command_str);

    std::cout << client.send(command) << std::endl;
}

void send_order( Client& client ){
    std::string user_input; // Reading directly from cin was giving me issues
    int count_items;
    std::string name;
    int quant;
    std::map<std::string, int> items;

    std::cout << "-----New order-----\n";
    std::cout << "How many components? ";
    getline(std::cin, user_input);
    count_items = stoi(user_input);

    for (int i=0; i<count_items; i++){

        std::cout << "Item type: ";
        getline(std::cin, name);
        std::cout << "Quantity: ";
        getline(std::cin, user_input);
        quant = stoi(user_input);

        items.insert(std::pair<std::string, int>(name, quant));
    }

    Order order ( "wherever", items );

    std::cout << client.send(order) << std::endl;
}

void send_update( Client& client ){
    std::string user_input; // Reading directly from cin was giving me issues
    int slot_id;
    std::string new_type;
    int new_quant;

    std::cout << "which slot? ";
    getline(std::cin, user_input);
    slot_id = stoi(user_input);
    std::cout << "what type? ";
    getline(std::cin, new_type);
    std::cout << "how many? ";
    getline(std::cin, user_input);
    new_quant = stoi(user_input);

    Update update(slot_id, new_type, new_quant);

    std::cout << client.send(update) << std::endl;
}

int main(int argc, char *argv[])
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

            if (user_input == "status" || user_input == "inv"){
                send_command(client, user_input);
            }
            else if (user_input == "order") send_order( client );
            else if (user_input == "update") send_update( client );
            else if (user_input == "help"){
                std::cout << "Available commands (enter key will repeat last command):" << std::endl;
                std::cout << "status    Get robot status" << std::endl;
                std::cout << "inv       Get current inventory" << std::endl;
                std::cout << "order     Place a new order" << std::endl;
                std::cout << "update    Update an individual slot" << std::endl;
                std::cout << "help      Print this message" << std::endl;
                std::cout << "exit      Quit" << std::endl;
            }
            else if (user_input == "exit") shutdown(0);
            else std::cout << "Not recognized (type 'help' for commands)" << std::endl;
        }
    }
}
