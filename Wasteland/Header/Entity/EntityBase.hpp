#pragma once

#include <string>
#include "ECS/Component.hpp"
#include "Utility/Builder.hpp"

using namespace Wasteland::ECS;
using namespace Wasteland::Utility;

namespace Wasteland::Entity
{
	class EntityBase;

	template <typename T>
	concept EntityType = std::derived_from<T, EntityBase>&& requires(T a)
	{
		{ a.GetRegistryName() } -> std::convertible_to<std::string>;
		{ a.GetCurrentHealth() } -> std::convertible_to<float>;
		{ a.GetMaximumHealth() } -> std::convertible_to<float>;
		{ a.GetMovementSpeed() } -> std::convertible_to<float>;
		{ a.GetRunningAccelerator() } -> std::convertible_to<float>;
		{ a.GetJumpHeight() } -> std::convertible_to<float>;
		{ a.GetCanJump() } -> std::convertible_to<bool>;
	};

	class EntityBase : public Component
	{

	public:

		EntityBase(const EntityBase&) = delete;
		EntityBase(EntityBase&&) = delete;
		EntityBase& operator=(const EntityBase&) = delete;
		EntityBase& operator=(EntityBase&&) = delete;

		std::string GetRegistryName() const
		{
			return RegistryName;
		}

		float GetCurrentHealth() const
		{
			return CurrentHealth;
		}

		float GetMaximumHealth() const
		{
			return MaximumHealth;
		}

		float GetMovementSpeed() const
		{
			return MovementSpeed;
		}

		float GetRunningAccelerator() const
		{
			return RunningAccelerator;
		}

		float GetJumpHeight() const
		{
			return JumpHeight;
		}

		bool GetCanJump() const
		{
			return CanJump;
		}

		template <typename T>
		static std::shared_ptr<T> Create()
		{
			return std::shared_ptr<T>(new T());
		}

	protected:

		EntityBase() = default;

		virtual ~EntityBase() = default;

	private:

		friend class Builder<EntityBase>;

		template <typename Class, typename MemberType, MemberType Class::* MemberPtr>
		friend struct Setter;

		BUILDABLE_PROPERTY(RegistryName, std::string, EntityBase)

		BUILDABLE_PROPERTY(CurrentHealth, float, EntityBase)
		BUILDABLE_PROPERTY(MaximumHealth, float, EntityBase)

		BUILDABLE_PROPERTY(MovementSpeed, float, EntityBase)
		BUILDABLE_PROPERTY(RunningAccelerator, float, EntityBase)

		BUILDABLE_PROPERTY(JumpHeight, float, EntityBase)
		BUILDABLE_PROPERTY(CanJump, bool, EntityBase)
	};
}