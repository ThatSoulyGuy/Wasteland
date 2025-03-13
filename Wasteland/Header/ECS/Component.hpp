#pragma once

#include <memory>

namespace Wasteland::Render
{
	class Camera;
}

namespace Wasteland::ECS
{
	class GameObject;

	class Component
	{

	public:

		using Super = Component;

		virtual ~Component() = default;

		virtual void Initialize() { }

		virtual void Update() { }

		virtual void Render(std::shared_ptr<Wasteland::Render::Camera>) { }

		std::shared_ptr<GameObject> GetGameObject() const
		{
			return gameObject.lock();
		}

	private:

		std::weak_ptr<GameObject> gameObject;

		friend class GameObject;

	};
}