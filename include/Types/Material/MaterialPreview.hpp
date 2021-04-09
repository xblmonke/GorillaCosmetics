#pragma once
#include "Types/Material/Material.hpp"
#include "Types/Material/MaterialPreviewButton.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "typedefs.h"

#include "UnityEngine/Vector3.hpp"

namespace GorillaCosmetics
{
    class MaterialPreview 
    {
        public:
            MaterialPreview(Material mat, Il2CppObject* parent, UnityEngine::Vector3 localPos, float scale);
            
        private:
            Il2CppObject* gameObject = nullptr;
            MaterialPreviewButton* button = nullptr;
    };
}