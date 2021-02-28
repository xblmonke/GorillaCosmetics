#pragma once
#include "beatsaber-hook/shared/utils/utils.h"

namespace GorillaCosmetics::CosmeticUtils
{
    void RefreshPlayer(Il2CppObject* __instance);
    void RefreshLocalPlayer();
    void RefreshAllPlayers();
    bool IsLocalPlayer(Il2CppObject* rig);
    void ChangeMaterial(Il2CppObject* __instance, int materialIndex);
    void ChangeHat(Il2CppObject* __instance);
}