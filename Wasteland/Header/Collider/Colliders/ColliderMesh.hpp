#pragma once

#include "Collider/ColliderBase.hpp"
#include "ECS/GameObject.hpp"
#include "Render/Vertex.hpp"

using namespace Wasteland::Collider;
using namespace Wasteland::Render;

namespace Wasteland::Collider::Colliders
{
    class ColliderMesh final : public ColliderBase<btBvhTriangleMeshShape>
    {

    public:

        ~ColliderMesh()
        {
            delete shape;

            shape = nullptr;
        }

        void Initialize() override
        {
            btTriangleMesh* triMesh = new btTriangleMesh();

            for (size_t i = 0; i < indices.size(); i += 3)
            {
                unsigned int idx0 = indices[i + 0];
                unsigned int idx1 = indices[i + 1];
                unsigned int idx2 = indices[i + 2];

                const Vector<float, 3>& p0 = vertices[idx0].position;
                const Vector<float, 3>& p1 = vertices[idx1].position;
                const Vector<float, 3>& p2 = vertices[idx2].position;

                triMesh->addTriangle(
                    btVector3(p0.x(), p0.y(), p0.z()),
                    btVector3(p1.x(), p1.y(), p1.z()),
                    btVector3(p2.x(), p2.y(), p2.z())
                );
            }

            bool useQuantizedAABB = true;
            shape = new btBvhTriangleMeshShape(triMesh, useQuantizedAABB);
        }

        void SetVertices(const std::vector<Vertex>& vertices)
        {
            this->vertices = vertices;
        }

        void SetIndices(const std::vector<unsigned int>& indices)
        {
            this->indices = indices;
        }

        btBvhTriangleMeshShape* GetColliderShape() override
        {
            return shape;
        }

        static std::shared_ptr<ColliderMesh> Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
        {
            std::shared_ptr<ColliderMesh> result(new ColliderMesh());

            result->vertices = vertices;
            result->indices = indices;

            return result;
        }
    
    private:

        ColliderMesh() = default;

        btBvhTriangleMeshShape* shape;

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

    };
}