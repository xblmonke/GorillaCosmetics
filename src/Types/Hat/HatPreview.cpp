#include "Types/Hat/HatPreview.hpp"
#include "Types/Hat/HatPreviewButton.hpp"
#include "logging.hpp"

#include "cosmeticsloader/shared/CosmeticLoader.hpp"
#include "typedefs.h"

#define run(value...) CRASH_UNLESS(il2cpp_utils::RunMethod(value))

using namespace CosmeticsLoader;

GorillaCosmetics::HatPreview::HatPreview(Hat hat, Il2CppObject* collider)
{
    INFO("Creating Hat preview for %s", hat.get_descriptor().get_name().c_str());
    Il2CppObject* theHat = hat.get_hat();
    if (theHat)
    {
        gameObject = run("UnityEngine", "Object", "Instantiate", theHat);
    }
    else // fake hat time
    {
        std::string path = "/sdcard/ModData/com.AnotherAxiom.GorillaTag/GorillaCosmetics/None/package.json";
        new CosmeticLoader(path, [&](std::string name, Il2CppObject* obj){
            gameObject = obj;
        }, "_Hat", il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));
    }

    INFO("Getting transform");
    Il2CppObject* transform = run(gameObject, "get_transform");
    Il2CppObject* colliderTransform = run(collider, "get_transform");
    Il2CppObject* colliderGO = run(collider, "get_gameObject");

    Vector3 pos = CRASH_UNLESS(il2cpp_utils::RunMethod<Vector3>(colliderTransform, "get_position"));
    Quaternion rot = CRASH_UNLESS(il2cpp_utils::RunMethod<Quaternion>(colliderTransform, "get_rotation"));
    Vector3 scale = {0.25f, 0.25f, 0.25f};

    run(transform, "set_localScale", scale);

    //run(transform, "SetParent", colliderTransform);

    run(gameObject, "set_layer", 18);
    run(transform, "set_position", pos);
    run(transform, "set_rotation", rot);
    
    
    run(gameObject, "DontDestroyOnLoad", gameObject);

    run(collider, "set_isTrigger", true);
    run(colliderGO, "set_layer", 18);

    INFO("adding button");
    HatPreviewButton* button = CRASH_UNLESS(il2cpp_utils::RunGenericMethod<HatPreviewButton*>(colliderGO, "AddComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("GorillaCosmetics", "HatPreviewButton")}));
    INFO("Setting hat");
    button->hat = new Hat(hat.get_manifest().get_filePath());
}