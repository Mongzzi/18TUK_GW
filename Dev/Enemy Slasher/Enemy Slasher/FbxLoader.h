#pragma once
#include <fbxsdk.h>

#define STONE_LIT_001_FBX "fbxsdk/Stone_lit_001.fbx"
#define STONE_BIG_001_FBX "fbxsdk/Stone_big_001.fbx"

class CFBXLoader {
public:
	CFBXLoader();
	~CFBXLoader();

	void InitializeSdkObjects();
	void DestroySdkObjects(bool pExitStatus);
	//void DisplayContent(FbxScene* pScene);
	//void DisplayContent(FbxNode* pNode);
	//void DisplayMesh(FbxNode* pNode);
	bool LoadScene(const char* pFilename);

	FbxScene* GetScene();
private:
	FbxManager* m_plSdkManager = NULL;
	FbxScene* m_plScene = NULL;
};





