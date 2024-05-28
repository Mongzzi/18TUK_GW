#define _CRT_SECURE_NO_WARNINGS
#include "PhysXManager.h"
#include "stdafx.h"
#include "Object.h"
#include <vector>

//using namespace physx;

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

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = gGravity;

    gDispatcher = PxDefaultCpuDispatcherCreate(2);
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

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);


    PxReal stackZ = 100.0f;

    PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
    gScene->addActor(*groundPlane);

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

physx::PxActor* CPhysXManager::AddCustomGeometry(CGameObject* object)
{
    using namespace physx;

    CMesh** ppMeshs = object->GetMeshes();
    int nMeshs = object->GetNumMeshes();
    XMFLOAT3 xmfPos = object->GetPosition();
    {
        //// 동적 Actor 작성
        //PxTransform transform(xmfPos.x, xmfPos.y, xmfPos.z); // 위치 지정
        //PxRigidDynamic* dynamicActor = gPhysics->createRigidDynamic(transform);
        //dynamicActor->setLinearDamping(0.2f);
        //dynamicActor->setAngularDamping(0.1f);

        //dynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_GYROSCOPIC_FORCES, true);
        //dynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, true);

        //// CookingParameta 작성
        //PxTolerancesScale scale;
        //PxCookingParams params(scale);
        //params.meshWeldTolerance = 0.001f;
        //params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
        //params.buildTriangleAdjacencies = false;
        //params.buildGPUData = true;

        //for (int i = 0; i < nMeshs; ++i) {
        //    CMesh* pMesh = ppMeshs[i];
        //    CVertex* gOriginVertices = pMesh->GetVertices();
        //    UINT* gOriginIndices = pMesh->GetIndices();
        //    UINT gVertexCount = pMesh->GetNumVertices();
        //    UINT gIndexCount = pMesh->GetNumIndices();

        //    PxArray<PxVec3> gVertices;
        //    PxArray<PxU32> gIndices;

        //    // Vertex 좌표만 추출한 배열 생성
        //    // Index 자료형 변환
        //    {
        //        gVertices.resize(gVertexCount);
        //        for (int j = 0; j < gVertexCount; ++j) {
        //            XMFLOAT3 oriVertex = gOriginVertices->GetVertex();
        //            gVertices[j] = PxVec3(oriVertex.x, oriVertex.y, oriVertex.z);
        //        }
        //        gIndices.resize(gIndexCount);
        //        for (int j = 0; j < gIndexCount; ++j) {
        //            gIndices[j] = PxU32(gOriginIndices[j]);
        //        }
        //    }

        //    // PhysX Mesh를 생성.
        //    PxTriangleMesh* gTriangleMesh = createMesh(params, gVertices, gIndices, 0.05f);

        //    // Shape를 생성.
        //    PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE;
        //    PxTriangleMeshGeometry meshGeometry;
        //    meshGeometry.triangleMesh = gTriangleMesh;
        //    meshGeometry.scale = PxVec3(1.f, 1.f, 1.f);
        //    PxShape* shape = PxRigidActorExt::createExclusiveShape(*dynamicActor, meshGeometry, *gMaterial);
        //    shape->setContactOffset(0.05f);
        //    shape->setRestOffset(0.0f);

        //    //// 물리 시뮬레이션에 오브젝트로 추가.
        //    //dynamicActor->attachShape(*shape);

        //    //shape->release();
        //}
    }

    // 동적 Actor 작성
    PxTransform transform(xmfPos.x, xmfPos.y, xmfPos.z); // 위치 지정
    PxRigidDynamic* dynamicActor = gPhysics->createRigidDynamic(transform);
    PxShape* meshShape;
    for (int i = 0; i < nMeshs; ++i) {
        CMesh* pMesh = ppMeshs[i];
        CVertex* gOriginVertices = pMesh->GetVertices();
        UINT* gOriginIndices = pMesh->GetIndices();
        UINT gVertexCount = pMesh->GetNumVertices();
        UINT gIndexCount = pMesh->GetNumIndices();

        PxU32 gNumVertices = gVertexCount;
        PxU32 gNumTriangles = gIndexCount / 3;
        PxVec3* gVertices;
        PxU32* gIndices;

        // Vertex 좌표만 추출한 배열 생성
        // Index 자료형 변환
        {
            //gVertices.resize(gVertexCount);
            gVertices = new PxVec3[gVertexCount];
            for (int j = 0; j < gVertexCount; ++j) {
                XMFLOAT3 oriVertex = gOriginVertices->GetVertex();
                gVertices[j] = PxVec3(oriVertex.x, oriVertex.y, oriVertex.z);
            }
            //gIndices.resize(gIndexCount);
            gIndices = new PxU32[gIndexCount];
            for (int j = 0; j < gIndexCount; ++j) {
                gIndices[j] = PxU32(gOriginIndices[j]);
            }
        }
        // PhysX Mesh를 생성.
        //createBV34TriangleMesh(gNumVertices, gVertices, gNumTriangles, gIndices, false, false, false, 4); // - 런타임 속도 최적화 - 쿠킹 속도가 오래 걸리므로 프로그램 시작시 또는 미리 쿠킹 시 유용
        PxTriangleMesh* gTriangleMesh = createBV34TriangleMesh(gNumVertices, gVertices, gNumTriangles, gIndices, true, false, true, 15); // - 쿠킹 속도 최적화 

        // Shape를 생성.
        PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE;
        PxTriangleMeshGeometry meshGeometry;
        meshGeometry.triangleMesh = gTriangleMesh;
        meshGeometry.scale = PxVec3(1.f, 1.f, 1.f);
        PxShape* shape = gPhysics->createShape(meshGeometry, *gMaterial);
        dynamicActor->attachShape(*shape);

        //PxShape* shape = PxRigidActorExt::createExclusiveShape(*dynamicActor, meshGeometry, *gMaterial);
        //shape->setContactOffset(0.05f);
        //shape->setRestOffset(0.0f);
    }


    gScene->addActor(*dynamicActor);
    return dynamicActor;
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
        CVertex* gOriginVertices = pMesh->GetVertices();
        UINT* gOriginIndices = pMesh->GetIndices();
        UINT gVertexCount = pMesh->GetNumVertices();
        UINT gIndexCount = pMesh->GetNumIndices();

        PxArray<PxVec3> gVertices;
        PxArray<PxU32> gIndices;

        // Vertex 좌표만 추출한 배열 생성
        // Index 자료형 변환
        {
            gVertices.resize(gVertexCount);
            for (int j = 0; j < gVertexCount; ++j) {
                XMFLOAT3 oriVertex = gOriginVertices->GetVertex();
                gVertices[j] = PxVec3(oriVertex.x, oriVertex.y, oriVertex.z);
            }
            gIndices.resize(gIndexCount);
            for (int j = 0; j < gIndexCount; ++j) {
                gIndices[j] = PxU32(gOriginIndices[j]);
            }
        }

        // PhysX Mesh를 생성.
        PxTriangleMeshDesc meshDesc;
        meshDesc.points.count = gVertexCount;
        meshDesc.points.stride = sizeof(PxVec3);
        meshDesc.points.data = reinterpret_cast<const void*>(&gVertices);

        meshDesc.triangles.count = gIndexCount / 3;
        meshDesc.triangles.stride = 3 * sizeof(PxU32);
        meshDesc.triangles.data = reinterpret_cast<const void*>(&gIndices);

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

physx::PxTriangleMesh* CPhysXManager::createMesh(physx::PxCookingParams& params, const physx::PxArray<physx::PxVec3>& triVerts, const physx::PxArray<physx::PxU32>& triIndices, physx::PxReal sdfSpacing,
    physx::PxU32 sdfSubgridSize, physx::PxSdfBitsPerSubgridPixel::Enum bitsPerSdfSubgridPixel)
{
    using namespace physx;

    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = triVerts.size();
    meshDesc.triangles.count = triIndices.size() / 3;
    meshDesc.points.stride = sizeof(float) * 3;
    meshDesc.triangles.stride = sizeof(int) * 3;
    meshDesc.points.data = triVerts.begin();
    meshDesc.triangles.data = triIndices.begin();

    params.meshPreprocessParams |= PxMeshPreprocessingFlag::eENABLE_INERTIA;
    params.meshWeldTolerance = 1e-7f;

    PxSDFDesc sdfDesc;

    if (sdfSpacing > 0.f)
    {
        sdfDesc.spacing = sdfSpacing;
        sdfDesc.subgridSize = sdfSubgridSize;
        sdfDesc.bitsPerSubgridPixel = bitsPerSdfSubgridPixel;
        sdfDesc.numThreadsForSdfConstruction = 16;

        meshDesc.sdfDesc = &sdfDesc;
    }

    bool enableCaching = false;

    if (enableCaching)
    {
        const char* path = "C:\\tmp\\PhysXSDFSnippetData.dat";
        bool ok = false;
        FILE* fp = fopen(path, "rb");
        if (fp)
        {
            fclose(fp);
            ok = true;
        }

        if (!ok)
        {
            PxDefaultFileOutputStream stream(path);
            ok = PxCookTriangleMesh(params, meshDesc, stream);
        }

        if (ok)
        {
            PxDefaultFileInputData stream(path);
            PxTriangleMesh* triangleMesh = gPhysics->createTriangleMesh(stream);
            return triangleMesh;
        }
        return NULL;
    }
    else
        return PxCreateTriangleMesh(params, meshDesc, gPhysics->getPhysicsInsertionCallback());
}

// Setup common cooking params
void CPhysXManager::setupCommonCookingParams(physx::PxCookingParams& params, bool skipMeshCleanup, bool skipEdgeData)
{
    using namespace physx;
    // we suppress the triangle mesh remap table computation to gain some speed, as we will not need it 
// in this snippet
    params.suppressTriangleMeshRemapTable = true;

    // If DISABLE_CLEAN_MESH is set, the mesh is not cleaned during the cooking. The input mesh must be valid. 
    // The following conditions are true for a valid triangle mesh :
    //  1. There are no duplicate vertices(within specified vertexWeldTolerance.See PxCookingParams::meshWeldTolerance)
    //  2. There are no large triangles(within specified PxTolerancesScale.)
    // It is recommended to run a separate validation check in debug/checked builds, see below.

    if (!skipMeshCleanup)
        params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
    else
        params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

    // If eDISABLE_ACTIVE_EDGES_PRECOMPUTE is set, the cooking does not compute the active (convex) edges, and instead 
    // marks all edges as active. This makes cooking faster but can slow down contact generation. This flag may change 
    // the collision behavior, as all edges of the triangle mesh will now be considered active.
    if (!skipEdgeData)
        params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);
    else
        params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
}

// Creates a triangle mesh using BVH34 midphase with different settings.
physx::PxTriangleMesh* CPhysXManager::createBV34TriangleMesh(physx::PxU32 numVertices, const physx::PxVec3* vertices, physx::PxU32 numTriangles, const physx::PxU32* indices,
    bool skipMeshCleanup, bool skipEdgeData, bool inserted, const physx::PxU32 numTrisPerLeaf)
{
    using namespace physx;

    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = numVertices;
    meshDesc.points.data = vertices;
    meshDesc.points.stride = sizeof(PxVec3);
    meshDesc.triangles.count = numTriangles;
    meshDesc.triangles.data = indices;
    meshDesc.triangles.stride = 3 * sizeof(PxU32);

    PxTolerancesScale scale;
    PxCookingParams params(scale);

    // Create BVH34 midphase
    params.midphaseDesc = PxMeshMidPhase::eBVH34;

    // setup common cooking params
    setupCommonCookingParams(params, skipMeshCleanup, skipEdgeData);

    // Cooking mesh with less triangles per leaf produces larger meshes with better runtime performance
    // and worse cooking performance. Cooking time is better when more triangles per leaf are used.
    params.midphaseDesc.mBVH34Desc.numPrimsPerLeaf = numTrisPerLeaf;

#if defined(PX_CHECKED) || defined(PX_DEBUG)
    // If DISABLE_CLEAN_MESH is set, the mesh is not cleaned during the cooking. 
    // We should check the validity of provided triangles in debug/checked builds though.
    if (skipMeshCleanup)
    {
        PX_ASSERT(PxValidateTriangleMesh(params, meshDesc));
    }
#endif // DEBUG


    PxTriangleMesh* triMesh = NULL;
    PxU32 meshSize = 0;

    // The cooked mesh may either be saved to a stream for later loading, or inserted directly into PxPhysics.
    if (inserted)
    {
        triMesh = PxCreateTriangleMesh(params, meshDesc, gPhysics->getPhysicsInsertionCallback());
    }
    else
    {
        PxDefaultMemoryOutputStream outBuffer;
        PxCookTriangleMesh(params, meshDesc, outBuffer);

        PxDefaultMemoryInputData stream(outBuffer.getData(), outBuffer.getSize());
        triMesh = gPhysics->createTriangleMesh(stream);

        meshSize = outBuffer.getSize();
    }

    return triMesh;
}