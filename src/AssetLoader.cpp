#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "config.hpp"
#include "logging.hpp"
#include "Utils/FileUtils.hpp"
#include "AssetLoader.hpp"

#include "quest-cosmetic-loader/shared/CosmeticLoader.hpp"

#include "Types/Material/MaterialPreview.hpp"
#include "Types/Hat/HatPreview.hpp"

#include "Types/Selector/HatRackSelector.hpp"
#include "Types/Selector/HatRackSelectorButton.hpp"

#include <algorithm>
#include <random>
#include <chrono>

#include <time.h>
#include "Utils/ZipUtils.hpp"
#include "typedefs.h"

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
        Il2CppString* savedHatCS = *il2cpp_utils::RunMethod<Il2CppString*>("UnityEngine", "PlayerPrefs", "GetString", hatProperty, defaultHat);
        std::string savedHat = to_utf8(csstrtostr(savedHatCS));
        
        if (savedHat.find("custom:") != std::string::npos)
        {
            savedHat.erase(0, 7);
        }

        selectedHat = SelectedHatFromConfig(savedHat);
        if (!selectedHat) config.lastActiveHat = "";
        
        // Load Mirror
        auto* mirrorLoader = new CosmeticsLoader::CosmeticLoader(folder + "Mirror", [&](std::string name, Il2CppObject* mirror){
            Il2CppObject* transform = CRASH_UNLESS(il2cpp_utils::RunMethod(mirror, "get_transform"));
            
            Vector3 scale = {0.29f, 0.29f, 0.29f};
            Vector3 pos = {-68.5f, 11.96f, -81.595f};
            Vector3 rot = {0.21f, -153.2f, -4.6f};
            Quaternion rotation = CRASH_UNLESS(il2cpp_utils::RunMethod<Quaternion>("UnityEngine", "Quaternion", "Euler", rot));

            CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_localScale", scale));
            CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_position", pos));
            CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_rotation", rotation));
            
            CRASH_UNLESS(il2cpp_utils::RunMethod(mirror, "DontDestroyOnLoad", mirror));
        }, "_Hat", il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));
        
        // Load Hat Rack
        Il2CppObject* HatRack = nullptr;
        auto* rackLoader = new CosmeticsLoader::CosmeticLoader(folder + "HatRack", [&](std::string name, Il2CppObject* rack){
            Il2CppObject* transform = CRASH_UNLESS(il2cpp_utils::RunMethod(rack, "get_transform"));

            Vector3 scale = {0.25f, 0.25f, 0.25f};
            Vector3 pos = {-68.003f, 11.471f, -80.637f};
            Vector3 rot = {0.0f, 0.0f, 0.0f};
            Quaternion rotation = CRASH_UNLESS(il2cpp_utils::RunMethod<Quaternion>("UnityEngine", "Quaternion", "Euler", rot));

            CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_localScale", scale));
            CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_position", pos));
            CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_rotation", rotation));
            CRASH_UNLESS(il2cpp_utils::RunMethod(rack, "DontDestroyOnLoad", rack));
            HatRack = rack;

        }, "_Hat", il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));

        while(!HatRack) usleep(1000);
        // how many hats
        int hatCount = GorillaHatObjects.size();
        // how many racks are needed for that amount of hats
        int rackCount = (hatCount / 6) + 1;
        // how many hats are on the last rack
        int lastRackCount = hatCount % 6;

        INFO("Hat count: %d, rack count: %d, last rack hat count: %d", hatCount, rackCount, lastRackCount);
        // some names to use in finding transforms
        Il2CppString* rackName = il2cpp_utils::createcsstr("HatRack");
        Il2CppString* selectionName = il2cpp_utils::createcsstr("Selection");
        Il2CppObject* rackTransform = CRASH_UNLESS(il2cpp_utils::RunMethod(HatRack, "get_transform"));
        
        // the actual rack transform, so this contains the 6 hats
        // mesh is now seperate, so only the colliders are being switched out
        Il2CppObject* actualRackTransform = *il2cpp_utils::RunMethod(rackTransform, "Find", rackName);
        Il2CppObject* selectionTransform = *il2cpp_utils::RunMethod(rackTransform, "Find", selectionName);
        Il2CppObject* actualRack = *il2cpp_utils::RunMethod(actualRackTransform, "get_gameObject");
        Il2CppObject* selectionGO = *il2cpp_utils::RunMethod(selectionTransform, "get_gameObject");

        std::vector<Il2CppClass*> colliderKlass = {il2cpp_utils::GetClassFromName("UnityEngine", "Collider")};
        HatRackSelector* rackSelector = *il2cpp_utils::RunGenericMethod<HatRackSelector*>(HatRack, "AddComponent", std::vector<Il2CppClass*>{classof(HatRackSelector*)});

        // if more than 1 rack is going to be used
        if (rackCount > 1)
        {
            // setup the arrow buttons
            Array<Il2CppObject*>* buttonColliders = CRASH_UNLESS(il2cpp_utils::RunGenericMethod<Array<Il2CppObject*>*>(selectionGO, "GetComponentsInChildren", colliderKlass, true));

            for (int i = 0; i < buttonColliders->Length(); i++)
            {
                INFO("Selector Button %d", i);
                Il2CppObject* collider = buttonColliders->values[i];
                Il2CppObject* colliderGO = *il2cpp_utils::RunMethod(collider, "get_gameObject");
                HatRackSelectorButton* button = *il2cpp_utils::RunGenericMethod<HatRackSelectorButton*>(colliderGO, "AddComponent", std::vector<Il2CppClass*>{classof(HatRackSelectorButton*)});
                button->selector = rackSelector;
                il2cpp_utils::RunMethod(collider, "set_isTrigger", true);
                // correct layer for buttons
                il2cpp_utils::RunMethod(colliderGO, "set_layer", 18);
            }
        }
        else
        {  
            // if not needed just delete it
            il2cpp_utils::RunMethod("UnityEngine", "Object", "Destroy", selectionGO);
        }

        // for the amount of racks needed
        for (int i = 0; i < rackCount; i++)
        {
            int hatsLeft = hatCount - (i * 6);
            if (hatsLeft > 6) // if not the last rack
            {
                Il2CppObject* theRack = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "Object", "Instantiate", actualRack));
                CRASH_UNLESS(il2cpp_utils::RunMethod(theRack, "DontDestroyOnLoad", theRack));
                Il2CppObject* theRackTransform = CRASH_UNLESS(il2cpp_utils::RunMethod(theRack, "get_transform"));
                il2cpp_utils::RunMethod(theRackTransform, "SetParent", rackTransform, false);

                // add to the rack selector list of racks
                il2cpp_utils::RunMethod(rackSelector->racks, "Add", theRack);
                Array<Il2CppObject*>* hatPosColliders = CRASH_UNLESS(il2cpp_utils::RunGenericMethod<Array<Il2CppObject*>*>(theRack, "GetComponentsInChildren", colliderKlass, true));
                
                // randomize order
                std::vector<int> index = {};
                for (int k = 0; k < 6; k++) index.push_back(k);
                shuffle (index.begin(), index.end(), std::default_random_engine(time(0)));

                // create previews for the current 6 hats
                for (int j = 0; j < 6; j++)
                {
                    Hat hat = get_hat(hatsLeft - index[j] - 1);
                    Il2CppObject* collider = hatPosColliders->values[j];
                    HatPreview(hat, collider);
                }
            } 
            else // if the last one (may or may not be full)
            {
                // add to rack list
                il2cpp_utils::RunMethod(rackSelector->racks, "Add", actualRack);
                Array<Il2CppObject*>* hatPosColliders = CRASH_UNLESS(il2cpp_utils::RunGenericMethod<Array<Il2CppObject*>*>(actualRack, "GetComponentsInChildren", colliderKlass, true));
                
                // randomize order
                std::vector<int> index = {};
                for (int k = 0; k < hatsLeft; k++) index.push_back(k);
                shuffle (index.begin(), index.end(), std::default_random_engine(time(0)));

                // create previews
                for (int j = 0; j < hatsLeft; j++)
                {   
                    Hat hat = get_hat(index[j]);
                    Il2CppObject* collider = hatPosColliders->values[j];
                    HatPreview(hat, collider);
                }
            }
        }
        
        // make sure only 1 of the racks is active right now
        rackSelector->UpdateRack();

        // Load Material Previews
        int matCount = GorillaMaterialObjects.size();
        int scaleCount = matCount > 6 ? matCount : 6; 
        float scale = (0.8f/ scaleCount);
        for (int i = 0; i < GorillaMaterialObjects.size(); i++)
        {
            Material material = GorillaMaterialObjects[i];
            Vector3 pos = {-68.287f, 12.04f - (scale * i), -81.251f};
            MaterialPreview(material, pos, scale * 0.85f);
        }

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