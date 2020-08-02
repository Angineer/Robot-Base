#include "BaseConfig.h"

BaseConfig::BaseConfig()
{
    configMap.emplace ( "home_id", "0" );
    configMap.emplace ( "couch_id", "1" );
}
