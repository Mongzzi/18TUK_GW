#pragma once
#include <fbxsdk.h>

// ��ȣ�մϴ� �߸� ::FbxManager
// ��ȣ�ϴٴ� class �տ� :: �ٿ�����. global namespace
void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);
//void DisplayContent(FbxScene* pScene);
//void DisplayContent(FbxNode* pNode);
//void DisplayMesh(FbxNode* pNode);
bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);



