#include "Types/Material/MaterialPreview.hpp"
#include "logging.hpp"


GorillaCosmetics::MaterialPreview::MaterialPreview(Material mat, Vector3 pos, float scale)
{
    INFO("Creating preview for %s", mat.get_descriptor().get_name().c_str());

    gameObject = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "GameObject", "CreatePrimitive", 0));

    CRASH_UNLESS(il2cpp_utils::RunMethod(gameObject, "set_layer", 18));

    Il2CppObject* transform = CRASH_UNLESS(il2cpp_utils::RunMethod(gameObject, "get_transform"));

    Vector3 localScale = {scale, scale, scale};
    Quaternion rotation = CRASH_UNLESS(il2cpp_utils::RunMethod<Quaternion>("UnityEngine", "Quaternion", "get_identity"));

    CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_localScale", localScale));
    CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_position", pos));
    CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_rotation", rotation));

    CRASH_UNLESS(il2cpp_utils::RunMethod(gameObject, "DontDestroyOnLoad", gameObject));

    Il2CppObject* collider = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(gameObject, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("UnityEngine", "Collider")}));
    CRASH_UNLESS(il2cpp_utils::RunMethod(collider, "set_isTrigger", true));
    button = CRASH_UNLESS(il2cpp_utils::RunGenericMethod<MaterialPreviewButton*>(gameObject, "AddComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("GorillaCosmetics", "MaterialPreviewButton")}));
    button->material = new Material(mat.get_manifest().get_filePath());
   
    Il2CppObject* theMat = mat.get_material();
    Il2CppObject* originalMat = nullptr;
    if (theMat)
    {
        INFO("Setting custom material");
        Il2CppObject* renderer = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(theMat, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("UnityEngine", "Renderer")}));
        originalMat = CRASH_UNLESS(il2cpp_utils::RunMethod(renderer, "get_material"));
    }
    else
    {
        originalMat = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "Resources", "Load", il2cpp_utils::createcsstr("objects/treeroom/materials/lightfur")));
        
    }

    Il2CppObject* otherRenderer = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(gameObject, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("UnityEngine", "Renderer")}));
    CRASH_UNLESS(il2cpp_utils::RunMethod(otherRenderer, "set_material", originalMat));
}