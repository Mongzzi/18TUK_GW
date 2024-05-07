#pragma once
#include "stdafx.h"
#include "fbxsdk.h"
#include "FbxObjectData.h"
#include <map>
#include <string>

class CFbxLoader_V2 {
public:
	CFbxLoader_V2();
	~CFbxLoader_V2();

private:
	FbxManager* m_plSdkManager = nullptr;

	std::map<std::string, CFbxData*> m_mLoadedDataMap;
public:
	void InitializeSDK();
	void DestroySDK();
	CFbxData* LoadFBX(const char* fileName);
	void LoadMesh(FbxNode* rootNode, CFbxData* loadData);
	void ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outNormal);
	void ReadUV(FbxMesh* inMesh, int inCtrlPointIndex, int inUVPointIndex, int inUVLayerNum, XMFLOAT2& outUV);


};