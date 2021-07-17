#pragma once

#include "custom-types/shared/macros.hpp"
#include "Types/Selector/HatRackSelector.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Collider.hpp"

DECLARE_CLASS_CODEGEN(GorillaCosmetics, NeonButton, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnTriggerEnter, UnityEngine::Collider* collider);
    DECLARE_INSTANCE_METHOD(void, ToggleNeon);
    DECLARE_INSTANCE_FIELD(bool, canPress);
)