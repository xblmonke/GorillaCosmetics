#include "logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "cosmeticsloader/shared/CosmeticLoader.hpp"
#include "cosmeticsloader/shared/Config.hpp"
#include "cosmeticsloader/shared/Descriptor.hpp"
#include "cosmeticsloader/shared/Manifest.hpp"

#include "config.hpp"

#include "AssetLoader.hpp"
#include "Utils/CosmeticUtils.hpp"
#include "Utils/ZipUtils.hpp"
#include "Utils/FileUtils.hpp"
#include "Types/Material/MaterialPreviewButton.hpp"
#include "Types/Hat/HatPreviewButton.hpp"

#include "Types/Selector/HatRackSelector.hpp"
#include "Types/Selector/HatRackSelectorButton.hpp"

#include "custom-types/shared/register.hpp"

#include "libil2cpp/il2cpp/libil2cpp/icalls/mscorlib/System/RuntimeTypeHandle.h"

typedef struct PhotonMessageInfo {
    int timeInt;
    Il2CppObject* sender;
    Il2CppObject* photonView;
} PhotonMessageInfo;

DEFINE_IL2CPP_ARG_TYPE(PhotonMessageInfo, "Photon.Pun", "PhotonMessageInfo");


ModInfo modInfo;

using namespace CosmeticsLoader;
using namespace GorillaCosmetics;

typedef CosmeticsLoader::Manifest<CosmeticsLoader::Config, CosmeticsLoader::Descriptor> MapManifest;

CosmeticLoader<MapManifest>* loader = nullptr;

std::string persistentPath;

MAKE_HOOK_OFFSETLESS(VRRig_ChangeMaterial, void, Il2CppObject* self, int materialIndex)
{
    VRRig_ChangeMaterial(self, materialIndex);
    Il2CppString* faceCS = *il2cpp_utils::GetFieldValue<Il2CppString*>(self, "face");
    std::string face = faceCS ? to_utf8(csstrtostr(faceCS)) : "";
 
    if (face.find("custom:") != std::string::npos)
    {
        face.erase(0, 7);
        CosmeticUtils::ChangeMaterial(self, materialIndex, face);
    }
}

MAKE_HOOK_OFFSETLESS(VRRig_Start, void, Il2CppObject* self)
{
    VRRig_Start(self);
    GorillaCosmetics::AssetLoader::Load();

    Il2CppString* hatCS = *il2cpp_utils::GetFieldValue<Il2CppString*>(self, "face");
    std::string hat = hatCS ? to_utf8(csstrtostr(hatCS)) : "";

    if (hat.find("custom:") != std::string::npos)
    {
        hat.erase(0, 7);
        CosmeticUtils::ChangeHat(self, hat);
    }
    else CosmeticUtils::ChangeHat(self, "None");

    Il2CppString* faceCS = *il2cpp_utils::GetFieldValue<Il2CppString*>(self, "face");
    std::string face = faceCS ? to_utf8(csstrtostr(faceCS)) : "";
    int setMatIndex = CRASH_UNLESS(il2cpp_utils::GetFieldValue<int>(self, "setMatIndex"));
    if (face.find("custom:") != std::string::npos)
    {
        face.erase(0, 7);
        CosmeticUtils::ChangeMaterial(self, setMatIndex, face);
    }
}

MAKE_HOOK_OFFSETLESS(VRRig_UpdateCosmetics, void, Il2CppObject* self, Il2CppString* newBadge, Il2CppString* newFace, Il2CppString* newHat, PhotonMessageInfo info)
{
    VRRig_UpdateCosmetics(self, newBadge, newFace, newHat, info);
    std::string badge = to_utf8(csstrtostr(newBadge));
    std::string face = to_utf8(csstrtostr(newFace));
    std::string hat = to_utf8(csstrtostr(newHat));

    getLogger().info("Normal Update: \n\tHat: %s\n\tBadge: %s\n\tFace: %s", hat.c_str(), badge.c_str(), face.c_str());

    Il2CppObject* player = *il2cpp_utils::RunMethod(info.photonView, "get_Owner");

    Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
    Il2CppObject* owner = *il2cpp_utils::RunMethod(photonView, "get_Owner");

    if (player == owner)
    {
        Il2CppString* nick = *il2cpp_utils::RunMethod<Il2CppString*>(player, "get_NickName");
        std::string nickname = to_utf8(csstrtostr(nick));
        getLogger().info("Update Requested for %s", nickname.c_str());

        if (hat.find("custom:") != std::string::npos)
        {
            hat.erase(0, 7);
            CosmeticUtils::ChangeHat(self, hat);
        }
        else CosmeticUtils::ChangeHat(self, "None");

        int setMatIndex = CRASH_UNLESS(il2cpp_utils::GetFieldValue<int>(self, "setMatIndex"));
        if (face.find("custom:") != std::string::npos)
        {
            face.erase(0, 7);
            CosmeticUtils::ChangeMaterial(self, setMatIndex, face);
        }
    }
    else getLogger().error("Player and Owner were not equal");
}

MAKE_HOOK_OFFSETLESS(VRRig_LocalUpdateCosmetics, void, Il2CppObject* self, Il2CppString* newBadge, Il2CppString* newFace, Il2CppString* newHat)
{
    VRRig_LocalUpdateCosmetics(self, newBadge, newFace, newHat);
    std::string badge = to_utf8(csstrtostr(newBadge));
    std::string face = to_utf8(csstrtostr(newFace));
    std::string hat = to_utf8(csstrtostr(newHat));
    getLogger().info("LocalUpdate: \n\tHat: %s\n\tBadge: %s\n\tFace: %s", hat.c_str(), badge.c_str(), face.c_str());

    if (hat.find("custom:") != std::string::npos)
    {
        hat.erase(0, 7);
        CosmeticUtils::ChangeHat(self, hat);
    }
    else CosmeticUtils::ChangeHat(self, "None");

    int setMatIndex = CRASH_UNLESS(il2cpp_utils::GetFieldValue<int>(self, "setMatIndex"));
    if (face.find("custom:") != std::string::npos)
    {
        face.erase(0, 7);
        CosmeticUtils::ChangeMaterial(self, setMatIndex, face);
    }
}

extern "C" void setup(ModInfo& info)
{
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
    INFO("Cosmetics Setup!");
}

void SetupFileStructure();

extern "C" void load()
{
    Logger& logger = getLogger();
    if (!LoadConfig()) SaveConfig();
    INFO("Installing Hooks...");

    INSTALL_HOOK_OFFSETLESS(logger, VRRig_ChangeMaterial, il2cpp_utils::FindMethodUnsafe("", "VRRig", "ChangeMaterial", 1));
    INSTALL_HOOK_OFFSETLESS(logger, VRRig_Start, il2cpp_utils::FindMethodUnsafe("", "VRRig", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(logger, VRRig_LocalUpdateCosmetics, il2cpp_utils::FindMethodUnsafe("", "VRRig", "LocalUpdateCosmetics", 3));
    INSTALL_HOOK_OFFSETLESS(logger, VRRig_UpdateCosmetics, il2cpp_utils::FindMethodUnsafe("", "VRRig", "UpdateCosmetics", 4));

    INFO("Installed Hooks!");

    INFO("Registering custom types...");

    custom_types::Register::RegisterType<MaterialPreviewButton>();
    custom_types::Register::RegisterType<HatPreviewButton>();
    custom_types::Register::RegisterTypes<HatRackSelector, HatRackSelectorButton>();

    INFO("Registered custom types!");

    SetupFileStructure();

    INFO("Cosmetics Loaded!");
}

void SetupFileStructure()
{
    std::string FolderPath = "/sdcard/ModData/com.AnotherAxiom.GorillaTag/Mods/";
    FolderPath += ID;
    FolderPath += "/";
    std::string MaterialsLocation = FolderPath + "Materials/";
    std::string HatsLocation = FolderPath + "Hats/";

    FileUtils::mkdir(FolderPath);
    FileUtils::mkdir(MaterialsLocation);
    FileUtils::mkdir(HatsLocation);
}
