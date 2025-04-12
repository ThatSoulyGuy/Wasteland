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

        btDiscreteDynamicsWorld* GetWorld()
        {
            return worldHandle;
        }

        void Uninitialize()
        {
            delete worldHandle;
        }

        static PhysicsGlobal& GetInstance()
		{
			std::call_once(initializationFlag, [&]()
			{
				instance = std::unique_ptr<PhysicsGlobal>(new PhysicsGlobal());
			});

			return *instance;
		}

    private:

        PhysicsGlobal()
        {
            btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
        
            btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
        
            btBroadphaseInterface* broadphase = new btDbvtBroadphase();
        
            btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
        
            worldHandle = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
        
            worldHandle->setGravity(btVector3(0, -9.81f, 0));
        }

        btDiscreteDynamicsWorld* worldHandle;

        static std::once_flag initializationFlag;
        static std::unique_ptr<PhysicsGlobal> instance;

    };

    std::once_flag PhysicsGlobal::initializationFlag;

    std::unique_ptr<PhysicsGlobal> PhysicsGlobal::instance;
}