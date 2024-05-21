#pragma once
#include "FbxLoader_V3.h"
#include "Object.h"
#include <map>

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


#define IDLE_ANI_FBX "fbxsdk/Idle.fbx"
#define RUN_ANI_FBX "fbxsdk/Run.fbx"
#define ANI_TEST_ANI_FBX "fbxsdk/aniTest.fbx"
#define ANI_TEST2_ANI_FBX "fbxsdk/aniTest2.fbx"



class CResorceManager
{
public:
	CResorceManager();
	~CResorceManager();

public:
	CFBXObject* LoadFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const string& filePath, const string& fileName);
	CFBXObject* LoadFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFbx_V3::CFbxData* pFbxData);
	//CTexture* LoadTexture(const char* fileName);

private:
	CFBXObject* LoadFBXObjectRecursive(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFbx_V3::ObjectData* pObjectData);

private:
	CFbxLoader_V3* m_pFBXLoader = NULL;

	std::map<std::string, CFBXObject*> m_mLoadedFBXDataMap;
};

