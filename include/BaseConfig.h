#ifndef BASE_CONFIG_H
#define BASE_CONFIG_H

#include <map>

class BaseConfig {
    public:
        BaseConfig();

        template <typename T>
        T getConfig ( const std::string& key );

    private:
        std::map<std::string, std::string> configMap;
};

#include "BaseConfig.tcc"
#endif
