#pragma once

#include <vector>
#include <algorithm>
#include <random>

//
// 최대한 간단한 기능만 넣고 상호작용?은 마스터가 하도록.
//

class CDeckData
{
public:
	CDeckData();
	~CDeckData();
private:

	std::vector<int> m_InitialDeck;	// 덱을 구성하는 원본
	std::vector<int> m_Deck;	// 실제 사용할 덱
	std::vector<int> m_Hand;
	std::vector<int> m_Used;
public:
	// 코드 가시성때문에 아래 세 함수에서 셔플을 빼는거 고려
	// 드로우
	bool Draw(std::default_random_engine& DRE);
	// 셔플
	void ShuffleDeck(std::default_random_engine& DRE);
	// 묘지로 덱 재생성
	int RefreshDeck(std::default_random_engine& DRE);

	// 원본 카드 추가
	void AddCardToInitialDeck(int i);
	// 원본 카드 추출
	// void ExtractCardFromDeck();
	// 패 버리기
	// 특정카드 검색?
	// void SearchCard();
	// 덱 최대 크기
	int GetMaxDeckSize();
	// 현재 덱 크기
	int GetCurrentDeckSize();
	// 현재 핸드 크기
	int GetCurrentHandSize();
	// 현재 묘지 크기
	int GetCurrentUsedSize();
	// 덱 초기화
	void InitializeDeck();
};

