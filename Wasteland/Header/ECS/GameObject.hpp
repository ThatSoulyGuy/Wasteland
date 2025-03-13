#pragma once

#include <concepts>
#include <string>
#include <memory>
#include <format>
#include <unordered_map>
#include <optional>
#include <typeindex>
#include "ECS/Component.hpp"
#include "Math/Transform.hpp"
#include "Utility/Exception/Exceptions/NoSuchElementException.hpp"

using namespace Wasteland::Math;
using namespace Wasteland::Utility::Exception::Exceptions;

namespace Wasteland::ECS
{
	template <typename T>
	concept ComponentType = std::derived_from<T, Component> && std::destructible<T>&& requires(T a, std::shared_ptr<Wasteland::Render::Camera> x)
	{
		{ a.Initialize() } -> std::same_as<void>;
		{ a.Update() } -> std::same_as<void>;
		{ a.Render(x) } -> std::same_as<void>;
	};

	class GameObject : public std::enable_shared_from_this<GameObject>
	{

	public:
		
		GameObject(const GameObject&) = delete;
		GameObject(GameObject&&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject& operator=(GameObject&&) = delete;

		void Update()
		{
			std::for_each(componentMap.begin(), componentMap.end(), [&](const auto& pair) { pair.second->Update(); });

			std::for_each(childrenMap.begin(), childrenMap.end(), [&](const auto& pair) { pair.second->Update(); });
		}

		void Render(std::shared_ptr<Wasteland::Render::Camera> camera)
		{
			std::for_each(componentMap.begin(), componentMap.end(), [&](const auto& pair) { pair.second->Render(camera); });

			std::for_each(childrenMap.begin(), childrenMap.end(), [&](const auto& pair) { pair.second->Render(camera); });
		}

		template <ComponentType T>
		std::shared_ptr<T> AddComponent(std::shared_ptr<T> component)
		{
			component->gameObject = shared_from_this();
			component->Initialize();

			componentMap.insert({ typeid(T), std::move(component) });

			return std::static_pointer_cast<T>(componentMap[typeid(T)]);
		}

		template <ComponentType T>
		std::optional<std::shared_ptr<T>> GetComponent()
		{
			return componentMap.contains(typeid(T)) ? std::make_optional<std::shared_ptr<T>>(std::static_pointer_cast<T>(componentMap[typeid(T)])) : std::nullopt;
		}

		std::shared_ptr<Transform> GetTransform()
		{
			return GetComponent<Transform>().value();
		}

		template <ComponentType T>
		bool HasComponent() const
		{
			return componentMap.contains(typeid(T));
		}

		template <ComponentType T>
		void RemoveComponent()
		{
			if (!componentMap.contains(typeid(T)))
				throw MAKE_EXCEPTION(NoSuchElementException, std::format("Component '{}' not found on GameObject '{}'!", typeid(T).name(), name));

			componentMap.erase(typeid(T));
		}

		std::shared_ptr<GameObject> AddChild(std::shared_ptr<GameObject> child)
		{
			std::string name = child->name;

			child->SetParent(shared_from_this());
			childrenMap.insert({ name, std::move(child) });

			return childrenMap[name];
		}

		std::shared_ptr<GameObject> RemoveChild(const std::string& name)
		{
			std::shared_ptr<GameObject> result = std::move(childrenMap[name]);

			result->SetParent(nullptr);

			childrenMap.erase(name);

			return result;
		}

		void SetParent(std::shared_ptr<GameObject> parent)
		{
			this->parent = parent == nullptr ? std::nullopt : std::make_optional<std::shared_ptr<GameObject>>(parent);

			GetTransform()->SetParent(parent == nullptr ? nullptr : parent->GetTransform());
		}

		std::string GetName() const
		{
			return name;
		}

		static std::shared_ptr<GameObject> Create(const std::string& name)
		{
			std::shared_ptr<GameObject> result(new GameObject());

			result->name = name;
			result->AddComponent(Transform::Create({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }));

			return result;
		}

	private:

		GameObject() = default;

		std::string name;

		std::optional<std::weak_ptr<GameObject>> parent;

		std::unordered_map<std::type_index, std::shared_ptr<Component>> componentMap;
		std::unordered_map<std::string, std::shared_ptr<GameObject>> childrenMap;

	};
}