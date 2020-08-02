#include "BaseConfig.h"

BaseConfig::BaseConfig()
{
    configMap.emplace ( "location_count", "2" );
    configMap.emplace ( "location_0_name", "couch" );
    configMap.emplace ( "location_1_name", "table" );
}
