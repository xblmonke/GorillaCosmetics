#include "Types/Selector/HatRackSelectorButton.hpp"
#include <string>
#include "logging.hpp"

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

DEFINE_TYPE(GorillaCosmetics::HatRackSelectorButton);

bool GorillaCosmetics::HatRackSelectorButton::canPress = true;

using namespace UnityEngine;

namespace GorillaCosmetics
{
    void HatRackSelectorButton::Awake()
    {
        // init the button, based on name change which direction it "scrolls" 
        Il2CppString* csName = get_gameObject()->get_name();
        std::string name = to_utf8(csstrtostr(csName));

        if (name.find("Left") != std::string::npos) // left means previous
            next = false;
        else // right means next
            next = true;
    }

    void HatRackSelectorButton::OnTriggerEnter(Collider* collider)
    {
        // if cooldown not passed
        if (!canPress) return;

        // normal interaction with colliders
        GorillaTriggerColliderHandIndicator* handIndicator = collider->GetComponentInParent<GorillaTriggerColliderHandIndicator*>();
        if (handIndicator && selector)
	    {
            canPress = false;
		    GorillaTriggerColliderHandIndicator* component = collider->GetComponent<GorillaTriggerColliderHandIndicator*>();

            // based on next bool go to next or previous
            if (next)
                selector->Next();
            else 
                selector->Previous();

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
                for (int i =0; i < 150; i++)
                {
                    usleep(1000);
                }
                canPress = true;
            });
            rePress.detach();
	    }
    }
}