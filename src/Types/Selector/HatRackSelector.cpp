#include "Types/Selector/HatRackSelector.hpp"
#include "logging.hpp"
DEFINE_TYPE(GorillaCosmetics::HatRackSelector);

namespace GorillaCosmetics
{
    void HatRackSelector::Awake()
    {
        // just initializes the class, makes sure the index is 0 and that the list exists
        INFO("RackSelector Awake");
        selectedIndex = 0;
        static std::vector<const Il2CppClass*> gameObjectKlass = {il2cpp_utils::GetClassFromName("UnityEngine", "GameObject")};
        static Il2CppClass* listKlass = il2cpp_utils::GetClassFromName("System.Collections.Generic", "List`1");
        static Il2CppClass* gameObjectListKlass = il2cpp_utils::MakeGeneric(listKlass, gameObjectKlass);
        racks = CRASH_UNLESS(il2cpp_utils::New<List<Il2CppObject*>*>(gameObjectListKlass));
        INFO("Racks: %p, this: %p", racks, this);
    }

    void HatRackSelector::Next()
    {
        // go to the next rack
        selectedIndex++;
        if (selectedIndex >= racks->size)
        {
            selectedIndex = 0;
        }
        UpdateRack();
    }

    void HatRackSelector::Previous()
    {
        // go to previous rack
        selectedIndex--;
        if (selectedIndex < 0)
        {
            selectedIndex = racks->size - 1;
        }

        UpdateRack();
    }

    void HatRackSelector::UpdateRack()
    {
        // make sure the right rack from the list is active
        for (int i = 0; i < racks->size; i++)
        {
            il2cpp_utils::RunMethod(racks->items->values[i], "SetActive", selectedIndex == i);
        }
    }
}