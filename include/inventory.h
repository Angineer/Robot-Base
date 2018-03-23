#ifndef INVENTORY_H
#define INVENTORY_H

#include <functional>
#include <iostream>
#include <netinet/in.h>
#include <queue>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "communication.h"

namespace robie_inv{
    using robie_comm::Message;
    using robie_comm::Server;

    class ItemType{
        private:
            std::string name;
        public:
            ItemType();
            ItemType(const std::string& name);

            const std::string& get_name() const;
            template <class Archive>
                void serialize(Archive & archive){
                    archive( name );
                }
            bool operator<(const ItemType &item) const;
    };

    class Order: public Message{
        private:
            std::map<ItemType, int> items;
        public:
            Order(std::map<ItemType, int> items);

            int get_count(ItemType type);
            int get_num_components();
            void serialize();
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
            std::vector<Slot> slots;
        public:
            Inventory(int count_slots);
            void add(ItemType type, int count);
            void change_slot_type(unsigned int slot, ItemType new_type);
            int get_available_count(ItemType);
            const ItemType* get_type(unsigned int slot) const;
            std::vector<ItemType> get_available_types();
            void remove(unsigned int slot, unsigned int count);
            void reserve(unsigned int slot, unsigned int count);
    };

    class Manager
    {
        private:
            Inventory* inventory;
            Server* server;
            std::deque<Order> queue;
            int status;

            void dispense_item(unsigned int slot, float quantity);
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
