#include "stdafx.h"
#include "Shader.h"
#include "ShaderManager.h"

CShaderManager::CShaderManager()
{
}

CShaderManager::~CShaderManager()
{
	for (CShader* a : m_vShaderManager) delete a;
	m_vShaderManager.clear();
}

void CShaderManager::ReleaseShaderVariables()
{
	for (CShader* a : m_vShaderManager) a->ReleaseShaderVariables();
}

void CShaderManager::BuildShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature)
{
	CShader* shader;

	// NON
	m_vShaderManager.push_back(new CShader());

	m_vShaderManager.push_back(new CObjectsShader());
	m_vShaderManager.push_back(new CUIObjectsShader());
	m_vShaderManager.push_back(new CObjectsNormalShader());
	m_vShaderManager.push_back(new CTerrainShader());
	m_vShaderManager.push_back(new CTextShader());

	for (auto d : m_vShaderManager)
		d->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
}

void CShaderManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, ShaderType pShaderType)
{
	// �����ս��� ������ ���� ��� �������� ���̴� ����� ���� �ϰ� ���� �� ����
	m_vShaderManager[(int)pShaderType]->Render(pd3dCommandList, pCamera);
}