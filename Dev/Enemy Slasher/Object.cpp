
//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Object.h"
#include "Shader.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers, int nRootParameters)
{
	m_nTextureType = nTextureType;

	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_ppd3dTextureUploadBuffers = new ID3D12Resource * [m_nTextures];
		m_ppd3dTextures = new ID3D12Resource * [m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_ppd3dTextureUploadBuffers[i] = m_ppd3dTextures[i] = NULL;

		m_ppstrTextureNames = new _TCHAR[m_nTextures][64];
		for (int i = 0; i < m_nTextures; i++) m_ppstrTextureNames[i][0] = '\0';

		m_pd3dSrvGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pd3dSrvGpuDescriptorHandles[i].ptr = NULL;

		m_pnResourceTypes = new UINT[m_nTextures];
		m_pdxgiBufferFormats = new DXGI_FORMAT[m_nTextures];
		m_pnBufferElements = new int[m_nTextures];
	}
	m_nRootParameters = nRootParameters;
	if (nRootParameters > 0) m_pnRootParameterIndices = new int[nRootParameters];
	for (int i = 0; i < m_nRootParameters; i++) m_pnRootParameterIndices[i] = -1;
	
	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
		delete[] m_ppd3dTextures;
	}

	if (m_ppstrTextureNames) delete[] m_ppstrTextureNames;

	if (m_pnResourceTypes) delete[] m_pnResourceTypes;
	if (m_pdxgiBufferFormats) delete[] m_pdxgiBufferFormats;
	if (m_pnBufferElements) delete[] m_pnBufferElements;

	if (m_pnRootParameterIndices) delete[] m_pnRootParameterIndices;
	if (m_pd3dSrvGpuDescriptorHandles) delete[] m_pd3dSrvGpuDescriptorHandles;

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootParameterIndex(int nIndex, UINT nRootParameterIndex)
{
	m_pnRootParameterIndices[nIndex] = nRootParameterIndex;
}

void CTexture::SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_nRootParameters == m_nTextures)
	{
		for (int i = 0; i < m_nRootParameters; i++)
		{
			if (m_pd3dSrvGpuDescriptorHandles[i].ptr && (m_pnRootParameterIndices[i] != -1)) pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[i], m_pd3dSrvGpuDescriptorHandles[i]);
		}
	}
	else
	{
		if (m_pd3dSrvGpuDescriptorHandles[0].ptr) pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[0], m_pd3dSrvGpuDescriptorHandles[0]);
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[nParameterIndex], m_pd3dSrvGpuDescriptorHandles[nTextureIndex]);
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void CTexture::LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ/*D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/);
}

void CTexture::LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = RESOURCE_BUFFER;
	m_pdxgiBufferFormats[nIndex] = ndxgiFormat;
	m_pnBufferElements[nIndex] = nElements;
	m_ppd3dTextures[nIndex] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pData, nElements * nStride, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, &m_ppd3dTextureUploadBuffers[nIndex]);
}

ID3D12Resource* CTexture::CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nIndex, UINT nResourceType, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTexture2DResource(pd3dDevice, pd3dCommandList, nWidth, nHeight, nElements, nMipLevels, dxgiFormat, d3dResourceFlags, d3dResourceStates, pd3dClearValue);
	return(m_ppd3dTextures[nIndex]);
}

int CTexture::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader, UINT nIndex)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = 64;
	UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrTextureName, sizeof(char), nStrLength, pInFile);
	pstrTextureName[nStrLength] = '\0';

	bool bDuplicated = false;
	bool bLoaded = false;
	if (strcmp(pstrTextureName, "null"))
	{
		bLoaded = true;
		char pstrFilePath[64] = { '\0' };
		strcpy_s(pstrFilePath, 64, "Model/Textures/");

		bDuplicated = (pstrTextureName[0] == '@');
		strcpy_s(pstrFilePath + 15, 64 - 15, (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcpy_s(pstrFilePath + 15 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 15 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

		size_t nConverted = 0;
		mbstowcs_s(&nConverted, m_ppstrTextureNames[nIndex], 64, pstrFilePath, _TRUNCATE);

#define _WITH_DISPLAY_TEXTURE_NAME

#ifdef _WITH_DISPLAY_TEXTURE_NAME
		static int nTextures = 0, nRepeatedTextures = 0;
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("Texture Name: %d %c %s\n"), (pstrTextureName[0] == '@') ? nRepeatedTextures++ : nTextures++, (pstrTextureName[0] == '@') ? '@' : ' ', m_ppstrTextureNames[nIndex]);
		OutputDebugString(pstrDebug);
#endif
		if (!bDuplicated)
		{
			LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, m_ppstrTextureNames[nIndex], RESOURCE_TEXTURE2D, nIndex);
			pShader->CreateShaderResourceView(pd3dDevice, this, nIndex);
#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS
			m_pnRootParameterIndices[nIndex] = PARAMETER_STANDARD_TEXTURE + nIndex;
#endif
		}
		else
		{
			if (pParent)
			{
				CGameObject* pRootGameObject = pParent;
				while (pRootGameObject)
				{
					if (!pRootGameObject->m_pParent) break;
					pRootGameObject = pRootGameObject->m_pParent;
				}
				D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle;
				int nParameterIndex = pRootGameObject->FindReplicatedTexture(m_ppstrTextureNames[nIndex], &d3dSrvGpuDescriptorHandle);
				if (nParameterIndex >= 0)
				{
					m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
					m_pnRootParameterIndices[nIndex] = nParameterIndex;
				}
			}
		}
	}
	return(bLoaded);
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc(int nIndex)
{
	ID3D12Resource* pShaderResource = GetResource(nIndex);
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	int nTextureType = GetTextureType(nIndex);
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: //[]
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = 1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.Format = m_pdxgiBufferFormats[nIndex];
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = m_pnBufferElements[nIndex];
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial()
{
}

CMaterial::~CMaterial()
{
	if (m_pTexture) m_pTexture->Release();
	if (m_pShader) m_pShader->Release();
}

void CMaterial::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::SetTexture(CTexture* pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pShader) m_pShader->ReleaseUploadBuffers();
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4AmbientColor, 16);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4AlbedoColor, 20);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4SpecularColor, 24);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4EmissiveColor, 28);

	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, &m_nType, 32);

	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader) m_pShader->ReleaseShaderVariables();
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject()
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();

	m_xmf4x4AABBTransform = Matrix4x4::Identity();
	m_xmf4x4AABBWorld = Matrix4x4::Identity();
}

CGameObject::CGameObject(int nMeshes, int nMaterials) : CGameObject()
{
	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh * [m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	}

	m_nMaterials = nMaterials;
	if (m_nMaterials > 0)
	{
		m_ppMaterials = new CMaterial * [m_nMaterials];
		for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	ReleaseShaderVariables();

	if (m_pd3dAABBVertexBuffer) m_pd3dAABBVertexBuffer->Release();

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i]) m_ppMaterials[i]->Release();
		}
	}
	if (m_ppMaterials) delete[] m_ppMaterials;
}

void CGameObject::AddRef() 
{ 
	m_nReferences++; 

	if (m_pSibling) m_pSibling->AddRef();
	if (m_pChild) m_pChild->AddRef();
}

void CGameObject::Release() 
{ 
	if (m_pSibling) m_pSibling->Release();
	if (m_pChild) m_pChild->Release();

	if (--m_nReferences <= 0) delete this; 
}

void CGameObject::SetChild(CGameObject *pChild)
{
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
	if (pChild)
	{
		pChild->m_pParent = this;
	}
}

void CGameObject::CreateAABB(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// m_xmf3AABBExtents 와 m_xmf3AABBCenter 계산
	if (m_nMeshes == 0) return;

	// 첫번째 메쉬의 AABB를 초기값으로 설정
	m_xmf3AABBCenter = m_ppMeshes[0]->GetAABBCenter();
	m_xmf3AABBExtents = m_ppMeshes[0]->GetAABBExtents();

	// 모든 메쉬의 AABB를 포함하는 AABB 계산
	for (int i = 1; i < m_nMeshes; ++i) {
		XMFLOAT3 meshCenter = m_ppMeshes[i]->GetAABBCenter();
		XMFLOAT3 meshExtents = m_ppMeshes[i]->GetAABBExtents();

		XMFLOAT3 maxBound = XMFLOAT3(m_xmf3AABBCenter.x + m_xmf3AABBExtents.x, m_xmf3AABBCenter.y + m_xmf3AABBExtents.y, m_xmf3AABBCenter.z + m_xmf3AABBExtents.z);
		XMFLOAT3 minBound = XMFLOAT3(m_xmf3AABBCenter.x - m_xmf3AABBExtents.x, m_xmf3AABBCenter.y - m_xmf3AABBExtents.y, m_xmf3AABBCenter.z - m_xmf3AABBExtents.z);

		XMFLOAT3 meshMaxBound = XMFLOAT3(meshCenter.x + meshExtents.x, meshCenter.y + meshExtents.y, meshCenter.z + meshExtents.z);
		XMFLOAT3 meshMinBound = XMFLOAT3(meshCenter.x - meshExtents.x, meshCenter.y - meshExtents.y, meshCenter.z - meshExtents.z);

		if (meshMaxBound.x > maxBound.x) maxBound.x = meshMaxBound.x;
		if (meshMaxBound.y > maxBound.y) maxBound.y = meshMaxBound.y;
		if (meshMaxBound.z > maxBound.z) maxBound.z = meshMaxBound.z;
		if (meshMinBound.x < minBound.x) minBound.x = meshMinBound.x;
		if (meshMinBound.y < minBound.y) minBound.y = meshMinBound.y;
		if (meshMinBound.z < minBound.z) minBound.z = meshMinBound.z;

		m_xmf3AABBCenter.x = (maxBound.x + minBound.x) * 0.5f;
		m_xmf3AABBCenter.y = (maxBound.y + minBound.y) * 0.5f;
		m_xmf3AABBCenter.z = (maxBound.z + minBound.z) * 0.5f;

		m_xmf3AABBExtents.x = (maxBound.x - minBound.x) * 0.5f;
		m_xmf3AABBExtents.y = (maxBound.y - minBound.y) * 0.5f;
		m_xmf3AABBExtents.z = (maxBound.z - minBound.z) * 0.5f;
	}

	if (m_xmf3AABBExtents.x != 0.0f || m_xmf3AABBExtents.y != 0.0f || m_xmf3AABBExtents.z != 0.0f) {
		// AABB의 8개 꼭지점 계산
		XMFLOAT3* vertices = new XMFLOAT3[8];
		vertices[0] = XMFLOAT3(m_xmf3AABBCenter.x - m_xmf3AABBExtents.x, m_xmf3AABBCenter.y - m_xmf3AABBExtents.y, m_xmf3AABBCenter.z - m_xmf3AABBExtents.z);
		vertices[1] = XMFLOAT3(m_xmf3AABBCenter.x - m_xmf3AABBExtents.x, m_xmf3AABBCenter.y - m_xmf3AABBExtents.y, m_xmf3AABBCenter.z + m_xmf3AABBExtents.z);
		vertices[2] = XMFLOAT3(m_xmf3AABBCenter.x - m_xmf3AABBExtents.x, m_xmf3AABBCenter.y + m_xmf3AABBExtents.y, m_xmf3AABBCenter.z - m_xmf3AABBExtents.z);
		vertices[3] = XMFLOAT3(m_xmf3AABBCenter.x - m_xmf3AABBExtents.x, m_xmf3AABBCenter.y + m_xmf3AABBExtents.y, m_xmf3AABBCenter.z + m_xmf3AABBExtents.z);
		vertices[4] = XMFLOAT3(m_xmf3AABBCenter.x + m_xmf3AABBExtents.x, m_xmf3AABBCenter.y - m_xmf3AABBExtents.y, m_xmf3AABBCenter.z - m_xmf3AABBExtents.z);
		vertices[5] = XMFLOAT3(m_xmf3AABBCenter.x + m_xmf3AABBExtents.x, m_xmf3AABBCenter.y - m_xmf3AABBExtents.y, m_xmf3AABBCenter.z + m_xmf3AABBExtents.z);
		vertices[6] = XMFLOAT3(m_xmf3AABBCenter.x + m_xmf3AABBExtents.x, m_xmf3AABBCenter.y + m_xmf3AABBExtents.y, m_xmf3AABBCenter.z - m_xmf3AABBExtents.z);
		vertices[7] = XMFLOAT3(m_xmf3AABBCenter.x + m_xmf3AABBExtents.x, m_xmf3AABBCenter.y + m_xmf3AABBExtents.y, m_xmf3AABBCenter.z + m_xmf3AABBExtents.z);

		// AABB를 구성하는 선들을 정의
		UINT indices[] = {
			0, 1,
			0, 2,
			0, 4,
			1, 3,
			1, 5,
			2, 3,
			2, 6,
			3, 7,
			4, 5,
			4, 6,
			5, 7,
			6, 7
		};
		m_nAABBVertices = 24;
		XMFLOAT3* lines = new XMFLOAT3[m_nAABBVertices];
		for (int i = 0; i < m_nAABBVertices; i++) {
			lines[i] = vertices[indices[i]];
		}

		m_pxmf3AABBVertices = lines;

		m_pd3dAABBVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3AABBVertices, sizeof(XMFLOAT3) * m_nAABBVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dAABBVertexUploadBuffer);

		m_d3dAABBVertexBufferView.BufferLocation = m_pd3dAABBVertexBuffer->GetGPUVirtualAddress();
		m_d3dAABBVertexBufferView.StrideInBytes = sizeof(XMFLOAT3);
		m_d3dAABBVertexBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nAABBVertices;

		m_bHasAABB = true;
	}
}

void CGameObject::CreateMainAABB(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	CalculateMainAABB(&m_xmf3AABBCenter, &m_xmf3AABBExtents);

	if (m_xmf3AABBExtents.x != 0.0f || m_xmf3AABBExtents.y != 0.0f || m_xmf3AABBExtents.z != 0.0f) {
		// AABB의 8개 꼭지점 계산
		XMFLOAT3* vertices = new XMFLOAT3[8];
		vertices[0] = XMFLOAT3(m_xmf3AABBCenter.x - m_xmf3AABBExtents.x, m_xmf3AABBCenter.y - m_xmf3AABBExtents.y, m_xmf3AABBCenter.z - m_xmf3AABBExtents.z);
		vertices[1] = XMFLOAT3(m_xmf3AABBCenter.x - m_xmf3AABBExtents.x, m_xmf3AABBCenter.y - m_xmf3AABBExtents.y, m_xmf3AABBCenter.z + m_xmf3AABBExtents.z);
		vertices[2] = XMFLOAT3(m_xmf3AABBCenter.x - m_xmf3AABBExtents.x, m_xmf3AABBCenter.y + m_xmf3AABBExtents.y, m_xmf3AABBCenter.z - m_xmf3AABBExtents.z);
		vertices[3] = XMFLOAT3(m_xmf3AABBCenter.x - m_xmf3AABBExtents.x, m_xmf3AABBCenter.y + m_xmf3AABBExtents.y, m_xmf3AABBCenter.z + m_xmf3AABBExtents.z);
		vertices[4] = XMFLOAT3(m_xmf3AABBCenter.x + m_xmf3AABBExtents.x, m_xmf3AABBCenter.y - m_xmf3AABBExtents.y, m_xmf3AABBCenter.z - m_xmf3AABBExtents.z);
		vertices[5] = XMFLOAT3(m_xmf3AABBCenter.x + m_xmf3AABBExtents.x, m_xmf3AABBCenter.y - m_xmf3AABBExtents.y, m_xmf3AABBCenter.z + m_xmf3AABBExtents.z);
		vertices[6] = XMFLOAT3(m_xmf3AABBCenter.x + m_xmf3AABBExtents.x, m_xmf3AABBCenter.y + m_xmf3AABBExtents.y, m_xmf3AABBCenter.z - m_xmf3AABBExtents.z);
		vertices[7] = XMFLOAT3(m_xmf3AABBCenter.x + m_xmf3AABBExtents.x, m_xmf3AABBCenter.y + m_xmf3AABBExtents.y, m_xmf3AABBCenter.z + m_xmf3AABBExtents.z);

		// AABB를 구성하는 선들을 정의
		UINT indices[] = {
			0, 1,
			0, 2,
			0, 4,
			1, 3,
			1, 5,
			2, 3,
			2, 6,
			3, 7,
			4, 5,
			4, 6,
			5, 7,
			6, 7
		};
		m_nAABBVertices = 24;
		XMFLOAT3* lines = new XMFLOAT3[m_nAABBVertices];
		for (int i = 0; i < m_nAABBVertices; i++) {
			lines[i] = vertices[indices[i]];
		}

		m_pxmf3AABBVertices = lines;

		m_pd3dAABBVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3AABBVertices, sizeof(XMFLOAT3) * m_nAABBVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dAABBVertexUploadBuffer);

		m_d3dAABBVertexBufferView.BufferLocation = m_pd3dAABBVertexBuffer->GetGPUVirtualAddress();
		m_d3dAABBVertexBufferView.StrideInBytes = sizeof(XMFLOAT3);
		m_d3dAABBVertexBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nAABBVertices;

		m_bHasAABB = true;
	}
}

void CGameObject::CalculateMainAABB(XMFLOAT3* AABBCenter, XMFLOAT3* AABBExtents)
{
	if (m_bHasAABB) // AABB를 가지고 있다면 Main AABB 계산에 합산
	{
		// 현재 GameObject의 AABB 정보를 부모 GameObject의 AABB에 합산
		XMFLOAT3 maxBound = XMFLOAT3(m_xmf3AABBCenter.x + m_xmf3AABBExtents.x, m_xmf3AABBCenter.y + m_xmf3AABBExtents.y, m_xmf3AABBCenter.z + m_xmf3AABBExtents.z);
		XMFLOAT3 minBound = XMFLOAT3(m_xmf3AABBCenter.x - m_xmf3AABBExtents.x, m_xmf3AABBCenter.y - m_xmf3AABBExtents.y, m_xmf3AABBCenter.z - m_xmf3AABBExtents.z);

		if (maxBound.x > AABBCenter->x + AABBExtents->x) AABBExtents->x = maxBound.x - AABBCenter->x;
		if (maxBound.y > AABBCenter->y + AABBExtents->y) AABBExtents->y = maxBound.y - AABBCenter->y;
		if (maxBound.z > AABBCenter->z + AABBExtents->z) AABBExtents->z = maxBound.z - AABBCenter->z;
		if (minBound.x < AABBCenter->x - AABBExtents->x) AABBExtents->x = AABBCenter->x - minBound.x;
		if (minBound.y < AABBCenter->y - AABBExtents->y) AABBExtents->y = AABBCenter->y - minBound.y;
		if (minBound.z < AABBCenter->z - AABBExtents->z) AABBExtents->z = AABBCenter->z - minBound.z;

		AABBCenter->x = (maxBound.x + minBound.x) * 0.5f;
		AABBCenter->y = (maxBound.y + minBound.y) * 0.5f;
		AABBCenter->z = (maxBound.z + minBound.z) * 0.5f;
	}

	if (m_pSibling) m_pSibling->CalculateMainAABB(AABBCenter, AABBExtents);
	if (m_pChild) m_pChild->CalculateMainAABB(AABBCenter, AABBExtents);
}

void CGameObject::UpdateAABBScaleByRotation(XMMATRIX mtxRotate)
{
	// 오브젝트의 회전이 있을 때 AABB를 Sclae하는 함수
	Matrix4x4::Multiply(mtxRotate, m_xmf4x4AABBTransform);
	UpdateAABBTransform(NULL);

	// 기존 AABB의 World에서의 중심과 크기를 구한다.
	XMFLOAT3 center, extents;
	XMStoreFloat3(&center, XMVector3Transform(XMLoadFloat3(&m_xmf3AABBCenter), XMLoadFloat4x4(&m_xmf4x4AABBWorld)));
	XMStoreFloat3(&extents, XMVector3Transform(XMLoadFloat3(&m_xmf3AABBExtents), XMLoadFloat4x4(&m_xmf4x4AABBWorld)));


	// 회전 변환 행렬의 각 축 벡터를 추출한다.
	// obb Code
	//XMFLOAT3 xAxis, yAxis, zAxis;
	//xAxis = XMFLOAT3(mtxRotate.r[0].m128_f32[0], mtxRotate.r[0].m128_f32[1], mtxRotate.r[0].m128_f32[2]);
	//yAxis = XMFLOAT3(mtxRotate.r[1].m128_f32[0], mtxRotate.r[1].m128_f32[1], mtxRotate.r[1].m128_f32[2]);
	//zAxis = XMFLOAT3(mtxRotate.r[2].m128_f32[0], mtxRotate.r[2].m128_f32[1], mtxRotate.r[2].m128_f32[2]);

	// 회전 변환 후의 AABB를 포함하는 최소 크기의 AABB를 계산한다.
	//float xExtent = (fabsf(extents.x * xAxis.x) + fabsf(extents.y * yAxis.x) + fabsf(extents.z * zAxis.x));
	//float yExtent = (fabsf(extents.x * xAxis.y) + fabsf(extents.y * yAxis.y) + fabsf(extents.z * zAxis.y));
	//float zExtent = (fabsf(extents.x * xAxis.z) + fabsf(extents.y * yAxis.z) + fabsf(extents.z * zAxis.z));

	// 새로 계산한 AABB의 중심과 크기를 저장한다.
	//m_xmf3AABBCenter = center;
	//m_xmf3AABBExtents = XMFLOAT3(xExtent, yExtent, zExtent);


	//if (m_pSibling) m_pSibling->UpdateAABBScaleByRotation(pxmf4x4AABBParent);
	//if (m_pChild) m_pChild->UpdateAABBScaleByRotation(&m_xmf4x4AABBWorld);
}

void CGameObject::SetMesh(int nIndex, CMesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}

void CGameObject::SetShader(int nMaterial, CShader *pShader)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->SetShader(pShader);
}

void CGameObject::SetMaterial(int nMaterial, CMaterial *pMaterial)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();
}

void CGameObject::Animate(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent)
{
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed, pxmf4x4Parent);
	if (m_pChild) m_pChild->Animate(fTimeElapsed, &m_xmf4x4World);
}

CGameObject *CGameObject::FindFrame(char *pstrFrameName)
{
	CGameObject *pFrameObject = NULL;
	if (!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_nMaterials > 1)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i])
			{
				if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
				m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
			}

			if (m_nMeshes == 1)
			{
				if (m_ppMeshes[0]) m_ppMeshes[0]->Render(pd3dCommandList, i);
			}
		}
	}
	else
	{
		if ((m_nMaterials == 1) && (m_ppMaterials[0]))
		{
			if (m_ppMaterials[0]->m_pShader) m_ppMaterials[0]->m_pShader->Render(pd3dCommandList, pCamera);
			m_ppMaterials[0]->UpdateShaderVariables(pd3dCommandList);
		}

		if (m_ppMeshes)
		{
			for (int i = 0; i < m_nMeshes; i++)
			{
				if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, 0);
			}
		}
	}

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}

void CGameObject::RenderAABB(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4AABBWorld);

	if (m_bHasAABB)
	{
		pd3dCommandList->IASetPrimitiveTopology(m_d3dAABBPrimitiveTopology);
		pd3dCommandList->IASetVertexBuffers(0, 1, &m_d3dAABBVertexBufferView);

		pd3dCommandList->DrawInstanced(m_nAABBVertices, 1, 0, 0);
	}

	if (m_pSibling) m_pSibling->RenderAABB(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->RenderAABB(pd3dCommandList, pCamera);
}

void CGameObject::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, CMaterial *pMaterial)
{
}

void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pd3dAABBVertexUploadBuffer) m_pd3dAABBVertexUploadBuffer->Release();
	m_pd3dAABBVertexUploadBuffer = NULL;

	for (int i = 0; i < m_nMeshes; i++)
	{
		if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
	}

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i]) m_ppMaterials[i]->ReleaseUploadBuffers();
	}

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObject::UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void CGameObject::UpdateAABBTransform(XMFLOAT4X4* pxmf4x4AABBParent)
{
	// Update AABB's matrix
	m_xmf4x4AABBWorld = (pxmf4x4AABBParent) ? Matrix4x4::Multiply(m_xmf4x4AABBTransform, *pxmf4x4AABBParent) : m_xmf4x4AABBTransform;

	if (m_pSibling) m_pSibling->UpdateAABBTransform(pxmf4x4AABBParent);
	if (m_pChild) m_pChild->UpdateAABBTransform(&m_xmf4x4AABBWorld);
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4Transform._41 = x;
	m_xmf4x4Transform._42 = y;
	m_xmf4x4Transform._43 = z;

	//AABB
	m_xmf4x4AABBTransform._41 = x;
	m_xmf4x4AABBTransform._42 = y;
	m_xmf4x4AABBTransform._43 = z;

	UpdateTransform(NULL);
	UpdateAABBTransform(NULL);
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	m_xmf4x4AABBTransform = Matrix4x4::Multiply(mtxScale, m_xmf4x4AABBTransform);

	UpdateTransform(NULL);
	UpdateAABBTransform(NULL);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	m_xmf4x4AABBTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4AABBTransform);

	UpdateTransform(NULL);
	UpdateAABBTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	m_xmf4x4AABBTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4AABBTransform);

	UpdateTransform(NULL);
	UpdateAABBTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT4 *pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	m_xmf4x4AABBTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4AABBTransform);

	UpdateTransform(NULL);
	UpdateAABBTransform(NULL);
}

//#define _WITH_DEBUG_FRAME_HIERARCHY

int CGameObject::FindReplicatedTexture(_TCHAR* pstrTextureName, D3D12_GPU_DESCRIPTOR_HANDLE* pd3dSrvGpuDescriptorHandle)
{
	int nParameterIndex = -1;

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i] && m_ppMaterials[i]->m_pTexture)
		{
			int nTextures = m_ppMaterials[i]->m_pTexture->GetTextures();
			for (int j = 0; j < nTextures; j++)
			{
				if (!_tcsncmp(m_ppMaterials[i]->m_pTexture->GetTextureName(j), pstrTextureName, _tcslen(pstrTextureName)))
				{
					*pd3dSrvGpuDescriptorHandle = m_ppMaterials[i]->m_pTexture->GetGpuDescriptorHandle(j);
					nParameterIndex = m_ppMaterials[i]->m_pTexture->GetRootParameter(j);
					return(nParameterIndex);
				}
			}
		}
	}
	if (m_pSibling) if ((nParameterIndex = m_pSibling->FindReplicatedTexture(pstrTextureName, pd3dSrvGpuDescriptorHandle)) > 0) return(nParameterIndex);
	if (m_pChild) if ((nParameterIndex = m_pChild->FindReplicatedTexture(pstrTextureName, pd3dSrvGpuDescriptorHandle)) > 0) return(nParameterIndex);

	return(nParameterIndex);
}

void CGameObject::LoadMaterialsFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CGameObject *pParent, FILE *pInFile, CShader *pShader)
{
	char pstrToken[64] = { '\0' };

	int nMaterial = 0;
	BYTE nStrLength = 0;

	UINT nReads = (UINT)::fread(&m_nMaterials, sizeof(int), 1, pInFile);

	m_ppMaterials = new CMaterial*[m_nMaterials];
	for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;

	CMaterial *pMaterial = NULL;
	CTexture* pTexture = NULL;

	for ( ; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile); 
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nReads = (UINT)::fread(&nMaterial, sizeof(int), 1, pInFile);

			pMaterial = new CMaterial(); 
#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS
			pTexture = new CTexture(7, RESOURCE_TEXTURE2D, 0, 7); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal
#else
			pTexture = new CTexture(7, RESOURCE_TEXTURE2D, 0, 1); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal
			pTexture->SetRootParameterIndex(0, PARAMETER_STANDARD_TEXTURE);
#endif
			pMaterial->SetTexture(pTexture);
			SetMaterial(nMaterial, pMaterial);

			UINT nMeshType = GetMeshType(0);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 0)) pMaterial->SetMaterialType(MATERIAL_ALBEDO_MAP);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 1)) pMaterial->SetMaterialType(MATERIAL_SPECULAR_MAP);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 2)) pMaterial->SetMaterialType(MATERIAL_NORMAL_MAP);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 3)) pMaterial->SetMaterialType(MATERIAL_METALLIC_MAP);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 4)) pMaterial->SetMaterialType(MATERIAL_EMISSION_MAP);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 5)) pMaterial->SetMaterialType(MATERIAL_DETAIL_ALBEDO_MAP);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, 6)) pMaterial->SetMaterialType(MATERIAL_DETAIL_NORMAL_MAP);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
}

CGameObject *CGameObject::LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CGameObject *pParent, FILE *pInFile, CShader *pShader)
{
	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	CGameObject *pGameObject = NULL;

	for ( ; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Frame>:"))
		{
			pGameObject = new CGameObject(1, 1);

			nReads = (UINT)::fread(&nFrame, sizeof(int), 1, pInFile);
			nReads = (UINT)::fread(&nTextures, sizeof(int), 1, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(pGameObject->m_pstrFrameName, sizeof(char), nStrLength, pInFile);
			pGameObject->m_pstrFrameName[nStrLength] = '\0';
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4Transform, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			CStandardMesh *pMesh = new CStandardMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pGameObject->SetMesh(0, pMesh);

			// Create AABB from Meshes
			pGameObject->CreateAABB(pd3dDevice, pd3dCommandList);
			if (pGameObject->m_bHasAABB)
				pGameObject->m_xmf4x4AABBTransform = pGameObject->m_xmf4x4Transform;
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			pGameObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = 0;
			nReads = (UINT)::fread(&nChilds, sizeof(int), 1, pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CGameObject *pChild = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, pInFile, pShader);
					if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pGameObject);
}

void CGameObject::PrintFrameInfo(CGameObject *pGameObject, CGameObject *pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) CGameObject::PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) CGameObject::PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

CGameObject *CGameObject::LoadGeometryFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName, CShader *pShader)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CGameObject *pGameObject = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader);

	//pGameObject->CreateAABB(pd3dDevice, pd3dCommandList);

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pGameObject, NULL);
#endif

	return(pGameObject);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
CSkyBox::CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(1, 1)
{
	CSkyBoxMesh *pSkyBoxMesh = new CSkyBoxMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 20.0f);
	SetMesh(0, pSkyBoxMesh);

	CTexture *pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0, 1);
	pSkyBoxTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"SkyBox/SkyBox_0.dds", RESOURCE_TEXTURE_CUBE, 0);

	CSkyBoxShader *pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pSkyBoxShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
	pSkyBoxShader->CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 0, PARAMETER_SKYBOX_CUBE_TEXTURE);

	CMaterial *pSkyBoxMaterial = new CMaterial();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	SetMaterial(0, pSkyBoxMaterial);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	CGameObject::Render(pd3dCommandList, pCamera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSuperCobraObject::CSuperCobraObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(0, 0)
{
}

CSuperCobraObject::~CSuperCobraObject()
{
}

void CSuperCobraObject::PrepareAnimate()
{
	m_pMainRotorFrame = FindFrame("MainRotor");
	m_pTailRotorFrame = FindFrame("TailRotor");
}

void CSuperCobraObject::Animate(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pMainRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->m_xmf4x4Transform);
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->m_xmf4x4Transform);
	}

	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGunshipObject::CGunshipObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(0, 0)
{
}

CGunshipObject::~CGunshipObject()
{
}

void CGunshipObject::PrepareAnimate()
{
	m_pMainRotorFrame = FindFrame("Rotor");
	m_pTailRotorFrame = FindFrame("Back_Rotor");
}

void CGunshipObject::Animate(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);
		m_pMainRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->m_xmf4x4Transform);
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->m_xmf4x4Transform);
	}

	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMi24Object::CMi24Object(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature) : CGameObject(0, 0)
{
}

CMi24Object::~CMi24Object()
{
}

void CMi24Object::PrepareAnimate()
{
	m_pMainRotorFrame = FindFrame("Top_Rotor");
	m_pTailRotorFrame = FindFrame("Tail_Rotor");
}

void CMi24Object::Animate(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);
		m_pMainRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->m_xmf4x4Transform);
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->m_xmf4x4Transform);
	}

	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(0, 1)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new CMesh * [m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;

	CHeightMapGridMesh* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}

	CTexture* pTerrainTexture = new CTexture(3, RESOURCE_TEXTURE2D, 0, 1);

	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Base_Texture.dds", RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Detail_Texture_7.dds", RESOURCE_TEXTURE2D, 1);
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/HeightMap(Alpha).dds", RESOURCE_TEXTURE2D, 2);

	CTerrainShader* pTerrainShader = new CTerrainShader();
	pTerrainShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pTerrainShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 3);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTerrainShader->CreateShaderResourceViews(pd3dDevice, pTerrainTexture, 0, 11);

	CMaterial* pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);
	pTerrainMaterial->SetShader(pTerrainShader);

	SetMaterial(0, pTerrainMaterial);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}



/////////////////////////////////////////
// Charactor
CCharacter::CCharacter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

CCharacter::~CCharacter()
{
}



CCutter::CCutter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

CCutter::~CCutter()
{
}

void CCutter::PrepareAnimate()
{
}

void CCutter::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{	
	XMMATRIX xmmtxTransform = XMMatrixTranslation(0.0f, 0.0f, -0.1f);
	m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxTransform, m_xmf4x4Transform);

	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

CColisionBoxData::CColisionBoxData()
{
}

CColisionBoxData::CColisionBoxData(XMFLOAT3* m_pxmf3AABBCenter, XMFLOAT3* m_pxmf3AABBExtents)
{
	SetAABBCenterExtent(m_pxmf3AABBCenter, m_pxmf3AABBExtents);
}

CColisionBoxData::~CColisionBoxData()
{
}

void CColisionBoxData::SetAABBCenterExtent(XMFLOAT3* m_pxmf3AABBCenter, XMFLOAT3* m_pxmf3AABBExtents)
{
	m_xmf3AABBCenter = *(m_pxmf3AABBCenter);
	m_xmf3AABBExtents = *(m_pxmf3AABBExtents);

	MakeMinMaxData();
}

void CColisionBoxData::MakeMinMaxData()
{
	m_xmf3AABBMax = XMFLOAT3(m_xmf3AABBCenter.x + m_xmf3AABBExtents.x, m_xmf3AABBCenter.y + m_xmf3AABBExtents.y, m_xmf3AABBCenter.z + m_xmf3AABBExtents.z);
	m_xmf3AABBMin = XMFLOAT3(m_xmf3AABBCenter.x - m_xmf3AABBExtents.x, m_xmf3AABBCenter.y - m_xmf3AABBExtents.y, m_xmf3AABBCenter.z - m_xmf3AABBExtents.z);
}
