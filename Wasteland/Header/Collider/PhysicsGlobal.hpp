#pragma once

#include <mutex>
#include <btBulletDynamicsCommon.h>

namespace Wasteland::Collider
{
    class PhysicsGlobal final
    {

    public:

        PhysicsGlobal(const PhysicsGlobal&) = delete;
        PhysicsGlobal(PhysicsGlobal&&) = delete;
        PhysicsGlobal& operator=(const PhysicsGlobal&) = delete;
        PhysicsGlobal& operator=(PhysicsGlobal&&) = delete;

        static btDiscreteDynamicsWorld* CreateOrGetWorld()
        {
            std::call_once(initializationFlag, [&]()
            {
                btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
            
                btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
            
                btBroadphaseInterface* broadphase = new btDbvtBroadphase();
            
                btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
            
                world = std::make_unique<btDiscreteDynamicsWorld>(dispatcher, broadphase, solver, collisionConfiguration);
            
                world->setGravity(btVector3(0, -9.81f, 0));
            });
        
            return world.get();
        }

    private:

        PhysicsGlobal() = default;

        static std::once_flag initializationFlag;
        static std::unique_ptr<btDiscreteDynamicsWorld> world;

    };

    std::once_flag PhysicsGlobal::initializationFlag;

    std::unique_ptr<btDiscreteDynamicsWorld> PhysicsGlobal::world;
}