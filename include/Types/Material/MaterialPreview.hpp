#pragma once
#include "Types/Material/Material.hpp"
#include "Types/Material/MaterialPreviewButton.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/typedefs.h"

namespace GorillaCosmetics
{
    class MaterialPreview 
    {
        public:
            MaterialPreview(Material mat, Vector3 pos, float scale) {};
            
        private:
            Il2CppObject* gameObject = nullptr;
            MaterialPreviewButton* button = nullptr;
    };
}