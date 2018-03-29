#include "../include/communication.h"
#include "../include/inventory.h"

#include <algorithm>
#include <iostream>

using namespace std;

robie_comm::Client client("localhost", 5000);
int count_items = 1;

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
    char buffer[256];
    map<robie_inv::ItemType, int> items;

    printf("-----New order-----\n");

    for (int i=0; i<count_items; i++){

        printf("Item type: ");
        bzero(buffer,64);
        fgets(buffer,63,stdin);
        string item_name(buffer);

        // Remove new line
        item_name.erase(remove(item_name.begin(), item_name.end(), '\n'), item_name.end());

        printf("Quantity: ");
        bzero(buffer,64);
        fgets(buffer,63,stdin);
        string quant_str(buffer);

        int quant = stoi(quant_str);
        robie_inv::ItemType type(item_name);

        items.insert(pair<robie_inv::ItemType, int>(type, quant));
    }

    robie_inv::Order order(items);

    client.send(order);
}

void send_update(){
    // TODO
}

int main(int argc, char *argv[])
{
    // Kill client gracefully on ctrl+c
    signal(SIGINT, shutdown);

    string user_input;

    while (true){
        cout << "Connecting to inventory server..." << endl;
        client.connect();
        bool connect = true;

        cout << "===================\n";
        cout << "Robie Admin Console\n";
        cout << "===================\n";
        while (connect){
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
                cout << "update    Update an item in the inventory" << endl;
                cout << "help      Print this message" << endl;
                cout << "exit      Quit" << endl;
            }
            else if (user_input == "exit") shutdown(0);
            else cout << "Not recognized (type 'help' for commands)" << endl;
        }
    }
}
