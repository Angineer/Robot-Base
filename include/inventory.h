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
    };

    class Order: public Message{
        private:
            map<ItemType, int> items;
        public:
            Order(map<ItemType, int> items);

            map<ItemType, int> get_order();
            void write_serial();
    };

    class Slot
    {
        private:
            const ItemType* type;
            int count;
            int reserved_count;
        public:
            Slot();
            void add_items(int quantity);
            void change_type(const ItemType* new_type);
            const ItemType* get_type() const;
            int get_count_available() const;
            int get_count_total() const;
            void remove_items(int quantity);
            void reserve(int quantity);
    };

    class Inventory
    {
        private:
            vector<Slot> slots;
            map<ItemType, int> slot_map;
        public:
            Inventory(int count_slots);

            const ItemType* get_slot_type(int slot) const;
            int change_slot_type(int slot, const ItemType* new_type);
            
            map<ItemType, int> get_current_inventory() const;

            void add(ItemType type, int count);
            void remove(ItemType type, int count);
            void reserve(ItemType type, int count);
    };

    class Manager
    {
        private:
            Inventory* inventory;
            Server* server;
            deque<Order> queue;
            StatusCode status;

            void dispense_item(ItemType item, float quantity);
            void get_robot_state();
            void handle_input(char* input, int len);
            void handle_command(char* input, int len);
            void handle_order(char* input, int len);
            void handle_status(char* input, int len);
            void process_queue();
        public:
            Manager(Inventory* inventory, Server* server);
            void run();
            void shutdown();
    };

}

#endif
