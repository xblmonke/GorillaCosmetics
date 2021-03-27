#include "Types/Hat/HatPreviewButton.hpp"
#include "AssetLoader.hpp"
#include "Utils/CosmeticUtils.hpp"
#include "logging.hpp"
#include "config.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include <thread>

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
        //CosmeticUtils::RefreshAllPlayers();

		if (component)
		{
            Il2CppObject* gorillaTagger = CRASH_UNLESS(il2cpp_utils::RunMethod("", "GorillaTagger", "get_Instance"));
            bool isLeftHand = CRASH_UNLESS(il2cpp_utils::GetFieldValue<bool>(component, "isLeftHand"));
            float tapHapticStrength = CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(gorillaTagger, "tapHapticStrength"));
            float tapHapticDuration = CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(gorillaTagger, "tapHapticDuration"));

            CRASH_UNLESS(il2cpp_utils::RunMethod(gorillaTagger, "StartVibration", isLeftHand, tapHapticStrength / 2.0f, tapHapticDuration));            
		}

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
    std::string name = hat->get_descriptor().get_name();
    static Il2CppString* propertyName = il2cpp_utils::createcsstr("hatCosmetic", il2cpp_utils::StringType::Manual);
    Il2CppString* hat;
    hat = il2cpp_utils::createcsstr("custom:" + name);
    il2cpp_utils::RunMethod("UnityEngine", "PlayerPrefs", "SetString", propertyName, hat);
    il2cpp_utils::RunMethod("UnityEngine", "PlayerPrefs", "Save");

    Il2CppObject* gorillaTagger = *il2cpp_utils::RunMethod("", "GorillaTagger", "get_Instance");
    Il2CppObject* offlineVRRig = *il2cpp_utils::GetFieldValue(gorillaTagger, "offlineVRRig");

    Il2CppString* badge = *il2cpp_utils::GetFieldValue<Il2CppString*>(offlineVRRig, "badge");
    Il2CppString* face = *il2cpp_utils::GetFieldValue<Il2CppString*>(offlineVRRig, "face");
    if (offlineVRRig)
    {
        il2cpp_utils::RunMethod(offlineVRRig, "LocalUpdateCosmetics", badge, face, hat);
    }
    else
    {
        ERROR("offline VRRig was nullptr");
    }

    Il2CppObject* myVRRig = *il2cpp_utils::GetFieldValue(gorillaTagger, "myVRRig");
    if (myVRRig)
    {
        Il2CppObject* photonView = *il2cpp_utils::RunMethod(myVRRig, "get_photonView");
        static Il2CppString* methodName = il2cpp_utils::createcsstr("UpdateCosmetics", il2cpp_utils::StringType::Manual);
        
        Array<Il2CppObject*>* argsArray = reinterpret_cast<Array<Il2CppObject*>*>(il2cpp_functions::array_new(classof(Il2CppObject*), 3));
        argsArray->values[0] = (Il2CppObject*)badge;
        argsArray->values[1] = (Il2CppObject*)face;
        argsArray->values[2] = (Il2CppObject*)hat;
        
        il2cpp_utils::RunMethod(photonView, "RPC", methodName, 0, argsArray);

        il2cpp_utils::RunMethod("Photon.Pun", "PhotonNetwork", "SendAllOutgoingCommands");
    }
    else
    {
        ERROR("My VRRig was nullptr");
    }
}