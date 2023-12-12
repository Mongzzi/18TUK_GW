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
	m_vShaderManager.push_back(new CTextureShader());

	for (auto d : m_vShaderManager)
		d->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
}

void CShaderManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, ShaderType pShaderType)
{
	// 퍼포먼스적 문제가 있을 경우 쓸데없는 쉐이더 등록을 많이 하고 있을 수 있음
	m_vShaderManager[(int)pShaderType]->Render(pd3dCommandList, pCamera);
}

void CShaderManager::CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews, ShaderType pShaderType)
{
	m_vShaderManager[(int)pShaderType]->CreateCbvSrvDescriptorHeaps(pd3dDevice, nConstantBufferViews, nShaderResourceViews);
}

void CShaderManager::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex, ShaderType pShaderType)
{
	m_vShaderManager[(int)pShaderType]->CreateShaderResourceViews(pd3dDevice,  pTexture,  nDescriptorHeapIndex,  nRootParameterStartIndex);
}

void CShaderManager::CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex, ShaderType pShaderType)
{
	m_vShaderManager[(int)pShaderType]->CreateShaderResourceView(pd3dDevice, pTexture, nIndex);
}

void CShaderManager::CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride, ShaderType pShaderType)
{
	m_vShaderManager[(int)pShaderType]->CreateConstantBufferViews(pd3dDevice, nConstantBufferViews, pd3dConstantBuffers, nStride);
}

