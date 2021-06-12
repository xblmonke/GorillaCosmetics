#include "Utils/UnityUtils.hpp"

#include "UnityEngine/EventSystems/EventTrigger.hpp"
#include "UnityEngine/EventSystems/UIBehaviour.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/Object.hpp"

using namespace UnityEngine;

namespace UnityUtils
{
    void SanitizePrefab(UnityEngine::GameObject* prefab)
    {
        Array<UnityEngine::EventSystems::EventTrigger*>* triggers = prefab->GetComponentsInChildren<UnityEngine::EventSystems::EventTrigger*>(true);
        int triggerLength = triggers->Length();
        for (int i = 0; i < triggerLength; i++)
        {
                Object::DestroyImmediate(triggers->values[i]);
        }

        Array<UnityEngine::EventSystems::UIBehaviour*>* uiBehaviours = prefab->GetComponentsInChildren<UnityEngine::EventSystems::UIBehaviour*>(true);
        int behavioursLength = triggers->Length();
        for (int i = 0; i < behavioursLength; i++)
        {
                Object::DestroyImmediate(uiBehaviours->values[i]);
        }

        Array<UnityEngine::Collider*>* colliders = prefab->GetComponentsInChildren<UnityEngine::Collider*>(true);
        int colliderLength = triggers->Length();
        for (int i = 0; i < colliderLength; i++)
        {
                Object::DestroyImmediate(colliders->values[i]);
        }
    }
}