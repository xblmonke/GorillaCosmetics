#pragma once
#include "beatsaber-hook/shared/utils/utils.h"
#include "Types/Config.hpp"
#include "cosmeticsloader/shared/Descriptor.hpp"
#include "cosmeticsloader/shared/Manifest.hpp"

namespace GorillaCosmetics
{
    class Hat
    {
        public:
            using Descriptor = CosmeticsLoader::Descriptor;
            using Config = GorillaCosmetics::Config;
            
            typedef CosmeticsLoader::Manifest<Config, Descriptor> HatManifest;

            void Load();
            Hat(std::string path);
            const Config& get_config() const;
            const Descriptor& get_descriptor() const;
            const HatManifest& get_manifest() const;
            Il2CppObject* get_hat();

        protected:
            Il2CppObject* object = nullptr;
            HatManifest manifest;
    };
}