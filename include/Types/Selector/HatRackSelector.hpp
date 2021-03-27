#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Material/Material.hpp"
#include "typedefs.h"

DECLARE_CLASS(GorillaCosmetics, HatRackSelector, "UnityEngine", "MonoBehaviour", sizeof(Il2CppObject) + sizeof(void*),
    DECLARE_METHOD(void, Awake);
    DECLARE_METHOD(void, Next);
    DECLARE_METHOD(void, Previous);
    DECLARE_METHOD(void, UpdateRack);
    DECLARE_INSTANCE_FIELD(List<Il2CppObject*>*, racks);
    DECLARE_INSTANCE_FIELD(int, selectedIndex);
    
    REGISTER_FUNCTION(HatRackSelector,
        REGISTER_METHOD(Awake);
        REGISTER_METHOD(Next);
        REGISTER_METHOD(Previous);
        REGISTER_METHOD(UpdateRack);
        REGISTER_FIELD(racks);
        REGISTER_FIELD(selectedIndex);
    )
)