#include "Types/Hat/HatPreview.hpp"
#include "Types/Hat/HatPreviewButton.hpp"
#include "logging.hpp"

#include "quest-cosmetic-loader/shared/CosmeticLoader.hpp"
#include "typedefs.h"

#include "UnityEngine/Object.hpp"
#include "UnityEngine/Transform.hpp"

using namespace CosmeticsLoader;
using namespace UnityEngine;

GorillaCosmetics::HatPreview::HatPreview(Hat hat, Collider* collider)
{
    INFO("Creating Hat preview for %s", hat.get_descriptor().get_name().c_str());
    GameObject* theHat = hat.get_hat();
    if (theHat)
    {
        gameObject = UnityEngine::Object::Instantiate(theHat);
    }
    else // fake hat time
    {
        std::string path = "/sdcard/ModData/com.AnotherAxiom.GorillaTag/Mods/GorillaCosmetics/None";
        new CosmeticLoader(path, [&](std::string name, Il2CppObject* obj){
            gameObject = (GameObject*)obj;
        }, "_Hat", il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));
    }
    if (!gameObject)
    {
        ERROR("gameObject was nullptr!");
        return;
    }
    
    INFO("Getting transform");

    gameObject->SetActive(true);
    Transform* transform = gameObject->get_transform();
    Transform* colliderTransform = collider->get_transform();
    transform->SetParent(colliderTransform);

    GameObject* colliderGO = collider->get_gameObject();

    transform->set_localScale(Vector3::get_one() * 0.7f);

    gameObject->set_layer(18);
    transform->set_localPosition(Vector3::get_zero());
    transform->set_rotation(colliderTransform->get_rotation());
    Object::DontDestroyOnLoad(gameObject);

    collider->set_isTrigger(true);
    colliderGO->set_layer(18);

    HatPreviewButton* button = colliderGO->AddComponent<HatPreviewButton*>();
    
    button->hat = new Hat(hat.get_manifest().get_filePath());
}