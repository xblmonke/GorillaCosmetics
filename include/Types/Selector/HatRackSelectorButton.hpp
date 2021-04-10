#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Selector/HatRackSelector.hpp"

DECLARE_CLASS(GorillaCosmetics, HatRackSelectorButton, "UnityEngine", "MonoBehaviour", sizeof(Il2CppObject) + sizeof(void*)/* * 2 + sizeof(bool)*/,
    DECLARE_METHOD(void, Awake);
    DECLARE_METHOD(void, OnTriggerEnter, Il2CppObject* collider);
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