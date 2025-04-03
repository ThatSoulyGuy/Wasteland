#pragma once

#include "Math/Vector.hpp"

using namespace Wasteland::Math;

namespace Wasteland::Utility
{
    class CoordinateHelper
    {

    public:

        CoordinateHelper(const CoordinateHelper&) = delete;
        CoordinateHelper(CoordinateHelper&&) = delete;
        CoordinateHelper& operator=(const CoordinateHelper&) = delete;
        CoordinateHelper& operator=(CoordinateHelper&&) = delete;

        static Vector<int, 3> WorldToChunkCoordinates(const Vector<float, 3>& position)
        {
            int x = std::floor(position.x() / CHUNK_SIZE);
            int y = std::floor(position.y() / CHUNK_SIZE);
            int z = std::floor(position.z() / CHUNK_SIZE);

            return { x, y, z };
        }

        static Vector<float, 3> ChunkToWorldCoordinates(const Vector<int, 3>& position)
        {
            float x = position.x() * CHUNK_SIZE;
            float y = position.y() * CHUNK_SIZE;
            float z = position.z() * CHUNK_SIZE;

            return { x, y, z };
        }
        
        static constexpr int CHUNK_SIZE = 32;

    private:

        CoordinateHelper() = default;

    };
}