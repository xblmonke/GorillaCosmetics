#pragma once

#include "UnityEngine/Material.hpp"
#include "UnityEngine/GameObject.hpp"

namespace LightingUtils
{
    void SetLightingStrength(UnityEngine::Material* material, float strength);
    bool SetExistingLightingStrength(UnityEngine::Material* material, float strength);
    float StrengthFromExistingColorFloat(float colorValue, float strength);

    void FixLighting(UnityEngine::GameObject* obj, float strength = 0.5f);
}