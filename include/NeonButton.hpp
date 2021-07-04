#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Selector/HatRackSelector.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Collider.hpp"

DECLARE_CLASS_CODEGEN(GorillaCosmetics, NeonButton, UnityEngine::MonoBehaviour,
    DECLARE_METHOD(void, Awake);
    DECLARE_METHOD(void, OnTriggerEnter, UnityEngine::Collider* collider);
    DECLARE_METHOD(void, ToggleNeon);
    DECLARE_INSTANCE_FIELD(bool, canPress);
    
    REGISTER_FUNCTION(
        REGISTER_METHOD(Awake);
        REGISTER_METHOD(OnTriggerEnter);
        REGISTER_METHOD(ToggleNeon);
        REGISTER_FIELD(canPress);
    )
)