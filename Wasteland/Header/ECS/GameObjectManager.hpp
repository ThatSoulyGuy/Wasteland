#pragma once

#include <future>
#include "ECS/GameObject.hpp"
#include "Thread/ThreadPool.hpp"

using namespace Wasteland::Thread;

namespace Wasteland::ECS
{
	enum class ActionType
	{
		Register,
		Unregister
	};
	
	struct PendingAction
	{
		ActionType type;

		std::string name;

		std::shared_ptr<GameObject> object;
	};

	class GameObjectManager
	{

	public:
		
		GameObjectManager(const GameObjectManager&) = delete;
		GameObjectManager(GameObjectManager&&) = delete;
		GameObjectManager& operator=(const GameObjectManager&) = delete;
		GameObjectManager& operator=(GameObjectManager&&) = delete;

		std::shared_ptr<GameObject> Register(std::shared_ptr<GameObject> gameObject)
		{
			std::unique_lock<std::mutex> lock(mutex);

			PendingAction action;

			action.type = ActionType::Register;
			action.name = gameObject->GetName();
			action.object = gameObject;

			pendingActions.push_back(std::move(action));

			return gameObject;
		}

		void Unregister(const std::string& name)
		{
			std::unique_lock<std::mutex> lock(mutex);

			PendingAction action;

			action.type = ActionType::Unregister;
			action.name = name;

			pendingActions.push_back(std::move(action));
		}

		void Update()
		{
			std::vector<std::shared_ptr<std::promise<void>>> promises;

			{
				std::unique_lock<std::mutex> lock(mutex);

				promises.reserve(gameObjectMap.size());

				for (auto& [name, gameObject] : gameObjectMap)
				{
					auto pm = std::make_shared<std::promise<void>>();
					promises.push_back(pm);

					pool.EnqueueTask([gameObject, pm]() 
					{
						try
						{
							gameObject->Update();
						}
						catch (...)
						{
							
						}

						pm->set_value();
					});
				}
			}

			for (auto& pm : promises)
				pm->get_future().wait();
				
			ApplyPendingActions();
		}

		void Render(std::optional<std::shared_ptr<Wasteland::Render::Camera>> camera)
		{
			if (!camera.has_value())
				return;

			std::unique_lock<std::mutex> lock(mutex);

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

		void ApplyPendingActions()
		{
			std::unique_lock<std::mutex> lock(mutex);

			for (auto& action : pendingActions)
			{
				if (action.type == ActionType::Register)
					gameObjectMap.insert({ action.name, action.object });
				else if (action.type == ActionType::Unregister)
					gameObjectMap.erase(action.name);
			}

			pendingActions.clear();
		}

		std::mutex mutex;

		std::unordered_map<std::string, std::shared_ptr<GameObject>> gameObjectMap;

		ThreadPool<3> pool = ThreadPool<3>();

		std::vector<PendingAction> pendingActions;

		static std::once_flag initalizationFlag;
		static std::unique_ptr<GameObjectManager> instance;

	};

	std::once_flag GameObjectManager::initalizationFlag;
	std::unique_ptr<GameObjectManager> GameObjectManager::instance;
}