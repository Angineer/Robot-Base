#ifndef INVENTORY_H
#define INVENTORY_H

#include <functional>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <queue>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "communication.h"

using namespace std;

namespace robie_inv{
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

    class Slot
    {
        private:
            ItemType type;
            int count;
            int reserved_count;
        public:
            Slot();
            void add_items(int quantity);
            void change_type(ItemType new_type);
            ItemType get_type() const;
            int get_count_available() const;
            int get_count_total() const;
            void remove_items(int quantity);
            void reserve(int quantity);
    };

    class Inventory
    {
        private:
            vector<Slot> slots;
        public:
            Inventory(int count_slots);

            ItemType get_slot_type(int slot) const;
            int change_slot_type(int slot, ItemType new_type);
            int get_matching_slot(ItemType type) const; // Returns first slot that matches the provided type
            
            map<ItemType, int> get_current_inventory() const;

            void set_count(int slot, int count);
            void add(int slot, int count);
            void remove(int slot, int count);
            void reserve(int slot, int count);
    };

    class Manager
    {
        private:
            Inventory* inventory;
            Server* server;
            deque<Order> queue;
            StatusCode status;

            void dispense_item(int slot, float quantity);
            int handle_input(string input, string& response);
            string handle_command(string input);
            void handle_order(string input);
            string handle_update(string input);
            StatusCode get_status();
            void process_queue();
        public:
            Manager(Inventory* inventory, Server* server);
            void run();
            void shutdown();
    };

}

#endif
