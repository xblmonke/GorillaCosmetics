#include "Types/Material/Material.hpp"
#include "quest-cosmetic-loader/shared/CosmeticLoader.hpp"
#include "logging.hpp"
#include "Utils/LightingUtils.hpp"

using namespace CosmeticsLoader;
using namespace UnityEngine;

extern Logger& getLogger();


namespace GorillaCosmetics
{
    Material::Material(std::string path) : manifest(MaterialManifest(path)) {};

    void Material::Load()
    {
        CosmeticLoader<MaterialManifest>* loader = new CosmeticLoader<MaterialManifest>(manifest, [&, loader](std::string name , Il2CppObject* obj){
            INFO("Loaded Material %s", this->manifest.get_descriptor().get_name().c_str());            
            this->object = (GameObject*)obj;
            LightingUtils::FixLighting(object, 0.7f);
            object->SetActive(false);
            //delete(loader);
        }, "_Material", il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));
    }

    const Config& Material::get_config() const
    {
        return manifest.get_config();
    }

    const Descriptor& Material::get_descriptor() const
    {
        return manifest.get_descriptor();
    }

    const Material::MaterialManifest& Material::get_manifest() const
    {
        return manifest;
    }
    
    GameObject* Material::get_material()
    {
        return object;
    }
}