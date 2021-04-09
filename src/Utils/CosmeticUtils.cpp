#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "Utils/CosmeticUtils.hpp"
#include "AssetLoader.hpp"    
#include "Types/Material/Material.hpp"
#include "Types/Hat/Hat.hpp"
#include "config.hpp"
#include "logging.hpp"
#include "typedefs.h"

#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils-fields.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

#include "UnityEngine/Material.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"

using namespace UnityEngine;

namespace GorillaCosmetics::CosmeticUtils
{
    // il2cpp'd
    void RefreshPlayer(Il2CppObject* rig)
    {
        if (rig)
        {
            int currentIndex = CRASH_UNLESS(il2cpp_utils::GetFieldValue<int>(rig, "currentMatIndex"));
            ChangeMaterial(rig, currentIndex);
            ChangeHat(rig);
        }
    }
    
    // il2cpp'd
    void RefreshLocalPlayer()
    {
        Il2CppObject* gorillaTagger = CRASH_UNLESS(il2cpp_utils::RunMethod("", "GorillaTagger", "get_Instance"));
        Il2CppObject* offlineVRRig = CRASH_UNLESS(il2cpp_utils::GetFieldValue(gorillaTagger, "offlineVRRig"));
        Il2CppObject* myVRRig = CRASH_UNLESS(il2cpp_utils::GetFieldValue(gorillaTagger, "myVRRig"));
        
        RefreshPlayer(offlineVRRig);
        RefreshPlayer(myVRRig);
    }
    
    // il2cpp'd
    void RefreshAllPlayers()
    {
        Array<Il2CppObject*>* allRigs = CRASH_UNLESS(il2cpp_utils::RunMethod<Array<Il2CppObject*>*>("UnityEngine", "GameObject", "FindObjectsOfType", il2cpp_utils::GetSystemType("", "VRRig")));
        int length = allRigs->Length();
        
        for(int i = 0; i < length; i++)
        {
            Il2CppObject* vrRig = allRigs->values[i];
            RefreshPlayer(vrRig);
        }
    }

    // il2cpp'd
    bool IsLocalPlayer(Il2CppObject* rig)
    {
        bool isOfflineVRRig = CRASH_UNLESS(il2cpp_utils::GetFieldValue<bool>(rig, "isOfflineVRRig"));
        bool isMyPlayer = CRASH_UNLESS(il2cpp_utils::GetFieldValue<bool>(rig, "isMyPlayer"));
        Il2CppObject* photonView = *il2cpp_utils::RunMethod(rig, "get_photonView");
        bool IsMine = photonView ? CRASH_UNLESS(il2cpp_utils::RunMethod<bool>(photonView, "get_IsMine")) : false;
        return isOfflineVRRig || isMyPlayer || IsMine;
    }

    bool IsMyPlayer(Il2CppObject* rig)
    {
        bool isMyPlayer = CRASH_UNLESS(il2cpp_utils::GetFieldValue<bool>(rig, "isMyPlayer"));
        Il2CppObject* photonView = *il2cpp_utils::RunMethod(rig, "get_photonView");
        bool IsMine = photonView ? CRASH_UNLESS(il2cpp_utils::RunMethod<bool>(photonView, "get_IsMine")) : false;
        return isMyPlayer || IsMine;
    }

    // il2cpp'd 
    void ChangeMaterial(Il2CppObject* rig, int materialIndex)
    {
        // this method is pretty much exclusively used for the local player now, so only the local player should get updated by this
        // for other players there is the variant that uses a material name to find which material to select
        if (!AssetLoader::IsLoaded()) AssetLoader::Load(); // make sure it's loaded
        if (!rig) return; // if rig is nullptr return
        if (materialIndex == 0) // if not it or infected
        {
            // default mat
            Material material = AssetLoader::SelectedMaterial();
            Il2CppObject* theMatObj = material.get_material();
            Il2CppObject* mainSkin = CRASH_UNLESS(il2cpp_utils::GetFieldValue(rig, "mainSkin"));
            Il2CppObject* instantiatedMat = nullptr;
            if (theMatObj)
            {
                Il2CppObject* renderer = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(theMatObj, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("UnityEngine", "Renderer")}));
                Il2CppObject* originalMat = CRASH_UNLESS(il2cpp_utils::RunMethod(renderer, "get_material"));
                instantiatedMat = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "Object", "Instantiate", originalMat));
            }
            else // default material time boi
            {
                INFO("Material was nullptr, setting default");
                Il2CppObject* originalMat = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "Resources", "Load", il2cpp_utils::createcsstr("objects/treeroom/materials/lightfur")));
                instantiatedMat = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "Object", "Instantiate", originalMat));
            }

            // custom colors changed, you need to get the color of materialsToChangeTo[0] now
            if (material.get_config().get_customColors())
            {
                INFO("Material Had custom colors, setting them");
                Array<Il2CppObject*>* materialsToChangeTo = *il2cpp_utils::GetFieldValue<Array<Il2CppObject*>*>(rig, "materialsToChangeTo");
                Il2CppObject* mat0 = materialsToChangeTo->values[0];
                Color color = *il2cpp_utils::RunMethod<Color>(mat0, "get_color");
                
                CRASH_UNLESS(il2cpp_utils::RunMethod(instantiatedMat, "set_color", color));
            }

            il2cpp_utils::RunMethod(mainSkin, "set_material", instantiatedMat);
        }
        else if (materialIndex > 0) // if infected or it
        {
            Material material = AssetLoader::SelectedInfectedMaterial();
            Il2CppObject* theMatObj = material.get_material();
            if (theMatObj)
            {
                Il2CppObject* renderer = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(theMatObj, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("UnityEngine", "Renderer")}));
                Il2CppObject* originalMat = CRASH_UNLESS(il2cpp_utils::RunMethod(renderer, "get_material"));
                Il2CppObject* instantiatedMat = UnityEngine::Object::Instantiate((UnityEngine::Material*)originalMat);
                
                Il2CppObject* mainSkin = CRASH_UNLESS(il2cpp_utils::GetFieldValue(rig, "mainSkin"));

                if (material.get_config().get_customColors())
                {
                    Il2CppObject* originalMat = CRASH_UNLESS(il2cpp_utils::RunMethod(mainSkin, "get_material"));
                    Color color = CRASH_UNLESS(il2cpp_utils::RunMethod<Color>(originalMat, "get_color"));
                    
                    CRASH_UNLESS(il2cpp_utils::RunMethod(instantiatedMat, "set_color", color));
                }
                CRASH_UNLESS(il2cpp_utils::RunMethod(mainSkin, "set_material", instantiatedMat));
            }
        }
    }

    // il2cpp'd 
    void ChangeMaterial(Il2CppObject* rig, int materialIndex, std::string materialName)
    {
        // this method is used for all players, and uses materialName to select the material to use
        if (!AssetLoader::IsLoaded()) AssetLoader::Load();
        if (!rig) return;

        // if this is actually hte local player, use the local player method instead
        if (IsLocalPlayer(rig))
        {
            ChangeMaterial(rig, materialIndex);
            return;
        }

        // get the selected material index
        int selectedMaterial = AssetLoader::SelectedMaterialFromConfig(materialName);
        // get that material
        Material material = AssetLoader::get_mat(selectedMaterial);

        // if not it or infected
        if (materialIndex == 0)
        {
            // default mat
            Il2CppObject* theMatObj = material.get_material();
            Il2CppObject* mainSkin = CRASH_UNLESS(il2cpp_utils::GetFieldValue(rig, "mainSkin"));
            Il2CppObject* instantiatedMat = nullptr;
            if (theMatObj)
            {
                Il2CppObject* renderer = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(theMatObj, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("UnityEngine", "Renderer")}));
                Il2CppObject* originalMat = CRASH_UNLESS(il2cpp_utils::RunMethod(renderer, "get_material"));
                instantiatedMat = UnityEngine::Object::Instantiate((UnityEngine::Material*)originalMat);
            }
            else // default material time boi
            {
                INFO("Material was nullptr, setting default");
                // Resources.Load<Material>("objects/treeroom/materials/lightfur");
                Il2CppObject* originalMat = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "Resources", "Load", il2cpp_utils::createcsstr("objects/treeroom/materials/lightfur")));
                instantiatedMat = UnityEngine::Object::Instantiate((UnityEngine::Material*)originalMat);
            }

            // also here, custom colors need to be done differently now
            if (material.get_config().get_customColors())
            {
                INFO("Material Had custom colors, setting them");
                Array<Il2CppObject*>* materialsToChangeTo = *il2cpp_utils::GetFieldValue<Array<Il2CppObject*>*>(rig, "materialsToChangeTo");
                Il2CppObject* mat0 = materialsToChangeTo->values[0];
                Color color = *il2cpp_utils::RunMethod<Color>(mat0, "get_color");

                CRASH_UNLESS(il2cpp_utils::RunMethod(instantiatedMat, "set_color", color));
            }

            il2cpp_utils::RunMethod(mainSkin, "set_material", instantiatedMat);
        }

        // skipping custom infected materials because that seems sketch to make custom (just select default and then boom you're unaware of what they are)
    }

    void ChangeHat(Il2CppObject* rig)
    {
        // local player change hat method
        if (!AssetLoader::IsLoaded()) AssetLoader::Load();
        if (!rig) return; 

        Il2CppObject* head = CRASH_UNLESS(il2cpp_utils::GetFieldValue(rig, "head"));
        Il2CppObject* rigTarget = CRASH_UNLESS(il2cpp_utils::GetFieldValue(head, "rigTarget"));
        static Il2CppString* hatName = il2cpp_utils::createcsstr("Hat", il2cpp_utils::StringType::Manual);
        
        // destroy original
        Il2CppObject* existingHat = *il2cpp_utils::RunMethod(rigTarget, "Find", hatName);
        if (existingHat)
        {
            Il2CppObject* hatGO = CRASH_UNLESS(il2cpp_utils::RunMethod(existingHat, "get_gameObject"));
            CRASH_UNLESS(il2cpp_utils::RunMethod(hatGO, "Destroy", hatGO));
        }

        if (IsLocalPlayer(rig))
        {
            Hat hat = AssetLoader::SelectedHat();
            std::string name = hat.get_descriptor().get_name();

            if (name != "None" && name != "none")
            {
                Il2CppObject* theHat = hat.get_hat();
                // if no hat pointer found just return and act as if it was none
                if (!theHat) return;
                Il2CppObject* hatObject = UnityEngine::Object::Instantiate((GameObject*)theHat);
                il2cpp_utils::RunMethod(hatObject, "SetActive", true);
                CRASH_UNLESS(il2cpp_utils::RunMethod(hatObject, "set_name", hatName));
                
                Il2CppObject* hatTransform = CRASH_UNLESS(il2cpp_utils::RunMethod(hatObject, "get_transform"));

                CRASH_UNLESS(il2cpp_utils::RunMethod(hatTransform, "SetParent", rigTarget));

                Vector3 scale = {0.25f, 0.25f, 0.25f};
                Vector3 pos = {0.0f, 0.365f, 0.04f};
                Quaternion rotation = CRASH_UNLESS(il2cpp_utils::RunMethod<Quaternion>("UnityEngine", "Quaternion", "get_identity"));
                Vector3 rot = {0.0f, 90.0f, 10.0f};

                CRASH_UNLESS(il2cpp_utils::RunMethod(hatTransform, "set_localScale", scale));
                CRASH_UNLESS(il2cpp_utils::RunMethod(hatTransform, "set_localPosition", pos));
                CRASH_UNLESS(il2cpp_utils::RunMethod(hatTransform, "set_localRotation", rotation));
                CRASH_UNLESS(il2cpp_utils::RunMethod(hatTransform, "Rotate", rot));
            }
        }
    }

    void ChangeHat(Il2CppObject* rig, std::string hatName)
    {
        // used for anything besides the local player, just makes it a bit easier to differentiate the 2
        if (!AssetLoader::IsLoaded()) AssetLoader::Load();
        if (!rig) return;

        // if local player, run the other method
        if (IsLocalPlayer(rig))
        {
            ChangeHat(rig);
            return;   
        }

        Il2CppObject* head = CRASH_UNLESS(il2cpp_utils::GetFieldValue(rig, "head"));
        Il2CppObject* rigTarget = CRASH_UNLESS(il2cpp_utils::GetFieldValue(head, "rigTarget"));
        static Il2CppString* hatTransformName = il2cpp_utils::createcsstr("Hat", il2cpp_utils::StringType::Manual);

        // destroy original
        Il2CppObject* existingHat = CRASH_UNLESS(il2cpp_utils::RunMethod(rigTarget, "Find", hatTransformName));
        if (existingHat)
        {
            Il2CppObject* hatGO = CRASH_UNLESS(il2cpp_utils::RunMethod(existingHat, "get_gameObject"));
            CRASH_UNLESS(il2cpp_utils::RunMethod(hatGO, "Destroy", hatGO));
        }

        int index = AssetLoader::SelectedHatFromConfig(hatName);
        Hat hat = AssetLoader::get_hat(index);
        std::string name = hat.get_descriptor().get_name();
        if (name != "None" && name != "none")
        {
            Il2CppObject* theHat = hat.get_hat();
            // if hat not found or something just return and act as if it is a None hat
            if (!theHat) return;
            
            Il2CppObject* hatObject = UnityEngine::Object::Instantiate((GameObject*)theHat);
            il2cpp_utils::RunMethod(hatObject, "SetActive", true);
            CRASH_UNLESS(il2cpp_utils::RunMethod(hatObject, "set_name", hatTransformName));
            
            Il2CppObject* hatTransform = CRASH_UNLESS(il2cpp_utils::RunMethod(hatObject, "get_transform"));

            CRASH_UNLESS(il2cpp_utils::RunMethod(hatTransform, "SetParent", rigTarget));

            Vector3 scale = {0.25f, 0.25f, 0.25f};
            Vector3 pos = {0.0f, 0.365f, 0.04f};
            
            Quaternion rotation = CRASH_UNLESS(il2cpp_utils::RunMethod<Quaternion>("UnityEngine", "Quaternion", "get_identity"));
            Vector3 rot = {0.0f, 90.0f, 10.0f};
            CRASH_UNLESS(il2cpp_utils::RunMethod(hatTransform, "set_localScale", scale));
            CRASH_UNLESS(il2cpp_utils::RunMethod(hatTransform, "set_localPosition", pos));
            CRASH_UNLESS(il2cpp_utils::RunMethod(hatTransform, "set_localRotation", rotation));
            CRASH_UNLESS(il2cpp_utils::RunMethod(hatTransform, "Rotate", rot));
        }
    }
}
