cmake_minimum_required (VERSION 2.6)
project (Robot-Base)

option(BUILD_TEST_CLIENT "Build test client" ON)

# Cereal dependency
set(CEREAL_INCLUDE_FOLDER "../cereal/include" CACHE FILEPATH "Cereal include folder")
include_directories(${CEREAL_INCLUDE_FOLDER})

# Robie library
add_library(robie SHARED ./src/communication.cpp
						 ./src/inventory.cpp)
install(TARGETS robie LIBRARY DESTINATION /usr/lib)

find_package(Threads)
target_link_libraries(robie ${CMAKE_THREAD_LIBS_INIT})

# Inventory server
add_executable(inventory_server ./src/inventory_server.cpp)

set_property(TARGET inventory_server PROPERTY CXX_STANDARD 11)
target_link_libraries(inventory_server robie)

# Test client (optional)
if(BUILD_TEST_CLIENT)
	add_executable(test_client ./src/test_client.cpp)

	set_property(TARGET test_client PROPERTY CXX_STANDARD 11)
	target_link_libraries(test_client robie)
endif()
