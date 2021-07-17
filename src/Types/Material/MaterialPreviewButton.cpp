#include "Types/Material/MaterialPreviewButton.hpp"
#include "AssetLoader.hpp"
#include "Utils/CosmeticUtils.hpp"
#include "logging.hpp"
#include "config.hpp"
#include <thread>

#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/prettywriter.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/stringbuffer.h"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/PlayerPrefs.hpp"

#include "GlobalNamespace/GorillaTriggerColliderHandIndicator.hpp"
#include "GlobalNamespace/GorillaTagger.hpp"
#include "GlobalNamespace/VRRig.hpp"

#include "Photon/Pun/PhotonView.hpp"
#include "Photon/Pun/PhotonNetwork.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace Photon::Pun;

DEFINE_TYPE(GorillaCosmetics, MaterialPreviewButton);

void GorillaCosmetics::MaterialPreviewButton::OnTriggerEnter(Collider* collider)
{
    if (!canPress()) return;
    GorillaTriggerColliderHandIndicator* handIndicator = collider->GetComponentInParent<GorillaTriggerColliderHandIndicator*>();
    if (handIndicator)
	{
        canPress() = false;
		GorillaTriggerColliderHandIndicator* component = collider->GetComponent<GorillaTriggerColliderHandIndicator*>();
		if (!material) 
        {
            ERROR("Material Was Nullptr, returning");
            return;
        }
        
        // do stuff
		GameObject* theMat = material->get_material();
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
            GorillaTagger* gorillaTagger = GorillaTagger::get_Instance();

            bool isLeftHand = component->isLeftHand;
            float tapHapticStrength = gorillaTagger->tapHapticStrength;
            float tapHapticDuration = gorillaTagger->tapHapticDuration;

            gorillaTagger->StartVibration(component->isLeftHand,  tapHapticStrength / 2.0f, tapHapticDuration);        
		}

        UpdateMaterialValue();

        std::thread rePress([&](){
            for (int i =0; i < 2000; i++)
            {
                usleep(1000);
            }
            canPress() = true;
        });

        rePress.detach();
	}
}

void GorillaCosmetics::MaterialPreviewButton::UpdateMaterialValue()
{
    // get material name
    std::string material = this->material->get_descriptor().get_name();

    // get the gorilla tagger
    GorillaTagger* gorillaTagger = GorillaTagger::get_Instance();
    // get offline VR rig
    VRRig* offlineVRRig = gorillaTagger->offlineVRRig;

    // because we always keep the hat name in the rigs, we can just get the hat name from that
    Il2CppString* hatCS = offlineVRRig->hat;
    Il2CppString* face = offlineVRRig->face;
    Il2CppString* badge = offlineVRRig->badge;
    
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
        offlineVRRig->LocalUpdateCosmetics(badge, face, hatMessage);
    }
    else
    {
        ERROR("offline VRRig was nullptr");
    }

    // this is for online play
    VRRig* myVRRig = gorillaTagger->myVRRig;
    if (myVRRig)
    {
        // again, just a copy of that GorillaHatButtonParent method
        PhotonView* photonView = myVRRig->get_photonView();
        static Il2CppString* methodName = il2cpp_utils::createcsstr("UpdateCosmetics", il2cpp_utils::StringType::Manual);
        
        Array<Il2CppObject*>* argsArray = reinterpret_cast<Array<Il2CppObject*>*>(il2cpp_functions::array_new(classof(Il2CppObject*), 3));
        argsArray->values[0] = (Il2CppObject*)badge;
        argsArray->values[1] = (Il2CppObject*)face;
        argsArray->values[2] = (Il2CppObject*)hatMessage;
        
        photonView->RPC(methodName, RpcTarget::All, argsArray);
        PhotonNetwork::SendAllOutgoingCommands();
    }
    else
    {
        ERROR("My VRRig was nullptr");
    }
}