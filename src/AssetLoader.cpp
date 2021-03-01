#include "AssetLoader.hpp"
#include "Utils/FileUtils.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "cosmeticsloader/shared/CosmeticLoader.hpp"

#include "Types/Material/MaterialPreview.hpp"
#include "Types/Hat/HatPreview.hpp"

#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/typedefs.h"

#include <algorithm>
#include <random>
#include <chrono>

#include <time.h>
#include "Utils/ZipUtils.hpp"
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
            std::string path = materialPath + "Unpacked/" + f.substr(0, f.find_first_of('.'));
            ZipUtils::Unzip(materialPath + f, path + "/");
            
            path += "/package.json";

            if (fileexists(path)) MaterialFiles.push_back(path);
            else ERROR("File '%s' Did not exist!", path.c_str());
        }
        GorillaMaterialObjects.push_back(Material(folder + "default.json"));
        LoadItems<Material>(MaterialFiles, GorillaMaterialObjects);
        
        // Load Hats
        std::string hatPath = folder + HatsLocation;
        std::vector<std::string> hatNames; 
        FileUtils::GetFilesInFolderPath("hat", hatPath, hatNames);

        for (auto f : hatNames)
        {
            std::string path = hatPath + "Unpacked/" + f.substr(0, f.find_first_of('.'));
            ZipUtils::Unzip(hatPath + f, path + "/");
            path += "/package.json";

            if (fileexists(path)) HatFiles.push_back(path);
            else ERROR("File '%s' Did not exist!", path.c_str());
        }
        
        GorillaHatObjects.push_back(Hat(folder + "None/package.json"));
        LoadItems<Hat>(HatFiles, GorillaHatObjects);

        // Parse Configs
        selectedMaterial = SelectedMaterialFromConfig(config.lastActiveMaterial);
        selectedInfectedMaterial = SelectedMaterialFromConfig(config.lastActiveInfectedMaterial);
        selectedHat = SelectedHatFromConfig(config.lastActiveHat);
        
        // Load Mirror

        auto* mirrorLoader = new CosmeticsLoader::CosmeticLoader(folder + MirrorLocation + "package.json", [&](std::string name, Il2CppObject* mirror){
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
        
        //Il2CppObject* gameObject = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "GameObject", "CreatePrimitive", 0));

        // Load Hat Rack
        Il2CppObject* HatRack = nullptr;
        auto* rackLoader = new CosmeticsLoader::CosmeticLoader(folder + RackLocation + "package.json", [&](std::string name, Il2CppObject* rack){
            Il2CppObject* transform = CRASH_UNLESS(il2cpp_utils::RunMethod(rack, "get_transform"));

            Vector3 scale = {3.696f, 3.696f, 0.677f};
            Vector3 pos = {-68.003f, 11.471f, -80.637f};
            Vector3 rot = {-90.0f, 0, 0};
            Quaternion rotation = CRASH_UNLESS(il2cpp_utils::RunMethod<Quaternion>("UnityEngine", "Quaternion", "Euler", rot));

            CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_localScale", scale));
            CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_position", pos));
            CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_rotation", rotation));
            CRASH_UNLESS(il2cpp_utils::RunMethod(rack, "DontDestroyOnLoad", rack));
            HatRack = rack;
        }, "_Hat", il2cpp_utils::GetSystemType("UnityEngine", "GameObject"));

        while(!HatRack) usleep(1000);

        // Check if we have enough hats for a second one
        Il2CppObject* HatRack2 = nullptr;
        if(GorillaHatObjects.size() > 6)
        {
            HatRack2 = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "Instantiate", "Instantiate", HatRack));
            Il2CppObject* transform = CRASH_UNLESS(il2cpp_utils::RunMethod(HatRack2, "get_transform"));

            Vector3 pos = {-67.895f, 11.511f, -80.41f};
            Vector3 rot = {-90.0f, 0, -68.608f};
            Quaternion rotation = CRASH_UNLESS(il2cpp_utils::RunMethod<Quaternion>("UnityEngine", "Quaternion", "Euler", rot));

            CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_position", pos));
            CRASH_UNLESS(il2cpp_utils::RunMethod(transform, "set_rotation", rotation));
            CRASH_UNLESS(il2cpp_utils::RunMethod(HatRack2, "DontDestroyOnLoad", HatRack2));
        }


        // Load Material Previews
        float scale = (0.8f/GorillaMaterialObjects.size());
        for (int i = 0; i < GorillaMaterialObjects.size(); i++)
        {
            Material material = GorillaMaterialObjects[i];
            Vector3 pos = {-68.287f, 12.04f - (scale * i), -81.251f};
            MaterialPreview(material, pos, scale * 0.85f);
        }

        // Load Hat Rack Previews
        Array<Il2CppObject*>* hatPosColliders = CRASH_UNLESS(il2cpp_utils::RunGenericMethod<Array<Il2CppObject*>*>(HatRack, "GetComponentsInChildren", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("UnityEngine", "Collider")}));
        int hatCount = GorillaHatObjects.size();
        
        std::vector<int> index = {};
        for (int i = 0; i < (hatCount >= 6 ? 6 : hatCount); i++) index.push_back(i);
        
        shuffle (index.begin(), index.end(), std::default_random_engine(time(0)));

        for (int i = 0; i < (hatCount >= 6 ? 6 : hatCount); i++)
        {
            int j = index[i];
            INFO("index is %d at %d", j, i);
            Hat hat = GorillaHatObjects[j];
            Il2CppObject* collider = hatPosColliders->values[i];
            HatPreview(hat, collider);
        }

        // Load Hat Rack Preview Again, if needed
        if(HatRack2)
        {
            index.clear();
            for (int i = 6; i < (hatCount >= 12 ? 12 : hatCount); i++) index.push_back(i);

            shuffle (index.begin(), index.end(), std::default_random_engine(time(0)));

            Array<Il2CppObject*>* hatPosColliders2 = CRASH_UNLESS(il2cpp_utils::RunGenericMethod<Array<Il2CppObject*>*>(HatRack2, "GetComponentsInChildren", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("UnityEngine", "Collider")}));
            for (int i = 6; i < (hatCount >= 12 ? 12 : hatCount); i++)
            {
                int j = index[i-6];
                INFO("index is %d at %d", j, i);
                Hat hat = GorillaHatObjects[j];
                Il2CppObject* collider = hatPosColliders->values[i-6];
                HatPreview(hat, collider);
            }
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
            INFO("Found %s", name.c_str());
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
            INFO("Found %s", name.c_str());
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
    }

    void AssetLoader::SelectHat(std::string name)
    {
        selectedHat = SelectedHatFromConfig(name);
    }
}