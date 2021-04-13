#include "Types/Selector/HatRackSelector.hpp"
#include "logging.hpp"
DEFINE_TYPE(GorillaCosmetics::HatRackSelector);

using namespace UnityEngine;

namespace GorillaCosmetics
{
    void HatRackSelector::ctor()
    {
        selectedIndex = 0;
        racks = *il2cpp_utils::New<List<GameObject*>*>();
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
        getLogger().info("Updating racks");
        for (int i = 0; i < racks->size; i++)
        {
            GameObject* obj = racks->items->values[i];
            obj->SetActive(selectedIndex == i);
        }
    }
}