#include "PhysXManager.h"
#include "stdafx.h"
#include "Object.h"
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

void CPhysXManager::AddCustomGeometry(CGameObject* object)
{
    using namespace physx;

    CMesh** ppMeshs = object->GetMeshes();
    int nMeshs = object->GetNumMeshes();
    XMFLOAT3 xmfPos = object->GetPosition();

    // 동적 Actor 작성
    PxTransform transform(xmfPos.x, xmfPos.y, xmfPos.z); // 위치 지정
    PxRigidDynamic* dynamicActor = gPhysics->createRigidDynamic(transform);
    
    for (int i = 0; i < nMeshs; ++i) {
        CMesh* pMesh = ppMeshs[i];
        CVertex* gOriginVertices = pMesh->GetVertices();

        UINT* gIndices = pMesh->GetIndices();
        UINT gVertexCount = pMesh->GetNumVertices();
        UINT gIndexCount = pMesh->GetNumIndices();

        // Vertex 좌표만 추출한 배열 생성
        PxArray<PxVec3> gVertices;
        gVertices.resize(gVertexCount);
        for (int j = 0; j < gVertexCount; ++j) {
            XMFLOAT3 oriVertex = gOriginVertices->GetVertex();
            gVertices[j] = PxVec3(oriVertex.x, oriVertex.y, oriVertex.z);
        }

        // PhysX Mesh를 생성.
        PxTolerancesScale scale;
        PxCookingParams params(scale);
        params.midphaseDesc.setToDefault(PxMeshMidPhase::eBVH34);
        params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
        params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

        PxTriangleMeshDesc triangleMeshDesc;
        triangleMeshDesc.points.count = gVertexCount;
        triangleMeshDesc.points.data = gVertices.begin();
        triangleMeshDesc.points.stride = sizeof(PxVec3);

        triangleMeshDesc.triangles.count = gIndexCount / 3;
        triangleMeshDesc.triangles.data = gIndices;
        triangleMeshDesc.triangles.stride = 3 * sizeof(UINT);

        PxTriangleMesh* gTriangleMesh = PxCreateTriangleMesh(params, triangleMeshDesc);


        // Shape를 생성.
        PxTriangleMeshGeometry meshGeometry(gTriangleMesh);
        PxShape* shape = gPhysics->createShape(meshGeometry, *gMaterial, true);

        // 물리 시뮬레이션에 오브젝트로 추가.
        dynamicActor->attachShape(*shape);
        shape->release();
    }

    gScene->addActor(*dynamicActor);
}
