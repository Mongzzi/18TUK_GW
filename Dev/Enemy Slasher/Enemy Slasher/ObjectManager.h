#pragma once

class CShaderManager;

enum class ObjectLayer : int;

enum class ObjectLayer : int { // enum class�� int������ �Ͻ��� ��ȯ�� ���������� ����� ����ȯ�� �ؾ� ��
	BackGround,				// ��� ������Ʈ
	Terrain,				// �ͷ��� ������Ʈ
	LightObject,			// ���� ������Ʈ
	InteractiveObject,		// ��ȣ�ۿ밡�� ������Ʈ (ex ��, ��ư)
	Object,					// �߸� ������Ʈ
	ObjectNormal,
	DestroyedObject,		// �ı��� ������Ʈ
	Enemy,					// �� ������Ʈ
	Player,					// �÷��̾� ������Ʈ
	Ray,					// ���� �׽�Ʈ�� ���̾�
	// ------------------------�� �Ʒ��� depth/stancil ����
	// ------------------------���� �տ� �׷������� �����Ϸ��� vector�󿡼��� ������ �ٲ����.
	UIObject,				// ��ȣ�ۿ� �Ұ����� UI ������Ʈ
	InteractiveUIObject,	// ��ȣ�ۿ� ������ UI ������Ʈ
	Count					// Layer ����
};

class CObjectManager
{
private:
	std::vector<CGameObject*> m_pvObjectManager[int(ObjectLayer::Count)];

public:
	CObjectManager();
	~CObjectManager();

	std::vector<CGameObject*>* GetObjectList() { return m_pvObjectManager; }

	void AddObj(CGameObject* object, ObjectLayer layer);
	void DelObj(CGameObject* object, ObjectLayer layer);

	void AnimateObjects(float fTimeElapsed);
	void DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed);

	void ReleaseObjects();
	void ReleaseUploadBuffers();
	
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShaderManager* pShaderManager);
};