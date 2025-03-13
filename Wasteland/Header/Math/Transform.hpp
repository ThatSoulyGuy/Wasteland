#pragma once

#include <memory>
#include <optional>
#include <numbers>
#include "ECS/Component.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"

using namespace Wasteland::ECS;

namespace Wasteland::Math
{
    class Transform final : public Component
    {

    public:

        void Translate(const Vector<float, 3>& translation)
        {
            localPosition += translation;
        }

        void Rotate(const Vector<float, 3>& rotation)
        {
            localRotation += rotation;

            for (int i = 0; i < 3; ++i)
            {
                localRotation[i] = std::fmod(localRotation[i], 360.0f);

                if (localRotation[i] < 0.0f)
                    localRotation[i] += 360.0f;
            }
        }

        void Scale(const Vector<float, 3>& scale)
        {
            localScale += scale;
        }

        Vector<float, 3> GetLocalPosition() const
        {
            return localPosition;
        }

        void SetLocalPosition(const Vector<float, 3>& value)
        {
            this->localPosition = value;
        }

        Vector<float, 3> GetLocalRotation() const
        {
            return localRotation;
        }

        void SetLocalRotation(const Vector<float, 3>& value)
        {
            this->localRotation = value;

            for (int i = 0; i < 3; ++i)
            {
                localRotation[i] = std::fmod(localRotation[i], 360.0f);

                if (localRotation[i] < 0.0f)
                    localRotation[i] += 360.0f;
            }
        }

        Vector<float, 3> GetLocalScale() const
        {
            return localScale;
        }

        void SetLocalScale(const Vector<float, 3>& value)
        {
            this->localScale = value;
        }

        Vector<float, 3> GetWorldPosition() const
        {
            Matrix<float, 4, 4> M = GetModelMatrix();

            return { M[0][3], M[1][3], M[2][3] };
        }

        Vector<float, 3> GetWorldScale() const
        {
            Matrix<float, 4, 4> M = GetModelMatrix();

            auto length = [](float a, float b, float c)
            {
                return std::sqrt(a * a + b * b + c * c);
            };

            return
            {
                length(M[0][0], M[0][1], M[0][2]),
                length(M[1][0], M[1][1], M[1][2]),
                length(M[2][0], M[2][1], M[2][2])
            };
        }

        Vector<float, 3> GetWorldRotation() const
        {
            Matrix<float, 4, 4> M = GetModelMatrix();

            Vector<float, 3> s = GetWorldScale();

            if (std::fabs(s.x()) > 1e-6f)
                M[0][0] /= s.x();  M[0][1] /= s.x();  M[0][2] /= s.x();
            
            if (std::fabs(s.y()) > 1e-6f)
                M[1][0] /= s.y();  M[1][1] /= s.y();  M[1][2] /= s.y();
            
            if (std::fabs(s.z()) > 1e-6f)
                M[2][0] /= s.z();  M[2][1] /= s.z();  M[2][2] /= s.z();
            
            M[0][3] = 0.0f;
            M[1][3] = 0.0f;
            M[2][3] = 0.0f;

            Vector<float, 3> euler{ 0, 0, 0 };

            float sy = -M[2][0];
            float cx = std::sqrt(M[0][0] * M[0][0] + M[1][0] * M[1][0]);

            if (std::fabs(cx) > 1e-6f)
            {
                euler.x() = std::atan2(sy, cx);
                euler.y() = std::atan2(M[1][0], M[0][0]);
                euler.z() = std::atan2(M[2][1], M[2][2]);
            }
            else
            {
                euler.x() = (sy > 0.0f) ? std::numbers::pi_v<float> *0.5f : -std::numbers::pi_v<float> *0.5f;
                euler.y() = std::atan2(-M[1][2], M[1][1]);
                euler.z() = 0.0f;
            }

            return euler * (180.0f / std::numbers::pi);
        }

        Vector<float, 3> GetForward() const
        {
            Matrix<float, 4, 4> M = GetModelMatrix();
            Vector<float, 3> f = { M[2][0], M[2][1], M[2][2] };

            return Vector<float, 3>::Normalize(f);
        }

        Vector<float, 3> GetRight() const
        {
            Matrix<float, 4, 4> M = GetModelMatrix();
            Vector<float, 3> r = { M[0][0], M[0][1], M[0][2] };

            return Vector<float, 3>::Normalize(r);
        }

        Vector<float, 3> GetUp() const
        {
            Matrix<float, 4, 4> M = GetModelMatrix();
            Vector<float, 3> u = { M[1][0], M[1][1], M[1][2] };

            return Vector<float, 3>::Normalize(u);
        }

        std::optional<std::weak_ptr<Transform>> GetParent() const
        {
            return parent;
        }

        void SetParent(std::shared_ptr<Transform> newParent)
        {
            if (!newParent)
                parent = std::nullopt;
            else
                parent = std::make_optional<std::weak_ptr<Transform>>(newParent);
        }

        Matrix<float, 4, 4> GetModelMatrix() const
        {
            Matrix<float, 4, 4> translation = Matrix<float, 4, 4>::Translation(localPosition);

            Matrix<float, 4, 4> rotationX = Matrix<float, 4, 4>::RotationX(localRotation.x() * (std::numbers::pi / 180.0f));
            Matrix<float, 4, 4> rotationY = Matrix<float, 4, 4>::RotationY(localRotation.y() * (std::numbers::pi / 180.0f));
            Matrix<float, 4, 4> rotationZ = Matrix<float, 4, 4>::RotationZ(localRotation.z() * (std::numbers::pi / 180.0f));

            Matrix<float, 4, 4> scale = Matrix<float, 4, 4>::Scale(localScale);

            Matrix<float, 4, 4> localMatrix = translation * (rotationX * rotationY * rotationZ) * scale;

            if (parent.has_value())
            {
                auto parentPointer = parent.value().lock();

                if (parentPointer)
                    return parentPointer->GetModelMatrix() * localMatrix;
            }

            return localMatrix;
        }

        static std::shared_ptr<Transform> Create(const Vector<float, 3>& position, const Vector<float, 3>& rotation, const Vector<float, 3>& scale)
        {
            std::shared_ptr<Transform> result(new Transform());

            result->localPosition = position;
            result->localRotation = rotation;
            result->localScale = scale;

            return result;
        }

    private:

        Transform() = default;

        std::optional<std::weak_ptr<Transform>> parent;

        Vector<float, 3> localPosition = { 0.0f, 0.0f, 0.0f };
        Vector<float, 3> localRotation = { 0.0f, 0.0f, 0.0f };
        Vector<float, 3> localScale = { 1.0f, 1.0f, 1.0f };
    };
}