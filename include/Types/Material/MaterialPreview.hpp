#pragma once
#include "Types/Material/Material.hpp"
#include "Types/Material/MaterialPreviewButton.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "typedefs.h"

#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"

namespace GorillaCosmetics
{
    class MaterialPreview 
    {
        public:
            MaterialPreview(Material mat, UnityEngine::Transform* parent, UnityEngine::Vector3 localPos, float scale);
            
        private:
            UnityEngine::GameObject* gameObject = nullptr;
            MaterialPreviewButton* button = nullptr;
    };
}