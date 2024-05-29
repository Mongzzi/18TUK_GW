#define _CRT_SECURE_NO_WARNINGS
#include "PhysXManager.h"
#include "stdafx.h"
#include "Object.h"
#include <vector>

using namespace physx;

#define PVD_HOST "127.0.0.1"

CPhysXManager::CPhysXManager()
{
    using namespace physx;

    PxU32 P_Version = 0x05030000; //PX_PHYSICS_VERSION;
    gFoundation = PxCreateFoundation(P_Version, gAllocator, gErrorCallback);

    gPvd = PxCreatePvd(*gFoundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

    gPhysics = PxCreatePhysics(P_Version, *gFoundation, PxTolerancesScale(), true, gPvd);

    gDispatcher = PxDefaultCpuDispatcherCreate(2);

    createScene();

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    //PxReal stackZ = 100.0f;

    //PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
    //gScene->addActor(*groundPlane);

    //for (PxU32 i = 0; i < 5; i++)
    //    createStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);

    //PxRigidDynamic* ball = createDynamic(PxTransform(PxVec3(0, 20, 100)), PxSphereGeometry(5), PxVec3(0, -25, -100));
    //PxRigidBodyExt::updateMassAndInertia(*ball, 1000.f);

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
    if (gPvd)
    {
        physx::PxPvdTransport* transport = gPvd->getTransport();
        gPvd->release();	gPvd = NULL;
        PX_RELEASE(transport);
    }
    PX_RELEASE(gFoundation);
}

physx::PxScene* CPhysXManager::createScene()
{
    if (gScene) {
        gScene->release();
        gScene = nullptr;
        if (gPvd)
        {
            physx::PxPvdTransport* transport = gPvd->getTransport();
            gPvd->release();	gPvd = NULL;
            PX_RELEASE(transport);
        }
    }

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = gGravity;

    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    gScene = gPhysics->createScene(sceneDesc);
    gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
    gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);

    PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    return gScene;
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

physx::PxActor* CPhysXManager::AddCapshulDynamic(CGameObject* object)
{
    using namespace physx;

    XMFLOAT3 xmfPos = object->GetPosition();
    PxTransform transform(xmfPos.x, xmfPos.y, xmfPos.z); // 위치 지정

    PxRigidDynamic* aCapsuleActor = gPhysics->createRigidDynamic(PxTransform(transform));

    PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));


    PxShape* aCapsuleShape = PxRigidActorExt::createExclusiveShape(*aCapsuleActor,
        PxCapsuleGeometry(100.f, 200.f), *gMaterial);
    aCapsuleShape->setLocalPose(relativePose);
    PxRigidBodyExt::updateMassAndInertia(*aCapsuleActor, 100.f);
    gScene->addActor(*aCapsuleActor);

    //aCapsuleActor->addForce(PxVec3(0.f, 0.f, 10000.f));
    aCapsuleActor->setLinearVelocity(PxVec3(0.f, 0.f, 100.f));

    //PxRigidDynamic* ball = createDynamic(transform, PxSphereGeometry(5), PxVec3(0, -25, -100));
    //PxRigidBodyExt::updateMassAndInertia(*ball, 1000.f);

    PxTransform newPos = aCapsuleActor->getGlobalPose();
    newPos.p = PxVec3(1000.f, 0.f, 500.f);
    aCapsuleActor->setGlobalPose(newPos);

    return aCapsuleActor;
}

physx::PxActor* CPhysXManager::AddStaticCustomGeometry(CGameObject* object)
{
    using namespace physx;

    CMesh** ppMeshs = object->GetMeshes();
    int nMeshs = object->GetNumMeshes();
    XMFLOAT3 xmfPos = object->GetPosition();

    PxTransform transform(xmfPos.x, xmfPos.y, xmfPos.z); // 위치 지정
    PxRigidStatic* staticActor = gPhysics->createRigidStatic(transform);
    for (int i = 0; i < nMeshs; ++i) {
        CMesh* pMesh = ppMeshs[i];
        CVertex_Skining* gOriginVertices = static_cast<CVertex_Skining*>(pMesh->GetVertices());
        UINT* gOriginIndices = pMesh->GetIndices();
        UINT gVertexCount = pMesh->GetNumVertices();
        UINT gIndexCount = pMesh->GetNumIndices(); 

        std::vector<PxVec3> gVertices;
        std::vector<PxU32> gIndices;

        // Vertex 좌표만 추출한 배열 생성
        // Index 자료형 변환
        {
            for (UINT j = 0; j < gVertexCount; ++j) {
                XMFLOAT3 oriVertex = gOriginVertices[j].GetVertex();
                gVertices.push_back(PxVec3(oriVertex.x, oriVertex.y, oriVertex.z));
            }

            for (UINT j = 0; j < gIndexCount; ++j) {
                gIndices.push_back(gOriginIndices[j]);
            }
        }

        // PhysX Mesh를 생성.
        PxTriangleMeshDesc meshDesc;
        meshDesc.points.count = static_cast<PxU32>(gVertices.size());
        meshDesc.points.stride = sizeof(PxVec3);
        meshDesc.points.data = gVertices.data();

        meshDesc.triangles.count = static_cast<PxU32>(gIndices.size() / 3);
        meshDesc.triangles.stride = 3 * sizeof(PxU32);
        meshDesc.triangles.data = gIndices.data();

        PxTolerancesScale scale;
        PxCookingParams params(scale);

        PxDefaultMemoryOutputStream writeBuffer;
        PxTriangleMeshCookingResult::Enum result;
        bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
        if (!status)
            return NULL;

        PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
        PxTriangleMesh* gTriangleMesh = gPhysics->createTriangleMesh(readBuffer);

        // Shape를 생성.
        PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE;
        PxTriangleMeshGeometry meshGeometry;
        meshGeometry.triangleMesh = gTriangleMesh;
        meshGeometry.scale = PxVec3(1.f, 1.f, 1.f);
        PxShape* shape = PxRigidActorExt::createExclusiveShape(*staticActor, meshGeometry, *gMaterial);
        shape->setContactOffset(0.05f);
        shape->setRestOffset(0.0f);

        // 물리 시뮬레이션에 오브젝트로 추가.
        staticActor->attachShape(*shape);

        shape->release();
    }

    gScene->addActor(*staticActor);
    return staticActor;
}