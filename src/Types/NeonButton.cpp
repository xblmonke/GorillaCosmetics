#include "NeonButton.hpp"
#include "config.hpp"
#include <string>
#include "logging.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/Text.hpp"
#include "UnityEngine/PlayerPrefs.hpp"

#include "GlobalNamespace/GorillaTriggerColliderHandIndicator.hpp"
#include "GlobalNamespace/GorillaTagger.hpp"
#include "GlobalNamespace/VRRig.hpp"

#include "Photon/Pun/PhotonView.hpp"
#include "Photon/Pun/PhotonNetwork.hpp"
#include "Utils/UnityUtils.hpp"
#include "gorilla-utils/shared/Utils/RPC.hpp"
#include "gorilla-utils/shared/CustomProperties/Player.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;
using namespace Photon::Pun;

DEFINE_TYPE(GorillaCosmetics, NeonButton);

using namespace UnityEngine;

namespace GorillaCosmetics
{
    void NeonButton::Awake()
    {
        get_gameObject()->set_layer(18);
        GetComponent<Collider*>()->set_isTrigger(true);
        canPress = true;

        // toggle twice for initialization
        ToggleNeon();
        ToggleNeon();
    }

    void NeonButton::OnTriggerEnter(Collider* collider)
    {
        // if cooldown not passed
        if (!canPress) return;

        // normal interaction with colliders
        GorillaTriggerColliderHandIndicator* handIndicator = collider->GetComponentInParent<GorillaTriggerColliderHandIndicator*>();
        if (handIndicator)
	    {
            canPress = false;
		    GorillaTriggerColliderHandIndicator* component = collider->GetComponent<GorillaTriggerColliderHandIndicator*>();

            ToggleNeon();
            
            if (component)
	    	{
                GorillaTagger* gorillaTagger = GorillaTagger::get_Instance();

                bool isLeftHand = component->isLeftHand;
                float tapHapticStrength = gorillaTagger->tapHapticStrength;
                float tapHapticDuration = gorillaTagger->tapHapticDuration;

                gorillaTagger->StartVibration(component->isLeftHand,  tapHapticStrength / 2.0f, tapHapticDuration);              
	    	}

            // cooldown thing in cpp because we have no coroutines
            std::thread rePress([&](){
                for (int i =0; i < 1000; i++)
                {
                    usleep(1000);
                }
                canPress = true;
            });
            rePress.detach();
	    }
    }

    void NeonButton::ToggleNeon()
    {
        // toggle value
        config.overrideNeon ^= 1;

        if (Photon::Pun::PhotonNetwork::get_InRoom())
        {
            Array<GlobalNamespace::VRRig*>* rigs = UnityEngine::Object::FindObjectsOfType<GlobalNamespace::VRRig*>();
            int length = rigs->Length();
            
            for (int i = 0; i < length; i++)
            {
                GlobalNamespace::VRRig* rig = rigs->values[i];
                if (!rig) continue;

                // ask for the colors, so that all colors match up
                Photon::Pun::PhotonView* photonView = rig->get_photonView();
                if (!photonView) continue;
                Photon::Realtime::Player* player = photonView->get_Owner();
                GorillaUtils::RPC::RPC(photonView, "RequestMaterialColor", player, Photon::Pun::PhotonNetwork::get_LocalPlayer(), true);
            }
        }

        if (config.overrideNeon)
        {
            GetComponent<Renderer*>()->get_material()->set_color(Color::get_green());
            GetComponentInChildren<UI::Text*>()->set_color(Color::get_black());

            GorillaUtils::Player::SetProperty(Photon::Pun::PhotonNetwork::get_LocalPlayer(), "overrideNeon", true);
        }
        else
        {
            static Color darkRed = Color(0.5f, 0.0f, 0.0f);
            GetComponent<Renderer*>()->get_material()->set_color(darkRed);
            GetComponentInChildren<UI::Text*>()->set_color(Color::get_white());
            GorillaUtils::Player::SetProperty(Photon::Pun::PhotonNetwork::get_LocalPlayer(), "overrideNeon", false);
        }
    }
}