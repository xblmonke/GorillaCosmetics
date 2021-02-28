#include "logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "cosmeticsloader/shared/CosmeticLoader.hpp"
#include "cosmeticsloader/shared/Config.hpp"
#include "cosmeticsloader/shared/Descriptor.hpp"
#include "cosmeticsloader/shared/Manifest.hpp"

#include "config.hpp"

#include "AssetLoader.hpp"
#include "Utils/CosmeticUtils.hpp"

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

extern "C" void load()
{
    Logger& logger = getLogger();
    if (!LoadConfig()) SaveConfig();
    INFO("Installing Hooks...");

    INSTALL_HOOK_OFFSETLESS(logger, VRRig_ChangeMaterial, il2cpp_utils::FindMethodUnsafe("", "VRRig", "ChangeMaterial", 1));
    INSTALL_HOOK_OFFSETLESS(logger, VRRig_Start, il2cpp_utils::FindMethodUnsafe("", "VRRig", "Start", 0));

    INFO("Installed Hooks!");

    INFO("Cosmetics Loaded!");
}