#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Hat/Hat.hpp"

DECLARE_CLASS(GorillaCosmetics, HatPreviewButton, "", "GorillaTriggerBox", sizeof(Il2CppObject) + sizeof(void*) * 3,
    DECLARE_METHOD(void, OnTriggerEnter, Il2CppObject* collider);

    public:
        Hat* hat;    
        
    REGISTER_FUNCTION(HatPreviewButtton,
        REGISTER_METHOD(OnTriggerEnter);
    )
)