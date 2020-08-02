#include <sstream>

template <typename T>
T BaseConfig::getConfig ( const std::string& key )
{
    std::string valueString = configMap[key];
    std::stringstream valueStream ( valueString );
    T valueT;
    valueStream >> valueT;
    return valueT;
}
