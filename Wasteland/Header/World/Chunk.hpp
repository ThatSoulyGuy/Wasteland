#pragma once

#include "ECS/GameObject.hpp"
#include "Render/Mesh.hpp"

using namespace Wasteland::Render;

namespace Wasteland::World
{
    class Chunk : public Component
    {

    public:

        Chunk(const Chunk&) = delete;
        Chunk(Chunk&&) = delete;
        Chunk& operator=(const Chunk&) = delete;
        Chunk& operator=(Chunk&&) = delete;

        void Generate()
        {
            const int gridVertices = 33;
            const float totalSize = 32.0f;
            const float unitSize = totalSize / (gridVertices - 1);
            const float frequency = 0.1f;
            const float amplitude = 3.0f;

            Vector<float, 3> chunkOffset = GetGameObject()->GetTransform()->GetWorldPosition();

            std::cout << chunkOffset << std::endl;

            vertices.clear();
            indices.clear();
            vertices.reserve(gridVertices * gridVertices);

            for (int j = 0; j < gridVertices; ++j)
            {
                for (int i = 0; i < gridVertices; ++i)
                {
                    float x = i * unitSize;
                    float z = j * unitSize;
                    
                    float y = FractalNoise(chunkOffset.x() + x, chunkOffset.z() + z, 4.0f, 0.5f, 0.1f) * amplitude;

                    Vertex vertex;

                    vertex.position = { x, y, z };
                    vertex.color = { 0.2f, 0.8f, 0.2f };
                    vertex.uvs = Vector<float, 2>{ static_cast<float>(i) / (gridVertices - 1), static_cast<float>(j) / (gridVertices - 1) } * 8;

                    float heightL = SmoothNoise(chunkOffset.x() + (x - unitSize) * frequency, chunkOffset.z() + z * frequency) * amplitude;
                    float heightR = SmoothNoise(chunkOffset.x() + (x + unitSize) * frequency, chunkOffset.z() + z * frequency) * amplitude;
                    float heightD = SmoothNoise(chunkOffset.x() + x * frequency, chunkOffset.z() + (z - unitSize) * frequency) * amplitude;
                    float heightU = SmoothNoise(chunkOffset.x() + x * frequency, chunkOffset.z() + (z + unitSize) * frequency) * amplitude;

                    Vector<float, 3> normal;

                    normal.x() = heightL - heightR;
                    normal.y() = 2.0f;
                    normal.z() = heightD - heightU;

                    float length = std::sqrt(normal.x() * normal.x() +
                                            normal.y() * normal.y() +
                                            normal.z() * normal.z());

                    if (length != 0)
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

            std::shared_ptr<Mesh> mesh = GetGameObject()->GetComponent<Mesh>().value();

            mesh->SetVertices(vertices);
            mesh->SetIndices(indices);

            mesh->Generate();
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

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    };
}