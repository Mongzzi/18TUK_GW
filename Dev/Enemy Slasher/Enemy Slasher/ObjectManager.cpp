#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "ObjectManager.h"

CObjectManager::CObjectManager()
{
	m_ppShaderManager = new CShader*[(int)ShaderType::Count];
}

CObjectManager::~CObjectManager()
{
	for (int i = 0;i < (int)ShaderType::Count;i++)
		if(m_ppShaderManager[i]) delete m_ppShaderManager[i];

	delete[] m_ppShaderManager;
}


void CObjectManager::AddObj(CGameObject* object, ObjectLayer layer)
{
	// NON이면 나중?에 해당 쉐이더 생성.
	// 쉐이더 리스트 확인해서 없으면생성.

	//쉐이더 매니저가 해야함. 씬에서 쉐이더메니저에게 물어봐야함.
	/*if (!m_ppShaderManager[(int)object->GetShaderType()]) {
		CShaderManager::쉐이더_만드는_함수((int)object->GetShaderType());
	}*/

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
