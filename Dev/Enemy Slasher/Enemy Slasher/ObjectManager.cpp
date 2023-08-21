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
	// NON�̸� ����?�� �ش� ���̴� ����.
	// ���̴� ����Ʈ Ȯ���ؼ� ���������.

	//���̴� �Ŵ����� �ؾ���. ������ ���̴��޴������� ���������.
	/*if (!m_ppShaderManager[(int)object->GetShaderType()]) {
		CShaderManager::���̴�_�����_�Լ�((int)object->GetShaderType());
	}*/

	// �ϴ� ���̴� �Ŵ����� �׻� ��� ���̴��� ������ ����

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
	
	if (!m_pvObjectManager[(int)ObjectLayer::Terrain].empty() && !m_pvObjectManager[(int)ObjectLayer::Player].empty()) { // Terrain�� Player�� �ִٸ�
		CPlayer* pPlayer = (CPlayer*)m_pvObjectManager[(int)ObjectLayer::Player][0];
		XMFLOAT3 xmfPlayerPos = pPlayer->GetPosition();
		float fHeight = ((CHeightMapTerrain*)m_pvObjectManager[(int)ObjectLayer::Terrain][0])->GetHeight(xmfPlayerPos.x, xmfPlayerPos.z);

		if (xmfPlayerPos.y < fHeight) {
			xmfPlayerPos.y = fHeight;
			pPlayer->SetPosition(xmfPlayerPos);
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
