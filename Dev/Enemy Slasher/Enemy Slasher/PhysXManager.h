#pragma once
#include "PxPhysicsAPI.h"

class CGameObject;
class CVertex;

class CustomObject : physx::PxCustomGeometry::Callbacks, physx::PxUserAllocated
{

};

class CPhysXManager
{
public:
    CPhysXManager();
    ~CPhysXManager();

    void stepPhysics(bool /*interactive*/, float elapsedTime = (1.0f / 60.0f));

    void cleanupPhysics(bool /*interactive*/);

    physx::PxRigidDynamic* createDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxVec3& velocity = physx::PxVec3(0));

    void createStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent);

    void renderCallback();

    void updateActors(physx::PxRigidActor** actors, const physx::PxU32 numActors);

public:
    physx::PxActor* AddCustomGeometry(CGameObject* object);

    physx::PxTriangleMesh* createMesh(physx::PxCookingParams& params, const physx::PxArray<physx::PxVec3>& triVerts, const physx::PxArray<physx::PxU32>& triIndices, physx::PxReal sdfSpacing,
        physx::PxU32 sdfSubgridSize = 6, physx::PxSdfBitsPerSubgridPixel::Enum bitsPerSdfSubgridPixel = physx::PxSdfBitsPerSubgridPixel::e16_BIT_PER_PIXEL);
    void CPhysXManager::setupCommonCookingParams(physx::PxCookingParams& params, bool skipMeshCleanup, bool skipEdgeData);
    physx::PxTriangleMesh* CPhysXManager::createBV34TriangleMesh(physx::PxU32 numVertices, const physx::PxVec3* vertices, physx::PxU32 numTriangles, const physx::PxU32* indices,
        bool skipMeshCleanup, bool skipEdgeData, bool inserted, const physx::PxU32 numTrisPerLeaf);

private:
    physx::PxDefaultAllocator		gAllocator;
    physx::PxDefaultErrorCallback	gErrorCallback;
    physx::PxFoundation*            gFoundation = nullptr;
    physx::PxPhysics*               gPhysics = nullptr;
    physx::PxCudaContextManager*    gCudaContextManager = nullptr;
    physx::PxDefaultCpuDispatcher*  gDispatcher = nullptr;
    physx::PxScene*                 gScene = nullptr;
    physx::PxMaterial*              gMaterial = nullptr;

    //Gravitational acceleration
    const physx::PxVec3 gGravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
};