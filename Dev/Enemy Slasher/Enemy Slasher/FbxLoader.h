#pragma once
#include <fbxsdk.h>
#include <map>
#include <string>

#define STONE_LIT_001_FBX "fbxsdk/Stone_lit_001.fbx"
#define STONE_LIT_002_FBX "fbxsdk/Stone_lit_002.fbx"
#define STONE_LIT_003_FBX "fbxsdk/Stone_lit_003.fbx"
#define STONE_BIG_001_FBX "fbxsdk/Stone_big_001.fbx"

#define AA1 "fbxsdk/tower.fbx"
#define AA2 "fbxsdk/torreMoba.fbx"
#define AA3 "fbxsdk/house.fbx"


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
	LoadResult LoadScene(const char* pFilename, CFBXLoader* pFBXLoader);
	bool LoadSceneFromFile(const char* pFilename);

	bool CheckFileNameList(const char* pFilename, CFBXLoader* pFBXLoader);

	FbxNode* GetNode(const char* filename);

	FbxScene* GetScene();
private:
	FbxManager* m_plSdkManager = NULL;
	FbxScene* m_plScene = NULL;

	std::map<std::string, FbxNode*> m_mfileNameList;
};





