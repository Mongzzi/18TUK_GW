#pragma once
#include <fbxsdk.h>
#include <vector>

#define STONE_LIT_001_FBX "fbxsdk/Stone_lit_001.fbx"
#define STONE_BIG_001_FBX "fbxsdk/Stone_big_001.fbx"

enum class LoadResult : int {
	False,
	First,
	Overlapping	// 대체어 찾아볼것.
};

class CFBXLoader {
public:
	CFBXLoader();
	~CFBXLoader();

	void InitializeSdkObjects();
	void DestroySdkObjects(bool pExitStatus);
	//void DisplayContent(FbxScene* pScene);
	//void DisplayContent(FbxNode* pNode);
	//void DisplayMesh(FbxNode* pNode);
	LoadResult LoadScene(const char* pFilename);
	bool LoadSceneFromFile(const char* pFilename);

	bool CheckFileNameList(const char* pFilename);

	FbxScene* GetScene();
private:
	FbxManager* m_plSdkManager = NULL;
	FbxScene* m_plScene = NULL;

	std::vector<const char*> m_vfileNameList;
};





