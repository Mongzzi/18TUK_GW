#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "ObjectManager.h"

void CObjectManager::AddObj(CGameObject* object, ObjectLayer layer)
{
	m_pvObjectManager[int(layer)].push_back(object);
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
	
}

void CObjectManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (std::vector<CGameObject*> a : m_pvObjectManager)
		for (CGameObject* b : a)
			b->Render(pd3dCommandList, pCamera);
}
