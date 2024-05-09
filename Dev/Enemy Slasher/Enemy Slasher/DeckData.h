#pragma once

#include <vector>

class CDeckData
{
public:
	CDeckData();
	~CDeckData();
private:
	std::vector<int> m_Deck;
	std::vector<int> m_Hand;
	std::vector<int> m_Used;
public:
	// 드로우
	// 셔플
	// 묘지로 덱 재생성
	// 덱 카드 추가
	// 덱 카드 추출
	// 특정카드 검색?
	// 덱 최대 크기
	// 현재 덱 크기
	// 현재 핸드 크기
	// 현재 묘지 크기
};

