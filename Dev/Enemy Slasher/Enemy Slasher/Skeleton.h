#pragma once
#include "stdafx.h"
#include "Mesh.h"
#include "FbxLoader.h"

class CSkeleton
{
public:
	CSkeleton();
	CSkeleton(string name, int childNum);
	~CSkeleton();

	void LoadHierarchy(FbxNode* pNode);
	void LoadHierarchyFromMesh(CFBXMesh* pMesh);
	void SetData(int indiceNum, double weight, FbxAMatrix transformMatrix, FbxAMatrix transformLinkMatrix);


	string GetName() { return m_strName; };

	int GetIndiceNum() { return m_iIndiceNUm; };
	int GetWeight() { return m_iWeight; };

	XMFLOAT4X4 GetTransformMatrix() { return m_xmf4x4TransformMatrix; };
	XMFLOAT4X4 GetTransformLinkMatrix() { return m_xmf4x4TransformLinkMatrix; };

private:
	string m_strName;

	CSkeleton* m_pChild = NULL;
	CSkeleton* m_pParent = NULL;

	int m_inChildNum;
	int m_iIndiceNUm;
	int m_iWeight;

	XMFLOAT4X4 m_xmf4x4TransformMatrix;
	XMFLOAT4X4 m_xmf4x4TransformLinkMatrix;
};