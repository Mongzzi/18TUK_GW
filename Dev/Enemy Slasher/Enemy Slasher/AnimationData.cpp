#include "stdafx.h"
#include "AnimationData.h"


CAnimationData::CAnimationData(int totalFrames)
{
	m_iTotalFrames = totalFrames;
}

CAnimationData::~CAnimationData()
{
}

void CAnimationData::insertData(std::pair<string, std::map<int, XMFLOAT4X4>> data)
{
	m_mAnimationData.insert(data);
}
