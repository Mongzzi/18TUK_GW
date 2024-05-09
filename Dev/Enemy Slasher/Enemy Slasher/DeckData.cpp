#include "DeckData.h"

CDeckData::CDeckData()
{
    // 일단 덱을 0~4로 5장씩 채움.
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            m_InitialDeck.push_back(i);
        }
    }
}

CDeckData::~CDeckData()
{
}

bool CDeckData::Draw(std::default_random_engine& DRE)
{
    if (!m_Deck.empty()) {
        int drawnCard = m_Deck.back();
        m_Deck.pop_back();
        m_Hand.push_back(drawnCard);
        return true;
    }
    else
    {
        if(!RefreshDeck(DRE))
            Draw(DRE);// 여기서 터질 수 있음.
    }
    // 덱과 묘지에 카드가 없으면 false 리턴.
    return false;
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
    // 최적화를 위해 위치를 바꿀지도
    std::sort(m_InitialDeck.begin(), m_InitialDeck.end());
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
}
