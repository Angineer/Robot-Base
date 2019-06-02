#include "communication/Update.h"

Update::Update(int slot_id, ItemType new_type, int new_quant){
    this->slot_id = slot_id;
    this->new_type = new_type;
    this->new_quant = new_quant;
}

void Update::write_serial(){
    stringstream ss;
    {
        cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

        oarchive(slot_id, new_type, new_quant); // Write the data to the archive
    }

    serial = "u" + ss.str();
}
