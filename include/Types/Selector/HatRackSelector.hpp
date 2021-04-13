#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Material/Material.hpp"
#include "typedefs.h"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

DECLARE_CLASS_CODEGEN(GorillaCosmetics, HatRackSelector, UnityEngine::MonoBehaviour,
    DECLARE_CTOR(ctor);
    DECLARE_METHOD(void, Next);
    DECLARE_METHOD(void, Previous);
    DECLARE_METHOD(void, UpdateRack);
    DECLARE_INSTANCE_FIELD(List<UnityEngine::GameObject*>*, racks);
    DECLARE_INSTANCE_FIELD(int, selectedIndex);
    
    REGISTER_FUNCTION(
        REGISTER_METHOD(ctor);
        REGISTER_METHOD(Next);
        REGISTER_METHOD(Previous);
        REGISTER_METHOD(UpdateRack);
        REGISTER_FIELD(racks);
        REGISTER_FIELD(selectedIndex);
    )
)