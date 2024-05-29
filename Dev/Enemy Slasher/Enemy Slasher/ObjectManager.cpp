#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "Player.h"
#include "ObjectManager.h"
#include "ShaderManager.h"
#include "PhysXManager.h"

CObjectManager::CObjectManager()
{
}

CObjectManager::~CObjectManager()
{
}


void CObjectManager::AddObj(CGameObject* object, ObjectLayer layer)
{
	// NON�̸� ����?�� �ش� ���̴� ����.
	// ���̴� ����Ʈ Ȯ���ؼ� ���������.

	//���̴� �Ŵ����� �ؾ���. ������ ���̴��޴������� ���������.
	/*if (!m_ppShaderManager[(int)object->GetShaderType()]) {
		CShaderManager::���̴�_�����_�Լ�((int)object->GetShaderType());
	}*/

	// �ϴ� ���̴� �Ŵ����� �׻� ��� ���̴��� ������ ����


	if (layer == ObjectLayer::Object)
	{
		((CDynamicShapeMesh*)((((CGameObject*)object)->GetMeshes()[0])))->SetCuttable(true);
		(object)->SetCuttable(true);
	}

	if (layer == ObjectLayer::ObjectPhysX && m_pPhysXManager != nullptr)
	{
		physx::PxActor* actor = m_pPhysXManager->AddStaticCustomGeometry(object);
		//m_vPhysxPairs.emplace_back(object, actor);
	}
	if (layer == ObjectLayer::Player && m_pPhysXManager != nullptr)
	{
		physx::PxActor* actor = m_pPhysXManager->AddCapshulDynamic(object);
		m_vPhysxPairs.emplace_back(object, actor);
	}

	m_pvObjectManager[(int)(layer)].push_back(object);
}

void CObjectManager::DelObj(CGameObject* object, ObjectLayer layer)
{
	delete object;
	std::vector<CGameObject*>* target = &(m_pvObjectManager[(int)layer]);
	target->erase(remove(target->begin(), target->end(), object), target->end());
}

void CObjectManager::DelObj(CGameObject* object)
{
	delete object;
	for (auto& target : m_pvObjectManager) {
		target.erase(remove(target.begin(), target.end(), object), target.end());
	}
}

void CObjectManager::ClearLayer(ObjectLayer layer)
{
	std::vector<CGameObject*>* target = &(m_pvObjectManager[(int)layer]);
	while (false == target->empty()) {
		delete target->back();
		target->pop_back();
	}
}

void CObjectManager::AnimateObjects(float fTimeTotal, float fTimeElapsed)
{
	for (std::vector<CGameObject*> a : m_pvObjectManager)
		for (CGameObject* b : a) {
			b->Animate(fTimeTotal, fTimeElapsed);
			//b->UpdateTransform(NULL);
		}

	{
		// ������ �ִ� ������Ʈ ���� ó��
		vector<CGameObject*> deleteObjects;
		for (CGameObject* b : m_pvObjectManager[(int)ObjectLayer::TemporaryObject]) {
			b->m_fLifeTime -= fTimeElapsed;
			if (b->m_fLifeTime < 0) {
				deleteObjects.push_back(b);
			}
		}
		for (const auto& a : deleteObjects) DelObj(a, ObjectLayer::TemporaryObject);	// ���� ������Ʈ ����
	}

	{
		if (m_pPhysXManager != nullptr) {
			// PhysX �� ���� �������� ����
			m_pPhysXManager->stepPhysics(true, fTimeElapsed);
			for (auto& p : m_vPhysxPairs) {
				physx::PxTransform transform = static_cast<physx::PxRigidActor*>(p.second)->getGlobalPose();
				physx::PxVec3 position = transform.p;
				//((CPlayer*)p.first)->SetPosition(XMFLOAT3(position.x, position.y, position.z));
			}
		}
	}
}

void CObjectManager::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fTimeElapsed, CDynamicShapeMesh::CutAlgorithm cutAlgorithm)
{
	if (false == m_pvObjectManager[(int)ObjectLayer::CutterObject].empty()) { // ������� �ʴٸ�
		vector<CGameObject*>* pvCutters = (&m_pvObjectManager[(int)ObjectLayer::CutterObject]);
		if (false == m_pvObjectManager[(int)ObjectLayer::Object].empty()) { // ������� �ʴٸ�
			vector<CGameObject*> pObjects = m_pvObjectManager[(int)ObjectLayer::Object];

			vector<CGameObject*> newObjects;
			vector<CGameObject*> deleteObjects;

			for (const auto& pCutter : *pvCutters) {
				for (const auto& pTargetObject : pObjects) {

					// �� ������Ʈ�� DynamicShapeObject��� ó��
					if (pTargetObject->GetCuttable()) {
						XMFLOAT4X4 objectWorldMat = pTargetObject->GetWorldMat();
						XMFLOAT4X4 cutterWorldMat = pCutter->GetWorldMat();
						if (CollisionCheck(pTargetObject->GetCollider(), objectWorldMat, pCutter->GetCollider(), cutterWorldMat)) {
							 //vector<CGameObject*> vRetVec = _AtomicDynamicShaping(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, /*������Ʈ�� ���̴� Ÿ��*/, fTimeElapsed, pTargetObject, pCutter, cutAlgorithm);
							// vector<CGameObject*> vRetVec = pDynamicShapeObject->DynamicShaping(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, /*������Ʈ�� ���̴� Ÿ��*/, fTimeElapsed, pCutter, cutAlgorithm);

							//if (false == vRetVec.empty()) { // ���ܿ� �����ߴٸ� �����͸� �غ��Ѵ�.
							//	newObjects.insert(newObjects.end(), vRetVec.begin(), vRetVec.end());
							//	deleteObjects.push_back(pDynamicShapeObject);
							//}
						}
					}
				}
			}
			for (const auto& a : deleteObjects) DelObj(a, ObjectLayer::Object);	// ���� ������Ʈ ����
			for (const auto& a : newObjects) AddObj(a, ObjectLayer::Object); // ���ܵ� ������Ʈ �߰�
		}
		//while (false == pvCutters->empty()) { pvCutters->pop_back(); } // �׻� CutterObjectLayer�� ����־�� �Ѵ�.

		while (false == pvCutters->empty()) { 
			pvCutters->back()->m_fLifeTime = 1.0f;
			AddObj(pvCutters->back(), ObjectLayer::TemporaryObject);
			pvCutters->pop_back(); 
		} // �׻� CutterObjectLayer�� ����־�� �Ѵ�.
	}
}

vector<CGameObject*> CObjectManager::_AtomicDynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	ShaderType stype, float fTimeElapsed, CGameObject* pTargetObject, CGameObject* pCutterObject, CDynamicShapeMesh::CutAlgorithm cutAlgorithm)
{
	int nTargerMeshes = pTargetObject->GetNumMeshes();
	CCutterMesh** ppTargetMeshes = (CCutterMesh**)pTargetObject->GetMeshes();
	XMFLOAT4X4 pxmfTargetMat = pTargetObject->GetWorldMat();

	int nCutterMeshes = pCutterObject->GetNumMeshes();
	CCutterMesh** ppCutterMeshes = (CCutterMesh**)pCutterObject->GetMeshes();
	XMFLOAT4X4 pxmfCutterMat = pCutterObject->GetWorldMat();


	// ���� �� ������ Mesh���� �ޱ����� ������ ����
	vector<CMesh*> newMeshs;
	XMFLOAT4X4 newWorldMat = Matrix4x4::Identity();
	XMFLOAT4X4 newTransformMat = Matrix4x4::Identity();

	if (pTargetObject->GetCuttable() && pCutterObject->GetAllowCutting()) {
		// �ٸ� ������Ʈ�� ���� ���� �߸� �� ���� ��

		XMFLOAT4X4 myWorldMat = pxmfTargetMat;
		XMFLOAT4X4 otherWorldMat = pCutterObject->GetWorldMat();

		for (int i = 0; i < nTargerMeshes; ++i) {
			for (int j = 0; j < nCutterMeshes; ++j) {
				if (CollisionCheck(ppTargetMeshes[i]->GetCollider(), myWorldMat, ppCutterMeshes[j]->GetCollider(), otherWorldMat)) {
					// �� ������Ʈ�� �浹�ϸ� DynamicShaping�� �õ��Ѵ�.
					vector<CMesh*> vRetVec = ppTargetMeshes[i]->DynamicShaping(pd3dDevice, pd3dCommandList, fTimeElapsed, pxmfTargetMat, ppCutterMeshes[j], pxmfCutterMat, cutAlgorithm);
					newMeshs.insert(newMeshs.end(), vRetVec.begin(), vRetVec.end());
				}
			}
		}
		if (false == newMeshs.empty()) {
			newWorldMat = pTargetObject->GetWorldMat();
			newTransformMat = pTargetObject->GetTransMat();
		}
	}

	// ���� ������ �Ͼ ���ο� Mesh�� ����ٸ� 2�� �̻��� ������Ʈ�� �����Ѵ�.
	// �ϳ��� Mesh �� �ϳ��� Object�� �ǵ��� ����. - ���߿� ������ ���� �ִ�
	vector<CGameObject*> newGameObjects;
	for (int i = 0; i < newMeshs.size(); ++i) {
		newGameObjects.push_back(new CGameObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype));

		newGameObjects[i]->SetMesh(0, newMeshs[i]);

		newGameObjects[i]->SetWorldMat(newWorldMat);
		newGameObjects[i]->SetTransMat(newTransformMat);
		//newGameObjects[i]->SetShaderType(ShaderType::CObjectsShader);
		newGameObjects[i]->SetCuttable(true);
	}

	return newGameObjects;
}

void CObjectManager::ReleaseObjects()
{
	for (std::vector<CGameObject*> a : m_pvObjectManager) {
		for (CGameObject* b : a)
			delete b;
		a.clear();
	}
}

void CObjectManager::ReleaseUploadBuffers()
{
	for (std::vector<CGameObject*> a : m_pvObjectManager)
		for (CGameObject* b : a)
			b->ReleaseUploadBuffers();
		
}

void CObjectManager::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CObjectManager::ReleaseShaderVariables()
{
}

void CObjectManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto& a : m_pvObjectManager) // ���� ��ҿ� ���� ������ ���
	{
		for (auto& b : a)
		{
			b->Render(pd3dCommandList, pCamera, false);
		}
	}

	// Render Collider
	if (m_pColliderShader) {
		// Shader Regist only one time
		m_pColliderShader->Render(pd3dCommandList, pCamera);
		for (auto& a : m_pvObjectManager)
		{
			for (auto& b : a)
			{
				b->RenderColliderMesh(pd3dCommandList, pCamera);
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Utility

bool CObjectManager::CollisionCheck_RayWithAABB(CRay* ray, CGameObject* obj, float& tmin, float& tmax)
{
	CAABBCollider* objCollider = obj->GetCollider();
	XMFLOAT4X4 objWorldMat = obj->GetWorldMat();

	if (objCollider) {

		XMFLOAT3 aabbMax = objCollider->GetAABBMaxPos();
		XMFLOAT3 aabbMin = objCollider->GetAABBMinPos();
		XMStoreFloat3(&aabbMax, XMVector3TransformCoord(XMLoadFloat3(&aabbMax), XMLoadFloat4x4(&objWorldMat)));
		XMStoreFloat3(&aabbMin, XMVector3TransformCoord(XMLoadFloat3(&aabbMin), XMLoadFloat4x4(&objWorldMat)));
		XMFLOAT3 ray_dir = ray->GetDir();
		XMFLOAT3 ray_origin = ray->GetOriginal();
		XMVECTOR invDirection = XMVectorReciprocal(XMLoadFloat3(&ray_dir));

		XMVECTOR t1 = (XMLoadFloat3(&aabbMin) - XMLoadFloat3(&ray_origin)) * invDirection;
		XMVECTOR t2 = (XMLoadFloat3(&aabbMax) - XMLoadFloat3(&ray_origin)) * invDirection;

		XMVECTOR tmin_vec = XMVectorMax(XMVectorMin(t1, t2), XMVectorZero());
		XMVECTOR tmax_vec = XMVectorMin(XMVectorMax(t1, t2), XMVectorReplicate(FLT_MAX));

		XMFLOAT3 tmin_array, tmax_array;
		XMStoreFloat3(&tmin_array, tmin_vec);
		XMStoreFloat3(&tmax_array, tmax_vec);

		float tmin_value = max(max(tmin_array.x, tmin_array.y), tmin_array.z);
		float tmax_value = min(min(tmax_array.x, tmax_array.y), tmax_array.z);

		// �������� ������ tmax < 0
		// tmin_value > tmax_value�� ������ AABB���� ������ ���ϱ� ���� ����
		bool result = tmax_value > 0 && tmin_value <= tmax_value;
		if (result) { // �浹�ߴٸ� �ּ���, �ִ��� ���� �Ѱ��ش�.
			tmin = tmin_value;
			tmax = tmax_value;

			return true;
		}
	}
	if (obj->GetChild())
		if (CollisionCheck_RayWithAABB(ray, obj->GetChild(), tmin, tmax))
			return true;
	if (obj->GetSibling())
		if (CollisionCheck_RayWithAABB(ray, obj->GetSibling(), tmin, tmax))
			return true;

	return false;
}

bool CObjectManager::CollisionCheck_RayWithOBB(CRay* ray, CGameObject* obj, float& tmin, float& tmax)
{
	COBBCollider* objCollider = obj->GetCollider();
	XMFLOAT4X4 objWorldMat = obj->GetWorldMat();

	// obj Collider Check
	if (objCollider) {
		if (__CollisionCheck_RayWithOBB_Recursive(ray, objCollider, &objWorldMat, tmin, tmax))
			return true;
	}

	// mesh Collider Check
	CMesh** objMeshs = obj->GetMeshes();
	for (int k = 0; k < obj->GetNumMeshes(); ++k) {
		objCollider = objMeshs[k]->GetCollider();
		if (__CollisionCheck_RayWithOBB_Recursive(ray, objCollider, &objWorldMat, tmin, tmax))
			return true;
	}
	
	// child Collider Check
	if (obj->GetChild())
		if (CollisionCheck_RayWithOBB(ray, obj->GetChild(), tmin, tmax))
			return true;

	// sibling Collider Check
	if (obj->GetSibling())
		if (CollisionCheck_RayWithOBB(ray, obj->GetSibling(), tmin, tmax))
			return true;

	return false;
}

bool CObjectManager::__CollisionCheck_RayWithOBB_Recursive(CRay* ray, COBBCollider* obb, XMFLOAT4X4* objWorldMat, float& tmin, float& tmax)
{
	bool isCollied = true;
	XMMATRIX obbWorldMat = XMLoadFloat4x4(objWorldMat);

	XMFLOAT3 obbCenter = obb->GetOBBCenter();
	XMFLOAT3 obbHalfExtents = obb->GetOBBHalfExtents();
	XMFLOAT3* obbOrientation = obb->GetOBBOrientation();

	XMVECTOR center = XMLoadFloat3(&obbCenter);
	XMVECTOR orientationX = XMLoadFloat3(&obbOrientation[0]);
	XMVECTOR orientationY = XMLoadFloat3(&obbOrientation[1]);
	XMVECTOR orientationZ = XMLoadFloat3(&obbOrientation[2]);

	// �߽������� ���� �ݰ浵 scale�� ������ �޾ƾ��Ѵ�.
	XMFLOAT3 xmf3originPos(0.f, 0.f, 0.f);
	XMVECTOR halfExtents = XMLoadFloat3(&obbHalfExtents);
	XMVECTOR oriVec = XMLoadFloat3(&xmf3originPos);
	oriVec = XMVector3TransformCoord(oriVec, obbWorldMat);
	halfExtents = oriVec - XMVector3TransformCoord(halfExtents, obbWorldMat);
	XMStoreFloat3(&obbHalfExtents, halfExtents);

	center = XMVector3TransformCoord(center, obbWorldMat);

	// �࿡ scale�� ����� �� ���� ª������ �浹������ �þ�� ���� ������� �浹������ �۾�����.
	// scale�� �� �ݴ�� ũ�Ⱑ �����ǹǷ� �࿡�� scale�� �������� ����.
	XMFLOAT4X4 nonScaleMat(*objWorldMat);
	nonScaleMat._11 = 1; nonScaleMat._22 = 1; nonScaleMat._33 = 1;
	XMMATRIX obbNonScaleMat = XMLoadFloat4x4(&nonScaleMat);

	orientationX = XMVector3TransformNormal(orientationX, obbNonScaleMat);
	orientationY = XMVector3TransformNormal(orientationY, obbNonScaleMat);
	orientationZ = XMVector3TransformNormal(orientationZ, obbNonScaleMat);

	XMFLOAT3 rayDir = ray->GetDir();
	XMFLOAT3 rayOrigin = ray->GetOriginal();
	XMVECTOR rayDirection = XMLoadFloat3(&rayDir);
	XMVECTOR rayOriginVec = XMLoadFloat3(&rayOrigin);

	XMVECTOR rayToCenter = center - rayOriginVec;
	float e, f, t1, t2;
	tmin = 0.0f;
	tmax = FLT_MAX;

	for (int i = 0; i < 3; i++) {
		XMVECTOR axis = (i == 0) ? orientationX : (i == 1) ? orientationY : orientationZ;
		float halfExtent = (i == 0) ? obbHalfExtents.x : (i == 1) ? obbHalfExtents.y : obbHalfExtents.z;
		e = XMVectorGetX(XMVector3Dot(axis, rayToCenter));
		f = XMVectorGetX(XMVector3Dot(axis, rayDirection));

		if (fabs(f) > 0.001f) {
			t1 = (e + halfExtent) / f;
			t2 = (e - halfExtent) / f;
			if (t1 > t2) std::swap(t1, t2);
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
			if (tmin > tmax) { isCollied = false; break; }
			if (tmax < 0) { isCollied = false; break; }
		}
		else if (-e - halfExtent > 0 || -e + halfExtent < 0) {
			isCollied = false; break;
		}
	}

	return isCollied;
}

void CObjectManager::ScreenBasedObjectMove(CGameObject* obj, CCamera* pCamera, float cxDelta, float cyDelta, float fSensitivity)
{
	XMFLOAT3 CameraRight = pCamera->GetRightVector();
	XMFLOAT3 CameraUp = pCamera->GetUpVector();

	XMFLOAT3 t0(cxDelta, cyDelta, 0.0f);
	XMFLOAT3 xmf3Delta = Vector3::Normalize(t0);

	XMFLOAT3 t1 = Vector3::ScalarProduct(CameraRight, xmf3Delta.x * fSensitivity);
	XMFLOAT3 t2 = Vector3::ScalarProduct(CameraUp, -xmf3Delta.y * fSensitivity);
	XMFLOAT3 moveVector = Vector3::Add(t1, t2);
	XMFLOAT3 oldPos = obj->GetPosition();
	XMFLOAT3 newPos = Vector3::Add(oldPos, moveVector);

	obj->SetPosition(newPos);
}
