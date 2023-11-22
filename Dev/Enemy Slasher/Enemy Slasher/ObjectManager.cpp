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
	std::vector<CGameObject*>* target = &(m_pvObjectManager[(int)layer]);
	target->erase(remove(target->begin(), target->end(), object), target->end());
}

void CObjectManager::DelObj(CGameObject* object)
{
	for (auto& target : m_pvObjectManager) {
		target.erase(remove(target.begin(), target.end(), object), target.end());
	}
}

void CObjectManager::AnimateObjects(float fTimeElapsed)
{
	for (std::vector<CGameObject*> a : m_pvObjectManager)
		for (CGameObject* b : a) {
			b->Animate(fTimeElapsed);
			//b->UpdateTransform(NULL);
		}
}

void CObjectManager::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	for (std::vector<CGameObject*> a : m_pvObjectManager)
		for (CGameObject* b : a) {
			if (CDynamicShapeObject* pDynamicShapeObject = dynamic_cast<CDynamicShapeObject*>(b)) {

			}
		}
	
	if (false == m_pvObjectManager[(int)ObjectLayer::CutterObject].empty()) { // 비어있지 않다면
		vector<CGameObject*>* pvCutters = &m_pvObjectManager[(int)ObjectLayer::CutterObject];
		if (false == m_pvObjectManager[(int)ObjectLayer::Object].empty()) { // 비어있지 않다면
			vector<CGameObject*> pObjects = m_pvObjectManager[(int)ObjectLayer::Object];

			vector<CGameObject*> newObjects;
			vector<CGameObject*> deleteObjects;
			CGameObject** ppDynamicShapedObjects = NULL;

			for (const auto& pCutter : *pvCutters) {
				for (const auto& pObject : pObjects) {

					// 이 오브젝트가 DynamicShapeObject라면 처리
					if (CDynamicShapeObject* pDynamicShapeObject = dynamic_cast<CDynamicShapeObject*>(pObject)) {
						if (pDynamicShapeObject->CollisionCheck(pCutter)) {
							ppDynamicShapedObjects = pDynamicShapeObject->DynamicShaping(pd3dDevice, pd3dCommandList, fTimeElapsed, pCutter);

							if (NULL != ppDynamicShapedObjects) { // 절단에 성공했다면 데이터를 준비한다.
								newObjects.push_back(ppDynamicShapedObjects[0]);
								newObjects.push_back(ppDynamicShapedObjects[1]);

								deleteObjects.push_back(pDynamicShapeObject);
							}
						}
					}
				}
			}
			for (const auto& a : deleteObjects) DelObj(a, ObjectLayer::Object);	// 원본 오브젝트 삭제
			for (const auto& a : newObjects) AddObj(a, ObjectLayer::Object); // 절단된 오브젝트 추가
		}
		while (false == pvCutters->empty()) { pvCutters->pop_back(); } // 항상 CutterObjectLayer는 비어있어야 한다.
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
