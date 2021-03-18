#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Material/Material.hpp"

DECLARE_CLASS(GorillaCosmetics, MaterialPreviewButton, "", "GorillaTriggerBox", sizeof(Il2CppObject) + sizeof(void*) * 3, 
    DECLARE_METHOD(void, OnTriggerEnter, Il2CppObject* collider);

    public:
        Material* material;    
    
    REGISTER_FUNCTION(MaterialPreviewButtton,
        REGISTER_METHOD(OnTriggerEnter);
    )
)