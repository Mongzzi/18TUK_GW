#pragma once
#include "stdafx.h"

class CSkeleton {
public:
	CSkeleton();
	~CSkeleton();

private:
	string name;

	CSkeleton* m_pChild = NULL;
	CSkeleton* m_pParent = NULL;

	XMFLOAT4X4 m_xmf4x4TransformMatrix;
	XMFLOAT4X4 m_xmf4x4TransformLinkMatrix;
};