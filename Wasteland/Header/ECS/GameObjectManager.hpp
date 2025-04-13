#pragma once

#include <future>
#include <shared_mutex>
#include "ECS/GameObject.hpp"

namespace Wasteland::ECS
{
    enum class ActionType
    {
        REGISTER,
        UNREGISTER
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
            PendingAction action;

            action.type = ActionType::REGISTER;
            action.name = gameObject->GetName();
            action.object = gameObject;

            pendingActions.push_back(std::move(action));

            return gameObject;
        }

        void Unregister(const std::string& name)
        {
            PendingAction action;

            action.type = ActionType::UNREGISTER;
            action.name = name;
            
            pendingActions.push_back(std::move(action));
        }

        void Update()
        {
            for (auto& [name, obj] : gameObjectMap)
                obj->Update();
            
            ApplyPendingActions();
        }

        void Render(std::optional<std::shared_ptr<Wasteland::Render::Camera>> camera)
        {
            if (!camera.has_value())
                return;

            std::for_each(gameObjectMap.begin(), gameObjectMap.end(), [&](const auto& pair) { pair.second->Render(camera.value()); });
        }

        void Uninitialize()
        {
            std::for_each(gameObjectMap.begin(), gameObjectMap.end(), [&](auto& pair) { pair.second.reset(); });

            gameObjectMap.clear();
        }

        static GameObjectManager& GetInstance()
        {
            std::call_once(initializationFlag, [&]()
            {
                instance = std::unique_ptr<GameObjectManager>(new GameObjectManager());
            });

            return *instance;
        }

    private:

        GameObjectManager() = default;

        void ApplyPendingActions()
        {
            for (auto& action : pendingActions)
            {
                if (action.type == ActionType::REGISTER)
                    gameObjectMap.insert({ action.name, action.object });
                else if (action.type == ActionType::UNREGISTER)
                    gameObjectMap.erase(action.name);
            }

            pendingActions.clear();
        }

        std::unordered_map<std::string, std::shared_ptr<GameObject>> gameObjectMap;

        std::vector<PendingAction> pendingActions;

        static std::once_flag initializationFlag;
        static std::unique_ptr<GameObjectManager> instance;

	};

	std::once_flag GameObjectManager::initializationFlag;
	std::unique_ptr<GameObjectManager> GameObjectManager::instance;
}