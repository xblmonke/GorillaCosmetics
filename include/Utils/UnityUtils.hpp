#pragma once

#include "UnityEngine/GameObject.hpp"

namespace UnityUtils
{
    void SanitizePrefab(UnityEngine::GameObject* prefab);

    template<class T>
    T max (T first, T second)
    {
        return first > second ? first : second;
    }

    template<class T>
    T max (T first, T second, T third)
    {
        return max(max(first, second), third);
    }
}