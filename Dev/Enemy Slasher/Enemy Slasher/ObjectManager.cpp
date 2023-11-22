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
	
	if (false == m_pvObjectManager[(int)ObjectLayer::CutterObject].empty()) { // ������� �ʴٸ�
		vector<CGameObject*>* pvCutters = &m_pvObjectManager[(int)ObjectLayer::CutterObject];
		if (false == m_pvObjectManager[(int)ObjectLayer::Object].empty()) { // ������� �ʴٸ�
			vector<CGameObject*> pObjects = m_pvObjectManager[(int)ObjectLayer::Object];

			vector<CGameObject*> newObjects;
			vector<CGameObject*> deleteObjects;
			CGameObject** ppDynamicShapedObjects = NULL;

			for (const auto& pCutter : *pvCutters) {
				for (const auto& pObject : pObjects) {

					// �� ������Ʈ�� DynamicShapeObject��� ó��
					if (CDynamicShapeObject* pDynamicShapeObject = dynamic_cast<CDynamicShapeObject*>(pObject)) {
						if (pDynamicShapeObject->CollisionCheck(pCutter)) {
							ppDynamicShapedObjects = pDynamicShapeObject->DynamicShaping(pd3dDevice, pd3dCommandList, fTimeElapsed, pCutter);

							if (NULL != ppDynamicShapedObjects) { // ���ܿ� �����ߴٸ� �����͸� �غ��Ѵ�.
								newObjects.push_back(ppDynamicShapedObjects[0]);
								newObjects.push_back(ppDynamicShapedObjects[1]);

								deleteObjects.push_back(pDynamicShapeObject);
							}
						}
					}
				}
			}
			for (const auto& a : deleteObjects) DelObj(a, ObjectLayer::Object);	// ���� ������Ʈ ����
			for (const auto& a : newObjects) AddObj(a, ObjectLayer::Object); // ���ܵ� ������Ʈ �߰�
		}
		while (false == pvCutters->empty()) { pvCutters->pop_back(); } // �׻� CutterObjectLayer�� ����־�� �Ѵ�.
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
