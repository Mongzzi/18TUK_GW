#pragma once

// Card -> 속성(attack/defence) -> 효과를 가진 카드

class Card
// Card의 최상위 클래스
{
private:
	int cost;

public:
	virtual void ShowRange() = 0; // 드래그 해서 패에서 끌어 낼 시 카드 효과의 범위를 보여줄 함수

	virtual void Active() = 0; // 카드가 발동 되었을 때 할 행동
};


class attackCard : Card
// 공격 속성 카드들의 상위 클래스
{
private:

public:

};


class defenceCard : Card
// 방어 속성 카드들의 상위 클래스
{
private:

public:

};


class bufCard : Card
// 버프 속성 카드들의 상위 클래스
{
private:

public:

};


class enhanceCard : Card
// 타 카드 효과 강화 속성 카드들의 상위 클래스
{
private:

public:

};


class counterCard : Card
// 카운터 카드들의 상위 클래스
{
private:

public:

};