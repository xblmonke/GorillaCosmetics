#pragma once
#include "Types/Hat/Hat.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/typedefs.h"

#include "UnityEngine/Collider.hpp"

namespace GorillaCosmetics
{
    class HatPreview 
    {
        public:
            HatPreview(Hat hat, UnityEngine::Collider* collider);
            
        private:
            UnityEngine::GameObject* gameObject = nullptr;
            
    };
}