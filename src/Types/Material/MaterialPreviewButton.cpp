#include "Types/Material/MaterialPreviewButton.hpp"
#include "AssetLoader.hpp"
#include "Utils/CosmeticUtils.hpp"
#include "logging.hpp"
#include "config.hpp"
#include <thread>

#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/prettywriter.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/stringbuffer.h"

DEFINE_CLASS(GorillaCosmetics::MaterialPreviewButton);

bool GorillaCosmetics::MaterialPreviewButton::canPress = true;

void GorillaCosmetics::MaterialPreviewButton::OnTriggerEnter(Il2CppObject* collider)
{
    if (!canPress) return;
    Il2CppObject* handIndicator = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(collider, "GetComponentInParent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("", "GorillaTriggerColliderHandIndicator")}));
    if (handIndicator)
	{
        canPress = false;
		Il2CppObject* component = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(collider, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("", "GorillaTriggerColliderHandIndicator")}));
		if (!material) 
        {
            ERROR("Material Was Nullptr, returning");
            return;
        }
        
        // do stuff
		Il2CppObject* theMat = material->get_material();
        std::string name = material->get_descriptor().get_name();
        if(theMat)
		{
			INFO("Swapping to: %s", name.c_str());
		}
        else
		{
			INFO("Swapping to default material, name on default is %s", name.c_str());
		}

        // update config value
		config.lastActiveMaterial = name;
		AssetLoader::SelectMaterial(config.lastActiveMaterial);

		if (component)
		{
            Il2CppObject* gorillaTagger = CRASH_UNLESS(il2cpp_utils::RunMethod("", "GorillaTagger", "get_Instance"));
            bool isLeftHand = CRASH_UNLESS(il2cpp_utils::GetFieldValue<bool>(component, "isLeftHand"));
            float tapHapticStrength = CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(gorillaTagger, "tapHapticStrength"));
            float tapHapticDuration = CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(gorillaTagger, "tapHapticDuration"));

            CRASH_UNLESS(il2cpp_utils::RunMethod(gorillaTagger, "StartVibration", isLeftHand, tapHapticStrength / 2.0f, tapHapticDuration));            
		}

        UpdateMaterialValue();

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

void GorillaCosmetics::MaterialPreviewButton::UpdateMaterialValue()
{
    // get material name
    std::string material = material->get_descriptor().get_name();

    Il2CppObject* gorillaTagger = *il2cpp_utils::RunMethod("", "GorillaTagger", "get_Instance");
    Il2CppObject* offlineVRRig = *il2cpp_utils::GetFieldValue(gorillaTagger, "offlineVRRig");

    // because we always keep the hat name in the rigs, we can just get the hat name from that
    Il2CppString* hatCS = *il2cpp_utils::GetFieldValue<Il2CppString*>(offlineVRRig, "hat");
    Il2CppString* face = *il2cpp_utils::GetFieldValue<Il2CppString*>(offlineVRRig, "face");
    Il2CppString* badge = *il2cpp_utils::GetFieldValue<Il2CppString*>(offlineVRRig, "badge");
    
    std::string hat = to_utf8(csstrtostr(hatCS));
    
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

    // make the json
    d.AddMember("hat", rapidjson::Value(hat.c_str(), hat.size(), allocator), allocator);
    d.AddMember("material", rapidjson::Value(material.c_str(), material.size(), allocator), allocator);
    
    rapidjson::StringBuffer buffer;
    buffer.Clear();

    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);

    std::string messageString(buffer.GetString(), buffer.GetSize());
    // the string to replace the hat update value with
    Il2CppString* hatMessage = il2cpp_utils::createcsstr(messageString);
    
    if (offlineVRRig) // if offline rig is defined, localupdate it
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
        // again, just a copy of that GorillaHatButtonParent method
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