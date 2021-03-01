#pragma once
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
namespace GorillaCosmetics
{
    class Config
    {    
        public:
            Config() {};
            Config(rapidjson::Value& val);

            bool get_customColors() const;
            bool get_disableInPublicLobbies() const;

        protected:
            bool customColors = true;
            bool disableInPublicLobbies = false;
    };
}