#pragma once
#include "PxPhysicsAPI.h"

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

    void renderCallback();
    void updateActors(physx::PxRigidActor** actors, const physx::PxU32 numActors);

public:
    physx::PxActor* AddCapshulDynamic(CGameObject* object);
    physx::PxActor* AddStaticCustomGeometry(CGameObject* object);

    physx::PxRigidStatic* AddStaticMapObject(CGameObject* object) {};

public:
    // Cook Triangle Mesh Collider
    physx::PxTriangleMesh* CreateCustomTriangleMeshCollider(CMesh* pMesh);

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