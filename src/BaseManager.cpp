#include "BaseManager.h"

BaseManager::BaseManager(Inventory* inventory, Server* server){
    // Set up inventory
    this->inventory = inventory;
    this->server = server;
    this->status = StatusCode::unknown;

    // Establish bluetooth link
    bl_link.connect();

    // Start heartbeat monitor thread
    thread t(bind(&BaseManager::listen_heartbeat, this));
    t.detach();
}
int BaseManager::handle_input(string input, string& response){
    char code = input[0];
    string message = input.substr(1, string::npos);

    if (code == 'c'){
        response = handle_command(message);
    }
    else if (code == 'o'){
        bool check = handle_order(message);
        if (check){
            response = "Order placed";
        }
        else{
            response = "Could not place order";
        }
    }
    else if (code == 'u'){
        response = handle_update(message);
    }
    else{
        response = "Unrecognized input!";
        return 1;
    }

    process_queue();

    return 0;
}
string BaseManager::handle_command(string input){
    if (input == "status"){
        return(robie_comm::status_to_string(status));
    }
    else if (input == "inv"){
        vector<Slot> existing = inventory->get_slots();
        stringstream inv_ss;

        for(auto it = existing.begin(); it != existing.end(); ++it){
            inv_ss << it - existing.begin() << ": " << it->type.get_name() << ", " << it->count << ", " << it->reserved_count;
            if(it != --existing.end()) inv_ss << "\n";
        }
        return inv_ss.str();
    }
    else if (input == "summary"){
        map<ItemType, int> existing = inventory->summarize_inventory();
        stringstream inv_ss;

        for(auto it = existing.begin(); it != existing.end(); ++it){
            inv_ss << it->second << " " << it->first.get_name();
            if(it != --existing.end()) inv_ss << "\n";
        }

        return inv_ss.str();
    }

    return "Command not recognized";
}
bool BaseManager::handle_order(string input){
    cout << "Processing order..." << endl;

    // Read in new order
    stringstream ss(input);
    map<ItemType, int> items;

    {
        cereal::BinaryInputArchive iarchive(ss); // Create an input archive

        iarchive(items); // Read the data from the archive
    }

    // Double check order validity
    bool valid_order = true;
    map<ItemType, int> existing = inventory->summarize_inventory();

    // Make sure items match inventory
    for(map<ItemType, int>::iterator it = items.begin(); it != items.end(); ++it){

        map<ItemType, int>::iterator item_in_inv = existing.find(it->first);

        if(item_in_inv == existing.end()){
            cout << "Order contains item not in inventory: " << it->first.get_name() << "!" << endl;
            valid_order = false;
            break;
        }
        if(item_in_inv->second < it->second){
            cout << "Insufficient quantity available: " + it->first.get_name() + "!" << endl;
            cout << "Asked for " << it->second << ", but inventory has " << item_in_inv->second << endl;
            valid_order = false;
            break;
        }
    }

    // If order is valid, reserve it
    if (valid_order){
        vector<Slot> slots = inventory->get_slots();

        int remaining, available;
        int start;
        for (auto it = items.begin(); it != items.end(); ++it){

            remaining = it->second;
            start = 0;

            // Loop through slots until this part of the order is fully reserved
            while(remaining > 0){
                for(int i = start; i < slots.size(); ++i){

                    // If we find a slot that matches, try to reserve there
                    if(slots[i].type == it->first){
                        start = i + 1;
                        available = slots[i].get_count_available();

                        // If sufficient items available, reserve remaining component here
                        if(available >= remaining){
                            inventory->reserve(i, remaining);
                            remaining = 0;
                        }
                        // Otherwise, reserve as much as we can and go to next slot
                        else{
                            inventory->reserve(i, available);
                            remaining -= available;
                        }
                        break;
                    }
                }
            }
        }

        // Add order to queue
        queue.emplace_back(items);

        cout << "New order placed!" << endl;
    }

    return valid_order;
}
string BaseManager::handle_update(string input){
    // Read in new order
    stringstream ss(input);
    int slot_id;
    ItemType new_type;
    int new_quant;

    {
        cereal::BinaryInputArchive iarchive(ss); // Create an input archive

        iarchive(slot_id, new_type, new_quant); // Read the data from the archive
    }

    inventory->set_type(slot_id, new_type);
    inventory->set_count(slot_id, new_quant);

    return "Inventory updated";
}
StatusCode BaseManager::get_status(){
    return status;
}
void BaseManager::process_queue(){
    // First, check current status
    // If robot is occupied, do nothing
    // If robot is ready to go and queue has orders, start processing them
    if (status == StatusCode::ready && queue.size() > 0){

        cout << "Processing queue with size " << queue.size() << "..." << endl;

        status = StatusCode::dispensing;
        bl_link.send("order");

        // Pop first order off queue
        Order curr_order = queue.front();
        queue.pop_front();

        map<ItemType, int> order = curr_order.get_order();

        vector<Slot> slots = inventory->get_slots();

        int remaining, available;
        int start;
        for (auto it = order.begin(); it != order.end(); ++it){

            remaining = it->second;
            start = 0;

            // Loop through slots until we find enough reserved items to fulfill this component
            while(remaining > 0){
                for(int i = start; i < slots.size(); ++i){

                    // If we find a slot that matches, try to dispense from there
                    if(slots[i].type == it->first){
                        start = i + 1;
                        available = slots[i].reserved_count;

                        // If sufficient items available, dispense remaining component from here
                        if(available >= remaining){
                            inventory->dispense(i, remaining);
                            inventory->reserve(i, -remaining);
                            remaining = 0;
                        }
                        // Otherwise, dispense as much as we can and go to next slot
                        else{
                            inventory->dispense(i, available);
                            inventory->reserve(i, -available);
                            remaining -= available;
                        }
                        break;
                    }
                }
            }
        }
        map<ItemType, int> curr_inv = inventory->summarize_inventory();

        cout << "After processing queue, the inventory status is:" << endl;
        for (map<ItemType, int>::iterator it = curr_inv.begin(); it != curr_inv.end(); ++it){
            cout << it->second << " x " << it->first.get_name() << endl;
        }
    }
}
void BaseManager::run(){

    // Create callback function that can be passed as argument
    function<int(string, string&)> callback_func(bind(&BaseManager::handle_input, this, placeholders::_1, placeholders::_2));

    // Run server and process callbacks
    server->serve(callback_func);
}
void BaseManager::shutdown(){
    bl_link.disconnect();
    this->server->shutdown();
}
void BaseManager::listen_heartbeat(){
    bool temp = true;

    // Check in with robie at 1 Hz
    while(true){
        this_thread::sleep_for(chrono::seconds(2));

        //bl_link.receive();

        // If status changed, process queue
        /*
        if (temp){
        */
            status = StatusCode::ready;
        /*
        }
        else{
            status = StatusCode::unavailable;
        }
        temp = !temp;

        cout << "Status changed to " << robie_comm::status_to_string(status) << endl;
        */
        process_queue();
    }
}
