#pragma once

#include <numbers>
#include "Core/Window.hpp"
#include "ECS/GameObject.hpp"
#include "Math/Matrix.hpp"

using namespace Wasteland::Core;
using namespace Wasteland::ECS;
using namespace Wasteland::Math;

namespace Wasteland::Render
{
	class Camera : public Component
	{

	public:

		Matrix<float, 4, 4> GetProjectionMatrix() const
		{
			return Matrix<float, 4, 4>::Perspective(fieldOfView * (std::numbers::pi / 180), (float)Window::GetInstance().GetDimensions().x() / Window::GetInstance().GetDimensions().y(), nearPlane, farPlane);
		}

		Matrix<float, 4, 4> GetViewMatrix() const
		{
			Vector<float, 3> position = Super::GetGameObject()->GetTransform()->GetWorldPosition();
			Vector<float, 3> forward = Super::GetGameObject()->GetTransform()->GetForward();
			Vector<float, 3> up = { 0.0f, 1.0f, 0.0f };

			return Matrix<float, 4, 4>::LookAt(position, position + forward, up);
		}
		
		static std::shared_ptr<Camera> Create(float fieldOfView, float nearPlane, float farPlane)
		{
			std::shared_ptr<Camera> result(new Camera());

			result->fieldOfView = fieldOfView;
			result->nearPlane = nearPlane;
			result->farPlane = farPlane;

			return result;
		}

	private:

		Camera() = default;

		float fieldOfView;
		float nearPlane;
		float farPlane;

	};
}