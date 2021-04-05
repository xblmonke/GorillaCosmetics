#include "Types/Hat/HatPreviewButton.hpp"
#include "AssetLoader.hpp"
#include "Utils/CosmeticUtils.hpp"
#include "logging.hpp"
#include "config.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include <thread>
#include "PlayerCosmeticsList.hpp"

#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/prettywriter.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/stringbuffer.h"

DEFINE_CLASS(GorillaCosmetics::HatPreviewButton);

bool GorillaCosmetics::HatPreviewButton::canPress = true;

void GorillaCosmetics::HatPreviewButton::OnTriggerEnter(Il2CppObject* collider)
{
    if (!canPress) return;
    Il2CppObject* handIndicator = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(collider, "GetComponentInParent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("", "GorillaTriggerColliderHandIndicator")}));
    if (handIndicator)
	{
        canPress = false;
		Il2CppObject* component = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(collider, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("", "GorillaTriggerColliderHandIndicator")}));
		if (!hat)
        {
            ERROR("Hat Was Nullptr, returning");
            return;
        }
        
        // do stuff
		Il2CppObject* theHat = hat->get_hat();
        std::string name = hat->get_descriptor().get_name();
        
        if(name != "None")
		{
			INFO("Swapping to: %s", name.c_str());
		}
        else
		{
			INFO("Swapping to default hat, name on default is %s", name.c_str());
		}

		config.lastActiveHat = name;
		AssetLoader::SelectHat(config.lastActiveHat);

		if (component)
		{
            Il2CppObject* gorillaTagger = CRASH_UNLESS(il2cpp_utils::RunMethod("", "GorillaTagger", "get_Instance"));
            bool isLeftHand = CRASH_UNLESS(il2cpp_utils::GetFieldValue<bool>(component, "isLeftHand"));
            float tapHapticStrength = CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(gorillaTagger, "tapHapticStrength"));
            float tapHapticDuration = CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(gorillaTagger, "tapHapticDuration"));

            CRASH_UNLESS(il2cpp_utils::RunMethod(gorillaTagger, "StartVibration", isLeftHand, tapHapticStrength / 2.0f, tapHapticDuration));            
		}

        // runs the update method and such to make sure the values are synced
        UpdateHatValue();

        std::thread rePress([&](){
            for (int i =0; i < 2000; i++)
            {
                usleep(1000);
            }
            canPress = true;
        });

        rePress.detach();
	}
}

void GorillaCosmetics::HatPreviewButton::UpdateHatValue()
{
    // get hat name
    std::string name = hat->get_descriptor().get_name();

    // player prefs value to save to
    static Il2CppString* propertyName = il2cpp_utils::createcsstr("hatCosmetic", il2cpp_utils::StringType::Manual);

    // the actual name that will be saved
    std::string hatString = "custom:" + name;
    Il2CppString*hatCS = il2cpp_utils::createcsstr(hatString);
    
    // save the custom hat to player prefs
    il2cpp_utils::RunMethod("UnityEngine", "PlayerPrefs", "SetString", propertyName, hatCS);
    il2cpp_utils::RunMethod("UnityEngine", "PlayerPrefs", "Save");

    // get the gorilla tagger
    Il2CppObject* gorillaTagger = *il2cpp_utils::RunMethod("", "GorillaTagger", "get_Instance");
    // get offline VR rig
    Il2CppObject* offlineVRRig = *il2cpp_utils::GetFieldValue(gorillaTagger, "offlineVRRig");

    // get badge and face names
    Il2CppString* badge = *il2cpp_utils::GetFieldValue<Il2CppString*>(offlineVRRig, "badge");
    Il2CppString* face = *il2cpp_utils::GetFieldValue<Il2CppString*>(offlineVRRig, "face");
    
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

    // add the members
    d.AddMember("hat", rapidjson::Value(hatString.c_str(), hatString.size(), allocator), allocator);

    // material can just be taken from config
    std::string materialString = config.lastActiveMaterial;
    d.AddMember("material", rapidjson::Value(materialString.c_str(), materialString.size(), allocator), allocator);

    rapidjson::StringBuffer buffer;
    buffer.Clear();

    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    
    // make the string
    std::string messageString(buffer.GetString(), buffer.GetSize());
    
    // the value to send instead of the hat name
    Il2CppString* hatMessage = il2cpp_utils::createcsstr(messageString);

    if (offlineVRRig) // if offline rig, run the update method on it
    {
        il2cpp_utils::RunMethod(offlineVRRig, "LocalUpdateCosmetics", badge, face, hatMessage);
    }
    else
    {
        ERROR("offline VRRig was nullptr");
    }

    // this is for online play
    Il2CppObject* myVRRig = *il2cpp_utils::GetFieldValue(gorillaTagger, "myVRRig");
    if (myVRRig)
    {
        // this all is basically a copy of the GorillaHatButtonParent update method or whatever the method is
        // the one that runs the UpdateCosmetics method and such
        Il2CppObject* photonView = *il2cpp_utils::RunMethod(myVRRig, "get_photonView");
        static Il2CppString* methodName = il2cpp_utils::createcsstr("UpdateCosmetics", il2cpp_utils::StringType::Manual);
        
        Array<Il2CppObject*>* argsArray = reinterpret_cast<Array<Il2CppObject*>*>(il2cpp_functions::array_new(classof(Il2CppObject*), 3));
        argsArray->values[0] = (Il2CppObject*)badge;
        argsArray->values[1] = (Il2CppObject*)face;
        argsArray->values[2] = (Il2CppObject*)hatMessage;
        
        il2cpp_utils::RunMethod(photonView, "RPC", methodName, 0, argsArray);

        il2cpp_utils::RunMethod("Photon.Pun", "PhotonNetwork", "SendAllOutgoingCommands");
    }
    else
    {
        ERROR("My VRRig was nullptr");
    }
}