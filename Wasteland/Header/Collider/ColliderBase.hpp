#pragma once

#include <memory>
#include <mutex>
#include <concepts>
#include <btBulletDynamicsCommon.h>
#include "ECS/Component.hpp"

using namespace Wasteland::ECS;

namespace Wasteland::Collider
{
    template <typename T>
    concept ColliderShape = std::derived_from<T, btCollisionShape>;

    template <ColliderShape T>
    class ColliderBase : public Component
    {
    
    public:

        virtual T* GetColliderShape() = 0;

    };

}