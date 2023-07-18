#include "stdafx.h"
#include "Shader.h"
#include "ShaderManager.h"

CShaderManager::CShaderManager()
{
}

CShaderManager::~CShaderManager()
{
}

void CShaderManager::BuildShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature)
{
	CShader* shader;

	// NON
	shader = new CShader();
	shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_vShaderManager.push_back(shader);

	// CObjectsShader
	shader = new CObjectsShader();
	shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_vShaderManager.push_back(shader);
}

void CShaderManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, ShaderType pShaderType)
{
	m_vShaderManager[(int)pShaderType]->Render(pd3dCommandList, pCamera);
}