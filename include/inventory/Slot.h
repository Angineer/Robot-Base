#ifndef SLOT_H
#define SLOT_H

#include <string>

class Slot
{
public:
    Slot ( std::string type );
    int get_count_available() const;
    std::string get_type() const;
    void set_type ( const std::string& new_type );
    int get_count() const;
    void add ( int quantity );
    void reserve ( int quantity );

private:
    std::string type;
    int count;
    int reserved_count;
};

#endif
