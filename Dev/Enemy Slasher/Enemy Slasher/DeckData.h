#pragma once

#include <vector>
#include <algorithm>
#include <random>

//
// �ִ��� ������ ��ɸ� �ְ� ��ȣ�ۿ�?�� �����Ͱ� �ϵ���.
//

class CDeckData
{
public:
	CDeckData();
	~CDeckData();
private:

	std::vector<int> m_InitialDeck;	// ���� �����ϴ� ����
	std::vector<int> m_Deck;	// ���� ����� ��
	std::vector<int> m_Hand;
	std::vector<int> m_Used;
public:
	// �ڵ� ���ü������� �Ʒ� �� �Լ����� ������ ���°� ���
	// ��ο�
	bool Draw(std::default_random_engine& DRE);
	// ����
	void ShuffleDeck(std::default_random_engine& DRE);
	// ������ �� �����
	int RefreshDeck(std::default_random_engine& DRE);

	// ���� ī�� �߰�
	void AddCardToInitialDeck(int i);
	// ���� ī�� ����
	// void ExtractCardFromDeck();
	// �� ������
	// Ư��ī�� �˻�?
	// void SearchCard();
	// �� �ִ� ũ��
	int GetMaxDeckSize();
	// ���� �� ũ��
	int GetCurrentDeckSize();
	// ���� �ڵ� ũ��
	int GetCurrentHandSize();
	// ���� ���� ũ��
	int GetCurrentUsedSize();
	// �� �ʱ�ȭ
	void InitializeDeck();
};

