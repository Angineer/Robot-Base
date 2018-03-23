#include "../include/communication.h"
#include "../include/inventory_manager.h"

#include <algorithm>

robot_comm::Client client("localhost", 5000);
int count_items = 2;

void shutdown(int signum){
    client.disconnect();
    std::cout << "\nStopping Test Client" << std::endl;
    exit(0);
}

void send_command(){
    char buffer[256];
    printf("Command: ");
    bzero(buffer,64);
    fgets(buffer,63,stdin);
    std::string command_str(buffer);

    // Remove new line
    command_str.erase(std::remove(command_str.begin(), command_str.end(), '\n'), command_str.end());

    robot_comm::Command command(command_str);

    client.send(command);
}

void send_order(){
    char buffer[256];
    std::vector<robot::Component> items;

    printf("-----New order-----\n");

    for (int i=0; i<count_items; i++){

        printf("Item type: ");
        bzero(buffer,64);
        fgets(buffer,63,stdin);
        std::string item_name(buffer);

        // Remove new line
        item_name.erase(std::remove(item_name.begin(), item_name.end(), '\n'), item_name.end());

        printf("Quantity: ");
        bzero(buffer,64);
        fgets(buffer,63,stdin);
        std::string quant_str(buffer);
        int quant = stoi(quant_str);
        robot::ItemType type(item_name);

        robot::Component item(type, quant);

        items.push_back(item);

    }

    robot::Order order(items);

    order.serialize();
    client.send(order);
}

void send_status(){
    char buffer[256];
    printf("Status code: ");
    bzero(buffer,64);
    fgets(buffer,63,stdin);
    std::string status_str(buffer);

    // Remove new line
    status_str.erase(std::remove(status_str.begin(), status_str.end(), '\n'), status_str.end());

    robot_comm::Status status(static_cast<robot_comm::StatusCode>(stoi(status_str)));

    client.send(status);
}

int main(int argc, char *argv[])
{
    // Kill client gracefully on ctrl+c
    std::signal(SIGINT, shutdown);

    char buffer[256];

    while (true){
        client.connect();
        std::cout << "Client connected" << std::endl;
        bool connect = true;

        while (connect){

            printf("What kind of message would you like to send?: ");
            bzero(buffer,64);
            fgets(buffer,63,stdin);

            if (buffer[0] == 'c') send_command();
            if (buffer[0] == 'o') send_order();
            if (buffer[0] == 's') send_status();


            std::cout << "Message sent" << std::endl;
        }
    }
}
