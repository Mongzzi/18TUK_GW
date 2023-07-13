#pragma once

enum class ObjectLayer : int;

enum class ObjectLayer : int { // enum class�� int������ �Ͻ��� ��ȯ�� ���������� ������ ����ȯ�� �ؾ� ��
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
	std::vector<CGameObject*> m_pvObjectManager[int(ObjectLayer::Count)];
	CShader** m_ppShaderManager;


public:
	CObjectManager();
	~CObjectManager();

	void AddObj(CGameObject* object, ObjectLayer layer);
	void DelObj(CGameObject* object, ObjectLayer layer);

	void AnimateObjects(float fTimeElapsed);
	
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);


	// m_vShaderManager

};