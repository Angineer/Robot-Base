#include "BaseConfig.h"

BaseConfig::BaseConfig()
{
    configMap.emplace ( "location_count", "2" );
    configMap.emplace ( "location_1_name", "couch" );
    configMap.emplace ( "location_2_name", "bench" );
    configMap.emplace ( "inventory_file", "/home/pi/robie.inv" );
}
