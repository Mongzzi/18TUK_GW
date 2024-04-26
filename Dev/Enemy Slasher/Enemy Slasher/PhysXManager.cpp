#include "PhysXManager.h"
#include "stdafx.h"
#include <vector>

//using namespace physx;

CPhysXManager::CPhysXManager()
{
    using namespace physx;

    PxU32 P_Version = 0x05030000; //PX_PHYSICS_VERSION;
    gFoundation = PxCreateFoundation(P_Version, gAllocator, gErrorCallback);

    gPhysics = PxCreatePhysics(P_Version, *gFoundation, PxTolerancesScale());

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = gGravity;
    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    gScene = gPhysics->createScene(sceneDesc);

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);


    PxReal stackZ = 100.0f;

    PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
    gScene->addActor(*groundPlane);

    for (PxU32 i = 0; i < 5; i++)
        createStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);

    PxRigidDynamic* ball = createDynamic(PxTransform(PxVec3(0, 20, 100)), PxSphereGeometry(5), PxVec3(0, -25, -100));
    PxRigidBodyExt::updateMassAndInertia(*ball, 1000.f);

    //createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));
}

CPhysXManager::~CPhysXManager()
{
    cleanupPhysics(true);
}

void CPhysXManager::stepPhysics(bool, float elapsedTime)
{
    gScene->simulate(elapsedTime);
    gScene->fetchResults(true);
}

void CPhysXManager::cleanupPhysics(bool)
{
    PX_RELEASE(gMaterial);
    PX_RELEASE(gScene);
    PX_RELEASE(gDispatcher);
    PX_RELEASE(gPhysics);
    //if (gPvd)
    //{
    //    PxPvdTransport* transport = gPvd->getTransport();
    //    gPvd->release();	gPvd = NULL;
    //    PX_RELEASE(transport);
    //}
    PX_RELEASE(gFoundation);
}

physx::PxRigidDynamic* CPhysXManager::createDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxVec3& velocity)
{
    physx::PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
    dynamic->setAngularDamping(0.5f);
    dynamic->setLinearVelocity(velocity);
    gScene->addActor(*dynamic);
    return dynamic;
}

void CPhysXManager::createStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent)
{
    physx::PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
    for (physx::PxU32 i = 0; i < size; i++)
    {
        for (physx::PxU32 j = 0; j < size - i; j++)
        {
            physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * halfExtent);
            physx::PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
            body->attachShape(*shape);
            physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
            gScene->addActor(*body);
        }
    }
    shape->release();
}

void CPhysXManager::renderCallback()
{
    stepPhysics(true);

    physx::PxScene* scene;
    PxGetPhysics().getScenes(&scene, 1);
    physx::PxU32 nbActors = scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
    if (nbActors)
    {
        std::vector<physx::PxRigidActor*> actors(nbActors);
        scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<physx::PxActor**>(&actors[0]), nbActors);
        updateActors(&actors[0], static_cast<physx::PxU32>(actors.size()));
    }
}

void CPhysXManager::updateActors(physx::PxRigidActor** actors, const physx::PxU32 numActors)
{
#define MAX_NUM_ACTOR_SHAPES 128
    physx::PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
    for (physx::PxU32 i = 0; i < numActors; i++)
    {
        const physx::PxU32 nbShapes = actors[i]->getNbShapes();
        PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
        actors[i]->getShapes(shapes, nbShapes);

        for (physx::PxU32 j = 0; j < nbShapes; j++)
        {
            const physx::PxMat44 shapePose(physx::PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
            const physx::PxGeometry& geom = shapes[j]->getGeometry();

            // render object
            //glMultMatrixf(&shapePose.column0.x);

            //renderGeometry(geom);
        }
    }
}
