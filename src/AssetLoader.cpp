#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "config.hpp"
#include "logging.hpp"
#include "Utils/FileUtils.hpp"
#include "AssetLoader.hpp"

#include "quest-cosmetic-loader/shared/CosmeticLoader.hpp"

#include "Types/Material/MaterialPreview.hpp"
#include "Types/Material/MaterialPreviewButton.hpp"
#include "Types/Hat/HatPreview.hpp"
#include "Types/Hat/HatPreviewButton.hpp"

#include "Types/Selector/HatRackSelector.hpp"
#include "Types/Selector/HatRackSelectorButton.hpp"

#include "NeonButton.hpp"

#include <algorithm>
#include <random>
#include <chrono>

#include <time.h>
#include "Utils/ZipUtils.hpp"
#include "typedefs.h"

#include "UnityEngine/PlayerPrefs.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

extern ModInfo modInfo;

static std::optional<std::string> dataDir;
std::string getDataDir(const ModInfo& info) {
    if (!dataDir) {
        dataDir = string_format(PERSISTENT_DIR, Modloader::getApplicationId().c_str());
        if (!direxists(*dataDir)) {
            mkpath(*dataDir);
        }
    }
    return *dataDir + info.id.c_str() + "/";
}

using namespace UnityEngine;
using namespace System::Collections::Generic;

namespace GorillaCosmetics
{
    //il2cpp'd
    Material AssetLoader::SelectedMaterial()
    {
        CRASH_UNLESS(Loaded);
        return GorillaMaterialObjects[selectedMaterial];
    }

    Material AssetLoader::get_mat(int index)
    {
        CRASH_UNLESS(GorillaMaterialObjects.size() > index);
        return GorillaMaterialObjects[index];
    }

    // il2cpp'd
    Material AssetLoader::SelectedInfectedMaterial()
    {
        CRASH_UNLESS(Loaded);
        return GorillaMaterialObjects[selectedInfectedMaterial];
    }

    // il2cpp'd
    Hat AssetLoader::SelectedHat()
    {
        CRASH_UNLESS(Loaded);
        return GorillaHatObjects[selectedHat];
    }

    Hat AssetLoader::get_hat(int index)
    {
        CRASH_UNLESS(GorillaHatObjects.size() > index);
        return GorillaHatObjects[index];
    }

    // il2cpp'd
    void AssetLoader::Load()
    {
        if (Loaded || Loading) 
        {
            if (Loading)
            {
                ERROR("Tried Loading assets while already loading!");
                while(Loading);
                INFO("Done Waiting For Load");
            }
            if (Loaded) return;
        }

        Loading = true;
        std::string folder = FolderPath;
        INFO("Loading things from %s", folder.c_str());

        // Load Materials
        std::string materialPath = folder + MaterialsLocation;
        std::vector<std::string> materialNames; 
        FileUtils::GetFilesInFolderPath("gmat", materialPath, materialNames);
        
        for (auto f : materialNames)
        {
            std::string path = materialPath + f;
            MaterialFiles.push_back(path);
        }   

        getLogger().info("Creating vector");
        GorillaMaterialObjects.push_back(Material(folder + "default"));

        getLogger().info("LoadItems");
        LoadItems<Material>(MaterialFiles, GorillaMaterialObjects);
        
        // Load Hats
        std::string hatPath = folder + HatsLocation;
        std::vector<std::string> hatNames; 
        FileUtils::GetFilesInFolderPath("hat", hatPath, hatNames);

        for (auto f : hatNames)
        {
            std::string path = hatPath + f;
            HatFiles.push_back(path);
        }
        
        GorillaHatObjects.push_back(Hat(folder + "None"));
        LoadItems<Hat>(HatFiles, GorillaHatObjects);

        // Get Values from the config
        selectedMaterial = SelectedMaterialFromConfig(config.lastActiveMaterial);
        if (!selectedMaterial) config.lastActiveMaterial = "";

        selectedInfectedMaterial = SelectedMaterialFromConfig(config.lastActiveInfectedMaterial);
        if (!selectedInfectedMaterial) config.lastActiveInfectedMaterial = "";
        
        // get hat from playerprefs instead
        static Il2CppString* hatProperty = il2cpp_utils::createcsstr("hatCosmetic", il2cpp_utils::StringType::Manual);
        Il2CppString* defaultHat = il2cpp_utils::createcsstr(config.lastActiveHat);
        Il2CppString* savedHatCS = PlayerPrefs::GetString(hatProperty, defaultHat);
        std::string savedHat = to_utf8(csstrtostr(savedHatCS));
        
        if (savedHat.find("custom:") != std::string::npos)
        {
            savedHat.erase(0, 7);
        }

        selectedHat = SelectedHatFromConfig(savedHat);
        if (!selectedHat) config.lastActiveHat = "";
        
        GameObject* theParent = *il2cpp_utils::New<GameObject*>();
        theParent->SetActive(true);
        Object::DontDestroyOnLoad(theParent);
        Transform* parentTransform = theParent->get_transform();

        GameObject* mirror = nullptr;
        Vector3 mirrorPos = {0.0f, 0.0f, 0.0f};
        // Load Mirror
        auto* mirrorLoader = new CosmeticsLoader::CosmeticLoader(folder + "Mirror", [&](std::string name, Il2CppObject* theMirror){
            mirror = (GameObject*)theMirror;
        }, "_Hat", il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));

        Transform* mirrorTransform = mirror->get_transform();
        mirrorTransform->SetParent(parentTransform);

        Il2CppString* mirrorString = il2cpp_utils::createcsstr("Level/treeroom/upper level/mirror");

        GameObject* gameMirror = UnityEngine::GameObject::Find(mirrorString);
        Transform* gameMirrorTransform = gameMirror->get_transform();
        gameMirror->SetActive(false);

        // use the game mirror pos as a base
        mirrorPos = gameMirrorTransform->get_position();

        // move it cause it's too low
        mirrorPos.y += 0.55f;
        Vector3 mirrorRot = {0.21f, -153.2f, -4.6f};

        mirrorTransform->set_localScale(Vector3::get_one() * 0.25f);
        mirrorTransform->set_position(mirrorPos);
        mirrorTransform->set_rotation(Quaternion::Euler(mirrorRot));
        
        Object::DontDestroyOnLoad(mirror);

        // Load Hat Rack
        GameObject* HatRack = nullptr;
        auto* rackLoader = new CosmeticsLoader::CosmeticLoader(folder + "HatRack", [&](std::string name, Il2CppObject* rack){
            HatRack = (GameObject*)rack;
        }, "_Hat", il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));

        Transform* rackTransform = HatRack->get_transform();
        rackTransform->SetParent(parentTransform);
        Vector3 rackScale = {0.25f, 0.25f, 0.25f};

        // this pos is offset from the mirror
        Vector3 rackPos = mirrorPos;
        rackPos.y -= 0.42f;
        rackPos.x -= 0.45f;
        rackPos.z -= 0.7f;
        Vector3 rackRot = {0.0f, -70.0f, 0.0f};

        rackTransform->set_localScale(Vector3::get_one() * 0.25f);
        rackTransform->set_position(rackPos);
        rackTransform->set_rotation(Quaternion::Euler(rackRot));

        Object::DontDestroyOnLoad(HatRack);

        // how many hats
        int hatCount = GorillaHatObjects.size();
        // how many hats are on the last rack
        int lastRackCount = hatCount % 6;
        // how many racks are needed for that amount of hats
        int rackCount = (hatCount / 6) + (lastRackCount == 0 ? 0 : 1);

        INFO("Hat count: %d, rack count: %d, last rack hat count: %d", hatCount, rackCount, lastRackCount);
        // some names to use in finding transforms
        Il2CppString* rackName = il2cpp_utils::createcsstr("HatRack");
        Il2CppString* selectionName = il2cpp_utils::createcsstr("Selection");
        
        // the actual rack transform, so this contains the 6 hats
        // mesh is now seperate, so only the colliders are being switched out
        Transform* actualRackTransform = rackTransform->Find(rackName);
        Transform* selectionTransform = rackTransform->Find(selectionName);
        GameObject* actualRack = actualRackTransform->get_gameObject();
        GameObject* selectionGO = selectionTransform->get_gameObject();

        HatRackSelector* rackSelector = *il2cpp_utils::RunGenericMethod<HatRackSelector*>(HatRack, "AddComponent", std::vector<Il2CppClass*>{classof(HatRackSelector*)});
        // if more than 1 rack is going to be used
        if (rackCount > 1)
        {
            // setup the arrow buttons
            Array<Collider*>* buttonColliders = selectionGO->GetComponentsInChildren<Collider*>(true);

            for (int i = 0; i < buttonColliders->Length(); i++)
            {
                INFO("Selector Button %d", i);
                Collider* collider = buttonColliders->values[i];
                GameObject* colliderGO = collider->get_gameObject();
                HatRackSelectorButton* button = colliderGO->AddComponent<HatRackSelectorButton*>();

                // what selector does it apply to?
                button->selector = rackSelector;
                // should be trigger
                collider->set_isTrigger(true);
                // correct layer for buttons
                colliderGO->set_layer(18);
            }
        }
        else
        {  
            // if not needed just delete it
            Object::Destroy(selectionGO);
        }

        // for the amount of racks needed
        for (int i = 0; i < rackCount; i++)
        {
            int hatsLeft = hatCount - (i * 6);
            if (hatsLeft > 6) // if not the last rack
            {
                GameObject* theRack = Object::Instantiate(actualRack);
                Object::DontDestroyOnLoad(theRack);
                Transform* theRackTransform = theRack->get_transform();
                theRackTransform->SetParent(rackTransform, false);

                // add to the rack selector list of racks
                rackSelector->racks->Add(theRack);
                
                Array<Collider*>* hatPosColliders = theRack->GetComponentsInChildren<Collider*>();
                
                // randomize order
                std::vector<int> index = {};
                for (int k = 0; k < 6; k++) index.push_back(k);
                shuffle (index.begin(), index.end(), std::default_random_engine(time(0)));

                // create previews for the current 6 hats
                for (int j = 0; j < 6; j++)
                {
                    Hat hat = get_hat(hatsLeft - index[j] - 1);
                    Collider* collider = hatPosColliders->values[j];
                    HatPreview(hat, collider);
                }
            } 
            else // if the last one (may or may not be full)
            {
                // add to rack list
                rackSelector->racks->Add(actualRack);
                Array<Collider*>* hatPosColliders = actualRack->GetComponentsInChildren<Collider*>();
                
                // randomize order
                std::vector<int> index = {};
                for (int k = 0; k < hatsLeft; k++) index.push_back(k);
                shuffle (index.begin(), index.end(), std::default_random_engine(time(0)));

                // create previews
                for (int j = 0; j < hatsLeft; j++)
                {   
                    Hat hat = get_hat(index[j]);
                    Collider* collider = hatPosColliders->values[j];
                    HatPreview(hat, collider);
                }
            }
        }
        
        // make sure only 1 of the racks is active right now
        rackSelector->UpdateRack();

        int materialCount = GorillaMaterialObjects.size();
        int lastMaterialCount = materialCount % 10;
        int materialPageCount = (materialCount / 10) + (lastMaterialCount == 0 ? 0 : 1);

        // get relevant object pointers
        Il2CppString* materialSelectionName = il2cpp_utils::createcsstr("Selection");
        Transform* materialSelectionTransform = mirrorTransform->Find(materialSelectionName);
        GameObject* materialSelectionGO = materialSelectionTransform->get_gameObject();

        Il2CppString* previewName = il2cpp_utils::createcsstr("Preview");
        Transform* previewTransform = mirrorTransform->Find(previewName);
        GameObject* preview = previewTransform->get_gameObject();

        Il2CppString* neonName = il2cpp_utils::createcsstr("Neon");
        Transform* neonTransform = mirrorTransform->Find(neonName);
        GameObject* neon = neonTransform->get_gameObject();

        using MatSelector = HatRackSelector;
        using MatSelectorButton = HatRackSelectorButton;

        MatSelector* matSelector = mirror->AddComponent<MatSelector*>();
        // if more than 1 add a selector
        if (materialPageCount > 1)
        {
            // setup the arrow buttons
            Array<Collider*>* buttonColliders = materialSelectionGO->GetComponentsInChildren<Collider*>(true);

            for (int i = 0; i < buttonColliders->Length(); i++)
            {
                INFO("Selector Button %d", i);
                Collider* collider = buttonColliders->values[i];
                GameObject* colliderGO = collider->get_gameObject();
                MatSelectorButton* button = colliderGO->AddComponent<MatSelectorButton*>();
                
                // what selector does it apply to?
                button->selector = matSelector;
                // should be trigger
                collider->set_isTrigger(true);
                // correct layer for buttons
                colliderGO->set_layer(18);
            }
        }
        else
        {
            Object::Destroy(materialSelectionGO);
        }

        // for each material page
        for (int i = 0; i < materialPageCount; i++)
        {
            int materialsLeft = materialCount - (i * 10);

            if (materialsLeft > 10) // if not the last one
            {
                GameObject* thePage = Object::Instantiate(preview);
                Object::DontDestroyOnLoad(thePage);
                Transform* thePageTransform = thePage->get_transform();
                thePageTransform->SetParent(mirrorTransform, false);

                // add to the rack selector list of racks
                matSelector->racks->Add(thePage);

                // create previews with a scale of 0.21
                float scale = 0.21f;
                for (int j = 0; j < 10; j++)
                {
                    int matIndex = materialsLeft - j - 1;
                    Material material = GorillaMaterialObjects[matIndex];
                    float height = (-0.5f * scale) - (scale * j) - 0.05f;
                    Vector3 pos = {0.0f, height, 0.0f};
                    MaterialPreview(material, thePageTransform, pos, scale * 0.85f);
                }
            }
            else // if the last one (may or may not be full)
            {
                // create previews with a scale of at most 2.1f / 6, or till 0.21f
                float scale = 2.1f / (materialsLeft > 6 ? materialsLeft : 6);
                matSelector->racks->Add(preview);

                for (int j = 0; j < materialsLeft; j++)
                {
                    int matIndex = j;
                    Material material = GorillaMaterialObjects[matIndex];

                    float height = (-0.5f * scale) - (scale * j) - 0.05f;
                    Vector3 pos = {0.0f, height, 0.0f};
                    MaterialPreview(material, previewTransform, pos, scale * 0.85f);
                }
            }
        }
        
        matSelector->UpdateRack();

        neon->AddComponent<NeonButton*>();
        
        mirror->SetActive(true);
        HatRack->SetActive(true);

        GorillaCosmetics::HatPreviewButton::canPress() = true;
        GorillaCosmetics::MaterialPreviewButton::canPress() = true;
        GorillaCosmetics::HatRackSelectorButton::canPress() = true;
        Loaded = true;
        Loading = false;
    }

    std::string toLower(const std::string& orig)
    {
        std::string result = "";
        for (auto c : orig)
        {
            result += tolower(c);
        }
        return result;
    }

    // il2cpp'd
    int AssetLoader::SelectedMaterialFromConfig(std::string configString)
    {
        std::string selectedMatString = toLower(configString);
        INFO("Looking for material %s", selectedMatString.c_str());
        for (int i = 1; i < GorillaMaterialObjects.size(); i++)
        {
            Material gorillaMaterialObject = GorillaMaterialObjects[i];
            std::string name = toLower(gorillaMaterialObject.get_descriptor().get_name());
            if (name == selectedMatString)
            {
                return i;
            }
            else if (toLower(gorillaMaterialObject.get_manifest().get_fileName()) == selectedMatString)
            {
                return i;
            }
        }
        return 0;
    }

    // il2cpp'd
    int AssetLoader::SelectedHatFromConfig(std::string configString)
    {
        std::string selectedHatString = toLower(configString);
        INFO("Looking for hat %s", selectedHatString.c_str());
        for (int i = 1; i < GorillaHatObjects.size(); i++)
        {
            Hat gorillaHatObject = GorillaHatObjects[i];
            std::string name = toLower(gorillaHatObject.get_descriptor().get_name());
            if (toLower(gorillaHatObject.get_descriptor().get_name()) == selectedHatString)
            {
                return i;
            }
            else if (toLower(gorillaHatObject.get_manifest().get_fileName()) == selectedHatString)
            {
                return i;
            }
        }
        return 0;
    }

    bool AssetLoader::IsLoaded()
    {
        return Loaded;
    }

    void AssetLoader::SelectMaterial(std::string name)
    {
        selectedMaterial = SelectedMaterialFromConfig(name);
        SaveConfig();
    }

    void AssetLoader::SelectHat(std::string name)
    {
        selectedHat = SelectedHatFromConfig(name);
        SaveConfig();
    }
}
