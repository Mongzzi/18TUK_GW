#pragma once

// Card -> �Ӽ�(attack/defence) -> ȿ���� ���� ī��

class Card
// Card�� �ֻ��� Ŭ����
{
private:
	int cost;

public:
	virtual void ShowRange() = 0; // �巡�� �ؼ� �п��� ���� �� �� ī�� ȿ���� ������ ������ �Լ�

	virtual void Active() = 0; // ī�尡 �ߵ� �Ǿ��� �� �� �ൿ
};


class attackCard : Card
// ���� �Ӽ� ī����� ���� Ŭ����
{
private:

public:

};


class defenceCard : Card
// ��� �Ӽ� ī����� ���� Ŭ����
{
private:

public:

};


class bufCard : Card
// ���� �Ӽ� ī����� ���� Ŭ����
{
private:

public:

};


class enhanceCard : Card
// Ÿ ī�� ȿ�� ��ȭ �Ӽ� ī����� ���� Ŭ����
{
private:

public:

};


class counterCard : Card
// ī���� ī����� ���� Ŭ����
{
private:

public:

};