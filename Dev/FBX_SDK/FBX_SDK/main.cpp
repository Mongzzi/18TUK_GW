#include <iostream>
#include <fbxsdk.h>
// Windows 헤더 파일:
#include <windows.h>

// C의 런타임 헤더 파일입니다.
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

#include <string>
#include <wrl.h>
#include <shellapi.h>

#include <fstream>
#include <vector>

using namespace std;

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include <Mmsystem.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")


void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
    // Create the FBX Manager
    pManager = FbxManager::Create();

    // Create an IOSettings object
    FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
    pManager->SetIOSettings(ios);

    // Create a new scene
    pScene = FbxScene::Create(pManager, "My Scene");
}

bool LoadScene(FbxManager* pManager, FbxScene* pScene, const char* pFilename)
{
    // Create the importer
    FbxImporter* pImporter = FbxImporter::Create(pManager, "");

    // Initialize the importer
    if (!pImporter->Initialize(pFilename, -1, pManager->GetIOSettings()))
    {
        std::cerr << "Failed to initialize importer: " << pImporter->GetStatus().GetErrorString() << std::endl;
        return false;
    }

    // Import the scene
    if (!pImporter->Import(pScene))
    {
        std::cerr << "Failed to import scene: " << pImporter->GetStatus().GetErrorString() << std::endl;
        return false;
    }

    // Destroy the importer
    pImporter->Destroy();

    return true;
}


void ExtractAnimationData(const char* pFilename, const char* pOutputFilename)
{
    // Initialize the FBX SDK objects
    FbxManager* pManager = nullptr;
    FbxScene* pScene = nullptr;
    InitializeSdkObjects(pManager, pScene);

    // Load the scene
    if (!LoadScene(pManager, pScene, pFilename))
    {
        std::cerr << "Failed to load scene: " << pFilename << std::endl;
        return;
    }

    // Open the output file
    std::ofstream outFile(pOutputFilename);
    if (!outFile.is_open())
    {
        std::cerr << "Failed to open output file: " << pOutputFilename << std::endl;
        return;
    }

    // Get the root node
    FbxNode* pRootNode = pScene->GetRootNode();
    if (!pRootNode)
    {
        std::cerr << "Root node is null." << std::endl;
        return;
    }

    // Traverse the scene and extract animation data
    for (int i = 0; i < pRootNode->GetChildCount(); i++)
    {
        FbxNode* pNode = pRootNode->GetChild(i);
        FbxNodeAttribute* pAttribute = pNode->GetNodeAttribute();

        if (pAttribute && pAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
        {
            // Extract animation data for this node
            outFile << "Node: " << pNode->GetName() << std::endl;

            FbxAnimStack* pAnimStack = pScene->GetMember<FbxAnimStack>(0);
            if (!pAnimStack)
            {
                std::cerr << "Animation stack is null." << std::endl;
                return;
            }

            FbxAnimLayer* pAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(0);
            if (!pAnimLayer)
            {
                std::cerr << "Animation layer is null." << std::endl;
                return;
            }

            FbxTime start = pAnimStack->GetLocalTimeSpan().GetStart();
            FbxTime end = pAnimStack->GetLocalTimeSpan().GetStop();

            outFile << "Start Time: " << start.GetSecondDouble() << std::endl;
            outFile << "End Time: " << end.GetSecondDouble() << std::endl;

            FbxTime time;
            FbxAMatrix matrix;

            for (time = start; time <= end; time += FbxTime::GetFramedTime())
            {
                matrix = pNode->EvaluateGlobalTransform(time);
                outFile << "Time: " << time.GetSecondDouble() << std::endl;
                outFile << "Matrix: " << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " << matrix[0][3] << std::endl;
                outFile << "        " << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " << matrix[1][3] << std::endl;
                outFile << "        " << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " << matrix[2][3] << std::endl;
                outFile << std::endl; // Add a newline character to separate animation data for different nodes
            }
        }
    }
    // Close the output file
    outFile.close();

    // Destroy the FBX SDK objects
    pManager->Destroy();
}


int main()
{

}