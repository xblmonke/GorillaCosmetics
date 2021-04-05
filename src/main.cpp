#include "logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

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
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

#include "PlayerCosmeticsList.hpp"

#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/prettywriter.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/stringbuffer.h"


typedef struct PhotonMessageInfo {
    int timeInt;
    Il2CppObject* sender;
    Il2CppObject* photonView;
} PhotonMessageInfo;

DEFINE_IL2CPP_ARG_TYPE(PhotonMessageInfo, "Photon.Pun", "PhotonMessageInfo");


ModInfo modInfo;

using namespace CosmeticsLoader;
using namespace GorillaCosmetics;

MAKE_HOOK_OFFSETLESS(VRRig_ChangeMaterial, void, Il2CppObject* self, int materialIndex)
{
    VRRig_ChangeMaterial(self, materialIndex);
    // postfix

    // get user ID
    Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
    Il2CppObject* owner = *il2cpp_utils::RunMethod(photonView, "get_Owner");

    Il2CppString* UserIDCS = *il2cpp_utils::RunMethod<Il2CppString*>(owner, "get_UserId");
    std::string UserID = UserIDCS ? to_utf8(csstrtostr(UserIDCS)) : "";

    // get material from cache
    std::string material = PlayerCosmeticsList::get_material(UserID);
    CosmeticUtils::ChangeMaterial(self, materialIndex, material);
}

MAKE_HOOK_OFFSETLESS(VRRig_Start, void, Il2CppObject* self)
{
    VRRig_Start(self);
    // postfix

    // load stuff
    GorillaCosmetics::AssetLoader::Load();

    // get user ID
    Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
    Il2CppObject* owner = *il2cpp_utils::RunMethod(photonView, "get_Owner");

    Il2CppString* UserIDCS = *il2cpp_utils::RunMethod<Il2CppString*>(owner, "get_UserId");
    std::string UserID = UserIDCS ? to_utf8(csstrtostr(UserIDCS)) : "";

    // get hat for user from the cached list
    std::string hat = PlayerCosmeticsList::get_hat(UserID);

    // if it's a custom one, set that as active
    if (hat.find("custom:") != std::string::npos)
    {
        hat.erase(0, 7);
        CosmeticUtils::ChangeHat(self, hat);
    }
    // if not custom set as None
    else CosmeticUtils::ChangeHat(self, "None");

    // set material from cache
    int setMatIndex = CRASH_UNLESS(il2cpp_utils::GetFieldValue<int>(self, "setMatIndex"));
    std::string material = PlayerCosmeticsList::get_material(UserID);
    CosmeticUtils::ChangeMaterial(self, setMatIndex, material);
}

MAKE_HOOK_OFFSETLESS(VRRig_UpdateCosmetics, void, Il2CppObject* self, Il2CppString* newBadge, Il2CppString* newFace, Il2CppString* newHat, PhotonMessageInfo info)
{
    std::string hatString = to_utf8(csstrtostr(newHat));
    std::string hat = "";
    std::string material = "default";
    bool isCustomUpdate = false;
    // if the hatname contains { and } it's a json object, so this is a custom called update
    if (hatString.find("{") != std::string::npos && hatString.find("}") != std::string::npos)
    {
        isCustomUpdate = true;
        getLogger().info("Found object:\n%s", hatString.c_str());
        rapidjson::Document d;
        d.Parse(hatString.c_str());
        
        // get values out of the json document (in C# would be some kind of deserializejson thing)
        hat = d["hat"].GetString();
        material = d["material"].GetString();

        // get user ID to save the value on the cosmetics list
        Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
        Il2CppObject* owner = *il2cpp_utils::RunMethod(photonView, "get_Owner");

        Il2CppString* UserIDCS = *il2cpp_utils::RunMethod<Il2CppString*>(owner, "get_UserId");
        std::string UserID = UserIDCS ? to_utf8(csstrtostr(UserIDCS)) : "";

        // save values to player ID
        PlayerCosmeticsList::set_player(UserID, hat, material);
    }
    // if the update was not custom, the hatstring should be used directly
    else hat = hatString;

    // run original method with new arg
    VRRig_UpdateCosmetics(self, newBadge, newFace, il2cpp_utils::createcsstr(hat), info);

    // postfix
    Il2CppObject* player = *il2cpp_utils::RunMethod(info.photonView, "get_Owner");

    Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
    Il2CppObject* owner = *il2cpp_utils::RunMethod(photonView, "get_Owner");

    // if the owner of this vr rig is also the one that sent the update
    if (player == owner)
    {
        // log the name for reasons unbeknownst to the universe
        Il2CppString* nick = *il2cpp_utils::RunMethod<Il2CppString*>(player, "get_NickName");
        std::string nickname = to_utf8(csstrtostr(nick));
        getLogger().info("Update Requested for %s", nickname.c_str());

        // if the hat is custom change it
        if (hat.find("custom:") != std::string::npos)
        {
            hat.erase(0, 7);
            CosmeticUtils::ChangeHat(self, hat);
        }
        // if not then change the custom hat to be none
        else CosmeticUtils::ChangeHat(self, "None");

        // if custom update change the material
        if (isCustomUpdate)
        {
            int setMatIndex = CRASH_UNLESS(il2cpp_utils::GetFieldValue<int>(self, "setMatIndex"));
            CosmeticUtils::ChangeMaterial(self, setMatIndex, material);
        }
    }
    // log because why not
    else getLogger().error("Player and Owner were not equal");
}

MAKE_HOOK_OFFSETLESS(VRRig_LocalUpdateCosmetics, void, Il2CppObject* self, Il2CppString* newBadge, Il2CppString* newFace, Il2CppString* newHat)
{
    std::string hatString = to_utf8(csstrtostr(newHat));
    std::string hat = "";
    std::string material = "default";
    bool isCustomUpdate = false;
    // if the hatname contains { and } it's a json object, so this is a custom called update
    if (hatString.find("{") != std::string::npos && hatString.find("}") != std::string::npos)
    {
        isCustomUpdate = true;
        getLogger().info("Found object:\n%s", hatString.c_str());
        rapidjson::Document d;
        d.Parse(hatString.c_str());
        
        // get values out of the json document (in C# would be some kind of deserializejson thing)
        hat = d["hat"].GetString();
        material = d["material"].GetString();

        // get user ID to save the value on the cosmetics list
        Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
        Il2CppObject* owner = *il2cpp_utils::RunMethod(photonView, "get_Owner");

        Il2CppString* UserIDCS = *il2cpp_utils::RunMethod<Il2CppString*>(owner, "get_UserId");
        std::string UserID = UserIDCS ? to_utf8(csstrtostr(UserIDCS)) : "";
        // save values to player ID
        PlayerCosmeticsList::set_player(UserID, hat, material);
    }
    // if the update was not custom, the hatstring should be used directly
    else hat = hatString;

    // run original method (stuff before here was a prefix)
    VRRig_LocalUpdateCosmetics(self, newBadge, newFace, il2cpp_utils::createcsstr(hat));

    //postfix

    // if the hat name contains the substring custom: it is a custom hat, and not a base game hat
    if (hat.find("custom:") != std::string::npos)
    {
        hat.erase(0, 7);
        CosmeticUtils::ChangeHat(self, hat);
    }
    // if it does not contain that substring, the custom stuff should be disabled, so select hat None
    else CosmeticUtils::ChangeHat(self, "None");
    
    // if the update was custom, update the material of this VRRig
    if (isCustomUpdate)
    {
        // get the material index
        int setMatIndex = CRASH_UNLESS(il2cpp_utils::GetFieldValue<int>(self, "setMatIndex"));
        // change material
        CosmeticUtils::ChangeMaterial(self, setMatIndex, material);
    }
}

MAKE_HOOK_OFFSETLESS(VRRig_RequestCosmetics, void, Il2CppObject* self, PhotonMessageInfo info)
{
    // we need to change behaviour on this method too, because otherwise the initial values will not be correct
    Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
    bool IsMine = *il2cpp_utils::RunMethod<bool>(photonView, "get_IsMine");

    // basically a copy of the normal method but with custom text
    // if this is my VR rig do the thing
    if (IsMine)
    {
        // get config, since this will only ever be called on our own local vrrig we can just use the config value
        std::string material = config.lastActiveMaterial;

        // get the configured values
        Il2CppString* hatCS = *il2cpp_utils::GetFieldValue<Il2CppString*>(self, "hat");
        Il2CppString* face = *il2cpp_utils::GetFieldValue<Il2CppString*>(self, "face");
        Il2CppString* badge = *il2cpp_utils::GetFieldValue<Il2CppString*>(self, "badge");

        // I need the hat name to put in the json object (could possibly not be a custom hat, so needed)
        std::string hat = to_utf8(csstrtostr(hatCS));

        // make json object (cpp specific)
        rapidjson::Document d;
        d.SetObject();
        rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

        /* Example json object with a custom hat:
        {
            "hat: "custom:Crown"
            "material": "camo"
        }
        */

        d.AddMember("hat", rapidjson::Value(hat.c_str(), hat.size(), allocator), allocator);
        d.AddMember("material", rapidjson::Value(material.c_str(), material.size(), allocator), allocator);

        // serialize to json string
        rapidjson::StringBuffer buffer;
        buffer.Clear();

        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);

        std::string messageString(buffer.GetString(), buffer.GetSize());

        // make it a C# string
        Il2CppString* hatMessage = il2cpp_utils::createcsstr(messageString);

        // run the RPC UpdateCosmetics method on the requesting vr rig photonview or whatever
        static Il2CppString* methodName = il2cpp_utils::createcsstr("UpdateCosmetics", il2cpp_utils::StringType::Manual);
        
        Array<Il2CppObject*>* argsArray = reinterpret_cast<Array<Il2CppObject*>*>(il2cpp_functions::array_new(classof(Il2CppObject*), 3));
        argsArray->values[0] = (Il2CppObject*)badge;
        argsArray->values[1] = (Il2CppObject*)face;
        argsArray->values[2] = (Il2CppObject*)hatMessage;
        
        il2cpp_utils::RunMethod(photonView, "RPC", methodName, info.sender, argsArray);

        il2cpp_utils::RunMethod("Photon.Pun", "PhotonNetwork", "SendAllOutgoingCommands");
    }

    // run the original method, so that people without the mod also get the correct values sent over
    VRRig_RequestCosmetics(self, info);
}

MAKE_HOOK_OFFSETLESS(VRRig_InitializeNoobMaterial, void, Il2CppObject* self, float red, float green, float blue)
{
    // when the noob material is intialized the mat needs to be updated to have the correct color
    VRRig_InitializeNoobMaterial(self, red, green, blue);
    // postfix

    // get user ID
    Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
    Il2CppObject* owner = *il2cpp_utils::RunMethod(photonView, "get_Owner");

    Il2CppString* UserIDCS = *il2cpp_utils::RunMethod<Il2CppString*>(owner, "get_UserId");
    std::string UserID = UserIDCS ? to_utf8(csstrtostr(UserIDCS)) : "";

    // change the material
    std::string material = PlayerCosmeticsList::get_material(UserID);
    int setMatIndex = CRASH_UNLESS(il2cpp_utils::GetFieldValue<int>(self, "setMatIndex"));
    CosmeticUtils::ChangeMaterial(self, setMatIndex, material);
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
    INSTALL_HOOK_OFFSETLESS(logger, VRRig_RequestCosmetics, il2cpp_utils::FindMethodUnsafe("", "VRRig", "RequestCosmetics", 1));
    INSTALL_HOOK_OFFSETLESS(logger, VRRig_InitializeNoobMaterial, il2cpp_utils::FindMethodUnsafe("", "VRRig", "InitializeNoobMaterial", 3));

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
