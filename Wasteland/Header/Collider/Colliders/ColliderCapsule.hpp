#pragma once

#include "Collider/ColliderBase.hpp"
#include "ECS/GameObject.hpp"
#include "Render/Vertex.hpp"

using namespace Wasteland::Collider;
using namespace Wasteland::Render;

namespace Wasteland::Collider::Colliders
{
    class ColliderCapsule final : public ColliderBase<btCapsuleShape>
    {

    public:

        ~ColliderCapsule()
        {
            delete shape;

            shape = nullptr;
        }

        void Initialize() override
        {
            shape = new btCapsuleShape(radius, height);
        }

        btCapsuleShape* GetColliderShape() override
        {
            return shape;
        }

        static std::shared_ptr<ColliderCapsule> Create(float radius, float height)
        {
            std::shared_ptr<ColliderCapsule> result(new ColliderCapsule());

            result->radius = radius;
            result->height = height;

            return result;
        }

    private:

        ColliderCapsule() = default;

        btCapsuleShape* shape;

        float radius;
        float height;

    };
}