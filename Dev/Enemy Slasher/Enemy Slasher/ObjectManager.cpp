#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "Player.h"
#include "ObjectManager.h"
#include "ShaderManager.h"

CObjectManager::CObjectManager()
{
}

CObjectManager::~CObjectManager()
{
}


void CObjectManager::AddObj(CGameObject* object, ObjectLayer layer)
{
	// NON이면 나중?에 해당 쉐이더 생성.
	// 쉐이더 리스트 확인해서 없으면생성.

	//쉐이더 매니저가 해야함. 씬에서 쉐이더메니저에게 물어봐야함.
	/*if (!m_ppShaderManager[(int)object->GetShaderType()]) {
		CShaderManager::쉐이더_만드는_함수((int)object->GetShaderType());
	}*/

	// 일단 쉐이더 매니저는 항상 모든 쉐이더를 가지고 있음

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

void CObjectManager::AnimateObjects(float fTimeElapsed)
{
	for (std::vector<CGameObject*> a : m_pvObjectManager)
		for (CGameObject* b : a) {
			b->Animate(fTimeElapsed);
			//b->UpdateTransform(NULL);
		}

	{
		// 수명이 있는 오브젝트 삭제 처리
		vector<CGameObject*> deleteObjects;
		for (CGameObject* b : m_pvObjectManager[(int)ObjectLayer::TemporaryObject]) {
			b->m_fLifeTime -= fTimeElapsed;
			if (b->m_fLifeTime < 0) {
				deleteObjects.push_back(b);
			}
		}
		for (const auto& a : deleteObjects) DelObj(a, ObjectLayer::TemporaryObject);	// 원본 오브젝트 삭제
	}
}

void CObjectManager::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, CDynamicShapeMesh::CutAlgorithm cutAlgorithm)
{
	for (std::vector<CGameObject*> a : m_pvObjectManager)
		for (CGameObject* b : a) {
			if (CDynamicShapeObject* pDynamicShapeObject = dynamic_cast<CDynamicShapeObject*>(b)) {

			}
		}
	
	if (false == m_pvObjectManager[(int)ObjectLayer::CutterObject].empty()) { // 비어있지 않다면
		vector<CGameObject*>* pvCutters = (&m_pvObjectManager[(int)ObjectLayer::CutterObject]);
		if (false == m_pvObjectManager[(int)ObjectLayer::Object].empty()) { // 비어있지 않다면
			vector<CGameObject*> pObjects = m_pvObjectManager[(int)ObjectLayer::Object];

			vector<CGameObject*> newObjects;
			vector<CGameObject*> deleteObjects;

			vector<CDynamicShapeObject*>* pvDSMCutters = (vector<CDynamicShapeObject*>*)(pvCutters);
			for (const auto& pCutter : *pvDSMCutters) {
				for (const auto& pObject : pObjects) {

					// 이 오브젝트가 DynamicShapeObject라면 처리
					if (CDynamicShapeObject* pDynamicShapeObject = dynamic_cast<CDynamicShapeObject*>(pObject)) {
						XMFLOAT4X4 objectWorldMat = pDynamicShapeObject->GetWorldMat();
						XMFLOAT4X4 cutterWorldMat = pCutter->GetWorldMat();
						if (CollisionCheck(pDynamicShapeObject->GetCollider(), objectWorldMat, pCutter->GetCollider(), cutterWorldMat)) {
							 vector<CGameObject*> vRetVec = pDynamicShapeObject->DynamicShaping(pd3dDevice, pd3dCommandList, fTimeElapsed, pCutter, cutAlgorithm);

							if (false == vRetVec.empty()) { // 절단에 성공했다면 데이터를 준비한다.
								newObjects.insert(newObjects.end(), vRetVec.begin(), vRetVec.end());
								deleteObjects.push_back(pDynamicShapeObject);
							}
						}
					}
				}
			}
			for (const auto& a : deleteObjects) DelObj(a, ObjectLayer::Object);	// 원본 오브젝트 삭제
			for (const auto& a : newObjects) AddObj(a, ObjectLayer::Object); // 절단된 오브젝트 추가
		}
		//while (false == pvCutters->empty()) { pvCutters->pop_back(); } // 항상 CutterObjectLayer는 비어있어야 한다.

		while (false == pvCutters->empty()) { 
			pvCutters->back()->m_fLifeTime = 1.0f;
			AddObj(pvCutters->back(), ObjectLayer::TemporaryObject);
			pvCutters->pop_back(); 
		} // 항상 CutterObjectLayer는 비어있어야 한다.
	}
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

void CObjectManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShaderManager* pShaderManager)
{
	for (std::vector<CGameObject*> a : m_pvObjectManager)
		for (CGameObject* b : a) {
			if (b->GetMaterial() && b->GetMaterial()->m_ShaderType != ShaderType::NON) {
				pShaderManager->Render(pd3dCommandList, pCamera, b->GetMaterial()->m_ShaderType);
				b->Render(pd3dCommandList, pCamera, true);
			}
		}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Utility

bool CObjectManager::CollisionCheck_RayWithAABB(CRay* ray, CInteractiveObject* obj, float& tmin, float& tmax)
{
	CAABBCollider* objCollider = obj->GetCollider();
	XMFLOAT4X4 objWorldMat = obj->GetWorldMat();

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

	// 교차하지 않으면 tmax < 0
	// tmin_value > tmax_value는 뒤집힌 AABB와의 교차를 피하기 위한 조건
	bool result = tmax_value > 0 && tmin_value <= tmax_value;
	if (result) { // 충돌했다면 최소점, 최대점 또한 넘겨준다.
		tmin = tmin_value;
		tmax = tmax_value;
	}

	return result;
}

void CObjectManager::ScreenBasedObjectMove(CInteractiveObject* obj, CCamera* pCamera, float cxDelta, float cyDelta, float fSensitivity)
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
