#pragma once

class CShaderManager;
class CPhysXManager;

enum class ObjectLayer : int;

enum class ObjectLayer : int { // enum class�� int������ �Ͻ��� ��ȯ�� ���������� ����� ����ȯ�� �ؾ� ��
	BackGround,				// ��� ������Ʈ
	SkyBox,
	Terrain,				// �ͷ��� ������Ʈ
	LightObject,			// ���� ������Ʈ
	InteractiveObject,		// ��ȣ�ۿ밡�� ������Ʈ (ex ��, ��ư)
	Object,					// �߸� ������Ʈ
	ObjectNormal,
	ObjectPhysX,			// �׽�Ʈ�� PhysX Layer
	DestroyedObject,		// �ı��� ������Ʈ
	Enemy,					// �� ������Ʈ
	Player,					// �÷��̾� ������Ʈ
	Ray,					// ���� �׽�Ʈ�� ���̾�
	CutterObject,			// �ٸ� ������Ʈ�� �ڸ� �� �ִ� ������Ʈ DynamicShaping�� ���� �� ������ �����ȴ�.
	TemporaryObject,		// �����ð��� ������ �ִ� ������Ʈ��
	BillBoardObject,		// ������ ������Ʈ�� ( �ν��Ͻ� �Ǿ��־� �������� �ѹ��� �׷��� )
	TextureObject,
	// ------------------------�� �Ʒ��� depth/stancil ����
	// ------------------------���� �տ� �׷������� �����Ϸ��� vector�󿡼��� ������ �ٲ����.
	UIObject,				// ��ȣ�ۿ� �Ұ����� UI ������Ʈ
	InteractiveUIObject,	// ��ȣ�ۿ� ������ UI ������Ʈ
	Count					// Layer ����
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
			ShaderType stype, float fTimeElapsed, CGameObject* pTargetObject, CGameObject* pCutterObject, CDynamicShapeMesh::CutAlgorithm cutAlgorithm = CDynamicShapeMesh::CutAlgorithm::Push); // ���ܵ� ������Ʈ 2���� �����Ѵ�.

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