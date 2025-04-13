#pragma once

#include "Collider/Colliders/ColliderMesh.hpp"
#include "ECS/GameObjectManager.hpp"
#include "Math/Rigidbody.hpp"
#include "Render/ShaderManager.hpp"
#include "Render/TextureManager.hpp"
#include "Thread/ThreadPool.hpp"
#include "Utility/CoordinateHelper.hpp"
#include "World/Chunk.hpp"

using namespace Wasteland::Collider::Colliders;
using namespace Wasteland::ECS;
using namespace Wasteland::Render;
using namespace Wasteland::Thread;
using namespace Wasteland::Utility;

namespace Wasteland::World
{
    enum class ActionType
    {
        ADD,
        REMOVE
    };

    struct PendingAction
    {
        ActionType type;
        Vector<int, 3> position;
    };

    enum class ChunkStatus
    {
        ACTIVE,
        PENDING_REMOVAL
    };

    struct ChunkInfo
    {
        std::weak_ptr<Chunk> chunk;
        ChunkStatus status = ChunkStatus::ACTIVE;
    };

    class WorldBase : public Component
    {
    
    public:

        WorldBase(const WorldBase&) = delete;
        WorldBase(WorldBase&&) = delete;
        WorldBase& operator=(const WorldBase&) = delete;
        WorldBase& operator=(WorldBase&&) = delete;

        void QueueAddChunk(const Vector<int, 3>& pos)
        {
            std::unique_lock<std::mutex> lock(mapMutex);

            auto it = chunkMap.find(pos);

            if (it != chunkMap.end())
                return;
            
            {
                std::unique_lock<std::mutex> pendingLock(pendingMutex);

                pendingActions.push_back({ ActionType::ADD, pos });
            }
        }

        void QueueRemoveChunk(const Vector<int, 3>& pos)
        {
            std::unique_lock<std::mutex> lock(mapMutex);

            auto it = chunkMap.find(pos);

            if (it == chunkMap.end())
                return;
            
            if (it->second.status == ChunkStatus::PENDING_REMOVAL)
                return;

            it->second.status = ChunkStatus::PENDING_REMOVAL;

            {
                std::unique_lock<std::mutex> pendingLock(pendingMutex);

                MainThreadExecutor::GetInstance().CancelTask(it->second.chunk.lock().get());

                pendingActions.push_back({ ActionType::REMOVE, pos });
            }
        }

        void Update() override
        {
            ApplyPendingActions();

            Vector<int, 3> playerChunk = CoordinateHelper::WorldToChunkCoordinates(chunkLoaderPosition);

            for (int y = 0; y < 1; ++y)
            {
                for (int z = playerChunk.z() - RENDER_DISTANCE; z <= playerChunk.z() + RENDER_DISTANCE; ++z)
                {
                    for (int x = playerChunk.x() - RENDER_DISTANCE; x <= playerChunk.x() + RENDER_DISTANCE; ++x)
                    {
                        Vector<int, 3> chunkPos{ x, y, z };

                        bool alreadyExists;

                        {
                            std::unique_lock<std::mutex> lock(mapMutex);

                            alreadyExists = chunkMap.contains(chunkPos);
                        }

                        if (!alreadyExists)
                        {
                            threadPool.EnqueueTask([this, chunkPos]()
                            {
                                QueueAddChunk(chunkPos);
                            });
                        }
                    }
                }
            }

            std::vector<Vector<int, 3>> chunksToUnload;
            {
                std::unique_lock<std::mutex> lock(mapMutex);

                for (auto& [chunkPos, weakChunk] : chunkMap)
                {
                    if (chunkPos.x() < playerChunk.x() - RENDER_DISTANCE || chunkPos.x() > playerChunk.x() + RENDER_DISTANCE || chunkPos.z() < playerChunk.z() - RENDER_DISTANCE || chunkPos.z() > playerChunk.z() + RENDER_DISTANCE)
                        chunksToUnload.push_back(chunkPos);
                }
            }

            for (auto& pos : chunksToUnload)
            {
                threadPool.EnqueueTask([this, pos]()
                {
                    QueueRemoveChunk(pos);
                });
            }

            ApplyPendingActions();
        }

        static std::shared_ptr<WorldBase> Create()
        {
            return std::shared_ptr<WorldBase>(new WorldBase());
        }

        Vector<float, 3> chunkLoaderPosition;

        static constexpr int RENDER_DISTANCE = 2;

    private:

        WorldBase() = default;

        void GenerateChunkInternal(const Vector<int, 3>& position)
        {
            auto chunkObject = GameObjectManager::GetInstance().Register(GameObject::Create(std::format("default.chunk_{}_{}_{}", position.x(), position.y(), position.z())));

            chunkObject->GetTransform()->SetLocalPosition(CoordinateHelper::ChunkToWorldCoordinates(position));
            chunkObject->AddComponent(ShaderManager::GetInstance().Get("default").value());
            chunkObject->AddComponent(TextureManager::GetInstance().Get("grass").value());
            chunkObject->AddComponent(Mesh::Create({}, {}));
            chunkObject->AddComponent(Chunk::Create());

            chunkObject->GetComponent<Chunk>().value()->Generate();

            chunkObject->AddComponent(Rigidbody<btBvhTriangleMeshShape>::Create(0.0f, true));
            chunkObject->GetTransform()->SetLocalPosition(CoordinateHelper::ChunkToWorldCoordinates(position));

            {
                std::unique_lock<std::mutex> lock(mapMutex);

                chunkMap.insert({ position, { chunkObject->GetComponent<Chunk>().value(), ChunkStatus::ACTIVE } });
            }
        }

        void RemoveChunkInternal(const Vector<int, 3>& position)
        {
            std::string objectName;
            {
                std::unique_lock<std::mutex> lock(mapMutex);

                auto it = chunkMap.find(position);

                if (it == chunkMap.end())
                    return;
                
                objectName = it->second.chunk.lock()->GetGameObject()->GetName();

                chunkMap.erase(it);
            }

            GameObjectManager::GetInstance().Unregister(objectName);
        }

        void ApplyPendingActions()
        {
            std::vector<PendingAction> actions;
            {
                std::unique_lock<std::mutex> lock(pendingMutex);
                actions.swap(pendingActions);
            }

            for (auto& act : actions)
            {
                if (act.type == ActionType::ADD)
                    GenerateChunkInternal(act.position);
                else if (act.type == ActionType::REMOVE)
                    RemoveChunkInternal(act.position);
            }
        }

        std::mutex mapMutex;

        ThreadPool<3> threadPool;

        std::unordered_map<Vector<int, 3>, ChunkInfo> chunkMap;

        std::mutex pendingMutex;
        std::vector<PendingAction> pendingActions;

    };
}