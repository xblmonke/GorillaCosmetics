#pragma once
#include <string>

struct config_t {
    std::string lastActiveMaterial = "";
    std::string lastActiveInfectedMaterial = "";
    std::string lastActiveHat = "";
    bool applyHatsToOtherPlayers = false;
    bool applyMaterialsToOtherPlayers = false;
    bool applyInfectedMaterialsToOtherPlayers = false;
    bool overrideNeon = true;
};

extern config_t config;

bool LoadConfig();
void SaveConfig();