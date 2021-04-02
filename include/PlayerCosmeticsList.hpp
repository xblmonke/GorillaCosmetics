#pragma once

#include <map>
#include <string>
#include "config.hpp"

struct CosmeticsNames {
    std::string hat;
    std::string material;
    CosmeticsNames() : hat("custom:None"), material("default") {}; 
    CosmeticsNames(std::string hat, std::string material) : hat(hat), material(material) {};
};

typedef std::map<std::string, CosmeticsNames> PlayerToCosmeticsMap;

class PlayerCosmeticsList
{
    public:
        static std::string get_hat(std::string UserID)
        {
            if (UserID == "") return config.lastActiveHat;
            PlayerToCosmeticsMap::iterator it = playerToCosmeticsMap.find(UserID);

            if (it == playerToCosmeticsMap.end())
            {
                return "None";
            }
            else return it->second.hat;
        }

        static std::string get_material(std::string UserID)
        {
            if (UserID == "") return config.lastActiveMaterial;
            PlayerToCosmeticsMap::iterator it = playerToCosmeticsMap.find(UserID);

            if (it == playerToCosmeticsMap.end())
            {
                return "default";
            }
            else return it->second.material;
        }

        static void set_player(std::string UserID, std::string hat, std::string material)
        {
            playerToCosmeticsMap[UserID] = CosmeticsNames(hat, material);
        }
    private:
        static inline PlayerToCosmeticsMap playerToCosmeticsMap = {};
};