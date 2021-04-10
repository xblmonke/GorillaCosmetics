#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Material/Material.hpp"

DECLARE_CLASS(GorillaCosmetics, MaterialPreviewButton, "", "GorillaTriggerBox", sizeof(Il2CppObject) + sizeof(void*) * 2,
    DECLARE_METHOD(void, OnTriggerEnter, Il2CppObject* collider);
    DECLARE_METHOD(void, UpdateMaterialValue);
    DECLARE_STATIC_FIELD(bool, canPress);
    public:
        Material* material;    
    
    REGISTER_FUNCTION(
        REGISTER_METHOD(OnTriggerEnter);
        REGISTER_METHOD(UpdateMaterialValue);
        REGISTER_FIELD(canPress);
    )
)