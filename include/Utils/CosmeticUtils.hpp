#pragma once
#include "beatsaber-hook/shared/utils/utils.h"

#include "GlobalNamespace/VRRig.hpp"

namespace GorillaCosmetics::CosmeticUtils
{
    void RefreshPlayer(GlobalNamespace::VRRig* rig);
    void RefreshLocalPlayer();
    void RefreshAllPlayers();
    bool IsLocalPlayer(GlobalNamespace::VRRig* rig);
    void ChangeMaterial(GlobalNamespace::VRRig* rig, int materialIndex);
    void ChangeMaterial(GlobalNamespace::VRRig* rig, int materialIndex, std::string materialName);
    void ChangeHat(GlobalNamespace::VRRig* rig);
    void ChangeHat(GlobalNamespace::VRRig* rig, std::string hatName);
}