#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Selector/HatRackSelector.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Collider.hpp"

DECLARE_CLASS_CODEGEN(GorillaCosmetics, HatRackSelectorButton, UnityEngine::MonoBehaviour,
    DECLARE_METHOD(void, Awake);
    DECLARE_METHOD(void, OnTriggerEnter, UnityEngine::Collider* collider);
    DECLARE_INSTANCE_FIELD(HatRackSelector*, selector);
    DECLARE_INSTANCE_FIELD(bool, next);
    DECLARE_STATIC_FIELD(bool, canPress);
    
    REGISTER_FUNCTION(
        REGISTER_METHOD(Awake);
        REGISTER_METHOD(OnTriggerEnter);
        REGISTER_FIELD(selector);
        REGISTER_FIELD(next);
        REGISTER_FIELD(canPress);
    )
)