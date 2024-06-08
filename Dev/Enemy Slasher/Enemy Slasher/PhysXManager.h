#pragma once
#include "PxPhysicsAPI.h"

#define PHYSX_CAPSUL_HEIGHT 200.f
#define MAX_NUM_ACTOR_SHAPES 128

class CGameObject;
class CMesh;
class CVertex;

class CustomObject : physx::PxCustomGeometry::Callbacks, physx::PxUserAllocated
{

};

class CPhysXManager
{
public:
    CPhysXManager();
    ~CPhysXManager();

    physx::PxScene* createScene();

    void stepPhysics(bool /*interactive*/, float elapsedTime = (1.0f / 60.0f));
    void cleanupPhysics(bool /*interactive*/);

    physx::PxRigidDynamic* createDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxVec3& velocity = physx::PxVec3(0));
    void createStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent);

public:
    physx::PxScene* GetScene() { return gScene; }
    physx::PxPhysics* GetPhysics() { return gPhysics; }

public:
    physx::PxActor* AddCapshulDynamic(CGameObject* object);
    physx::PxActor* AddCapshulKinematic(CGameObject* object);
    physx::PxActor* AddStaticCustomGeometry(CGameObject* object);

    physx::PxRigidStatic* AddStaticMapObject(CGameObject* object) {};

public:
    // Cook Triangle Mesh Collider
    physx::PxTriangleMesh* CreateCustomTriangleMeshCollider(CMesh* pMesh);
    physx::PxConvexMesh* ConvertConvexMeshCollider(physx::PxTriangleMesh* pTriangleMesh);

public:
    void MoveKineticActor(physx::PxRigidDynamic* kineticActor, const physx::PxVec3& deltaMove);
    void CPhysXManager::MoveDynamicActor(physx::PxRigidDynamic* dynamicActor, const physx::PxVec3& targetPosition, float speed, float deltaTime);

    physx::PxReal GetGroundHeightUsingSweep(physx::PxScene* scene, const physx::PxVec3& position, physx::PxReal maxDistance = 100.0f);

public:
    

private:
    physx::PxDefaultAllocator		gAllocator;
    physx::PxDefaultErrorCallback	gErrorCallback;
    physx::PxFoundation*            gFoundation = nullptr;
    physx::PxPhysics*               gPhysics = nullptr;
    physx::PxCudaContextManager*    gCudaContextManager = nullptr;
    physx::PxDefaultCpuDispatcher*  gDispatcher = nullptr;
    physx::PxScene*                 gScene = nullptr;
    physx::PxMaterial*              gMaterial = nullptr;
    physx::PxPvd*                   gPvd = nullptr;

    //Gravitational acceleration
    const physx::PxVec3             gGravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
};