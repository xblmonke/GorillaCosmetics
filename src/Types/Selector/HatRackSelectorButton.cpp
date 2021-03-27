#include "Types/Selector/HatRackSelectorButton.hpp"
#include <string>
#include "logging.hpp"

DEFINE_CLASS(GorillaCosmetics::HatRackSelectorButton);

bool GorillaCosmetics::HatRackSelectorButton::canPress = true;

namespace GorillaCosmetics
{
    void HatRackSelectorButton::Awake()
    {
        Il2CppObject* go = *il2cpp_utils::RunMethod(this, "get_gameObject");
        Il2CppString* csName = *il2cpp_utils::RunMethod<Il2CppString*>(go, "get_name");
        std::string name = to_utf8(csstrtostr(csName));

        if (name.find("Left") != std::string::npos)
            next = false;
        else 
            next = true;
        INFO("Found selector button %s", name.c_str());
    }

    void HatRackSelectorButton::OnTriggerEnter(Il2CppObject* collider)
    {
        INFO("OnTriggerEnter: %d, Selector: %p, collider %p", next, selector, collider);
        if (!canPress) return;
        Il2CppObject* handIndicator = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(collider, "GetComponentInParent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("", "GorillaTriggerColliderHandIndicator")}));
        if (handIndicator && selector)
	    {
            canPress = false;
	    	Il2CppObject* component = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(collider, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("", "GorillaTriggerColliderHandIndicator")}));
 
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