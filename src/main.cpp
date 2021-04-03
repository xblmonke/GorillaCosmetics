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
    
    Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
    Il2CppObject* owner = *il2cpp_utils::RunMethod(photonView, "get_Owner");

    Il2CppString* UserIDCS = *il2cpp_utils::RunMethod<Il2CppString*>(owner, "get_UserId");
    std::string UserID = UserIDCS ? to_utf8(csstrtostr(UserIDCS)) : "";

    std::string material = PlayerCosmeticsList::get_material(UserID);
    CosmeticUtils::ChangeMaterial(self, materialIndex, material);
}

MAKE_HOOK_OFFSETLESS(VRRig_Start, void, Il2CppObject* self)
{
    VRRig_Start(self);
    GorillaCosmetics::AssetLoader::Load();

    Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
    Il2CppObject* owner = *il2cpp_utils::RunMethod(photonView, "get_Owner");

    Il2CppString* UserIDCS = *il2cpp_utils::RunMethod<Il2CppString*>(owner, "get_UserId");
    std::string UserID = UserIDCS ? to_utf8(csstrtostr(UserIDCS)) : "";

    std::string hat = PlayerCosmeticsList::get_hat(UserID);

    if (hat.find("custom:") != std::string::npos)
    {
        hat.erase(0, 7);
        CosmeticUtils::ChangeHat(self, hat);
    }
    else CosmeticUtils::ChangeHat(self, "None");

    Il2CppString* faceCS = *il2cpp_utils::GetFieldValue<Il2CppString*>(self, "face");
    std::string face = faceCS ? to_utf8(csstrtostr(faceCS)) : "";
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
    
    if (hatString.find("{") != std::string::npos && hatString.find("}") != std::string::npos)
    {
        isCustomUpdate = true;
        getLogger().info("Found object:\n%s", hatString.c_str());
        rapidjson::Document d;
        d.Parse(hatString.c_str());
        
        hat = d["hat"].GetString();
        material = d["material"].GetString();

        Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
        Il2CppObject* owner = *il2cpp_utils::RunMethod(photonView, "get_Owner");

        Il2CppString* UserIDCS = *il2cpp_utils::RunMethod<Il2CppString*>(owner, "get_UserId");
        std::string UserID = UserIDCS ? to_utf8(csstrtostr(UserIDCS)) : "";
        PlayerCosmeticsList::set_player(UserID, hat, material);
    }
    else hat = hatString;

    VRRig_UpdateCosmetics(self, newBadge, newFace, il2cpp_utils::createcsstr(hat), info);

    std::string badge = to_utf8(csstrtostr(newBadge));
    std::string face = to_utf8(csstrtostr(newFace));

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

        if (isCustomUpdate)
        {
            int setMatIndex = CRASH_UNLESS(il2cpp_utils::GetFieldValue<int>(self, "setMatIndex"));
            CosmeticUtils::ChangeMaterial(self, setMatIndex, material);
        }
    }
    else getLogger().error("Player and Owner were not equal");
}

MAKE_HOOK_OFFSETLESS(VRRig_LocalUpdateCosmetics, void, Il2CppObject* self, Il2CppString* newBadge, Il2CppString* newFace, Il2CppString* newHat)
{
    std::string hatString = to_utf8(csstrtostr(newHat));
    std::string hat = "";
    std::string material = "default";
    bool isCustomUpdate = false;
    if (hatString.find("{") != std::string::npos && hatString.find("}") != std::string::npos)
    {
        isCustomUpdate = true;
        getLogger().info("Found object:\n%s", hatString.c_str());
        rapidjson::Document d;
        d.Parse(hatString.c_str());
        
        hat = d["hat"].GetString();
        material = d["material"].GetString();

        Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
        Il2CppObject* owner = *il2cpp_utils::RunMethod(photonView, "get_Owner");

        Il2CppString* UserIDCS = *il2cpp_utils::RunMethod<Il2CppString*>(owner, "get_UserId");
        std::string UserID = UserIDCS ? to_utf8(csstrtostr(UserIDCS)) : "";
        PlayerCosmeticsList::set_player(UserID, hat, material);
    }
    else hat = hatString;


    VRRig_LocalUpdateCosmetics(self, newBadge, newFace, il2cpp_utils::createcsstr(hat));

    std::string badge = to_utf8(csstrtostr(newBadge));
    std::string face = to_utf8(csstrtostr(newFace));

    if (hat.find("custom:") != std::string::npos)
    {
        hat.erase(0, 7);
        CosmeticUtils::ChangeHat(self, hat);
    }
    else CosmeticUtils::ChangeHat(self, "None");

    if (isCustomUpdate)
    {
        int setMatIndex = CRASH_UNLESS(il2cpp_utils::GetFieldValue<int>(self, "setMatIndex"));
        CosmeticUtils::ChangeMaterial(self, setMatIndex, material);
    }
}

MAKE_HOOK_OFFSETLESS(VRRig_RequestCosmetics, void, Il2CppObject* self, PhotonMessageInfo info)
{
    Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
    bool IsMine = *il2cpp_utils::RunMethod<bool>(photonView, "get_IsMine");

    if (IsMine)
    {
        std::string material = config.lastActiveMaterial;
        Il2CppString* hatCS = *il2cpp_utils::GetFieldValue<Il2CppString*>(self, "hat");
        Il2CppString* face = *il2cpp_utils::GetFieldValue<Il2CppString*>(self, "face");
        Il2CppString* badge = *il2cpp_utils::GetFieldValue<Il2CppString*>(self, "badge");

        std::string hat = to_utf8(csstrtostr(hatCS));
    
        rapidjson::Document d;
        d.SetObject();
        rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

        d.AddMember("hat", rapidjson::Value(hat.c_str(), hat.size(), allocator), allocator);
        d.AddMember("material", rapidjson::Value(material.c_str(), material.size(), allocator), allocator);

        rapidjson::StringBuffer buffer;
        buffer.Clear();

        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);

        std::string messageString(buffer.GetString(), buffer.GetSize());

        Il2CppString* hatMessage = il2cpp_utils::createcsstr(messageString);


        static Il2CppString* methodName = il2cpp_utils::createcsstr("UpdateCosmetics", il2cpp_utils::StringType::Manual);
        
        Array<Il2CppObject*>* argsArray = reinterpret_cast<Array<Il2CppObject*>*>(il2cpp_functions::array_new(classof(Il2CppObject*), 3));
        argsArray->values[0] = (Il2CppObject*)badge;
        argsArray->values[1] = (Il2CppObject*)face;
        argsArray->values[2] = (Il2CppObject*)hatMessage;
        
        il2cpp_utils::RunMethod(photonView, "RPC", methodName, info.sender, argsArray);

        il2cpp_utils::RunMethod("Photon.Pun", "PhotonNetwork", "SendAllOutgoingCommands");
    }

    VRRig_RequestCosmetics(self, info);
}

MAKE_HOOK_OFFSETLESS(VRRig_InitializeNoobMaterial, void, Il2CppObject* self, float red, float green, float blue)
{
    VRRig_InitializeNoobMaterial(self, red, green, blue);
    /*
    il2cpp_utils::SetFieldValue(self, "red", red);
    il2cpp_utils::SetFieldValue(self, "green", green);
    il2cpp_utils::SetFieldValue(self, "blue", blue);
    */
    Il2CppObject* photonView = *il2cpp_utils::RunMethod(self, "get_photonView");
    Il2CppObject* owner = *il2cpp_utils::RunMethod(photonView, "get_Owner");

    Il2CppString* UserIDCS = *il2cpp_utils::RunMethod<Il2CppString*>(owner, "get_UserId");
    std::string UserID = UserIDCS ? to_utf8(csstrtostr(UserIDCS)) : "";

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
