#pragma once

#include "ECS/GameObject.hpp"

namespace Wasteland::ECS
{
	class GameObjectManager
	{

	public:
		
		GameObjectManager(const GameObjectManager&) = delete;
		GameObjectManager(GameObjectManager&&) = delete;
		GameObjectManager& operator=(const GameObjectManager&) = delete;
		GameObjectManager& operator=(GameObjectManager&&) = delete;

		std::shared_ptr<GameObject> Register(std::shared_ptr<GameObject> gameObject)
		{
			std::string name = gameObject->GetName();

			gameObjectMap.insert({ name, std::move(gameObject) });

			return gameObjectMap[name];
		}

		void Unregister(const std::string& name)
		{
			gameObjectMap.erase(name);
		}

		void Update()
		{
			std::for_each(gameObjectMap.begin(), gameObjectMap.end(), [&](const auto& pair) { pair.second->Update(); });
		}

		void Render(std::optional<std::shared_ptr<Wasteland::Render::Camera>> camera)
		{
			if (!camera.has_value())
				return;

			std::for_each(gameObjectMap.begin(), gameObjectMap.end(), [&](const auto& pair) { pair.second->Render(camera.value()); });
		}

		static GameObjectManager& GetInstance()
		{
			std::call_once(initalizationFlag, [&]()
			{
				instance = std::unique_ptr<GameObjectManager>(new GameObjectManager());
			});

			return *instance;
		}

	private:

		GameObjectManager() = default;

		std::unordered_map<std::string, std::shared_ptr<GameObject>> gameObjectMap;

		static std::once_flag initalizationFlag;
		static std::unique_ptr<GameObjectManager> instance;

	};

	std::once_flag GameObjectManager::initalizationFlag;
	std::unique_ptr<GameObjectManager> GameObjectManager::instance;
}