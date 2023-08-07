#pragma once
#include <fbxsdk.h>

// 모호합니다 뜨면 ::FbxManager
// 모호하다는 class 앞에 :: 붙여볼것. global namespace
void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);
//void DisplayContent(FbxScene* pScene);
//void DisplayContent(FbxNode* pNode);
//void DisplayMesh(FbxNode* pNode);
bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);



