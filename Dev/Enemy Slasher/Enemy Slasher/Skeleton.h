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

	int GetIndicesCount() { return m_iIndicesCount; };

	int* GetIndices() { return m_ipIndices; };
	double* GetWeights() { return m_dpWeights; };
	XMFLOAT4X4 GetOffsetMatrix();

	XMFLOAT4X4 GetTransformMatrix() { return m_xmf4x4TransformMatrix; };
	void SetTransformMatrix(XMFLOAT4X4 in) { m_xmf4x4TransformMatrix = in; };
	XMFLOAT4X4 GetTransformLinkMatrix() { return m_xmf4x4TransformLinkMatrix; };


	friend std::ostream& operator << (std::ostream& out, const CSkeleton& skel)
	{
		out << "name " << skel.m_strName << endl;
		out << "lIndicesCount : " << skel.m_iIndicesCount << endl;
		for (int i = 0;i < skel.m_iIndicesCount;i++)
		{
			out << "lIndices : " << skel.m_ipIndices[i] << ", lWeights : " << skel.m_dpWeights[i] << endl;
		}

		return out;
	}

private:
	string m_strName;

	CSkeleton** m_ppChild = NULL;
	CSkeleton* m_pParent = NULL;

	int m_inChildNum;
	int m_iIndicesCount;

	int* m_ipIndices;
	double* m_dpWeights;

	XMFLOAT4X4 m_xmf4x4TransformMatrix;
	XMFLOAT4X4 m_xmf4x4TransformLinkMatrix;
};