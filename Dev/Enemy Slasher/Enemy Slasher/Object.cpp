#include "stdafx.h"
#include "Shader.h"
#include "Object.h"


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
			//if (m_pd3dSrvGpuDescriptorHandles[i].ptr && (m_pnRootParameterIndices[i] != -1)) std::cout << "PTR\n";
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

void CTexture::LoadTextureFromWICFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromWICFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ/*D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/);
}

void CTexture::LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ/*D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/);
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



CMaterial::CMaterial()
{
	m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

CMaterial::~CMaterial()
{
	if (m_pShader) m_pShader->Release();
}

void CMaterial::SetTexture(CTexture* pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CMaterial::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pShader)
	{
		m_pShader->UpdateShaderVariables(pd3dCommandList);
	}
	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader) m_pShader->ReleaseShaderVariables();
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}


CGameObject::CGameObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype, int nMeshes)
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;

	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh * [m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;
	}
	CGameObject::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CGameObject::~CGameObject()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}

	if (m_pMaterial) m_pMaterial->Release();

	if (m_pCollider) delete m_pCollider;
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


void CGameObject::SetMaterial(CMaterial* pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}

void CGameObject::SetMaterial(UINT nReflection)
{
	if (!m_pMaterial) m_pMaterial = new CMaterial();
	m_pMaterial->m_nReflection = nReflection;
}


void CGameObject::SetMesh(int nIndex, CMesh* pMesh, bool bColliderFlag)
{
	if (m_ppMeshes == NULL || nIndex >= m_nMeshes)
		SetMesh(nIndex + 1);
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();

		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
	if (bColliderFlag == true)
		MakeCollider();
}

void CGameObject::SetMesh(int nIndexSize)
{
	CMesh** NewMeshes = NULL;

	NewMeshes = new CMesh * [nIndexSize];
	for (int i = 0; i < nIndexSize; i++) NewMeshes[i] = NULL;
	if (m_ppMeshes)
		for (int i = 0; i < m_nMeshes; i++) NewMeshes[i] = m_ppMeshes[i];
	if (m_ppMeshes)
		delete[] m_ppMeshes;
	m_ppMeshes = NewMeshes;
}

void CGameObject::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 

	for (int i = 0; i < m_nMeshes; i++)
	{
		if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
	}

	//for (int i = 0; i < m_nMaterials; i++)
	//{
	//	if (m_ppMaterials[i]) m_ppMaterials[i]->ReleaseUploadBuffers();
	//}

	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();

}

void CGameObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed, pxmf4x4Parent);
	if (m_pChild) m_pChild->Animate(fTimeElapsed, &m_xmf4x4World);
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption)
{
	OnPrepareRender();

	//객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다. 
	UpdateShaderVariables(pd3dCommandList);

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		}

		if (m_pMaterial->m_pTexture)
		{
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}

	}
	// 서술자힙 연결할지말지 작성 고민...

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, pRenderOption);
		}
	}

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}

void CGameObject::SetChild(CGameObject* pChild)
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

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbGameObject->Map(0, NULL, (void**)&m_pcbMappedGameObject);
}

void CGameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}
	if (m_pMaterial)m_pMaterial->ReleaseShaderVariables();
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//XMFLOAT4X4 xmf4x4World;
	//XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	////객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다. 
	//pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);

	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	if (m_pMaterial)m_pcbMappedGameObject->m_nMaterialID = m_pMaterial->m_nReflection;

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pd3dcbGameObject->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(1, d3dcbGameObjectGpuVirtualAddress);
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	//XMFLOAT4X4 xmf4x4World;
	//XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	//pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);

	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	if (m_pMaterial)m_pcbMappedGameObject->m_nMaterialID = m_pMaterial->m_nReflection;

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pd3dcbGameObject->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(1, d3dcbGameObjectGpuVirtualAddress);
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void CGameObject::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype)
{
	switch (stype)
	{
	case ShaderType::NON:
		break;
	case ShaderType::CObjectsShader:
	{
		CObjectsShader* pObjectsShader = new CObjectsShader();
		pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		SetShader(pObjectsShader);
		break;
	}
	case ShaderType::CUIObjectsShader:
	{
		CUIObjectsShader* pObjectsShader = new CUIObjectsShader();
		pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		SetShader(pObjectsShader);
		break;
	}
	case ShaderType::CObjectsNormalShader:
	{
		CObjectsNormalShader* pObjectsShader = new CObjectsNormalShader();
		pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		SetShader(pObjectsShader);
		break;
	}
	case ShaderType::CTerrainShader:
	{
		CTerrainShader* pObjectsShader = new CTerrainShader();
		pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		SetShader(pObjectsShader);
		break;
	}
	case ShaderType::CTextShader:
	{
		CTextShader* pObjectsShader = new CTextShader();
		pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		SetShader(pObjectsShader);
		break;
	}
	case ShaderType::CTextureShader:
	{
		CTextureShader* pObjectsShader = new CTextureShader();
		pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		SetShader(pObjectsShader);
		break;
	}
	case ShaderType::CUITextureShader:
	{
		CUITextureShader* pObjectsShader = new CUITextureShader();
		pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		SetShader(pObjectsShader);
		break;
	}
	case ShaderType::CTexture_Position_Texcoord_Shader:
	{
		CTexture_Position_Texcoord_Shader* pObjectsShader = new CTexture_Position_Texcoord_Shader();
		pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		SetShader(pObjectsShader);
		break;
	}
	case ShaderType::CSkyBoxShader:
	{
		CSkyBoxShader* pObjectsShader = new CSkyBoxShader();
		pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		SetShader(pObjectsShader);
		break;
	}
	case ShaderType::CBillBoardInstanceShader:
	{
		CBillBoardInstanceShader* pObjectsShader = new CBillBoardInstanceShader();
		pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		SetShader(pObjectsShader);
		break;
	}


	}
}


void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4Transform._41 = x;
	m_xmf4x4Transform._42 = y;
	m_xmf4x4Transform._43 = z;

	UpdateTransform(NULL);
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::SetScale(XMFLOAT3 scale)
{
	XMMATRIX mtxScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

//게임 객체의 로컬 z-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetLook()
{
	XMFLOAT3 a = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	return(Vector3::Normalize(a));
}

//게임 객체의 로컬 y-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetUp()
{

	XMFLOAT3 a = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	return(Vector3::Normalize(a));
}

//게임 객체의 로컬 x-축 벡터를 반환한다. 
XMFLOAT3 CGameObject::GetRight()
{
	XMFLOAT3 a = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	return(Vector3::Normalize(a));
}

//게임 객체를 로컬 x-축 방향으로 이동한다. 
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 로컬 y-축 방향으로 이동한다. 
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 로컬 z-축 방향으로 이동한다. 
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//게임 객체를 주어진 각도로 회전한다. 
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT4* pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void CGameObject::SetShader(CShader* pShader)
{
	if (!m_pMaterial)
	{
		m_pMaterial = new CMaterial();
		m_pMaterial->AddRef();
	}

	if (m_pMaterial) m_pMaterial->SetShader(pShader);
}

void CGameObject::MakeCollider()
{
	if (m_pCollider) delete m_pCollider;
	m_pCollider = new COBBColliderWithMesh;
	XMFLOAT4X4 myWorldMat = GetWorldMat();
	for (int i = 0; i < m_nMeshes; ++i) {
		if (CColliderMesh* colliderMesh = dynamic_cast<CColliderMesh*>(m_ppMeshes[i]))
			m_pCollider->UpdateColliderWithOBB((colliderMesh)->GetCollider(), myWorldMat);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////

CDynamicShapeObject::CDynamicShapeObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype, int nMeshes)
	: CGameObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype, nMeshes)
{
}

CDynamicShapeObject::~CDynamicShapeObject()
{
}

vector<CGameObject*> CDynamicShapeObject::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	ShaderType stype, float fTimeElapsed, CGameObject* pCutterObject, CDynamicShapeMesh::CutAlgorithm cutAlgorithm)
{
	CDynamicShapeObject* pDynamicShapeObject = static_cast<CDynamicShapeObject*>(pCutterObject);

	int nCutterMeshes = pDynamicShapeObject->GetNumMeshes();
	CCutterMesh** ppCutterMeshes = (CCutterMesh**)pDynamicShapeObject->GetMeshes();
	CCutterMesh** ppMeshes = (CCutterMesh**)m_ppMeshes;
	XMFLOAT4X4 pxmfCutterMat = pDynamicShapeObject->GetWorldMat();

	// 절단 후 생성될 Mesh들을 받기위한 데이터 정의
	vector<CMesh*> newMeshs;
	XMFLOAT4X4 newWorldMat = Matrix4x4::Identity();
	XMFLOAT4X4 newTransformMat = Matrix4x4::Identity();

	if (m_bCuttable && pDynamicShapeObject->GetAllowCutting()) {
		// 다른 오브젝트에 의해 내가 잘릴 수 있을 때

		XMFLOAT4X4 myWorldMat = GetWorldMat();
		XMFLOAT4X4 otherWorldMat = pDynamicShapeObject->GetWorldMat();

		for (int i = 0; i < m_nMeshes; ++i) {
			for (int j = 0; j < nCutterMeshes; ++j) {
				if (CollisionCheck(ppMeshes[i]->GetCollider(), myWorldMat, ppCutterMeshes[j]->GetCollider(), otherWorldMat)) {
					// 두 오브젝트가 충돌하면 DynamicShaping을 시도한다.
					vector<CMesh*> vRetVec = ppMeshes[i]->DynamicShaping(pd3dDevice, pd3dCommandList, fTimeElapsed, m_xmf4x4World, ppCutterMeshes[j], pxmfCutterMat, cutAlgorithm);
					newMeshs.insert(newMeshs.end(), vRetVec.begin(), vRetVec.end());
				}
			}
		}
		if (false == newMeshs.empty()) {
			newWorldMat = GetWorldMat();
			newTransformMat = GetTransMat();
		}
	}

	// 만약 절단이 일어나 새로운 Mesh가 생겼다면 2개 이상의 오브젝트를 생성한다.
	// 하나의 Mesh 는 하나의 Object가 되도록 하자. - 나중에 변경할 수도 있다
	vector<CGameObject*> newGameObjects;
	for (int i = 0; i < newMeshs.size(); ++i) {
		newGameObjects.push_back(new CDynamicShapeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype));

		newGameObjects[i]->SetMesh(0, newMeshs[i]);

		newGameObjects[i]->SetWorldMat(newWorldMat);
		newGameObjects[i]->SetTransMat(newTransformMat);
		//newGameObjects[i]->SetShaderType(ShaderType::CObjectsShader);
		((CDynamicShapeObject*)newGameObjects[i])->SetCuttable(true);
	}

	return newGameObjects;
}

CFBXObject::CFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFBXLoader* pFBXLoader, const char* fileName, ShaderType stype)
	: CDynamicShapeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype, 0)//  모델에 mesh가 있으면 LoadContent에서 증가시킨다.
{
	//------------------------------------------------------------------------------------------
	//FbxManager* plSdkManager = NULL;
	//FbxScene* plScene = NULL;
	LoadResult lResult;

	//InitializeSdkObjects(plSdkManager, plScene);

	FbxString lFilePath(fileName);

	lResult = pFBXLoader->LoadScene(lFilePath);

	if (lResult == LoadResult::False)
	{
#ifdef _DEBUG
		FBXSDK_printf("\n\nAn error occurred while loading the scene...\n");
#endif // _DEBUG

	}
	else if (lResult == LoadResult::First)
	{
		m_sFileName = fileName;
		LoadContent(pd3dDevice, pd3dCommandList, pFBXLoader, lFilePath.Buffer());
		LoadHierarchy(pFBXLoader, lFilePath.Buffer()); // 이 함수가 true 를 반환해야 본이 있는것. false를 반환하면 없어야함.
		LoadHierarchyFromMesh(); //이건 꼭 해야하는가 의문이 들긴 함.
		pFBXLoader->LoadAnimation(lFilePath.Buffer());
	}
	else if (lResult == LoadResult::Overlapping)
	{
		m_sFileName = fileName;
		// 일단 아직 위와 동일한 코드를 넣어놓는다.
		LoadContent(pd3dDevice, pd3dCommandList, pFBXLoader, lFilePath.Buffer());
		LoadHierarchy(pFBXLoader, lFilePath.Buffer()); // 이 함수가 true 를 반환해야 본이 있는것. false를 반환하면 없어야함.
		LoadHierarchyFromMesh();
		// 애니메이션은 중복 로드 할 필요가 없음. 경우도 없을것으로 예상.
	}

	//------------------------------------------------------------------------------------------

	//SetShaderType(shaderType);


	MakeCollider();
}

CFBXObject::~CFBXObject()
{
}

void CFBXObject::LoadContent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, const char* filePath)
{
	int i;
	FbxNode* lNode = pFBXLoader->GetRootNode(filePath);
	if (lNode)
	{
		for (i = 0; i < lNode->GetChildCount(); i++)
		{
			LoadContent(pd3dDevice, pd3dCommandList, lNode->GetChild(i), i);
		}
	}
}

void CFBXObject::LoadContent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pNode, int childId)
{
	FbxNodeAttribute::EType lAttributeType;
	CFBXMesh* pFBXMesh = NULL;
	int p = 0;

	if (pNode->GetNodeAttribute() == NULL)
	{
		//FBXSDK_printf("NULL Node Attribute\n\n");
	}
	else
	{
		lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

		switch (lAttributeType)
		{
		default:
			break;
		case FbxNodeAttribute::eMarker:
			//DisplayMarker(pNode);
			break;

		case FbxNodeAttribute::eSkeleton:
			//DisplaySkeleton(pNode);
			break;

		case FbxNodeAttribute::eMesh:
			pFBXMesh = new CFBXMesh(pd3dDevice, pd3dCommandList);
			if (pFBXMesh->LoadMesh(pd3dDevice, pd3dCommandList, pNode))
			{
				// 지금은 Mesh 가 아니면 아무것도 안 하므로 이렇게 한다.
				SetMesh(m_nMeshes, pFBXMesh);
				m_nMeshes++;
			}
			break;

		case FbxNodeAttribute::eNurbs:
			//DisplayNurb(pNode);
			break;

		case FbxNodeAttribute::ePatch:
			//DisplayPatch(pNode);
			break;

		case FbxNodeAttribute::eCamera:
			//DisplayCamera(pNode);
			break;

		case FbxNodeAttribute::eLight:
			//DisplayLight(pNode);
			break;

		case FbxNodeAttribute::eLODGroup:
			//DisplayLodGroup(pNode);
			break;
		}
	}

	//DisplayUserProperties(pNode);
	//DisplayTarget(pNode);
	//DisplayPivotsAndLimits(pNode);
	//DisplayTransformPropagation(pNode);
	//DisplayGeometricTransform(pNode);

	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		LoadContent(pd3dDevice, pd3dCommandList, pNode->GetChild(i), i);
	}
}

bool CFBXObject::LoadHierarchy(CFBXLoader* pFBXLoader, const char* fileName)
{
	int i;
	FbxNode* lNode = pFBXLoader->GetRootNode(fileName);

	for (i = 0; i < lNode->GetChildCount(); i++)
	{
		if (LoadHierarchy(lNode->GetChild(i)))
			return true;
	}
	return false;
}

bool CFBXObject::LoadHierarchy(FbxNode* pNode)
{
	FbxNodeAttribute::EType lAttributeType;

	if (pNode->GetNodeAttribute() == NULL)
	{
		//FBXSDK_printf("NULL Node Attribute\n\n");
	}
	else
	{
		lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

		if (lAttributeType == FbxNodeAttribute::eSkeleton)
		{
			m_skelRoot = new CSkeleton(pNode->GetName(), pNode->GetChildCount());
			m_skelRoot->LoadHierarchy(pNode);
			return true;
		}
		else
		{
			for (int i = 0; i < pNode->GetChildCount(); i++)
			{
				if (LoadHierarchy(pNode->GetChild(i)))
					return true;
			}
		}
	}
	return false;
}

void CFBXObject::LoadHierarchyFromMesh()
{
	CFBXMesh* pMesh;

	if (m_ppMeshes[0])
	{
		pMesh = (CFBXMesh*)m_ppMeshes[0];
		if (m_skelRoot)
			m_skelRoot->LoadHierarchyFromMesh(pMesh);
	}

}

//void CFBXObject::LoadAnimation(CFBXLoader* pFBXLoader, const char* fileName)
//{
//	int i;
//	FbxScene* pScene = pFBXLoader->GetScene();
//
//	// 스택이 하나임을 가정한다.
//	for (i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
//	{
//		FbxAnimStack* lAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);
//
//		//FbxString lOutputString = "Animation Stack Name: ";
//		//lOutputString += lAnimStack->GetName();
//		//lOutputString += "\n";
//		//FBXSDK_printf(lOutputString);
//
//
//		// pFBXLoader의 m_mAnimationList에 새로운 애니메이션을 저장
//		// 저장할때 파일 이름을 기준으로 저장하므로 주의할것.
//		// 함수 인자 줄줄이 바꿔야함.
//		LoadAnimation(lAnimStack, pScene->GetRootNode());
//	}
//}
//
//void CFBXObject::LoadAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, bool isSwitcher)
//{
//	int l;
//	int nbAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
//	//int nbAudioLayers = pAnimStack->GetMemberCount<FbxAudioLayer>();
//	//FbxString lOutputString;
//
//	//lOutputString = "   contains ";
//	//if (nbAnimLayers == 0 /*&& nbAudioLayers == 0*/)
//	//	lOutputString += "no layers";
//
//	//if (nbAnimLayers)
//	//{
//	//	lOutputString += nbAnimLayers;
//	//	lOutputString += " Animation Layer";
//	//	if (nbAnimLayers > 1)
//	//		lOutputString += "s";
//	//}
//
//	//if (nbAudioLayers)
//	//{
//	//	if (nbAnimLayers)
//	//		lOutputString += " and ";
//	//
//	//	lOutputString += nbAudioLayers;
//	//	lOutputString += " Audio Layer";
//	//	if (nbAudioLayers > 1)
//	//		lOutputString += "s";
//	//}
//	//lOutputString += "\n\n";
//	//FBXSDK_printf(lOutputString);
//
//	// 레이어는 하나임을 가정한다.
//	for (l = 0; l < nbAnimLayers; l++)
//	{
//		FbxAnimLayer* lAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(l);
//
//		//lOutputString = "AnimLayer ";
//		//lOutputString += l;
//		//lOutputString += "\n";
//		//FBXSDK_printf(lOutputString);
//
//		LoadAnimation(lAnimLayer, pNode, isSwitcher);
//	}
//
//	//for (l = 0; l < nbAudioLayers; l++)
//	//{
//	//	FbxAudioLayer* lAudioLayer = pAnimStack->GetMember<FbxAudioLayer>(l);
//	//
//	//	lOutputString = "AudioLayer ";
//	//	lOutputString += l;
//	//	lOutputString += "\n";
//	//	FBXSDK_printf(lOutputString);
//	//
//	//	DisplayAnimation(lAudioLayer, isSwitcher);
//	//	FBXSDK_printf("\n");
//	//}
//}
//
//void CFBXObject::LoadAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode, bool isSwitcher)
//{
//	int lModelCount;
//	//FbxString lOutputString;
//
//	//lOutputString = "     Node Name: ";
//	//lOutputString += pNode->GetName();
//	//lOutputString += "\n\n";
//	//FBXSDK_printf(lOutputString);
//
//	// 아래의 함수에서 
//	//DisplayChannels(pNode, pAnimLayer, DisplayCurveKeys, DisplayListCurveKeys, isSwitcher);
//	//FBXSDK_printf("\n");
//
//	for (lModelCount = 0; lModelCount < pNode->GetChildCount(); lModelCount++)
//	{
//		LoadAnimation(pAnimLayer, pNode->GetChild(lModelCount), isSwitcher);
//	}
//}

// 
//bool CFBXObject::IsCursorOverObject()
//{
//	return false;
//}
//
//void CFBXObject::ButtenDown()
//{
//}
//
//void CFBXObject::ButtenUp()
//{
//}

CAABB* CFBXObject::GetAABB()
{
	// 만약 하나의 오브젝트에 매쉬가 더 늘어날 경우 수정해야함.
	CFBXMesh* tmp = (CFBXMesh*)m_ppMeshes[0];
	return tmp->GetAABB(m_xmf4x4World);
}

void CFBXObject::SetAnimation(CAnimationData* ani, bool loofFlag)
{
	if (m_adCurrentAnimationData)
	{
		m_adOldAnimationData = m_adCurrentAnimationData;
		m_bOldLoofFlag = m_bCurrentLoofFlag;
	}
	m_adCurrentAnimationData = ani;
	m_bCurrentLoofFlag = loofFlag;

	m_fProgressedFrame = 0.f;
}

void CFBXObject::RestoreAnimation()
{
	if (m_adOldAnimationData)
	{
		m_adCurrentAnimationData = m_adOldAnimationData;
		m_bCurrentLoofFlag = m_bOldLoofFlag;
	}
	m_fProgressedFrame = 0.f;
}


void CFBXObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_adCurrentAnimationData)
	{
		// 애니메이션 데이터에 있는 정보로 본 정보 바꾸기.
		// 0번 매쉬가 본의 리스트를 갖고 있다고 가정.

		CSkeleton* skelList = ((CFBXMesh*)m_ppMeshes[0])->GetSkeletonList();
		int clusterCOunt = ((CFBXMesh*)m_ppMeshes[0])->GetClusterCount();
		for (const auto& pair : m_adCurrentAnimationData->m_mAnimationData)
		{
			for (int i = 0; i < clusterCOunt; i++)
			{
				if (pair.first == skelList[i].GetName())
				{
					XMFLOAT4X4 mat = pair.second.at((int)m_fProgressedFrame);
					skelList[i].SetTransformMatrix(mat);
					break;
				}
			}
		}

		// --
		m_fProgressedFrame += fTimeElapsed * DEFAULT_FRAME_RATIO;
		if (m_fProgressedFrame > m_adCurrentAnimationData->GetTotalFrames())
		{
			if (m_bCurrentLoofFlag)
				m_fProgressedFrame = 0.f;
			else
			{
				RestoreAnimation();
			}
		}
	}
	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}



CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName,
	int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, ShaderType stype)
	: CGameObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype)
{
	//지형에 사용할 높이 맵의 가로, 세로의 크기이다.
	m_nWidth = nWidth;
	m_nLength = nLength;

	/*지형 객체는 격자 메쉬들의 배열로 만들 것이다. nBlockWidth, nBlockLength는 격자 메쉬 하나의 가로, 세로 크
	기이다. cxQuadsPerBlock, czQuadsPerBlock은 격자 메쉬의 가로 방향과 세로 방향 사각형의 개수이다.*/

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	//xmf3Scale는 지형을 실제로 몇 배 확대할 것인가를 나타낸다. 
	m_xmf3Scale = xmf3Scale;

	//지형에 사용할 높이 맵을 생성한다. 
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	//지형에서 가로 방향, 세로 방향으로 격자 메쉬가 몇 개가 있는 가를 나타낸다. 
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	//지형 전체를 표현하기 위한 격자 메쉬의 개수이다. 
	m_nMeshes = cxBlocks * czBlocks;

	//지형 전체를 표현하기 위한 격자 메쉬에 대한 포인터 배열을 생성한다. 
	m_ppMeshes = new CMesh * [m_nMeshes];

	for (int i = 0; i < m_nMeshes; i++)m_ppMeshes[i] = NULL;
	CHeightMapGridMesh* pHeightMapGridMesh = NULL;

	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			//지형의 일부분을 나타내는 격자 메쉬의 시작 위치(좌표)이다. 
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);

			//지형의 일부분을 나타내는 격자 메쉬를 생성하여 지형 메쉬에 저장한다. 
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}
	CTexture* pTerrainTexture = new CTexture(3, RESOURCE_TEXTURE2D, 0, 3);

	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Base_Texture.dds", RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Detail_Texture_7.dds", RESOURCE_TEXTURE2D, 1);
	pTerrainTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/HeightMap(Alpha).dds", RESOURCE_TEXTURE2D, 2);

	if (m_pMaterial) {
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 3);
			m_pMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, pTerrainTexture, 0, 5);
			m_pMaterial->SetTexture(pTerrainTexture);
		}
	}
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

void CTextObject::Render(ID2D1DeviceContext3* pd2dDeviceContext)
{
	//pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Translation(m_position.x, m_position.y));
	//pd2dDeviceContext->DrawText(
	//	m_text.c_str(),
	//	static_cast<UINT32>(m_text.size()),
	//	s_formats[m_format].Get(),
	//	&m_rect,
	//	s_brushes[m_brush].Get()
	//);
}

CRayObject::CRayObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype)
	: CGameObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype)
{
	m_vOriginal = { 0,0,0 };
	m_xmf3DirOld = { 0,0,1 };
}

CRayObject::~CRayObject()
{
}

void CRayObject::Reset(CRay ray)
{
	m_vOriginal = ray.GetOriginal();
	m_xmf3Dir = ray.GetDir();

	XMVECTOR vFrom = XMLoadFloat3(&m_xmf3DirOld);
	XMFLOAT3 dir = ray.GetDir();
	XMVECTOR vTo = XMLoadFloat3(&dir);

	// 정규화된 벡터로 변환
	vFrom = XMVector3Normalize(vFrom);
	vTo = XMVector3Normalize(vTo);

	// 축 벡터 계산
	XMVECTOR axis = XMVector3Cross(vFrom, vTo);

	if (!XMVector3Equal(axis, XMVectorZero()))
	{
		XMFLOAT3 fAxis;
		XMStoreFloat3(&fAxis, axis);

		// 두 벡터 간의 각도 계산
		float angle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vFrom, vTo)));

		// 결과값 설정
		XMStoreFloat4x4(&m_xmf4x4Transform, XMMatrixIdentity());
		Rotate(&fAxis, angle);
	}
	// 결과값 설정
	SetPosition(m_vOriginal);
#ifdef _DEBUG
	//std::cout << "m_xmf3Dir: " << ray.GetDir().x << ", " << ray.GetDir().y << ", " << ray.GetDir().z << std::endl;
	//std::cout << "m_vOriginal: " << ray.GetOriginal().x << ", " << ray.GetOriginal().y << ", " << ray.GetOriginal().z << std::endl;
#endif // _DEBUG
}

CUIObject::CUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName, ShaderType stype)
	: CFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pFBXLoader, fileName, stype)
{
	m_xmfScale.z = m_xmfScale.y = m_xmfScale.z = 1.0;

	m_fCurrntScale = m_fTargetScale = 1.0f;
	SetCamera(pCamera);

	m_iUInum = -1;
}

CUIObject::CUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName, int UInum, ShaderType stype)
	: CFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pFBXLoader, fileName, stype)
{
	m_xmfScale.z = m_xmfScale.y = m_xmfScale.z = 1.0;

	m_fCurrntScale = m_fTargetScale = 1.0f;
	SetCamera(pCamera);

	m_iUInum = UInum;
}

CUIObject::~CUIObject()
{
}

void CUIObject::ScreenSpaceToWorldSpace()
{
	XMStoreFloat4x4(&m_xmf4x4Transform, XMMatrixIdentity());// 초기화

	D3D12_VIEWPORT viewPort = m_pCamera->GetViewport();
	XMFLOAT4X4 projMat = m_pCamera->GetProjectionMatrix();
	XMFLOAT4X4 viewMat = m_pCamera->GetViewMatrix();
	float ndcX = ((2.f * m_iXPosition) / viewPort.Width - 1.f);
	float ndcY = ((-2.f * m_iYPosition) / viewPort.Height + 1.f);

	XMFLOAT4 ClipPosition(ndcX, ndcY, 1.0f, 1.0f);// 

	XMFLOAT4X4 projInv = Matrix4x4::Inverse(projMat);

	XMFLOAT4X4 viewInv = Matrix4x4::Inverse(viewMat);

	XMVECTOR rayO = XMVector3TransformCoord(XMLoadFloat4(&ClipPosition), XMLoadFloat4x4(&projInv));

	XMVECTOR rayW = XMVector3TransformCoord(rayO, XMLoadFloat4x4(&viewInv));

	// 카메라의 역행렬을 가져옴. 회전때문에
	// 이걸 사용하면 카메라벡터의 반대를 보게됨.
	// 카메라의Look의 반대, 카메라UP으로 만들어도 될듯? 아마?
	XMStoreFloat4x4(&m_xmf4x4Transform, XMLoadFloat4x4(&viewInv));

	// 스케일 적용.
	SetScale(m_xmfScale);

	// 카메라를 바라보게 만드는것도 좋을지도

	XMFLOAT3 position;
	XMStoreFloat3(&position, rayW);

	SetPosition(position);


	// 회전은 항상 카메라를 향해야함.


}

void CUIObject::SetPositionUI(int x, int y)
{
	m_iXPosition = x;
	m_iYPosition = y;

	ScreenSpaceToWorldSpace();
}

void CUIObject::SetPositionUI(POINT pos)
{
	m_iXPosition = pos.x;
	m_iYPosition = pos.y;

	ScreenSpaceToWorldSpace();
}

void CUIObject::AddPositionUI(int x, int y)
{
	m_iXPosition += x;
	m_iYPosition += y;

	ScreenSpaceToWorldSpace();
}

void CUIObject::AddPositionUI(POINT pos)
{
	m_iXPosition += pos.x;
	m_iYPosition += pos.y;

	ScreenSpaceToWorldSpace();
}

void CUIObject::SetScale(float x, float y, float z)
{
	m_xmfScale.x = x;
	m_xmfScale.y = y;
	m_xmfScale.z = z;

	m_xmfScale = Vector3::ScalarProduct(m_xmfScale, m_fCurrntScale, false);
	CGameObject::SetScale(m_xmfScale.x, m_xmfScale.y, m_xmfScale.z);
}

void CUIObject::SetScale(XMFLOAT3 scale)
{
	m_xmfScale = scale;

	CGameObject::SetScale(Vector3::ScalarProduct(m_xmfScale, m_fCurrntScale, false));
}

CCardUIObject::CCardUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName, ShaderType stype)
	: CUIObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pFBXLoader, pCamera, fileName, stype)
{
}

CCardUIObject::CCardUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName, int UInum, ShaderType stype)
	: CUIObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pFBXLoader, pCamera, fileName, UInum, stype)
{
	m_Card_Ui_Num = UInum;
	//CreateShaderVariables(pd3dDevice, pd3dCommandList);


	CTexture* ppTextures[5];

	ppTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[0]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/attack.jpg", RESOURCE_TEXTURE2D, 0);

	ppTextures[1] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[1]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/attack2.jpg", RESOURCE_TEXTURE2D, 0);


	ppTextures[2] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[2]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/attack3.jpg", RESOURCE_TEXTURE2D, 0);

	ppTextures[3] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[3]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/attack4.jpg", RESOURCE_TEXTURE2D, 0);

	ppTextures[4] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[4]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/attack5.jpg", RESOURCE_TEXTURE2D, 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수


	if (m_pMaterial) {
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
			m_pMaterial->m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
			//m_pMaterial->m_pShader->CreateConstantBufferViews(pd3dDevice, 1, m_pd3dcbGameObject, ncbElementBytes);
			m_pMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, ppTextures[0], 0, 4);

			m_pMaterial->SetTexture(ppTextures[0]);
			//SetCbvGPUDescriptorHandle(m_pMaterial->m_pShader->GetGPUCbvDescriptorStartHandle());
		}
	}

}

CCardUIObject::~CCardUIObject()
{
}

void CCardUIObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	ScreenSpaceToWorldSpace();

	if (std::fabs(m_fTargetScale - m_fCurrntScale) > EPSILON)
	{
		if (m_fTargetScale > m_fCurrntScale)
		{
			m_fCurrntScale += fTimeElapsed * TIME_ELAPSE_RATIO;
			if (m_fTargetScale < m_fCurrntScale)
				m_fCurrntScale = m_fTargetScale;
		}
		else
		{
			m_fCurrntScale -= fTimeElapsed * TIME_ELAPSE_RATIO;
			if (m_fTargetScale > m_fCurrntScale)
				m_fCurrntScale = m_fTargetScale;
		}
	}
	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

void CCardUIObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void**)&m_pcbMappedGameObject);
}

void CCardUIObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);

	if (m_pcbMappedGameObject)
	{
		XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
		//if (m_pMaterial) m_pcbMappedGameObject->m_nMaterial = m_pMaterial->m_nReflection;
	}
}

//void CCardUIObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption)
//{
//	OnPrepareRender();
//
//	UpdateShaderVariables(pd3dCommandList);
//
//	if (m_pMaterial)
//	{
//		if (m_pMaterial->m_pShader)
//		{
//			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
//		}
//
//		if (m_pMaterial->m_pTexture)
//		{
//			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
//		}
//
//	}
//	pd3dCommandList->SetGraphicsRootDescriptorTable(3, m_d3dCbvGPUDescriptorHandle);
//
//	if (m_ppMeshes)
//	{
//		for (int i = 0; i < m_nMeshes; i++)
//		{
//			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, pRenderOption);
//		}
//	}
//
//	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
//	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
//}

void CCardUIObject::CursorOverObject(bool flag)
{
	if (flag)
		m_fTargetScale = TARGET_SCALE;
	else
		m_fTargetScale = DEFUALT_SCALE;
}

void CCardUIObject::ButtenDown()
{
	// 선택 상태가 된다.
}

void CCardUIObject::ButtenUp()
{
	// 모든 카드는 공격카드
	switch (m_iUInum)
	{
	case 0:
		// 세로로 절단
		break;
	case 1:
		// 가로 절단
		break;
	case 2:
		// 랜덤 절단
		break;
	case 3:
		// 이동 후 절단.
		// 이동은 가장 가까운 오브젝트를 향함.
		break;
	case 4:

		break;
	default:
		break;
	}
	cout << m_iUInum << endl;
}

CSkyBox::CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype, int nMeshes)
	: CGameObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype, nMeshes)
{
	CTexturedRectMesh* pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 2000.0f, 2000.0f, 0.0f, 0.0f, 0.0f, +1000.0f);
	SetMesh(0, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 2000.0f, 2000.0f, 0.0f, 0.0f, 0.0f, -1000.0f);
	SetMesh(1, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 2000.0f, 2000.0f, -1000.0f, 0.0f, 0.0f);
	SetMesh(2, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 2000.0f, 2000.0f, +1000.0f, 0.0f, 0.0f);
	SetMesh(3, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 2000.0f, 0.0f, 2000.0f, 0.0f, +1000.0f, 0.0f);
	SetMesh(4, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 2000.0f, 0.0f, 2000.0f, 0.0f, -1000.0f, 0.0f);
	SetMesh(5, pSkyBoxMesh);

	CTexture* pSkyBoxTexture = new CTexture(6, RESOURCE_TEXTURE2D, 0, 1);

	pSkyBoxTexture->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Front_0.dds", RESOURCE_TEXTURE2D, 0);
	pSkyBoxTexture->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Back_0.dds", RESOURCE_TEXTURE2D, 1);
	pSkyBoxTexture->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Left_0.dds", RESOURCE_TEXTURE2D, 2);
	pSkyBoxTexture->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Right_0.dds", RESOURCE_TEXTURE2D, 3);
	pSkyBoxTexture->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Top_0.dds", RESOURCE_TEXTURE2D, 4);
	pSkyBoxTexture->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Bottom_0.dds", RESOURCE_TEXTURE2D, 5);

	if (m_pMaterial) {
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 6);
			m_pMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 0, 4);
			m_pMaterial->SetTexture(pSkyBoxTexture);
		}
	}
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption)
{

	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	OnPrepareRender();
	UpdateShaderVariables(pd3dCommandList, &m_xmf4x4World);

	if (m_pMaterial && m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_pMaterial && m_pMaterial->m_pTexture) m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0, i);
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}

}

CTreeObject::CTreeObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFBXLoader* pFBXLoader, const char* fileName, ShaderType stype) :CFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pFBXLoader, fileName, stype)
{
	CTexture* ppTextures[1];

	ppTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[0]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/tree_texture.png", RESOURCE_TEXTURE2D, 0);

	if (m_pMaterial) {
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
			m_pMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, ppTextures[0], 0, 4);
			m_pMaterial->SetTexture(ppTextures[0]);
		}
	}
}

CTreeObject::~CTreeObject()
{
}

CBillBoardInstanceObject::CBillBoardInstanceObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, ShaderType stype, int nMeshes)
	: CGameObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype, nMeshes)
{

	// 빌보드 텍스처들 생성------------------------------------------------------------------
	CTexture* pBillboardTexture = new CTexture(7, RESOURCE_TEXTURE2D_ARRAY, 0, 1);
	pBillboardTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Grass01.dds", RESOURCE_TEXTURE2D, 0);
	pBillboardTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Grass02.dds", RESOURCE_TEXTURE2D, 1);
	pBillboardTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Flower01.dds", RESOURCE_TEXTURE2D, 2);
	pBillboardTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Flower02.dds", RESOURCE_TEXTURE2D, 3);
	pBillboardTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Tree01.dds", RESOURCE_TEXTURE2D, 4);
	pBillboardTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Tree02.dds", RESOURCE_TEXTURE2D, 5);
	pBillboardTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Tree03.dds", RESOURCE_TEXTURE2D, 6);

	if (m_pMaterial) {
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 7);
			m_pMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, pBillboardTexture, 0, 9);
			m_pMaterial->SetTexture(pBillboardTexture);
		}
	}

	// 빌보드 메쉬 생성 --------------------------------------------------------------------
	CTexturedVertex pVertices[6];
	pVertices[0] = CTexturedVertex(XMFLOAT3(-400.0f, +400.0f, 1.0f), XMFLOAT2(0.0f, 0.0f));
	pVertices[1] = CTexturedVertex(XMFLOAT3(+400.0f, +400.0f, 1.0f), XMFLOAT2(1.0f, 0.0f));
	pVertices[2] = CTexturedVertex(XMFLOAT3(+400.0f, -400.0f, 1.0f), XMFLOAT2(1.0f, 1.0f));
	pVertices[3] = CTexturedVertex(XMFLOAT3(-400.0f, +400.0f, 1.0f), XMFLOAT2(0.0f, 0.0f));
	pVertices[4] = CTexturedVertex(XMFLOAT3(+400.0f, -400.0f, 1.0f), XMFLOAT2(1.0f, 1.0f));
	pVertices[5] = CTexturedVertex(XMFLOAT3(-400.0f, -400.0f, 1.0f), XMFLOAT2(0.0f, 1.0f));

	// 버텍스 버퍼 생성 이유 -> 기존 메쉬 렌더부분에서는 버텍스버퍼 하나만 사용 하지만 해당 빌보드는 인스턴스 오브젝트 이므로 인스턴스버퍼 뷰도 같이 넘겨준 후 렌더해야함
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, sizeof(CTexturedVertex) * 6, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = sizeof(CTexturedVertex);
	m_d3dVertexBufferView.SizeInBytes = sizeof(CTexturedVertex) * 6;

	// 인스턴스 오브젝트 개수 초기화
	m_nInstances = 25;

	// 빌보드 인스턴스 구조체 생성 // 인스턴스 버퍼,버퍼뷰 만들기 위함
	VS_VB_BILLBOARD_INSTANCE* pInstanceInfos = new VS_VB_BILLBOARD_INSTANCE[m_nInstances];

	// 터레인 불러옴 높이값 초기화하기위함
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	//XMFLOAT3 xmf3TerrainScale = pTerrain->GetScale();

	int nBillboardType = 0; 
	int nTextureType = 0; //1:Grass0, 2:Grass1, 3:Flower0, 4:Flower1, 5:Tree1, 6: Tree2, 7: Tree3
	float fxWidth = 0.0f, fyHeight = 0.0f;

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<> uid(1, 7);

	float xpitch = 257.0f * 24.0f / 5.0f;
	float zpitch = 257.0f * 24.0f / 3.0f;

	int nObjects = 0;
	for (int x = 0; x < 5; x++) {
		for (int z = 0; z < 5; z++) {
			fxWidth = 400.0f;
			fyHeight = 400.0f;

			nBillboardType = 0;
			nTextureType = uid(dre);
			switch (nTextureType)
			{
			case 1:fxWidth = 100; fyHeight = 100; break;
			case 2:fxWidth = 400; fyHeight = 300; break;
			case 3:fxWidth = 100; fyHeight = 200; break;
			case 4:fxWidth = 100; fyHeight = 200; break;
			case 5:fxWidth = 200; fyHeight = 300; break;
			case 6:fxWidth = 2000; fyHeight = 3000; break;
			case 7:fxWidth = 1600; fyHeight = 3000; break;
				
			}

			float xPosition = x * xpitch;
			float zPosition = z * zpitch;

			float fyOffset = fyHeight * 0.5f;
			float fHeight = pTerrain->GetHeight(xPosition, zPosition);

			pInstanceInfos[nObjects].m_xmf3Position = XMFLOAT3(xPosition, fHeight + fyOffset, zPosition);
			pInstanceInfos[nObjects++].m_xmf4BillboardInfo = XMFLOAT4(fxWidth, fyHeight, float(nBillboardType), float(nTextureType));
		}
	}
	m_pd3dInstancesBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pInstanceInfos, sizeof(VS_VB_BILLBOARD_INSTANCE) * m_nInstances, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dInstanceUploadBuffer);

	m_d3dInstancingBufferView.BufferLocation = m_pd3dInstancesBuffer->GetGPUVirtualAddress();
	m_d3dInstancingBufferView.StrideInBytes = sizeof(VS_VB_BILLBOARD_INSTANCE);
	m_d3dInstancingBufferView.SizeInBytes = sizeof(VS_VB_BILLBOARD_INSTANCE) * m_nInstances;

	if (pInstanceInfos) delete[] pInstanceInfos;

}

CBillBoardInstanceObject::~CBillBoardInstanceObject()
{
	if (m_pMaterial) m_pMaterial->Release();
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dInstancesBuffer) m_pd3dInstancesBuffer->Release();
}

void CBillBoardInstanceObject::ReleaseUploadBuffers()
{
	CGameObject::ReleaseUploadBuffers();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	if (m_pd3dInstanceUploadBuffer) m_pd3dInstanceUploadBuffer->Release();
}

void CBillBoardInstanceObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption)
{

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		}

		if (m_pMaterial->m_pTexture)
		{
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}
	}
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[] = { m_d3dVertexBufferView, m_d3dInstancingBufferView };
	pd3dCommandList->IASetVertexBuffers(0, _countof(pVertexBufferViews), pVertexBufferViews);
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->DrawInstanced(6, m_nInstances, 0, 0);
}

void CBillBoardInstanceObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CBillBoardInstanceObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CBillBoardInstanceObject::ReleaseShaderVariables()
{
}

