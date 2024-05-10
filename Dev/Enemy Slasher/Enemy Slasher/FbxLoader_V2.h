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
	CFbxData* LoadFBX(const char* fileName);

private:
	void InitializeSDK();
	void DestroySDK();

	void LoadMesh(FbxNode* rootNode, FbxScene* lScene, CFbxData* loadData);
	void LoadControlPoints(FbxNode* rootNode, CFbxData* loadData);
	
	void ReadControlPoints(FbxMesh* inMesh, CFbxData* loadData);
	void ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outNormal);
	void ReadUV(FbxMesh* inMesh, int inCtrlPointIndex, int inUVPointIndex, int inUVLayerNum, XMFLOAT2& outUV);

	void ProcessSkeletonHierarchy(FbxNode* inRootNode, CFbxData* loadData);
	void ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int myIndex, int inParentIndex, CFbxData* loadData);

	void ProcessJoints(FbxNode* inNode, CFbxData* loadData);
	void ProcessAnimations(FbxNode* inNode, FbxScene* lScene, CFbxData* loadData);
	void ProcessJointsAndAnimation(FbxNode* inNode, FbxScene* lScene, CFbxData* loadData);

	FbxAMatrix GetGeometryTransformation(FbxNode* inNode);
	unsigned int FindJointIndexUsingName(std::string JointName, CFbxData* loadData);
	void storeFbxAMat2XMFLOAT4x4(XMFLOAT4X4& dest, FbxAMatrix& sorce);
};