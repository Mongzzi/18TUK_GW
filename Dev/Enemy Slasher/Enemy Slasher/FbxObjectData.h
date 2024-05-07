#pragma once
#include "stdafx.h"
#include <vector>

class CFbxVertex {
public:
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Normal;
	XMFLOAT2 m_UV;
};

class CFbxData {
public:
	CFbxData() {};
	~CFbxData() {};

public:
	std::vector<CFbxVertex> m_vVertex;
	std::vector<int> m_vIndices;
};