#ifndef INVENTORY_H
#define INVENTORY_H

#include "communication.h"

#include <iostream>
#include <map>
#include <netinet/in.h>
#include <queue>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

using namespace std;

namespace robie_inv{
    using robie_comm::BluetoothLink;
    using robie_comm::Message;
    using robie_comm::Server;
    using robie_comm::StatusCode;

    class ItemType{
        private:
            string name;
        public:
            ItemType();
            ItemType(const string& name);

            const string& get_name() const;
            template <class Archive>
                void serialize(Archive & archive){
                    archive( name );
                }
            bool operator<(const ItemType &item) const;
            bool operator==(const ItemType &item) const;
    };

    class Order: public Message{
        private:
            map<ItemType, int> items;
        public:
            Order(map<ItemType, int> items);

            map<ItemType, int> get_order();
            void write_serial();
    };

    class Update: public Message{
        private:
            int slot_id;
            ItemType new_type;
            int new_quant;
        public:
            Update(int slot_id, ItemType new_type, int new_quant);
            void write_serial();
    };

    struct Slot
    {
        public:
            ItemType type;
            int count;
            int reserved_count;
        
            Slot();
            int get_count_available() const;
    };

    class Inventory
    {
        private:
            vector<Slot> slots;
        public:
            Inventory(int count_slots);

            vector<Slot> get_slots() const;
            void set_type(int slot, ItemType type);
            void set_count(int slot, int count);

            void dispense(int slot, int count);
            void reserve(int slot, int count);

            map<ItemType, int> summarize_inventory() const;
    };

    class Manager
    {
        private:
            Inventory* inventory;
            Server* server;
            BluetoothLink bl_link;
            deque<Order> queue;
            StatusCode status;

            void dispense_item(int slot, float quantity);
            int handle_input(string input, string& response);
            string handle_command(string input);
            bool handle_order(string input);
            string handle_update(string input);
            StatusCode get_status();
            void process_queue();
            void listen_heartbeat();
        public:
            Manager(Inventory* inventory, Server* server);
            void run();
            void shutdown();
    };

}

#endif
