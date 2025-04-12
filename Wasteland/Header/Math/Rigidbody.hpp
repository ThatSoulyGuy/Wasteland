#pragma once

#include <btBulletDynamicsCommon.h>
#include "Collider/PhysicsGlobal.hpp"
#include "Collider/Colliders/ColliderMesh.hpp"
#include "ECS/GameObject.hpp"
#include "Utility/Exception/Exceptions/NullPointerException.hpp"

using namespace Wasteland::Collider;
using namespace Wasteland::Collider::Colliders;
using namespace Wasteland::ECS;

namespace Wasteland::Math
{
    template <ColliderShape T>
    class Rigidbody final : public Component
    {

    public:

        ~Rigidbody()
        {
            if (handle)
            {
                PhysicsGlobal::GetInstance().GetWorld()->removeRigidBody(handle);

                delete handle;

                handle = nullptr;
            }
        }

        Rigidbody(const Rigidbody&) = delete;
        Rigidbody(Rigidbody&&) = delete;
        Rigidbody& operator=(const Rigidbody&) = delete;
        Rigidbody& operator=(Rigidbody&&) = delete;

        void Initialize() override
        {
            auto collider = Super::GetGameObject()->template GetComponent<ColliderBase<T>>().value_or(nullptr);

            if (!collider)
                throw std::runtime_error("Collider was null...");

            btCollisionShape* shape = collider->GetColliderShape();

            if (!shape)
                throw std::runtime_error("Shape was null...");

            float actualMass = isStatic ? 0.f : mass;

            btVector3 localInertia(0, 0, 0);

            if (actualMass > 0.f)
                shape->calculateLocalInertia(actualMass, localInertia);

            btRigidBody::btRigidBodyConstructionInfo rbInfo(actualMass, nullptr, shape, localInertia);

            rbInfo.m_linearDamping = 0.01f;
            rbInfo.m_angularDamping = 0.05f;

            handle = new btRigidBody(rbInfo);

            if (isStatic)
                handle->setCollisionFlags(handle->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
                                        
            short group, mask;

            if (isStatic)
            {
                group = btBroadphaseProxy::StaticFilter;
                mask  = btBroadphaseProxy::DefaultFilter;
            }
            else
            {
                group = btBroadphaseProxy::DefaultFilter;
                mask  = btBroadphaseProxy::AllFilter;
            }

            PhysicsGlobal::GetInstance().GetWorld()->addRigidBody(handle, group, mask);

            Super::GetGameObject()->GetTransform()->AddOnPositionChangedCallback([&](Vector<float, 3> position)
            {
                handle->getWorldTransform().setOrigin({ position.x(), position.y(), position.z() });
            });

            Super::GetGameObject()->GetTransform()->AddOnRotationChangedCallback([&](Vector<float, 3> rotation)
            {
                handle->getWorldTransform().setRotation({ rotation.x(), rotation.y(), rotation.z() });
            });
        }

        void Update() override
        {
            if (handle && !isStatic)
            {
                btTransform& transform = handle->getWorldTransform();

                btScalar rx, ry, rz;

                transform.getRotation().getEulerZYX(rz, ry, rx);

                Super::GetGameObject()->GetTransform()->SetLocalPosition({ transform.getOrigin().getX(), transform.getOrigin().getY(), transform.getOrigin().getZ()}, false);
                Super::GetGameObject()->GetTransform()->SetLocalRotation({rx, ry, rz}, false);
            }
        }

        void SetLinearVelocity(const btVector3& vel)
        {
            if (handle && !isStatic)
                handle->setLinearVelocity(vel);
        }

        btVector3 GetLinearVelocity() const
        {
            return handle ? handle->getLinearVelocity() : btVector3(0,0,0);
        }

        void SetAngularVelocity(const btVector3& vel)
        {
            if (handle && !isStatic)
                handle->setAngularVelocity(vel);
        }

        btVector3 GetAngularVelocity() const
        {
            return handle ? handle->getAngularVelocity() : btVector3(0,0,0);
        }

        void ApplyCentralForce(const btVector3& force)
        {
            if (handle && !isStatic)
                handle->applyCentralForce(force);
        }

        void ApplyTorque(const btVector3& torque)
        {
            if (handle && !isStatic)
                handle->applyTorque(torque);
        }

        void ApplyImpulse(const btVector3& impulse, const btVector3& rel_pos)
        {
            if (handle && !isStatic)
                handle->applyImpulse(impulse, rel_pos);
        }

        void ClearForces()
        {
            if (handle)
                handle->clearForces(); 
        }

        btRigidBody* GetHandle() const
        {
            return handle; 
        }

        static std::shared_ptr<Rigidbody> Create(float mass, bool isStatic = false)
        {
            auto result = std::shared_ptr<Rigidbody>(new Rigidbody());

            result->mass = mass;
            result->isStatic = isStatic;

            return result;
        }

    private:

        Rigidbody() = default;

        float mass = 0.f;
        bool isStatic = false;

        btRigidBody* handle = nullptr;
    };
}