cmake_minimum_required ( VERSION 2.6 )
project ( Robot-Base )

# Need threads
find_package ( Threads )

# Cereal dependency
set ( CEREAL_INCLUDE_FOLDER "../cereal/include" CACHE FILEPATH "Cereal include folder" )
include_directories ( ${CEREAL_INCLUDE_FOLDER} )

# Robie library
add_library ( robie SHARED ./src/BaseManager.cpp
                           ./src/MotorController.cpp
                           ./src/communication/BluetoothLink.cpp
                           ./src/communication/Client.cpp
                           ./src/communication/Command.cpp
                           ./src/communication/Message.cpp
                           ./src/communication/Order.cpp
                           ./src/communication/Server.cpp
                           ./src/communication/Socket.cpp
                           ./src/communication/Status.cpp
                           ./src/communication/Update.cpp
                           ./src/inventory/Inventory.cpp
                           ./src/inventory/Slot.cpp
                           ./src/inventory/Snack.cpp )

install ( TARGETS robie LIBRARY DESTINATION /usr/lib )
target_include_directories ( robie PUBLIC ./include
                                   PUBLIC ./include/communication
                                   PUBLIC ./include/inventory )
target_link_libraries ( robie ${CMAKE_THREAD_LIBS_INIT} )

# Inventory server
add_executable ( inventory_server ./src/inventory_server.cpp )

set_property ( TARGET inventory_server PROPERTY CXX_STANDARD 11 )
target_link_libraries ( inventory_server robie ${CMAKE_THREAD_LIBS_INIT} )

# Admin Client
add_executable ( admin_client ./src/admin_client.cpp )

set_property ( TARGET admin_client PROPERTY CXX_STANDARD 11 )
target_link_libraries ( admin_client robie )
