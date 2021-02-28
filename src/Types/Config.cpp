#include "Types/Config.hpp"

namespace GorillaCosmetics
{
    Config::Config(rapidjson::Value& val)
    {
        customColors = val["customColors"].GetBool();
        disableInPublicLobbies = val["disableInPublicLobbies"].GetBool();
    }

    bool Config::get_customColors() const
    {
        return customColors;
    }

    bool Config::get_disableInPublicLobbies() const
    {
        return disableInPublicLobbies;
    }
}