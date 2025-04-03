#pragma once

#include "ECS/GameObjectManager.hpp"
#include "Render/ShaderManager.hpp"
#include "Render/TextureManager.hpp"
#include "Utility/CoordinateHelper.hpp"
#include "World/Chunk.hpp"

using namespace Wasteland::ECS;
using namespace Wasteland::Render;
using namespace Wasteland::Utility;

namespace Wasteland::World
{
    class WorldBase : public Component
    {
    
    public:

        WorldBase(const WorldBase&) = delete;
        WorldBase(WorldBase&&) = delete;
        WorldBase& operator=(const WorldBase&) = delete;
        WorldBase& operator=(WorldBase&&) = delete;

        void GenerateChunk(const Vector<int, 3>& position)
        {
            auto chunkObject = GameObjectManager::GetInstance().Register(GameObject::Create(std::format("chunk_{}_{}_{}", position.x(), position.y(), position.z())));

            chunkObject->GetTransform()->SetLocalPosition(CoordinateHelper::ChunkToWorldCoordinates(position));

            chunkObject->AddComponent(ShaderManager::GetInstance().Get("default").value());
			chunkObject->AddComponent(TextureManager::GetInstance().Get("grass").value());
			chunkObject->AddComponent(Mesh::Create({ }, { }));
			chunkObject->AddComponent(Chunk::Create());

			chunkObject->GetComponent<Chunk>().value()->Generate();

            chunkMap.insert({ position, chunkObject->GetComponent<Chunk>().value() });
        }

        void RemoveChunk(const Vector<int, 3>& position)
        {
            if (!chunkMap.contains(position))
                throw MAKE_EXCEPTION(NoSuchElementException, std::format("No chunk found at position: ({}, {}, {})", position.x(), position.y(), position.z()));

            GameObjectManager::GetInstance().Unregister(chunkMap[position].lock()->GetGameObject()->GetName()); 

            chunkMap.erase(position);
        }

        void Update() override
        {
            Vector<int, 3> playerChunk = CoordinateHelper::WorldToChunkCoordinates(chunkLoaderPosition);
    
            for (int y = 0; y < 1; ++y)
            {
                for (int z = playerChunk.z() - RENDER_DISTANCE; z <= playerChunk.z() + RENDER_DISTANCE; ++z)
                {
                    for (int x = playerChunk.x() - RENDER_DISTANCE; x <= playerChunk.x() + RENDER_DISTANCE; ++x)
                    {
                        Vector<int, 3> chunkPos { x, y, z };

                        if (!chunkMap.contains(chunkPos))
                            GenerateChunk(chunkPos);
                    }
                }
            }

            std::vector<Vector<int, 3>> chunksToUnload;

            for (const auto& [chunkPos, weakChunk] : chunkMap)
            {
                if (chunkPos.x() < playerChunk.x() - RENDER_DISTANCE || chunkPos.x() > playerChunk.x() + RENDER_DISTANCE || chunkPos.z() < playerChunk.z() - RENDER_DISTANCE || chunkPos.z() > playerChunk.z() + RENDER_DISTANCE)
                    chunksToUnload.push_back(chunkPos);
            }

            for (const auto& pos : chunksToUnload)
                RemoveChunk(pos);
        }

        static std::shared_ptr<WorldBase> Create()
        {
            return std::shared_ptr<WorldBase>(new WorldBase());
        }

        Vector<float, 3> chunkLoaderPosition;

        static constexpr int RENDER_DISTANCE = 2;

    private:

        WorldBase() = default;

        std::unordered_map<Vector<int, 3>, std::weak_ptr<Chunk>> chunkMap;

    };
}