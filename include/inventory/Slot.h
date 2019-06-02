#ifndef SLOT_H
#define SLOT_H

struct Slot
{
    public:
        ItemType type;
        int count;
        int reserved_count;
    
        Slot();
        int get_count_available() const;
};

#endif
