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
	Ray,					// 광선 테스트용 레이어
	CutterObject,			// 다른 오브젝트를 자를 수 있는 오브젝트 DynamicShaping을 진행 후 무조건 삭제된다.
	TemporaryObject,		// 생존시간에 제한이 있는 오브젝트들
	// ------------------------이 아래는 depth/stancil 무시
	// ------------------------따라서 앞에 그려질지를 결정하려면 vector상에서의 순서를 바꿔야함.
	UIObject,				// 상호작용 불가능한 UI 오브젝트
	InteractiveUIObject,	// 상호작용 가능한 UI 오브젝트
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
	void DelObj(CGameObject* object);

	void ClearLayer(ObjectLayer layer);

	void AnimateObjects(float fTimeElapsed);
	void DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, CDynamicShapeMesh::CutAlgorithm cutAlgorithm = CDynamicShapeMesh::CutAlgorithm::Push);

	void ReleaseObjects();
	void ReleaseUploadBuffers();
	
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShaderManager* pShaderManager);

public:
	// Utility
	bool CollisionCheck_RayWithAABB(CRay* ray, CInteractiveObject* obj, float& tmin, float& tmax);
	void ScreenBasedObjectMove(CInteractiveObject* obj, CCamera* pCamera, float cxDelta, float cyDelta, float fSensitivity = 1.0f);
};