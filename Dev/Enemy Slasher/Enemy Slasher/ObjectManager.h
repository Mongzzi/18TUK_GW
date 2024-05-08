#pragma once

class CShaderManager;
class CPhysXManager;

enum class ObjectLayer : int;

enum class ObjectLayer : int { // enum class는 int형으로 암시적 변환을 불허함으로 명시적 형변환을 해야 함
	BackGround,				// 배경 오브젝트
	SkyBox,
	Terrain,				// 터레인 오브젝트
	LightObject,			// 조명 오브젝트
	InteractiveObject,		// 상호작용가능 오브젝트 (ex 문, 버튼)
	Object,					// 중립 오브젝트
	ObjectNormal,
	ObjectPhysX,			// 테스트용 PhysX Layer
	DestroyedObject,		// 파괴된 오브젝트
	Enemy,					// 적 오브젝트
	Player,					// 플레이어 오브젝트
	Ray,					// 광선 테스트용 레이어
	CutterObject,			// 다른 오브젝트를 자를 수 있는 오브젝트 DynamicShaping을 진행 후 무조건 삭제된다.
	TemporaryObject,		// 생존시간에 제한이 있는 오브젝트들
	BillBoardObject,		// 빌보드 오브젝트들 ( 인스턴스 되어있어 여러개가 한번에 그려짐 )
	TextureObject,
	// ------------------------이 아래는 depth/stancil 무시
	// ------------------------따라서 앞에 그려질지를 결정하려면 vector상에서의 순서를 바꿔야함.
	UIObject,				// 상호작용 불가능한 UI 오브젝트
	InteractiveUIObject,	// 상호작용 가능한 UI 오브젝트
	Count					// Layer 개수
};

namespace physx {
	class PxActor;
}
class CObjectManager
{
private:
	std::vector<CGameObject*> m_pvObjectManager[int(ObjectLayer::Count)];

	CPhysXManager* m_pPhysXManager = nullptr;
	vector<pair<CGameObject*, physx::PxActor*>> m_vPhysxPairs;

public:
	CObjectManager();
	~CObjectManager();

	std::vector<CGameObject*>* GetObjectList() { return m_pvObjectManager; }

	std::vector<CGameObject*> GetObjectList(ObjectLayer layer) { return m_pvObjectManager[(int)layer]; }

	void SetPhysXManager(CPhysXManager* physxManager) { m_pPhysXManager = physxManager; }

	void AddObj(CGameObject* object, ObjectLayer layer);
	void DelObj(CGameObject* object, ObjectLayer layer);
	void DelObj(CGameObject* object);

	void ClearLayer(ObjectLayer layer);

	void AnimateObjects(float fTimeTotal, float fTimeElapsed);
	void DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fTimeElapsed, CDynamicShapeMesh::CutAlgorithm cutAlgorithm = CDynamicShapeMesh::CutAlgorithm::Push);
	vector<CGameObject*> _AtomicDynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
			ShaderType stype, float fTimeElapsed, CGameObject* pTargetObject, CGameObject* pCutterObject, CDynamicShapeMesh::CutAlgorithm cutAlgorithm = CDynamicShapeMesh::CutAlgorithm::Push); // 절단된 오브젝트 2개를 리턴한다.

	void ReleaseObjects();
	void ReleaseUploadBuffers();
	
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

public:
	// Utility
	bool CollisionCheck_RayWithAABB(CRay* ray, CGameObject* obj, float& tmin, float& tmax);
	void ScreenBasedObjectMove(CGameObject* obj, CCamera* pCamera, float cxDelta, float cyDelta, float fSensitivity = 1.0f);
};