#ifndef SNACK_H
#define SNACK_H

#include <string>

#include "cereal/cereal.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/string.hpp"

class Snack
{

// For access to serialization function
friend cereal::access;

public:
    Snack () = default;
    Snack ( std::string name );
    std::string get_name() const;
    bool operator< ( const Snack& other ) const;

private:
    std::string name;

    template<class Archive>
    void serialize ( Archive & archive )
    {
        archive ( name ); 
    }
};

#endif
