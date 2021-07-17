#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Hat/Hat.hpp"

#include "UnityEngine/Collider.hpp"
#include "GlobalNamespace/GorillaTriggerBox.hpp"

DECLARE_CLASS_CODEGEN(GorillaCosmetics, HatPreviewButton, GlobalNamespace::GorillaTriggerBox,
    DECLARE_INSTANCE_METHOD(void, OnTriggerEnter, UnityEngine::Collider* collider);
    DECLARE_INSTANCE_METHOD(void, UpdateHatValue);
    DECLARE_STATIC_FIELD(bool, canPress);

    public:
        Hat* hat;    
)