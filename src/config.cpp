#include "config.hpp"
#include "logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

static ModInfo modInfo = {ID, VERSION};
config_t config;

Configuration& get_config()
{
    static Configuration config(modInfo);
    config.Load();
    return config;
}

void SaveConfig()
{
    INFO("Saving Config...");
    Configuration& configuration = get_config();
    configuration.config.RemoveAllMembers();
    rapidjson::Document::AllocatorType& allocator = configuration.config.GetAllocator();
    
    configuration.config.AddMember("lastActiveHat", rapidjson::Value(config.lastActiveHat.c_str(), config.lastActiveHat.size(), allocator), allocator);
    configuration.config.AddMember("lastActiveMaterial", rapidjson::Value(config.lastActiveMaterial.c_str(), config.lastActiveMaterial.size(), allocator), allocator);
    configuration.config.AddMember("lastActiveInfectedMaterial", rapidjson::Value(config.lastActiveInfectedMaterial.c_str(), config.lastActiveInfectedMaterial.size(), allocator), allocator);
    configuration.config.AddMember("applyHatsToOtherPlayers", config.applyHatsToOtherPlayers, allocator);
    configuration.config.AddMember("applyMaterialsToOtherPlayers", config.applyMaterialsToOtherPlayers, allocator);
    configuration.config.AddMember("applyInfectedMaterialsToOtherPlayers", config.applyInfectedMaterialsToOtherPlayers, allocator);

    configuration.Write();
    INFO("Saved Config!");
}

bool LoadConfig()
{
    INFO("Loading Config...");
    const Configuration& configuration = get_config();
    const rapidjson::Document& d = configuration.config;
    bool foundEverything = true;

    if (d.HasMember("lastActiveMaterial") && d["lastActiveMaterial"].IsString()) {
        config.lastActiveMaterial = d["lastActiveMaterial"].GetString();
    }
    else {
        foundEverything = false;
    }
    if (d.HasMember("lastActiveHat") && d["lastActiveHat"].IsString()) {
        config.lastActiveHat = d["lastActiveHat"].GetString();
    }
    else {
        foundEverything = false;
    }
    if (d.HasMember("lastActiveInfectedMaterial") && d["lastActiveInfectedMaterial"].IsString()) {
        config.lastActiveInfectedMaterial = d["lastActiveInfectedMaterial"].GetString();
    }
    else {
        foundEverything = false;
    }
    if (d.HasMember("applyHatsToOtherPlayers") && d["applyHatsToOtherPlayers"].IsBool()) {
        config.applyHatsToOtherPlayers = d["applyHatsToOtherPlayers"].GetBool();
    }
    else {
        foundEverything = false;
    }
    if (d.HasMember("applyMaterialsToOtherPlayers") && d["applyMaterialsToOtherPlayers"].IsBool()) {
        config.applyMaterialsToOtherPlayers = d["applyMaterialsToOtherPlayers"].GetBool();
    }
    else {
        foundEverything = false;
    }
    if (d.HasMember("applyInfectedMaterialsToOtherPlayers") && d["applyInfectedMaterialsToOtherPlayers"].IsBool()) {
        config.applyInfectedMaterialsToOtherPlayers = d["applyInfectedMaterialsToOtherPlayers"].GetBool();
    }
    else {
        foundEverything = false;
    }
    if (foundEverything) INFO("Config Loaded Succesfully!");
    return foundEverything;
}