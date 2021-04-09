#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Material/Material.hpp"
#include "typedefs.h"
#include "System/Collections/Generic/List_1.hpp"
#include "UnityEngine/GameObject.hpp"

DECLARE_CLASS(GorillaCosmetics, HatRackSelector, "UnityEngine", "MonoBehaviour", sizeof(Il2CppObject) + sizeof(void*),
    DECLARE_METHOD(void, Init);
    DECLARE_METHOD(void, Next);
    DECLARE_METHOD(void, Previous);
    DECLARE_METHOD(void, UpdateRack);
    DECLARE_INSTANCE_FIELD(System::Collections::Generic::List_1<UnityEngine::GameObject*>*, racks);
    DECLARE_INSTANCE_FIELD(int, selectedIndex);
    
    REGISTER_FUNCTION(HatRackSelector,
        REGISTER_METHOD(Init);
        REGISTER_METHOD(Next);
        REGISTER_METHOD(Previous);
        REGISTER_METHOD(UpdateRack);
        REGISTER_FIELD(racks);
        REGISTER_FIELD(selectedIndex);
    )
)