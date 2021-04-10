#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Hat/Hat.hpp"

DECLARE_CLASS(GorillaCosmetics, HatPreviewButton, "", "GorillaTriggerBox", sizeof(Il2CppObject) + sizeof(void*) * 2,
    DECLARE_METHOD(void, OnTriggerEnter, Il2CppObject* collider);
    DECLARE_METHOD(void, UpdateHatValue);
    DECLARE_STATIC_FIELD(bool, canPress);

    public:
        Hat* hat;    
        
    REGISTER_FUNCTION(
        REGISTER_METHOD(OnTriggerEnter);
        REGISTER_METHOD(UpdateHatValue);
        REGISTER_FIELD(canPress);
    )
)