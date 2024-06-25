#define _CRT_SECURE_NO_WARNINGS
#include "PhysXManager.h"
#include "stdafx.h"
#include "Object.h"
#include <vector>
#include <stack>

using namespace physx;

#define PVD_HOST "127.0.0.1"

static void DecomposeTransform(const XMFLOAT4X4& transformMatrix, XMFLOAT3& scale, XMFLOAT4& rotation, XMFLOAT3& position) {
    XMMATRIX matrix = XMLoadFloat4x4(&transformMatrix);
    XMVECTOR xmvScale, xmvRotate, xmvPos;

    XMMatrixDecompose(&xmvScale, &xmvRotate, &xmvPos, matrix);

    XMStoreFloat3(&scale, xmvScale);
    XMStoreFloat4(&rotation, xmvRotate);
    XMStoreFloat3(&position, xmvPos);
}

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

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.1f);

    //PxReal stackZ = 100.0f;

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

physx::PxActor* CPhysXManager::AddCapshulDynamic(CGameObject* object)
{
    using namespace physx;

    XMFLOAT3 xmf3Scale, xmf3Position;
    XMFLOAT4 xmf4Rotation;
    DecomposeTransform(object->GetWorldMat(), xmf3Scale, xmf4Rotation, xmf3Position);

    PxVec3 spawnPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
    PxReal groundHeight = GetGroundHeightUsingSweep(gScene, spawnPosition);
    PxReal capsuleRadius = 100.f;
    PxReal capsuleHalfHeight = PHYSX_CAPSUL_HEIGHT;
    PxVec3 initialPosition(spawnPosition.x, groundHeight + capsuleRadius + capsuleHalfHeight + 0.1f, spawnPosition.z);

    PxTransform transform(initialPosition, PxQuat(xmf4Rotation.x, xmf4Rotation.y, xmf4Rotation.z, xmf4Rotation.w)); // 위치 지정

    PxRigidDynamic* aCapsuleActor = gPhysics->createRigidDynamic(PxTransform(transform));

    PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));

    PxShape* aCapsuleShape = PxRigidActorExt::createExclusiveShape(*aCapsuleActor,
        PxCapsuleGeometry(capsuleRadius, capsuleHalfHeight), *gMaterial);
    aCapsuleShape->setLocalPose(relativePose);
    PxRigidBodyExt::updateMassAndInertia(*aCapsuleActor, 1000.f);
    aCapsuleActor->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);

    // actor가 자신의 object*를 알도록 저장
    aCapsuleActor->userData = object;

    gScene->addActor(*aCapsuleActor);

    object->m_vpPhysXShape.push_back(aCapsuleShape);
    object->m_pPhysXActor = aCapsuleActor;
    object->m_PhysXActorType = PhysXActorType::Dynamic;

    return aCapsuleActor;

    //CMesh** ppMeshs = object->GetMeshes();
    //int nMeshs = object->GetNumMeshes();
    //PxRigidDynamic* convexMeshActor = gPhysics->createRigidDynamic(PxTransform(transform));
    //for (int i = 0; i < nMeshs; ++i) {
    //    CMesh* pMesh = ppMeshs[i];
    //    PxConvexMesh* gConvexMesh;

    //    if (object->m_vpPhysXMesh.size() > i)
    //        gConvexMesh = ConvertConvexMeshCollider(object->m_vpPhysXMesh[i]);
    //    else
    //        gConvexMesh = ConvertConvexMeshCollider(CreateCustomTriangleMeshCollider(pMesh));

    //    // Shape를 생성.
    //    PxConvexMeshGeometry meshGeometry;
    //    meshGeometry.convexMesh = gConvexMesh;
    //    meshGeometry.scale = PxVec3(xmf3Scale.x, xmf3Scale.y, xmf3Scale.z);
    //    PxShape* shape = gPhysics->createShape(meshGeometry, *gMaterial);
    //    if (shape) {
    //        shape->setContactOffset(0.05f);
    //        shape->setRestOffset(0.0f);

    //        // 물리 시뮬레이션에 오브젝트로 추가.
    //        convexMeshActor->attachShape(*shape);

    //        shape->release();
    //    }
    //}

    //gScene->addActor(*convexMeshActor);

    //object->m_pPhysXActor = convexMeshActor;
    //object->m_PhysXActorType = PhysXActorType::Dynamic;

    //if (object->GetChild()) AddCapshulDynamic(object->GetChild());
    //if (object->GetSibling()) AddCapshulDynamic(object->GetSibling());

    //return convexMeshActor;
}

physx::PxActor* CPhysXManager::AddCapshulKinematic(CGameObject* object)
{
    using namespace physx;

    XMFLOAT3 xmf3Scale, xmf3Position;
    XMFLOAT4 xmf4Rotation;
    DecomposeTransform(object->GetWorldMat(), xmf3Scale, xmf4Rotation, xmf3Position);

    PxTransform transform(xmf3Position.x, xmf3Position.y, xmf3Position.z, PxQuat(xmf4Rotation.x, xmf4Rotation.y, xmf4Rotation.z, xmf4Rotation.w)); // 위치 지정

    PxRigidDynamic* aCapsuleActor = gPhysics->createRigidDynamic(PxTransform(transform));
    aCapsuleActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

    PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));


    PxShape* aCapsuleShape = PxRigidActorExt::createExclusiveShape(*aCapsuleActor,
        PxCapsuleGeometry(100.f, 200.f), *gMaterial);
    aCapsuleShape->setLocalPose(relativePose);
    PxRigidBodyExt::updateMassAndInertia(*aCapsuleActor, 100.f);
    aCapsuleActor->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);

    // actor가 자신의 object*를 알도록 저장
    aCapsuleActor->userData = object;

    gScene->addActor(*aCapsuleActor);

    object->m_pPhysXActor = aCapsuleActor;
    object->m_PhysXActorType = PhysXActorType::Kinematic;

    if (object->GetChild()) AddCapshulDynamic(object->GetChild());
    if (object->GetSibling()) AddCapshulDynamic(object->GetSibling());

    return aCapsuleActor;
}

physx::PxActor* CPhysXManager::AddStaticCustomGeometry(CGameObject* object)
{
    using namespace physx;

    CMesh** ppMeshs = object->GetMeshes();
    int nMeshs = object->GetNumMeshes();

    XMFLOAT3 xmf3Scale, xmf3Position;
    XMFLOAT4 xmf4Rotation;
    DecomposeTransform(object->GetWorldMat(), xmf3Scale, xmf4Rotation, xmf3Position);

    PxTransform transform(xmf3Position.x, xmf3Position.y, xmf3Position.z, PxQuat(xmf4Rotation.x, xmf4Rotation.y, xmf4Rotation.z, xmf4Rotation.w)); // 위치 지정
    PxRigidStatic* staticActor = gPhysics->createRigidStatic(transform);
    for (int i = 0; i < nMeshs; ++i) {
        CMesh* pMesh = ppMeshs[i];
        PxTriangleMesh* gTriangleMesh;

        if (object->m_vpPhysXMesh.size() > i) {
            gTriangleMesh = object->m_vpPhysXMesh[i];
        }
        else {
            gTriangleMesh = CreateCustomTriangleMeshCollider(pMesh);
            object->m_vpPhysXMesh.push_back(gTriangleMesh);
        }

        // Shape를 생성.
        //PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE;
        PxTriangleMeshGeometry meshGeometry;
        meshGeometry.triangleMesh = gTriangleMesh;
        meshGeometry.scale = PxVec3(xmf3Scale.x, xmf3Scale.y, xmf3Scale.z);
        //PxShape* shape = PxRigidActorExt::createExclusiveShape(*staticActor, meshGeometry, *gMaterial);
        PxShape* shape = gPhysics->createShape(meshGeometry, *gMaterial);
        if (shape) {
            shape->setContactOffset(0.05f);
            shape->setRestOffset(0.0f);

            // 물리 시뮬레이션에 오브젝트로 추가.
            staticActor->attachShape(*shape);
            object->m_vpPhysXShape.push_back(shape);

            //shape->release();
        }
    }

    // actor가 자신의 object*를 알도록 저장
    if (staticActor)
    {
        staticActor->userData = object;

        gScene->addActor(*staticActor);
    }

    
    object->m_pPhysXActor = staticActor;
    object->m_PhysXActorType = PhysXActorType::Static;

    if (object->GetChild()) AddStaticCustomGeometry(object->GetChild());
    if (object->GetSibling()) AddStaticCustomGeometry(object->GetSibling());

    return staticActor;
}

physx::PxActor* CPhysXManager::AddDynamicConvexCustomGeometry(CGameObject* object)
{
    using namespace physx;

    CMesh** ppMeshs = object->GetMeshes();
    int nMeshs = object->GetNumMeshes();

    XMFLOAT3 xmf3Scale, xmf3Position;
    XMFLOAT4 xmf4Rotation;
    DecomposeTransform(object->GetWorldMat(), xmf3Scale, xmf4Rotation, xmf3Position);

    PxTransform transform(xmf3Position.x, xmf3Position.y, xmf3Position.z, PxQuat(xmf4Rotation.x, xmf4Rotation.y, xmf4Rotation.z, xmf4Rotation.w)); // 위치 지정
    PxRigidDynamic* dynamicActor = gPhysics->createRigidDynamic(transform);
    for (int i = 0; i < nMeshs; ++i) {
        CMesh* pMesh = ppMeshs[i];
        PxConvexMesh* gConvexMesh;
        gConvexMesh = CreateCustomConvexMeshCollider(pMesh);

        // Shape를 생성.
        PxConvexMeshGeometry meshGeometry;
        meshGeometry.convexMesh = gConvexMesh;
        meshGeometry.scale = PxVec3(xmf3Scale.x, xmf3Scale.y, xmf3Scale.z);
        PxShape* shape = gPhysics->createShape(meshGeometry, *gMaterial);
        if (shape) {
            shape->setContactOffset(0.05f);
            shape->setRestOffset(0.0f);

            // 물리 시뮬레이션에 오브젝트로 추가.
            dynamicActor->attachShape(*shape);
            object->m_vpPhysXShape.push_back(shape);

            //shape->release();
        }
    }

    // actor가 자신의 object*를 알도록 저장
    if (dynamicActor)
    {
        PxRigidBodyExt::updateMassAndInertia(*dynamicActor, 1000.f);

        dynamicActor->userData = object;

        gScene->addActor(*dynamicActor);
    }

    object->m_pPhysXActor = dynamicActor;
    object->m_PhysXActorType = PhysXActorType::Dynamic_Allow_Rotate;

    if (object->GetChild()) AddStaticCustomGeometry(object->GetChild());
    if (object->GetSibling()) AddStaticCustomGeometry(object->GetSibling());

    return dynamicActor;
}

physx::PxTriangleMesh* CPhysXManager::CreateCustomTriangleMeshCollider(CMesh* pMesh)
{
    UINT nStride = pMesh->GetStride();
    UINT* gOriginIndices = pMesh->GetIndices();
    UINT gVertexCount = pMesh->GetNumVertices();
    UINT gIndexCount = pMesh->GetNumIndices();

    std::vector<PxVec3> gVertices;
    std::vector<PxU32> gIndices;

    // Vertex 좌표만 추출한 배열 생성
    {
        if (nStride == sizeof(CVertex)) {
            CVertex* gOriginVertices = static_cast<CVertex*>(pMesh->GetVertices());
            for (UINT j = 0; j < gVertexCount; ++j) {
                XMFLOAT3 oriVertex = gOriginVertices[j].GetVertex();
                gVertices.push_back(PxVec3(oriVertex.x, oriVertex.y, oriVertex.z));
            }
        }
        else if (nStride == sizeof(CVertex_Skining)) {
            CVertex_Skining* gOriginVertices = static_cast<CVertex_Skining*>(pMesh->GetVertices());
            for (UINT j = 0; j < gVertexCount; ++j) {
                XMFLOAT3 oriVertex = gOriginVertices[j].GetVertex();
                gVertices.push_back(PxVec3(oriVertex.x, oriVertex.y, oriVertex.z));
            }
        }
    }
    // Index 자료형 변환
    {
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

    return gTriangleMesh;
}

physx::PxConvexMesh* CPhysXManager::CreateCustomConvexMeshCollider(CMesh* pMesh)
{
    UINT nStride = pMesh->GetStride();
    UINT* gOriginIndices = pMesh->GetIndices();
    UINT gVertexCount = pMesh->GetNumVertices();
    UINT gIndexCount = pMesh->GetNumIndices();

    std::vector<PxVec3> gVertices;
    std::vector<PxU32> gIndices;

    // Vertex 좌표만 추출한 배열 생성
    {
        if (nStride == sizeof(CVertex)) {
            CVertex* gOriginVertices = static_cast<CVertex*>(pMesh->GetVertices());
            for (UINT j = 0; j < gVertexCount; ++j) {
                XMFLOAT3 oriVertex = gOriginVertices[j].GetVertex();
                gVertices.push_back(PxVec3(oriVertex.x, oriVertex.y, oriVertex.z));
            }
        }
        else if (nStride == sizeof(CVertex_Skining)) {
            CVertex_Skining* gOriginVertices = static_cast<CVertex_Skining*>(pMesh->GetVertices());
            for (UINT j = 0; j < gVertexCount; ++j) {
                XMFLOAT3 oriVertex = gOriginVertices[j].GetVertex();
                gVertices.push_back(PxVec3(oriVertex.x, oriVertex.y, oriVertex.z));
            }
        }
    }
    // Index 자료형 변환
    {
        for (UINT j = 0; j < gIndexCount; ++j) {
            gIndices.push_back(gOriginIndices[j]);
        }
    }

    // PhysX Mesh를 생성.
    PxConvexMeshDesc convexDesc;
    convexDesc.points.count = static_cast<PxU32>(gVertices.size());
    convexDesc.points.stride = sizeof(PxVec3);
    convexDesc.points.data = gVertices.data();
    convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

    PxTolerancesScale scale;
    PxCookingParams params(scale);

    PxDefaultMemoryOutputStream writeBuffer;
    PxConvexMeshCookingResult::Enum result;
    bool status = PxCookConvexMesh(params, convexDesc, writeBuffer, &result);
    if (!status)
        return NULL;

    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    PxConvexMesh* gConvexMesh = gPhysics->createConvexMesh(readBuffer);

    return gConvexMesh;
}

physx::PxConvexMesh* CPhysXManager::ConvertConvexMeshCollider(physx::PxTriangleMesh* pTriangleMesh)
{
    PxConvexMeshDesc convexDesc;
    convexDesc.points.count = pTriangleMesh->getNbVertices();
    convexDesc.points.stride = sizeof(PxVec3);
    convexDesc.points.data = pTriangleMesh->getVertices();
    convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

    PxTolerancesScale scale;
    PxCookingParams params(scale);

    PxDefaultMemoryOutputStream writeBuffer;
    PxConvexMeshCookingResult::Enum result;
    bool status = PxCookConvexMesh(params, convexDesc, writeBuffer, &result);
    if (!status)
        return NULL;

    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    PxConvexMesh* gConvexMesh = gPhysics->createConvexMesh(readBuffer);

    return gConvexMesh;
}


// 이동 함수 (사용자 입력에 따라 위치 변경, 충돌 검사 포함)
void CPhysXManager::MoveKineticActor(physx::PxRigidDynamic* kineticActor, const physx::PxVec3& deltaMove) {
    PxTransform playerTransform = kineticActor->getGlobalPose();
    PxVec3 newPosition = playerTransform.p + deltaMove;

    // Sweep 테스트를 사용한 충돌 검사
    PxSweepBuffer hit;
    PxCapsuleGeometry capsuleGeometry(0.5f, 1.0f);
    PxTransform sweepStart = playerTransform;
    PxTransform sweepEnd(newPosition);

    bool hasHit = gScene->sweep(capsuleGeometry, sweepStart, deltaMove.getNormalized(), deltaMove.magnitude(), hit, PxHitFlag::eDEFAULT, PxQueryFilterData());

    if (!hasHit) {
        // 충돌이 없으면 이동
        kineticActor->setKinematicTarget(PxTransform(newPosition));
    }
    else {
        // 충돌이 있으면 이동 취소
        kineticActor->setKinematicTarget(PxTransform(playerTransform.p));
    }
}

// 이동 함수 (목표 방향으로 이동)
void CPhysXManager::MoveDynamicActor(physx::PxRigidDynamic* dynamicActor, const physx::PxVec3& targetPosition, float speed, float deltaTime) {
    PxVec3 currentPosition = dynamicActor->getGlobalPose().p;
    PxVec3 direction = (targetPosition - currentPosition).getNormalized();
    PxVec3 velocity = direction * speed;

    dynamicActor->setLinearVelocity(velocity);
}


// 바닥 높이 측정을 위한 Sweep 테스트 함수
physx::PxReal CPhysXManager::GetGroundHeightUsingSweep(physx::PxScene* scene, const physx::PxVec3& position, physx::PxReal maxDistance) {
    PxCapsuleGeometry capsuleGeometry(0.5f, 1.0f);
    PxTransform sweepStart(position);
    PxVec3 sweepDirection(0.0f, -1.0f, 0.0f);

    PxSweepBuffer hit;
    if (scene->sweep(capsuleGeometry, sweepStart, sweepDirection, maxDistance, hit, PxHitFlag::eDEFAULT, PxQueryFilterData())) {
        return hit.block.position.y;
    }

    return position.y; // 바닥을 찾지 못한 경우, 기본 높이를 반환
}