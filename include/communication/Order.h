#ifndef ORDER_H
#define ORDER_H

#include <map>
#include "cereal/types/map.hpp"

#include "communication/Message.h"
#include "inventory/Snack.h"

class Order: public Message
{
    public:
        Order ( std::map<Snack, int> items );

        std::map<Snack, int> get_order();
        void write_serial();
    private:
        std::map<Snack, int> items;
};

#endif
