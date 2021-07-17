#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Material/Material.hpp"
#include "UnityEngine/Collider.hpp"
#include "GlobalNamespace/GorillaTriggerBox.hpp"

DECLARE_CLASS_CODEGEN(GorillaCosmetics, MaterialPreviewButton, GlobalNamespace::GorillaTriggerBox,
    DECLARE_INSTANCE_METHOD(void, OnTriggerEnter, UnityEngine::Collider* collider);
    DECLARE_INSTANCE_METHOD(void, UpdateMaterialValue);
    DECLARE_STATIC_FIELD(bool, canPress);
    public:
        Material* material;
)