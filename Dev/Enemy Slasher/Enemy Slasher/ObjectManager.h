#pragma once

class CShaderManager;

enum class ObjectLayer : int;

enum class ObjectLayer : int { // enum class는 int형으로 암시적 변환을 불허함으로 명시적 형변환을 해야 함
	BackGround,				// 배경 오브젝트
	Terrain,				// 터레인 오브젝트
	LightObject,			// 조명 오브젝트
	InteractiveObject,		// 상호작용가능 오브젝트 (ex 문, 버튼)
	Object,					// 중립 오브젝트
	ObjectNormal,
	DestroyedObject,		// 파괴된 오브젝트
	Enemy,					// 적 오브젝트
	Player,					// 플레이어 오브젝트
	UIObject,				// 플레이어 UI 오브젝트	 depth/stancil 무시하고 항상 앞에 그려져야 함
	Ray,					// 광선 테스트용 레이어
	Count					// Layer 개수
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

	void ReleaseObjects();
	void ReleaseUploadBuffers();
	
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShaderManager* pShaderManager);
};