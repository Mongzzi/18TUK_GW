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


	/*if (layer == ObjectLayer::Enemy)
	{
		((CDynamicShapeMesh*)((((CGameObject*)object)->GetMeshes()[0])))->SetCuttable(true);
		(object)->SetCuttable(true);
	}*/

	//if (layer == ObjectLayer::ObjectPhysX && m_pPhysXManager != nullptr)
	//{
	//	physx::PxActor* actor = m_pPhysXManager->AddStaticCustomGeometry(object);
	//	//m_vPhysxPairs.emplace_back(object, actor);
	//}
	if (m_pPhysXManager != nullptr) {

		if ((layer == ObjectLayer::Player || layer == ObjectLayer::Enemy))
		{
			physx::PxActor* actor = m_pPhysXManager->AddCapshulDynamic(object);
		}

		else if ((layer == ObjectLayer::Map || layer == ObjectLayer::TextureObject || layer == ObjectLayer::InteractiveUIObject))
		{
			physx::PxActor* actor = m_pPhysXManager->AddStaticCustomGeometry(object);
		}

		else if ((layer == ObjectLayer::DestroyedObject))
		{
			physx::PxActor* actor = m_pPhysXManager->AddDynamicConvexCustomGeometry(object);
		}
	}

	m_pvObjectManager[(int)(layer)].push_back(object);
}

void CObjectManager::DelObj(CGameObject* object, ObjectLayer layer)
{
	if (object->m_pPhysXActor) {
		m_pPhysXManager->GetScene()->removeActor(*(object->m_pPhysXActor));
		object->m_pPhysXActor->release();
	}
	delete object;
	std::vector<CGameObject*>* target = &(m_pvObjectManager[(int)layer]);
	target->erase(remove(target->begin(), target->end(), object), target->end());
}

void CObjectManager::DelObj(CGameObject* object)
{
	if (object->m_pPhysXActor) {
		m_pPhysXManager->GetScene()->removeActor(*(object->m_pPhysXActor));
		object->m_pPhysXActor->release();
	}
	delete object;
	for (auto& target : m_pvObjectManager) {
		target.erase(remove(target.begin(), target.end(), object), target.end());
	}
}

void CObjectManager::ClearLayer(ObjectLayer layer)
{
	std::vector<CGameObject*>* target = &(m_pvObjectManager[(int)layer]);
	while (false == target->empty()) {
		CGameObject* object = target->back();
		if (object->m_pPhysXActor) {
			m_pPhysXManager->GetScene()->removeActor(*(object->m_pPhysXActor));
			object->m_pPhysXActor->release();
		}
		delete object;
		target->pop_back();
	}
}

void CObjectManager::AnimateObjects(float fTimeTotal, float fTimeElapsed)
{
	for (std::vector<CGameObject*> a : m_pvObjectManager)
		for (CGameObject* b : a) {
			b->Animate(fTimeTotal, fTimeElapsed);
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

	// PhysX �� ���� �������� ����
	if (m_pPhysXManager != nullptr) {
		m_pPhysXManager->stepPhysics(true, fTimeElapsed);
		physx::PxTransform transform;
		for (std::vector<CGameObject*> a : m_pvObjectManager)
			for (CGameObject* b : a) {
				if (b->m_PhysXActorType == PhysXActorType::Dynamic &&
					b->m_pPhysXActor != nullptr) {
					transform = static_cast<physx::PxRigidDynamic*>(b->m_pPhysXActor)->getGlobalPose();
					b->SetPosition(transform.p.x, transform.p.y - PHYSX_CAPSUL_HEIGHT, transform.p.z);
				}
			}
	}
}

void CObjectManager::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fTimeElapsed, CDynamicShapeMesh::CutAlgorithm cutAlgorithm)
{
	if (false == m_pvObjectManager[(int)ObjectLayer::CutterObject].empty()) { // ������� �ʴٸ�
		vector<CGameObject*>* pvCutters = (&m_pvObjectManager[(int)ObjectLayer::CutterObject]);

		XMFLOAT3 cutterpos;
		XMFLOAT3 cutterHalfExtents;
		for (const auto& pCutter : *pvCutters) {
			COBBCollider* pCutterOBB = pCutter->GetCollider();
			cutterpos = Vector3::TransformCoord(pCutterOBB->GetOBBCenter(), pCutter->GetWorldMat());
			cutterHalfExtents = pCutterOBB->GetOBBHalfExtents();
			physx::PxTransform cutterTransform(cutterpos.x, cutterpos.y, cutterpos.z);
			//physx::PxBoxGeometry cutterGeometry(physx::PxVec3(cutterHalfExtents.x, cutterHalfExtents.y, cutterHalfExtents.z));
			physx::PxSphereGeometry cutterGeometry(100.f);

			{ // physx debugging
				//physx::PxRigidStatic* staticActor = m_pPhysXManager->GetPhysics()->createRigidStatic(cutterTransform);
				//physx::PxShape* shape = m_pPhysXManager->GetPhysics()->createShape(cutterGeometry, *m_pPhysXManager->GetPhysics()->createMaterial(0.5f, 0.5f, 0.5f));
				//staticActor->attachShape(*shape);
				//shape->release();
				//m_pPhysXManager->GetScene()->addActor(*staticActor);
				//pCutter->m_pPhysXActor = staticActor;
			}

			vector<CGameObject*> newObjects;
			vector<CGameObject*> deleteObjects;

			for (const auto& pTarget : m_pvObjectManager[(int)ObjectLayer::Enemy]) {
				// �߸� �� �ִٸ�
				if (pTarget->GetCuttable() == true) {
					const physx::PxGeometryQueryFlags queryFlags = physx::PxGeometryQueryFlag::eDEFAULT;
					physx::PxOverlapThreadContext* threadContext = NULL;

					physx::PxRigidActor* pTargetActor = static_cast<physx::PxRigidActor*>(pTarget->m_pPhysXActor);

					vector<CMesh*> newMeshs;

					// ��� shape�� ���� overlap test (��� mesh�� ���ؼ��� ���� ȿ��)
					for (physx::PxU32 i = 0; i < pTarget->m_vpPhysXShape.size(); ++i) {
						const physx::PxTransform shapePose(physx::PxShapeExt::getGlobalPose(*pTarget->m_vpPhysXShape[i], *pTargetActor));
						const physx::PxGeometry& geom = pTarget->m_vpPhysXShape[i]->getGeometry();

						bool isOverlapping = physx::PxGeometryQuery::overlap(cutterGeometry, cutterTransform, geom, shapePose, queryFlags, threadContext);
						// �浹�ߴٸ�
						if (isOverlapping == true) {
							if (pTarget->GetMeshes() == nullptr)
							{
								auto pTargetChild = pTarget->GetChild();

								vector<CMesh*> vRetMeshs = static_cast<CDynamicShapeMesh*>(pTargetChild->GetMeshes()[i])->
									DynamicShaping(pd3dDevice, pd3dCommandList, fTimeElapsed, pTargetChild->GetWorldMat(),
										static_cast<CDynamicShapeMesh*>(pCutter->GetMeshes()[0]), pCutter->GetWorldMat(),
										cutAlgorithm);
								newMeshs.insert(newMeshs.end(), vRetMeshs.begin(), vRetMeshs.end());
							}
							else
							{
								vector<CMesh*> vRetMeshs = static_cast<CDynamicShapeMesh*>(pTarget->GetMeshes()[i])->
									DynamicShaping(pd3dDevice, pd3dCommandList, fTimeElapsed, pTarget->GetWorldMat(),
										static_cast<CDynamicShapeMesh*>(pCutter->GetMeshes()[0]), pCutter->GetWorldMat(),
										cutAlgorithm);
								newMeshs.insert(newMeshs.end(), vRetMeshs.begin(), vRetMeshs.end());
							}
						}
					}

					if (newMeshs.empty() == false) {
						deleteObjects.push_back(pTarget);

						for (auto& meshData : newMeshs) { // �ϳ��� ������Ʈ�� �ϳ��� �޽� �Ҵ�
							CGameObject* newObj = new CGameObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, ShaderType::CTextureShader);

							// Material Set
							CMaterial* currMaterial = newObj->GetMaterial();
							CMaterial* oriMaterial;
							oriMaterial = pTarget->GetMaterial();
							if (pTarget->GetChild())
								oriMaterial = pTarget->GetChild()->GetMaterial();
							currMaterial->m_xmf4Albedo = oriMaterial->m_xmf4Albedo;
							if (oriMaterial->m_pTexture) {
								currMaterial->m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
								currMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, oriMaterial->m_pTexture, 0, 4);
								currMaterial->SetTexture(oriMaterial->m_pTexture);
							}

							newObj->SetWorldMat(pTarget->GetWorldMat());
							newObj->SetMesh(0, meshData, true);
							newObjects.push_back(newObj);
						}
					}
				}
			}

			for (const auto& a : deleteObjects) DelObj(a);	// ���� ������Ʈ ����
			for (const auto& a : newObjects) AddObj(a, ObjectLayer::DestroyedObject); // ���ܵ� ������Ʈ �߰�
		}

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
