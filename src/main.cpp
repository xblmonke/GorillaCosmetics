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

#include "custom-types/shared/register.hpp"

#include "libil2cpp/il2cpp/libil2cpp/icalls/mscorlib/System/RuntimeTypeHandle.h"

ModInfo modInfo;

using namespace CosmeticsLoader;
using namespace GorillaCosmetics;

typedef CosmeticsLoader::Manifest<CosmeticsLoader::Config, CosmeticsLoader::Descriptor> MapManifest;

CosmeticLoader<MapManifest>* loader = nullptr;

std::string persistentPath;

MAKE_HOOK_OFFSETLESS(VRRig_ChangeMaterial, void, Il2CppObject* self, int materialIndex)
{
    VRRig_ChangeMaterial(self, materialIndex);
    CosmeticUtils::ChangeMaterial(self, materialIndex);
}

MAKE_HOOK_OFFSETLESS(VRRig_Start, void, Il2CppObject* self)
{
    VRRig_Start(self);
    GorillaCosmetics::AssetLoader::Load();
    CosmeticUtils::ChangeHat(self);

    int setMatIndex = CRASH_UNLESS(il2cpp_utils::GetFieldValue<int>(self, "setMatIndex"));
    CosmeticUtils::ChangeMaterial(self, setMatIndex);
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

    INFO("Installed Hooks!");

    INFO("Registering custom types...");

    custom_types::Register::RegisterType<MaterialPreviewButton>();
    custom_types::Register::RegisterType<HatPreviewButton>();

    INFO("Registered custom types!");

    SetupFileStructure();

    INFO("Cosmetics Loaded!");
}

void SetupFileStructure()
{
    std::string FolderPath = "/sdcard/ModData/com.AnotherAxiom.GorillaTag/Mods/";
    FolderPath += ID;
    FolderPath += "/";
    std::string MirrorLocation = FolderPath + "Mirror/";
    std::string RackLocation = FolderPath + "Rack/";
    std::string MaterialsLocation = FolderPath + "Materials/";
    std::string HatsLocation = FolderPath + "Hats/";
    std::string NoneLocation = FolderPath + "None/";

    FileUtils::mkdir(FolderPath);
    FileUtils::mkdir(MaterialsLocation);
    FileUtils::mkdir(HatsLocation);
    FileUtils::mkdir(MaterialsLocation + "Unpacked/");
    FileUtils::mkdir(HatsLocation + "Unpacked/");

    if (!direxists(RackLocation)) 
    {
        std::string rack = FolderPath + "HatRack.hat";
        ZipUtils::Unzip(rack, RackLocation);
    }
    else deletefile(FolderPath + "HatRack");

    if (!direxists(MirrorLocation)) 
    {
        std::string mirror = FolderPath + "Mirror.hat";
        ZipUtils::Unzip(mirror, MirrorLocation);
    }
    else deletefile(FolderPath + "Mirror");
    
    if (!direxists(NoneLocation)) 
    {
        std::string none = FolderPath + "None.hat";
        ZipUtils::Unzip(none, NoneLocation);
    }
    else deletefile(FolderPath + "None");
}
