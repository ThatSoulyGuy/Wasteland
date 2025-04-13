#pragma once

#include "Collider/Colliders/ColliderCapsule.hpp"
#include "Core/InputManager.hpp"
#include "Core/Window.hpp"
#include "ECS/GameObjectManager.hpp"
#include "Entity/Entities/EntityPlayer.hpp"
#include "Math/Matrix.hpp"
#include "Render/Mesh.hpp"
#include "Render/ShaderManager.hpp"
#include "Render/TextureManager.hpp"
#include "Utility/Time.hpp"
#include "World/WorldBase.hpp"

using namespace Wasteland::Core;
using namespace Wasteland::ECS;
using namespace Wasteland::Entity;
using namespace Wasteland::Entity::Entities;
using namespace Wasteland::Render;
using namespace Wasteland::Utility;
using namespace Wasteland::World;

namespace Wasteland
{
	class Application final
	{

	public:

		void PreInitialize()
		{
			Window::GetInstance().Initialize("Wasteland* 9.2.3-alpha", { 750, 450 });

			InputManager::GetInstance().Initialize();

			ShaderManager::GetInstance().Register(Shader::Create("default", { "Wasteland", "Shader/Default" }));
			TextureManager::GetInstance().Register(Texture::Create("debug", { "Wasteland", "Texture/Debug.png" }));
			TextureManager::GetInstance().Register(Texture::Create("grass", { "Wasteland", "Texture/Grass.png" }));
		}

		void Initialize()
		{
			worldObject = GameObjectManager::GetInstance().Register(GameObject::Create("default.world"));
			
			worldObject.lock()->AddComponent(WorldBase::Create());

			playerObject = GameObjectManager::GetInstance().Register(GameObject::Create("default.player"));

			playerObject.lock()->AddComponent(ColliderCapsule::Create(0.5f, 2.0f));
			playerObject.lock()->AddComponent(Rigidbody<btCapsuleShape>::Create(10.0f));
			playerObject.lock()->AddComponent(EntityBase::Create<EntityPlayer>());

			playerObject.lock()->GetComponent<Rigidbody<btCapsuleShape>>().value()->SetRotationConstraints({ false, false, false });
			
			playerObject.lock()->GetTransform()->SetLocalPosition({ 10.0f, 20.0f, 10.0f });
		}

		void Update()
		{
			worldObject.lock()->GetComponent<WorldBase>().value()->chunkLoaderPosition = playerObject.lock()->GetTransform()->GetWorldPosition();

			GameObjectManager::GetInstance().Update();

			PhysicsGlobal::GetInstance().GetWorld()->stepSimulation(Time::GetInstance().GetDeltaTime());

			Time::GetInstance().Update();
		}

		void Render()
		{
			Window::GetInstance().Clear();

			GameObjectManager::GetInstance().Render(playerObject.lock()->GetComponent<EntityPlayer>().value()->GetCamera());

			MainThreadExecutor::GetInstance().Execute();

			InputManager::GetInstance().Update();

			Window::GetInstance().Present();
		}

		void Uninitialize()
		{
			GameObjectManager::GetInstance().Uninitialize();

			PhysicsGlobal::GetInstance().Uninitialize();

			Window::GetInstance().Uninitialize();
		}

		bool IsRunning()
		{
			return Window::GetInstance().IsRunning();
		}

		static Application& GetInstance()
		{
			std::call_once(initializationFlag, [&]()
			{
				instance = std::unique_ptr<Application>(new Application());
			});

			return *instance;
		}

	private:

		std::weak_ptr<GameObject> playerObject;
		std::weak_ptr<GameObject> worldObject;

		static std::once_flag initializationFlag;
		static std::unique_ptr<Application> instance;

	};

	std::once_flag Application::initializationFlag;
	std::unique_ptr<Application> Application::instance;
}