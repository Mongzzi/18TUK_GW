#pragma once
#include "stdafx.h"
#include "Mesh.h"
#include "FbxLoader.h"

class CFBXMesh;

class CSkeleton
{
public:
	CSkeleton();
	CSkeleton(string name, int childNum);
	~CSkeleton();

	void LoadHierarchy(FbxNode* pNode);
	void LoadHierarchyFromMesh(CFBXMesh* pMesh);
	void SetData(string name, int indexCount, int* indices, double* weights, FbxAMatrix transformMatrix, FbxAMatrix transformLinkMatrix);


	string GetName() { return m_strName; };

	int GetIndexCount() { return m_iIndexCount; };

	int* GetIndices() { return m_ipIndices; };
	double* GetWeights() { return m_dpWeights; };

	XMFLOAT4X4 GetTransformMatrix() { return m_xmf4x4TransformMatrix; };
	XMFLOAT4X4 GetTransformLinkMatrix() { return m_xmf4x4TransformLinkMatrix; };


	friend std::ostream& operator << (std::ostream& out, const CSkeleton& skel)
	{
		out << "name " << skel.m_strName << ", childNum " << skel.m_inChildNum << ", indexCount " << skel.m_iIndexCount << endl << "indices " << skel.m_ipIndices[0] << endl << "weights " << skel.m_dpWeights[0] << " )";

		return out;
	}

private:
	string m_strName;

	CSkeleton** m_pChild = NULL;
	CSkeleton* m_pParent = NULL;

	int m_inChildNum;
	int m_iIndexCount;

	int* m_ipIndices;
	double* m_dpWeights;

	XMFLOAT4X4 m_xmf4x4TransformMatrix;
	XMFLOAT4X4 m_xmf4x4TransformLinkMatrix;
};