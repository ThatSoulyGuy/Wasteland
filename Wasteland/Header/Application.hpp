#pragma once

#include "Core/InputManager.hpp"
#include "Core/Window.hpp"
#include "ECS/GameObjectManager.hpp"
#include "Entity/Entities/EntityPlayer.hpp"
#include "Math/Matrix.hpp"
#include "Render/Mesh.hpp"
#include "Render/ShaderManager.hpp"
#include "Render/TextureManager.hpp"
#include "World/WorldBase.hpp"

using namespace Wasteland::Core;
using namespace Wasteland::ECS;
using namespace Wasteland::Entity;
using namespace Wasteland::Entity::Entities;
using namespace Wasteland::Render;
using namespace Wasteland::World;

namespace Wasteland
{
	class Application final
	{

	public:

		void PreInitialize()
		{
			Window::GetInstance().Initialize("Wasteland* 1.11.9", { 750, 450 });

			InputManager::GetInstance().Initialize();

			ShaderManager::GetInstance().Register(Shader::Create("default", { "Wasteland", "Shader/Default" }));
			TextureManager::GetInstance().Register(Texture::Create("debug", { "Wasteland", "Texture/Debug.png" }));
			TextureManager::GetInstance().Register(Texture::Create("grass", { "Wasteland", "Texture/Grass.png" }));
		}

		void Initialize()
		{
			worldObject = GameObjectManager::GetInstance().Register(GameObject::Create("default.world"));
			
			worldObject->AddComponent(WorldBase::Create());

			playerObject = GameObjectManager::GetInstance().Register(GameObject::Create("default.player"));

			playerObject->AddComponent(EntityBase::Create<EntityPlayer>());
		}

		void Update()
		{
			worldObject->GetComponent<WorldBase>().value()->chunkLoaderPosition = playerObject->GetTransform()->GetWorldPosition();

			GameObjectManager::GetInstance().Update();
		}

		void Render()
		{
			Window::GetInstance().Clear();

			GameObjectManager::GetInstance().Render(playerObject->GetComponent<EntityPlayer>().value()->GetCamera());

			InputManager::GetInstance().Update();

			Window::GetInstance().Present();
		}

		void Uninitialize()
		{
			Window::GetInstance().Uninitialize();
		}

		bool IsRunning()
		{
			return Window::GetInstance().IsRunning();
		}

		static Application& GetInstance()
		{
			std::call_once(initalizationFlag, [&]()
			{
				instance = std::unique_ptr<Application>(new Application());
			});

			return *instance;
		}

	private:

		std::shared_ptr<GameObject> playerObject;
		std::shared_ptr<GameObject> worldObject;

		static std::once_flag initalizationFlag;
		static std::unique_ptr<Application> instance;

	};

	std::once_flag Application::initalizationFlag;
	std::unique_ptr<Application> Application::instance;
}