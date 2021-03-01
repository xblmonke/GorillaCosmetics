#include "Types/Config.hpp"

namespace GorillaCosmetics
{
    Config::Config(rapidjson::Value& val)
    {
        customColors = val.HasMember("customColors") ? val["customColors"].GetBool() : true;
        disableInPublicLobbies = val.HasMember("disableInPublicLobbies") ? val["disableInPublicLobbies"].GetBool() : false;
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