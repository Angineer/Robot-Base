#ifndef BASE_MANAGER_H
#define BASE_MANAGER_H

class BaseManager
{
private:
    Inventory* inventory;
    Server* server;
    BluetoothLink bl_link;
    deque<Order> queue;
    StatusCode status;

    int handle_input(string input, string& response);
    string handle_command(string input);
    bool handle_order(string input);
    string handle_update(string input);
    StatusCode get_status();
    void process_queue();
    void listen_heartbeat();

public:
    BaseManager(Inventory* inventory, Server* server);
    void run();
    void shutdown();
};

#endif
