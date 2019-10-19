cmake_minimum_required ( VERSION 2.6 )
project ( robot-base )

# Need threads
find_package ( Threads )

# Cereal dependency
set ( CEREAL_INCLUDE_FOLDER $ENV{HOME}/cereal/include CACHE FILEPATH "Cereal include folder" )

# Inventory server
add_executable ( inventory_server ./src/inventory_server.cpp
                                  ./src/BaseManager.cpp
                                  ./src/MotorController.cpp
                                  ./src/inventory/Inventory.cpp
                                  ./src/inventory/Slot.cpp )
set_property ( TARGET inventory_server PROPERTY CXX_STANDARD 11 )
target_include_directories ( inventory_server PUBLIC ./include
                                              PUBLIC ./include/inventory
                                              PUBLIC $ENV{HOME}/robot-comm/include
                                              PUBLIC ${CEREAL_INCLUDE_FOLDER} )
target_link_libraries ( inventory_server robot-comm ${CMAKE_THREAD_LIBS_INIT} )

# Admin Client
add_executable ( admin_client ./src/admin_client.cpp )
set_property ( TARGET admin_client PROPERTY CXX_STANDARD 11 )
target_include_directories ( admin_client PUBLIC ./include
                                          PUBLIC ./include/inventory
                                          PUBLIC $ENV{HOME}/robot-comm/include )
target_link_libraries ( admin_client robot-comm )
