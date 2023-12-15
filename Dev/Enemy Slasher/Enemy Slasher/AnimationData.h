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

	//			(��� �̸�,	�� ����� (������, ��ȯ���))
	std::map<std::string, std::map<int, XMFLOAT4X4>> m_mAnimationData;

};

