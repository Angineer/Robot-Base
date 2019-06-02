#ifndef UPDATE_H
#define UPDATE_H

#include "communication/Message.h"
#include "inventory/Snack.h"

class Update: public Message
{
    private:
        int slot_id;
        Snack new_type;
        int new_quant;
    public:
        Update(int slot_id, Snack new_type, int new_quant);
        void write_serial();
};

#endif
