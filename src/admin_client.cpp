#include "../include/communication.h"
#include "../include/inventory.h"

#include <algorithm>
#include <csignal>
#include <iostream>

using namespace std;

robie_comm::Client client("localhost", 5000);

void shutdown(int signum){
    client.disconnect();
    cout << "Stopping Admin Client" << endl;
    exit(0);
}

void send_command(string command_str){
    robie_comm::Command command(command_str);

    cout << client.send(command) << endl;
}

void send_order(){
    string user_input; // Reading directly from cin was giving me issues
    int count_items;
    string name;
    int quant;
    map<robie_inv::ItemType, int> items;

    cout << "-----New order-----\n";
    cout << "How many components? ";
    getline(cin, user_input);
    count_items = stoi(user_input);

    for (int i=0; i<count_items; i++){

        cout << "Item type: ";
        getline(cin, name);
        cout << "Quantity: ";
        getline(cin, user_input);
        quant = stoi(user_input);

        robie_inv::ItemType type(name);

        items.insert(pair<robie_inv::ItemType, int>(type, quant));
    }

    robie_inv::Order order(items);

    cout << client.send(order) << endl;
}

void send_update(){
    string user_input; // Reading directly from cin was giving me issues
    int slot_id;
    string new_type;
    int new_quant;

    cout << "which slot? ";
    getline(cin, user_input);
    slot_id = stoi(user_input);
    cout << "what type? ";
    getline(cin, new_type);
    cout << "how many? ";
    getline(cin, user_input);
    new_quant = stoi(user_input);

    robie_inv::Update update(slot_id, robie_inv::ItemType(new_type), new_quant);

    cout << client.send(update) << endl;
}

int main(int argc, char *argv[])
{
    // Kill client gracefully on ctrl+c
    signal(SIGINT, shutdown);

    string user_input;

    cout << "Connecting to inventory server..." << endl;
    int success = client.connect();

    if(success == 0){
        cout << "===================\n";
        cout << "Robie Admin Console\n";
        cout << "===================\n";
        while (true){
            cout << "admin@robie: ";

            getline(cin, user_input);

            if (user_input == "status" || user_input == "inv"){
                send_command(user_input);
            }
            else if (user_input == "order") send_order();
            else if (user_input == "update") send_update();
            else if (user_input == "help"){
                cout << "Available commands:" << endl;
                cout << "status    Get robot status" << endl;
                cout << "inv       Get current inventory" << endl;
                cout << "order     Place a new order" << endl;
                cout << "update    Update an individual slot" << endl;
                cout << "help      Print this message" << endl;
                cout << "exit      Quit" << endl;
            }
            else if (user_input == "exit") shutdown(0);
            else cout << "Not recognized (type 'help' for commands)" << endl;
        }
    }
}
