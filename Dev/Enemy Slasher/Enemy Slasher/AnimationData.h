#pragma once
#include <map>

class CAnimationData
{
public:
	CAnimationData(int totalFrames);
	~CAnimationData();

	int GetTotalFrames() { return m_iTotalFrames; };

	void insertData(std::pair<string, std::map<int, XMFLOAT4X4>> data);
private:
	int m_iTotalFrames;

	//			(노드 이름,	그 노드의 (프레임, 변환행렬))
	std::map<std::string, std::map<int, XMFLOAT4X4>> m_mAnimationData;

};

