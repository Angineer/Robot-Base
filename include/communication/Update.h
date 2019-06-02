#ifndef UPDATE_H
#define UPDATE_H

class Update: public Message{
    private:
        int slot_id;
        ItemType new_type;
        int new_quant;
    public:
        Update(int slot_id, ItemType new_type, int new_quant);
        void write_serial();
};

#endif
