#pragma once
#include "beatsaber-hook/shared/utils/utils.h"
#include "Types/Config.hpp"
#include "cosmeticsloader/shared/Descriptor.hpp"
#include "cosmeticsloader/shared/Manifest.hpp"

namespace GorillaCosmetics
{
    class Material
    {
        public:
            using Descriptor = CosmeticsLoader::Descriptor;
            using Config = GorillaCosmetics::Config;
            
            typedef CosmeticsLoader::Manifest<Config, Descriptor> MaterialManifest;
            
            void Load();
            Material(std::string path);
            const Config& get_config() const;
            const Descriptor& get_descriptor() const;
            const MaterialManifest& get_manifest() const;
            Il2CppObject* get_material();

        protected:
            Il2CppObject* object = nullptr;
            MaterialManifest manifest;
    };
}