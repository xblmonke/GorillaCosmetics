#include "Types/Selector/HatRackSelectorButton.hpp"
#include <string>
#include "logging.hpp"

DEFINE_CLASS(GorillaCosmetics::HatRackSelectorButton);

bool GorillaCosmetics::HatRackSelectorButton::canPress = true;

namespace GorillaCosmetics
{
    void HatRackSelectorButton::Awake()
    {
        // init the button, based on name change which direction it "scrolls" 
        Il2CppObject* go = *il2cpp_utils::RunMethod(this, "get_gameObject");
        Il2CppString* csName = *il2cpp_utils::RunMethod<Il2CppString*>(go, "get_name");
        std::string name = to_utf8(csstrtostr(csName));

        if (name.find("Left") != std::string::npos) // left means previous
            next = false;
        else // right means next
            next = true;
    }

    void HatRackSelectorButton::OnTriggerEnter(Il2CppObject* collider)
    {
        // if cooldown not passed
        if (!canPress) return;

        // normal interaction with colliders
        Il2CppObject* handIndicator = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(collider, "GetComponentInParent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("", "GorillaTriggerColliderHandIndicator")}));
        if (handIndicator && selector)
	    {
            canPress = false;
	    	Il2CppObject* component = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(collider, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("", "GorillaTriggerColliderHandIndicator")}));

            // based on next bool go to next or previous
            if (next)
                selector->Next();
            else 
                selector->Previous();

            if (component)
	    	{
                Il2CppObject* gorillaTagger = CRASH_UNLESS(il2cpp_utils::RunMethod("", "GorillaTagger", "get_Instance"));
                bool isLeftHand = CRASH_UNLESS(il2cpp_utils::GetFieldValue<bool>(component, "isLeftHand"));
                float tapHapticStrength = CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(gorillaTagger, "tapHapticStrength"));
                float tapHapticDuration = CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(gorillaTagger, "tapHapticDuration"));
                CRASH_UNLESS(il2cpp_utils::RunMethod(gorillaTagger, "StartVibration", isLeftHand, tapHapticStrength / 2.0f, tapHapticDuration));            
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