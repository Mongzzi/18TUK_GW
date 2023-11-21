#pragma once
#include "stdafx.h"
#include "fbxLoader.h"

class CSkeleton {
public:
	CSkeleton();
	CSkeleton(string name);
	~CSkeleton();

	void LoadHierarchy(FbxNode* pNode);
	void SetDeta();

private:
	string m_strName;

	CSkeleton* m_pChild = NULL;
	CSkeleton* m_pParent = NULL;

	XMFLOAT4X4 m_xmf4x4TransformMatrix;
	XMFLOAT4X4 m_xmf4x4TransformLinkMatrix;
};