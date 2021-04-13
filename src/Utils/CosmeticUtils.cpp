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

#include "UnityEngine/SkinnedMeshRenderer.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Resources.hpp"

#include "GlobalNamespace/GorillaTagger.hpp"
#include "GlobalNamespace/VRMap.hpp"
#include "Photon/Pun/PhotonView.hpp"

using namespace UnityEngine;
using namespace Photon::Pun;

namespace GorillaCosmetics::CosmeticUtils
{
    // il2cpp'd
    void RefreshPlayer(GlobalNamespace::VRRig* rig)
    {
        if (rig)
        {
            ChangeMaterial(rig, rig->setMatIndex);
            ChangeHat(rig);
        }
    }
    
    // il2cpp'd
    void RefreshLocalPlayer()
    {
        GlobalNamespace::GorillaTagger* gorillaTagger = GlobalNamespace::GorillaTagger::get_Instance();
        
        RefreshPlayer(gorillaTagger->offlineVRRig);
        RefreshPlayer(gorillaTagger->myVRRig);
    }
    
    // il2cpp'd
    void RefreshAllPlayers()
    {
        Array<GlobalNamespace::VRRig*>* allRigs = Object::FindObjectsOfType<GlobalNamespace::VRRig*>();
        int length = allRigs->Length();
        for(int i = 0; i < length; i++)
        {
            GlobalNamespace::VRRig* vrRig = allRigs->values[i];
            RefreshPlayer(vrRig);
        }
    }

    // il2cpp'd
    bool IsLocalPlayer(GlobalNamespace::VRRig* rig)
    {
        bool isOfflineVRRig = rig->isOfflineVRRig;
        bool isMyPlayer = rig->isMyPlayer;
        PhotonView* photonView = rig->get_photonView();
        bool IsMine = photonView ? photonView->get_IsMine() : false;
        return isOfflineVRRig || isMyPlayer || IsMine;
    }

    // il2cpp'd 
    void ChangeMaterial(GlobalNamespace::VRRig* rig, int materialIndex)
    {
        // this method is pretty much exclusively used for the local player now, so only the local player should get updated by this
        // for other players there is the variant that uses a material name to find which material to select
        if (!AssetLoader::IsLoaded()) AssetLoader::Load(); // make sure it's loaded
        if (!rig) return; // if rig is nullptr return
        if (materialIndex == 0) // if not it or infected
        {
            // default mat
            GorillaCosmetics::Material material = AssetLoader::SelectedMaterial();
            GameObject* theMatObj = material.get_material();
            SkinnedMeshRenderer* mainSkin = rig->mainSkin;
            UnityEngine::Material* instantiatedMat = nullptr;
            if (theMatObj)
            {
                Renderer* renderer = theMatObj->GetComponent<Renderer*>();
                instantiatedMat = Object::Instantiate(renderer->get_material());
            }
            else // default material time boi
            {
                INFO("Material was nullptr, setting default");
                UnityEngine::Material* originalMat = Resources::Load<UnityEngine::Material*>(il2cpp_utils::createcsstr("objects/treeroom/materials/lightfur"));
                instantiatedMat = Object::Instantiate(originalMat);
            }

            // custom colors changed, you need to get the color of materialsToChangeTo[0] now
            if (material.get_config().get_customColors())
            {
                INFO("Material Had custom colors, setting them");
                UnityEngine::Material* mat0 = rig->materialsToChangeTo->values[0];
                Color color = mat0->get_color();
                
                instantiatedMat->set_color(color);
            }

            mainSkin->set_material(instantiatedMat);
        }
        else if (materialIndex > 0) // if infected or it
        {
            GorillaCosmetics::Material material = AssetLoader::SelectedInfectedMaterial();
            GameObject* theMatObj = material.get_material();
            if (theMatObj)
            {
                Renderer* renderer = theMatObj->GetComponent<Renderer*>();
                UnityEngine::Material* instantiatedMat = UnityEngine::Object::Instantiate(renderer->get_material());
                
                SkinnedMeshRenderer* mainSkin = rig->mainSkin;

                if (material.get_config().get_customColors())
                {
                    UnityEngine::Material* originalMat = mainSkin->get_material();
                    Color color = originalMat->get_color();
                    
                    instantiatedMat->set_color(color);
                }
                mainSkin->set_material(instantiatedMat);
            }
        }
    }

    // il2cpp'd 
    void ChangeMaterial(GlobalNamespace::VRRig* rig, int materialIndex, std::string materialName)
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
        GorillaCosmetics::Material material = AssetLoader::get_mat(selectedMaterial);

        // if not it or infected
        if (materialIndex == 0)
        {
            // default mat
            GameObject* theMatObj = material.get_material();
            SkinnedMeshRenderer* mainSkin = rig->mainSkin;
            UnityEngine::Material* instantiatedMat = nullptr;
            if (theMatObj)
            {
                Renderer* renderer = theMatObj->GetComponent<Renderer*>();
                instantiatedMat = Object::Instantiate(renderer->get_material());
            }
            else // default material time boi
            {
                INFO("Material was nullptr, setting default");
                // Resources.Load<Material>("objects/treeroom/materials/lightfur");
                UnityEngine::Material* originalMat = Resources::Load<UnityEngine::Material*>(il2cpp_utils::createcsstr("objects/treeroom/materials/lightfur"));
                instantiatedMat = Object::Instantiate(originalMat);
            }

            // also here, custom colors need to be done differently now
            if (material.get_config().get_customColors())
            {
                INFO("Material Had custom colors, setting them");
                UnityEngine::Material* mat0 = rig->materialsToChangeTo->values[0];
                Color color = mat0->get_color();
                
                instantiatedMat->set_color(color);
            }
            mainSkin->set_material(instantiatedMat);
        }
        // skipping custom infected materials because that seems sketch to make custom (just select default and then boom you're unaware of what they are)
    }

    void ChangeHat(GlobalNamespace::VRRig* rig)
    {
        // local player change hat method
        if (!AssetLoader::IsLoaded()) AssetLoader::Load();
        if (!rig) return; 

        GlobalNamespace::VRMap* head = rig->head;
        Transform* rigTarget = head->rigTarget;
        static Il2CppString* hatNameCS = il2cpp_utils::createcsstr("Hat", il2cpp_utils::StringType::Manual);
        
        // destroy original
        Transform* existingHat = rigTarget->Find(hatNameCS);
        if (existingHat)
        {
            GameObject* hatGO = existingHat->get_gameObject();
            Object::Destroy(hatGO);
        }

        if (IsLocalPlayer(rig))
        {
            Hat hat = AssetLoader::SelectedHat();
            std::string name = hat.get_descriptor().get_name();

            if (name != "None" && name != "none")
            {
                GameObject* theHat = hat.get_hat();
                // if no hat pointer found just return and act as if it was none
                if (!theHat) return;
                GameObject* hatObject = Object::Instantiate(theHat);
                hatObject->SetActive(true);
                hatObject->set_name(hatNameCS);
                
                Transform* hatTransform = hatObject->get_transform();

                hatTransform->SetParent(rigTarget);

                Vector3 pos = {0.0f, 0.365f, 0.4f};
                Vector3 rot = {0.0f, 90.0f, 10.0f};

                hatTransform->set_localScale(Vector3::get_one() * 0.25f);
                hatTransform->set_localPosition(pos);
                hatTransform->set_localRotation(Quaternion::get_identity());
                hatTransform->Rotate(rot);
            }
        }
    }

    void ChangeHat(GlobalNamespace::VRRig* rig, std::string hatName)
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

        GlobalNamespace::VRMap* head = rig->head;
        Transform* rigTarget = head->rigTarget;
        static Il2CppString* hatNameCS = il2cpp_utils::createcsstr("Hat", il2cpp_utils::StringType::Manual);
        
        // destroy original
        Transform* existingHat = rigTarget->Find(hatNameCS);
        if (existingHat)
        {
            GameObject* hatGO = existingHat->get_gameObject();
            Object::Destroy(hatGO);
        }

        int index = AssetLoader::SelectedHatFromConfig(hatName);
        Hat hat = AssetLoader::get_hat(index);
        std::string name = hat.get_descriptor().get_name();
        if (name != "None" && name != "none")
        {
            GameObject* theHat = hat.get_hat();
                // if no hat pointer found just return and act as if it was none
                if (!theHat) return;
                GameObject* hatObject = Object::Instantiate(theHat);
                hatObject->SetActive(true);
                hatObject->set_name(hatNameCS);
                
                Transform* hatTransform = hatObject->get_transform();

                hatTransform->SetParent(rigTarget);
                Vector3 pos = {0.0f, 0.365f, 0.4f};
                Vector3 rot = {0.0f, 90.0f, 10.0f};

                hatTransform->set_localScale(Vector3::get_one() * 0.25f);
                hatTransform->set_localPosition(pos);
                hatTransform->set_localRotation(Quaternion::get_identity());
                hatTransform->Rotate(rot);
        }
    }
}
