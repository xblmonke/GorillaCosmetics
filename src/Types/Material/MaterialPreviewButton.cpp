#include "Types/Material/MaterialPreviewButton.hpp"
#include "AssetLoader.hpp"
#include "Utils/CosmeticUtils.hpp"
#include "logging.hpp"
#include "config.hpp"
#include <thread>

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

		config.lastActiveMaterial = name;
		AssetLoader::SelectMaterial(config.lastActiveMaterial);
        //CosmeticUtils::RefreshAllPlayers();

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
    std::string name = material->get_descriptor().get_name();
    static Il2CppString* propertyName = il2cpp_utils::createcsstr("faceCosmetic", il2cpp_utils::StringType::Manual);
    Il2CppString* face;
    if (name != "None")
        face = il2cpp_utils::createcsstr("custom:" + name);
    else
        face = il2cpp_utils::createcsstr("none");

    il2cpp_utils::RunMethod("UnityEngine", "PlayerPrefs", "SetString", propertyName, face);
    il2cpp_utils::RunMethod("UnityEngine", "PlayerPrefs", "Save");

    Il2CppObject* gorillaTagger = *il2cpp_utils::RunMethod("", "GorillaTagger", "get_Instance");
    Il2CppObject* offlineVRRig = *il2cpp_utils::GetFieldValue(gorillaTagger, "offlineVRRig");

    Il2CppString* hat = *il2cpp_utils::GetFieldValue<Il2CppString*>(offlineVRRig, "hat");
    Il2CppString* badge = *il2cpp_utils::GetFieldValue<Il2CppString*>(offlineVRRig, "badge");
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