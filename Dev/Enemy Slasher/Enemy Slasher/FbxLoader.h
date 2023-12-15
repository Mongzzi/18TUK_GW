#pragma once
#include <fbxsdk.h>
#include <vector>
#include <map>
#include <string>
#include "AnimationData.h"

#define STONE_LIT_001_FBX "fbxsdk/Stone_lit_001.fbx"
#define STONE_LIT_002_FBX "fbxsdk/Stone_lit_002.fbx"
#define STONE_LIT_003_FBX "fbxsdk/Stone_lit_003.fbx"
#define STONE_BIG_001_FBX "fbxsdk/Stone_big_001.fbx"

#define CARD_FBX "fbxsdk/Card.fbx"
#define CARDHIERARCHY_FBX "fbxsdk/CardHierarchy.fbx"

#define PEASANT_1_FBX "fbxsdk/peasant_1.fbx"
#define SWORD_2_FBX "fbxsdk/_sword_2.fbx"


#define AA1 "fbxsdk/tower.fbx"
#define AA2 "fbxsdk/torreMoba.fbx"
#define HOUSE_FBX "fbxsdk/house.fbx"

#define TREE1 "fbxsdk/Tree_temp_climate_001.fbx"
#define TREE2 "fbxsdk/Tree_temp_climate_002.fbx"
#define TREE3 "fbxsdk/Tree_temp_climate_003.fbx"
#define TREE4 "fbxsdk/Tree_temp_climate_004.fbx"
#define TREE5 "fbxsdk/Tree_temp_climate_005.fbx"

#define DEFAULT_TIME_MODE FbxTime::eFrames24

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
	LoadResult LoadScene(FbxString lFilePath);
	bool LoadSceneFromFile(const char* pFilename);

	bool CheckFileNameList(const char* pFilename);

	FbxNode* GetRootNode(const char* filename);

	FbxScene* GetScene();

	void LoadAnimaitionOnly(const char* fileName);
	void LoadAnimation(const char* fileName);
	void LoadAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, CAnimationData* pNewAniData, bool isSwitcher = false);
	void LoadAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode, CAnimationData* pNewAniData, bool isSwitcher = false);

	std::map<int, XMFLOAT4X4> DisplayChannels(FbxNode* pNode, FbxAnimLayer* pAnimLayer, bool isSwitcher);

	std::map<int, float> DisplayCurve(FbxAnimCurve* pCurve);

	CAnimationData* GetAnimationData(string name);

private:
	FbxManager* m_plSdkManager = NULL;
	FbxScene* m_plScene = NULL;

	std::map<std::string, FbxNode*> m_mfileNameList;
	std::map<std::string, CAnimationData*> m_mAnimationList;
};





