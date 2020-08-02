cmake_minimum_required ( VERSION 2.6 )
project ( robot-base )

# Need threads
find_package ( Threads )

# Cereal dependency
set ( CEREAL_INCLUDE_FOLDER ../cereal/include CACHE FILEPATH "Cereal include folder" )

# Base server
add_executable ( base_server ./src/base_server.cpp
                             ./src/BaseConfig.cpp
                             ./src/BaseManager.cpp
                             ./src/MotorController.cpp
                             ./src/inventory/InventoryManager.cpp
                             ./src/inventory/Slot.cpp )
set_property ( TARGET base_server PROPERTY CXX_STANDARD 11 )
target_include_directories ( base_server PUBLIC ./src
                                         PUBLIC ../robot-comm/core
                                         PUBLIC ../robot-comm/msgs
                                         PUBLIC ${CEREAL_INCLUDE_FOLDER} )
target_link_libraries ( base_server robot-comm ${CMAKE_THREAD_LIBS_INIT} )

# Admin Client
add_executable ( admin_client ./src/admin_client.cpp )
set_property ( TARGET admin_client PROPERTY CXX_STANDARD 11 )
target_include_directories ( admin_client PUBLIC ./src
                                          PUBLIC ../robot-comm/core
                                          PUBLIC ../robot-comm/msgs )
target_link_libraries ( admin_client robot-comm )
