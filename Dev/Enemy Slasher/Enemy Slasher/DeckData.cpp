#include "DeckData.h"

CDeckData::CDeckData()
{
    // �ϴ� ���� 0~4�� 2�徿 ä��.
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 5; ++j) {
            m_InitialDeck.push_back(j);
        }
    }
    InitializeDeck();// ���� ���� �ٸ������� �ؾ���.
}

CDeckData::~CDeckData()
{
}

int CDeckData::Draw(std::default_random_engine& DRE)
{
    if (m_Deck.size() != 0) {
        if (m_Hand.size() < 5)// �̰� �ٲ� �� ����.
        {
            int drawnCard = m_Deck.back();
            m_Deck.pop_back();
            m_Hand.push_back(drawnCard);
            return drawnCard;
        }
    }
    else
    {
        if (RefreshDeck(DRE) != 0)
            Draw(DRE);// ���⼭ ���� �� ����.
    }
    // ���� ������ ī�尡 ������ -1 ����.
    return -1;
}

void CDeckData::ShuffleDeck(std::default_random_engine& DRE)
{
    std::shuffle(m_Deck.begin(), m_Deck.end(), DRE);
}

int CDeckData::RefreshDeck(std::default_random_engine& DRE)
{
    m_Deck = m_Used;
    m_Used.clear();
    ShuffleDeck(DRE);
    return GetCurrentDeckSize();
}

void CDeckData::AddCardToInitialDeck(int i)
{
    m_InitialDeck.push_back(i);
    // ����ȭ�� ���� ��ġ�� �ٲ�����
    std::sort(m_InitialDeck.begin(), m_InitialDeck.end());
}

void CDeckData::SendHandToUsedByIndex(int index)
{
    int card = m_Hand[index];
    m_Hand.erase(m_Hand.begin() + index);
    m_Used.push_back(card);
}

void CDeckData::SendHandToUsedByNum(int num)
{
    auto it = std::find(m_Hand.begin(), m_Hand.end(), num);

    if (it != m_Hand.end()) {
        m_Used.push_back(*it);
        m_Hand.erase(it);
    }
    else
        ;
}

int CDeckData::GetMaxDeckSize()
{
    return m_Deck.size()+ m_Used.size()+ m_Hand.size();
}

int CDeckData::GetCurrentDeckSize()
{
    return m_Deck.size();
}

int CDeckData::GetCurrentHandSize()
{
    return m_Hand.size();
}

int CDeckData::GetCurrentUsedSize()
{
    return m_Used.size();
}

void CDeckData::InitializeDeck()
{
    m_Deck = m_InitialDeck;
    m_Hand.clear();
    m_Used.clear();
}


