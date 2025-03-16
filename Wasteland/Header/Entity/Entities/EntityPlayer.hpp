#pragma once

#include "Core/InputManager.hpp"
#include "ECS/GameObject.hpp"
#include "Entity/EntityBase.hpp"
#include "Render/Camera.hpp"

using namespace Wasteland::Core;
using namespace Wasteland::Entity;
using namespace Wasteland::Render;

namespace Wasteland::Entity::Entities
{
	class EntityPlayer : public EntityBase
	{

	public:

		EntityPlayer(const EntityPlayer&) = delete;
		EntityPlayer(EntityPlayer&&) = delete;
		EntityPlayer& operator=(const EntityPlayer&) = delete;
		EntityPlayer& operator=(EntityPlayer&&) = delete;

		EntityPlayer()
		{
			Builder<EntityBase>::New()
				.Set(EntityBase::RegistryNameSetter{ "entity_player" })
				.Set(EntityBase::CurrentHealthSetter{ 100.0f })
				.Set(EntityBase::MaximumHealthSetter{ 100.0f })
				.Set(EntityBase::MovementSpeedSetter{ 0.04f })
				.Set(EntityBase::RunningAcceleratorSetter{ 1.0f })
				.Set(EntityBase::JumpHeightSetter{ 5.0f })
				.Set(EntityBase::CanJumpSetter{ true })
				.Build(static_cast<EntityBase&>(*this));

			Builder<EntityPlayer>::New()
				.Set(EntityPlayer::MouseSensitivitySetter{ 0.1f })
				.Build(static_cast<EntityPlayer&>(*this));
		}

		void Initialize() override
		{
			auto cameraObject = GetGameObject()->AddChild(GameObject::Create("default.camera"));

			camera = cameraObject->AddComponent(Camera::Create(45.0f, 0.01f, 1000.0f));

			InputManager::GetInstance().SetMouseMode(MouseMode::LOCKED);
		}

		void Update() override
		{
			if (InputManager::GetInstance().GetKeyState(KeyCode::ESCAPE, KeyState::PRESSED))
				InputManager::GetInstance().SetMouseMode(!InputManager::GetInstance().GetMouseMode());

			UpdateMouselook();
			UpdateMovement();
		}

		std::shared_ptr<Camera> GetCamera() const
		{
			return camera;
		}

	private:

		void UpdateMouselook()
		{
			Vector<float, 2> mouseDelta = InputManager::GetInstance().GetMouseDelta();
			float sensitivity = 0.03f;

			auto transform = camera->GetGameObject()->GetTransform();
			Vector<float, 3> rotation = transform->GetLocalRotation();

			rotation.y() -= mouseDelta.x() * sensitivity;
			rotation.x() += mouseDelta.y() * sensitivity;

			transform->SetLocalRotation(rotation);
		}

		void UpdateMovement()
		{
			Vector<float, 3> movement = { 0.0f, 0.0f, 0.0f };

			Vector<float, 3> forward = camera->GetGameObject()->GetTransform()->GetForward();
			Vector<float, 3> right = camera->GetGameObject()->GetTransform()->GetRight();

			if (InputManager::GetInstance().GetKeyState(KeyCode::W, KeyState::HELD))
				movement += forward * MovementSpeed;

			if (InputManager::GetInstance().GetKeyState(KeyCode::S, KeyState::HELD))
				movement -= forward * MovementSpeed;

			if (InputManager::GetInstance().GetKeyState(KeyCode::A, KeyState::HELD))
				movement += right * MovementSpeed;

			if (InputManager::GetInstance().GetKeyState(KeyCode::D, KeyState::HELD))
				movement -= right * MovementSpeed;

			GetGameObject()->GetTransform()->Translate(movement);
		}

		std::shared_ptr<Camera> camera;

		BUILDABLE_PROPERTY(MouseSensitivity, float, EntityPlayer)

	};
}