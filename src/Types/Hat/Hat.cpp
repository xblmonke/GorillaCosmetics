#include "Types/Hat/Hat.hpp"
#include "quest-cosmetic-loader/shared/CosmeticLoader.hpp"
#include "logging.hpp"
#include "Utils/LightingUtils.hpp"
#include "Utils/UnityUtils.hpp"

using namespace CosmeticsLoader;
using namespace UnityEngine;

namespace GorillaCosmetics
{
    Hat::Hat(std::string path) : manifest(HatManifest(path)) {};

    void Hat::Load()
    {
        CosmeticLoader<HatManifest>* loader = new CosmeticLoader<HatManifest>(manifest, [&, loader](std::string name , Il2CppObject* obj){
            INFO("Loaded Hat %s", this->manifest.get_descriptor().get_name().c_str());            
            UnityUtils::SanitizePrefab((GameObject*)obj);
            this->object = (GameObject*)obj;
            LightingUtils::FixLighting(object, 0.8f);
            object->SetActive(false);
            //delete(loader);
        }, "_Hat", il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));
    }

    const Hat::Config& Hat::get_config() const
    {
        return manifest.get_config();
    }

    const Descriptor& Hat::get_descriptor() const
    {
        return manifest.get_descriptor();
    }

    const Hat::HatManifest& Hat::get_manifest() const
    {
        return manifest;
    }

    UnityEngine::GameObject* Hat::get_hat()
    {
        return object;
    }
}