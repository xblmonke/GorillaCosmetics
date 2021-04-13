#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Hat/Hat.hpp"

#include "UnityEngine/Collider.hpp"
#include "GlobalNamespace/GorillaTriggerBox.hpp"

DECLARE_CLASS_CODEGEN(GorillaCosmetics, HatPreviewButton, GlobalNamespace::GorillaTriggerBox,
    DECLARE_METHOD(void, OnTriggerEnter, UnityEngine::Collider* collider);
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