#include "Types/Material/MaterialPreviewButton.hpp"
#include "AssetLoader.hpp"
#include "Utils/CosmeticUtils.hpp"
#include "logging.hpp"
#include "config.hpp"

DEFINE_CLASS(GorillaCosmetics::MaterialPreviewButton);

void GorillaCosmetics::MaterialPreviewButton::OnTriggerEnter(Il2CppObject* collider)
{
    Il2CppObject* handIndicator = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(collider, "GetComponentInParent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("", "GorillaTriggerColliderHandIndicator")}));
    if (handIndicator)
	{
		Il2CppObject* component = CRASH_UNLESS(il2cpp_utils::RunGenericMethod(collider, "GetComponent", std::vector<Il2CppClass*>{il2cpp_utils::GetClassFromName("", "GorillaTriggerColliderHandIndicator")}));
		
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
        CosmeticUtils::RefreshAllPlayers();

		if (component)
		{
            Il2CppObject* gorillaTagger = CRASH_UNLESS(il2cpp_utils::RunMethod("", "GorillaTagger", "get_Instance"));
            bool isLeftHand = CRASH_UNLESS(il2cpp_utils::GetFieldValue<bool>(component, "isLeftHand"));
            float tapHapticStrength = CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(gorillaTagger, "tapHapticStrength"));
            float tapHapticDuration = CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(gorillaTagger, "tapHapticDuration"));

            CRASH_UNLESS(il2cpp_utils::RunMethod(gorillaTagger, "StartVibration", isLeftHand, tapHapticStrength / 2.0f, tapHapticDuration));            
		}
	}
}