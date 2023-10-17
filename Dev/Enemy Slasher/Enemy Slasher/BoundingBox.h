#pragma once
#include "stdafx.h"

class CAABB
{
public:
	CAABB();
	CAABB(XMFLOAT3 center, XMFLOAT3 edgeDist) : m_xmf3Center(center), m_xmf3EdgeDistances(edgeDist) {};
	~CAABB();
	XMFLOAT3 GetCenter() { return m_xmf3Center; };
	XMFLOAT3 GetEdgeDistances() { return m_xmf3EdgeDistances; };
private:
	XMFLOAT3 m_xmf3Center;
	XMFLOAT3 m_xmf3EdgeDistances;
};

