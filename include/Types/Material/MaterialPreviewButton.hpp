#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Material/Material.hpp"
#include "UnityEngine/Collider.hpp"
#include "GlobalNamespace/GorillaTriggerBox.hpp"

DECLARE_CLASS_CODEGEN(GorillaCosmetics, MaterialPreviewButton, GlobalNamespace::GorillaTriggerBox,
    DECLARE_METHOD(void, OnTriggerEnter, UnityEngine::Collider* collider);
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