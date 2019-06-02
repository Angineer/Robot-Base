#ifndef ORDER_H
#define ORDER_H

class Order: public Message{
    private:
        map<ItemType, int> items;
    public:
        Order(map<ItemType, int> items);

        map<ItemType, int> get_order();
        void write_serial();
};

#endif
