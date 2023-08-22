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
		for (CGameObject* b : a)
			b->Animate(fTimeElapsed);
	
	if (!m_pvObjectManager[(int)ObjectLayer::Terrain].empty() && !m_pvObjectManager[(int)ObjectLayer::Player].empty()) { // Terrain과 Player가 있다면
		CPlayer* pPlayer = (CPlayer*)m_pvObjectManager[(int)ObjectLayer::Player][0];
		XMFLOAT3 xmfPlayerPos = pPlayer->GetPosition();
		float fHeight = ((CHeightMapTerrain*)m_pvObjectManager[(int)ObjectLayer::Terrain][0])->GetHeight(xmfPlayerPos.x, xmfPlayerPos.z);

		if (xmfPlayerPos.y < fHeight) {
			xmfPlayerPos.y = fHeight;
			pPlayer->SetPosition(xmfPlayerPos);
			XMFLOAT3 xmfVelocity = pPlayer->GetVelocity();
			xmfVelocity.y = 0.0f;
			pPlayer->SetVelocity(xmfVelocity);
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

void CObjectManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, CShaderManager* pShaderManager)
{
	for (std::vector<CGameObject*> a : m_pvObjectManager)
		for (CGameObject* b : a) {
			pShaderManager->Render(pd3dCommandList, pCamera, b->GetShaderType());
			b->Render(pd3dCommandList, pCamera);
		}
}
