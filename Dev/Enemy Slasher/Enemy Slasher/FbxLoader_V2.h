#pragma once
#include "stdafx.h"
#include "fbxsdk.h"
#include <map>
#include <string>

class CFbxData
{
public:
	CFbxData() {};
	~CFbxData() {};

public:

public:
	int m_nChildCount = 0;
	CFbxData** m_ppChilds = nullptr;
};

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
	CFbxData* LoadNode(FbxNode* node);
	void LoadContent(FbxNode* node, CFbxData* pData);

};