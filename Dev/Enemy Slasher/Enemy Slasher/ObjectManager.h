#pragma once

enum class ObjectLayer : int;

enum class ObjectLayer : int { // enum class�� int������ �Ͻ��� ��ȯ�� ���������� ����� ����ȯ�� �ؾ� ��
	BackGround,				// ��� ������Ʈ
	LightObject,			// ���� ������Ʈ
	InteractiveObject,		// ��ȣ�ۿ밡�� ������Ʈ (ex ��, ��ư)
	Object,					// �߸� ������Ʈ
	DestroyedObject,		// �ı��� ������Ʈ
	Enemy,					// �� ������Ʈ
	Player,					// �÷��̾� ������Ʈ
	UIObject,				// �÷��̾� UI ������Ʈ	 depth/stancil �����ϰ� �׻� �տ� �׷����� ��

	Count					// Layer ����
};

class CObjectManager
{
private:
	std::vector<CGameObject*> m_vObjectManager[int(ObjectLayer::Count)];
	std::vector<CShader*> m_vShaderManager;

public:
	void AddObj(CGameObject* object, ObjectLayer layer);
	void DelObj(CGameObject* object, ObjectLayer layer);
	void AddShader(CGameObject* object);
	void DelShader(CGameObject* object);

	void Render();
};