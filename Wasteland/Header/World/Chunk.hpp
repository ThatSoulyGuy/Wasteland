#pragma once

#include <btBulletDynamicsCommon.h>
#include "ECS/GameObject.hpp"
#include "Render/Mesh.hpp"
#include "Thread/MainThreadExecutor.hpp"

using namespace Wasteland::Render;

namespace Wasteland::World
{
    class Chunk final : public Component, public std::enable_shared_from_this<Chunk>
    {

    public:

        Chunk(const Chunk&) = delete;
        Chunk(Chunk&&) = delete;
        Chunk& operator=(const Chunk&) = delete;
        Chunk& operator=(Chunk&&) = delete;

        void Generate()
        {
            const int gridVertices = resolution;
            const float totalSize = 32.0f;
            const float unitSize = totalSize / (gridVertices - 1);

            const float baseAmplitude = 1.5f;

            const float baseFrequency = 0.1f;

            Vector<float, 3> chunkOffset = GetGameObject()->GetTransform()->GetWorldPosition();

            vertices.clear();
            indices.clear();

            vertices.reserve(static_cast<size_t>(gridVertices) * gridVertices);

            for (int j = 0; j < gridVertices; ++j)
            {
                for (int i = 0; i < gridVertices; ++i)
                {
                    float x = i * unitSize;
                    float z = j * unitSize;

                    float regionFreq = 0.02f;
                    float regionNoise = SmoothNoise((chunkOffset.x() + x) * regionFreq, (chunkOffset.z() + z) * regionFreq);

                    float regionFactor = 0.5f + regionNoise * 1.5f;

                    float y = FractalNoise(chunkOffset.x() + x, chunkOffset.z() + z, 4.0f, 0.5f, baseFrequency) * baseAmplitude * regionFactor;

                    Vertex vertex{ };

                    vertex.position = { x, y, z };
                    vertex.color = { 0.2f, 0.8f, 0.2f };

                    vertex.uvs = Vector<float, 2>
                    {
                        static_cast<float>(i) / (gridVertices - 1),
                        static_cast<float>(j) / (gridVertices - 1)
                    } * 8.0f;

                    float freq = 0.1f;

                    float heightL = SmoothNoise(chunkOffset.x() + (x - unitSize) * freq,
                        chunkOffset.z() + z * freq) * baseAmplitude;
                    float heightR = SmoothNoise(chunkOffset.x() + (x + unitSize) * freq,
                        chunkOffset.z() + z * freq) * baseAmplitude;
                    float heightD = SmoothNoise(chunkOffset.x() + x * freq,
                        chunkOffset.z() + (z - unitSize) * freq) * baseAmplitude;
                    float heightU = SmoothNoise(chunkOffset.x() + x * freq,
                        chunkOffset.z() + (z + unitSize) * freq) * baseAmplitude;

                    Vector<float, 3> normal;
                    normal.x() = heightL - heightR;
                    normal.y() = 2.0f;
                    normal.z() = heightD - heightU;

                    float length = std::sqrt(normal.x() * normal.x() + normal.y() * normal.y() + normal.z() * normal.z());

                    if (length != 0.0f)
                    {
                        normal.x() /= length;
                        normal.y() /= length;
                        normal.z() /= length;
                    }

                    vertex.normal = normal;

                    vertices.push_back(vertex);
                }
            }

            for (int j = 0; j < gridVertices - 1; ++j)
            {
                for (int i = 0; i < gridVertices - 1; ++i)
                {
                    int topLeft = i + j * gridVertices;
                    int topRight = (i + 1) + j * gridVertices;
                    int bottomLeft = i + (j + 1) * gridVertices;
                    int bottomRight = (i + 1) + (j + 1) * gridVertices;

                    indices.push_back(topLeft);
                    indices.push_back(bottomLeft);
                    indices.push_back(topRight);

                    indices.push_back(topRight);
                    indices.push_back(bottomLeft);
                    indices.push_back(bottomRight);
                }
            }

            auto selfWeak = std::weak_ptr<Chunk>(shared_from_this());

            MainThreadExecutor::GetInstance().EnqueueTask(this, [weakSelf = std::move(selfWeak)]()
            {
                auto self = weakSelf.lock();

                if (!self)
                    return;
                    
                auto mesh = self->GetGameObject()->GetComponent<Mesh>().value();

                mesh->SetVertices(self->vertices);
                mesh->SetIndices(self->indices);
                mesh->Generate();
            });

            Super::GetGameObject()->AddComponent(ColliderMesh::Create(vertices, indices));
        }

        static std::shared_ptr<Chunk> Create()
        {
            return std::shared_ptr<Chunk>(new Chunk());
        }

    private:

        Chunk() = default;

        static float Lerp(float a, float b, float t)
        {
            return a + t * (b - a);
        }

        static float Noise(int x, int z)
        {
            int n = x + z * 57;

            n = (n << 13) ^ n;

            return 1.0f - static_cast<float>((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f;
        }

        static float Fade(float t)
        {
            return t * t * t * (t * (t * 6 - 15) + 10);
        }

        static float SmoothNoise(float x, float z)
        {
            int xInt = static_cast<int>(std::floor(x));
            int zInt = static_cast<int>(std::floor(z));
            
            float fracX = x - xInt;
            float fracZ = z - zInt;

            float u = Fade(fracX);
            float v = Fade(fracZ);

            float n00 = Noise(xInt, zInt);
            float n10 = Noise(xInt + 1, zInt);
            float n01 = Noise(xInt, zInt + 1);
            float n11 = Noise(xInt + 1, zInt + 1);

            float i1 = Lerp(n00, n10, u);
            float i2 = Lerp(n01, n11, u);

            return Lerp(i1, i2, v);
        }

        float FractalNoise(float x, float z, int octaves, float persistence, float baseFrequency) 
        {
            float total = 0.0f;
            float amplitude = 1.0f;
            float freq = baseFrequency;

            for (int i = 0; i < octaves; ++i)
            {
                total += SmoothNoise(x * freq, z * freq) * amplitude;

                freq *= 2.0f;
                amplitude *= persistence;
            }

            return total;
        }

        int resolution = 33;

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    };
}