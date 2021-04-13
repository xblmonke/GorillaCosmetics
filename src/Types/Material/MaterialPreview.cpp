#include "Types/Material/MaterialPreview.hpp"
#include "logging.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Collider.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Material.hpp"

using namespace UnityEngine;

GorillaCosmetics::MaterialPreview::MaterialPreview(Material mat, Transform* parent, Vector3 localPos, float scale)
{
    INFO("Creating preview for %s", mat.get_descriptor().get_name().c_str());

    gameObject = GameObject::CreatePrimitive(PrimitiveType::Sphere);
    gameObject->set_layer(18);

    Transform* transform = gameObject->get_transform();

    transform->SetParent(parent);

    transform->set_localScale(Vector3::get_one() * scale);
    transform->set_localPosition(localPos);
    transform->set_rotation(Quaternion::get_identity());
    Object::DontDestroyOnLoad(gameObject);

    Collider* collider = gameObject->GetComponent<Collider*>();
    collider->set_isTrigger(true);

    button = gameObject->AddComponent<MaterialPreviewButton*>();
    button->material = new Material(mat.get_manifest().get_filePath());
   
    GameObject* theMat = mat.get_material();
    UnityEngine::Material* originalMat = nullptr;
    if (theMat)
    {
        INFO("Setting custom material");
        Renderer* renderer = theMat->GetComponent<Renderer*>();
        originalMat = renderer->get_material();
    }
    else
    {
        originalMat = Resources::Load<UnityEngine::Material*>(il2cpp_utils::createcsstr("objects/treeroom/materials/lightfur"));
    }
    Renderer* otherRenderer = gameObject->GetComponent<Renderer*>();
    otherRenderer->set_material(originalMat);
}