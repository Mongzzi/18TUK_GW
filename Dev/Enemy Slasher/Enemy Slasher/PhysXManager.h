#pragma once
//#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
//#define _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS
//#include <physx/PxPhysicsAPI.h>
//#include <physx/PxPhysicsAPI.h>

//class CGameObject;
//class CVertex;

class CPhysXManager
{
public:
    CPhysXManager()
    {
    /*    using namespace physx;

        gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

        gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());

        PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
        sceneDesc.gravity = gGravity;
        gDispatcher = PxDefaultCpuDispatcherCreate(2);
        sceneDesc.cpuDispatcher = gDispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;

        gScene = gPhysics->createScene(sceneDesc);

        gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);*/
    }
    ~CPhysXManager()
    {
        //PX_RELEASE(gMaterial);
        //PX_RELEASE(gScene);
        //PX_RELEASE(gDispatcher);
        //PX_RELEASE(gPhysics);
        //PX_RELEASE(gFoundation);
    }

private:
    //physx::PxDefaultAllocator		gAllocator;
    //physx::PxDefaultErrorCallback	gErrorCallback;
    //physx::PxFoundation*            gFoundation = nullptr;
    //physx::PxPhysics*               gPhysics    = nullptr;
    //physx::PxDefaultCpuDispatcher*  gDispatcher = nullptr;
    //physx::PxScene*                 gScene      = nullptr;
    //physx::PxMaterial*              gMaterial   = nullptr;

    ////Gravitational acceleration
    //const physx::PxVec3 gGravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
};

