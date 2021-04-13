#pragma once
#include "beatsaber-hook/shared/utils/utils.h"
#include "Types/Config.hpp"
#include "quest-cosmetic-loader/shared/Descriptor.hpp"
#include "quest-cosmetic-loader/shared/Manifest.hpp"

#include "UnityEngine/GameObject.hpp"

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
            UnityEngine::GameObject* get_hat();

        protected:
            UnityEngine::GameObject* object = nullptr;
            HatManifest manifest;
    };
}