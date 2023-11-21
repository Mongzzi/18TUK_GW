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
	std::vector<CGameObject*> target = m_pvObjectManager[(int)layer];
	target.erase(remove(target.begin(), target.end(), object), target.end());
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

	if (!m_pvObjectManager[(int)ObjectLayer::Object].empty()) {
		vector<CGameObject*> pObjects = m_pvObjectManager[(int)ObjectLayer::Object];
		if (1 < pObjects.size()) { // 2개 이상의 오브젝트가 있다면 충돌 후 다이나믹 쉐이핑을 진행한다.

			// 두 오브젝트의 충돌처리를 위한 이중 for문
			for (int i = 0; i < (pObjects.size() - 1); ++i) {
				for (int j = i + 1; j < (pObjects.size()); ++j) {

					// 이 오브젝트가 DynamicShapeObject라면 처리
					if (CDynamicShapeObject* pDynamicShapeObject = dynamic_cast<CDynamicShapeObject*>(pObjects[i])) {
						if (pDynamicShapeObject->CollisionCheck(pObjects[j])) {
							pDynamicShapeObject->DynamicShaping(pd3dDevice, pd3dCommandList, fTimeElapsed, pObjects[j]);
						}
					}

				}
			}
		}
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
