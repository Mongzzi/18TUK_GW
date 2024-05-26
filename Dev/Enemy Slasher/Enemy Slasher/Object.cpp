#include "stdafx.h"
#include "Shader.h"
#include "Object.h"
#include "Player.h"

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
	m_xmf4x4InitialRotate = Matrix4x4::Identity();
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


void CGameObject::SetMesh(int nIndex, CMesh* pMesh, bool bMakeColliderFlag)
{
	if (m_ppMeshes == NULL || nIndex >= m_nMeshes)
		SetMesh(nIndex + 1);
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();

		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
	if (bMakeColliderFlag == true)
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

	m_nMeshes = nIndexSize;
	m_ppMeshes = NewMeshes;
}

void CGameObject::ReleaseUploadBuffers()
{
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��. 

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

void CGameObject::Animate(float fTimeTotal, float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pSibling) m_pSibling->Animate(fTimeTotal, fTimeElapsed, pxmf4x4Parent);
	if (m_pChild) m_pChild->Animate(fTimeTotal, fTimeElapsed, &m_xmf4x4World);
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption)
{
	OnPrepareRender();

	//��ü�� ������ ���̴� ����(��� ����)�� �����Ѵ�. 
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
	// �������� ������������ �ۼ� ���...

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

void CGameObject::RenderColliderMesh(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{

	if (m_ppMeshes)
	{
		UpdateShaderVariables(pd3dCommandList);

		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->RenderCollider(pd3dCommandList);
		}
	}
	
	if (m_pSibling) m_pSibling->RenderColliderMesh(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->RenderColliderMesh(pd3dCommandList, pCamera);
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
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
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
	////��ü�� ���� ��ȯ ����� ��Ʈ ���(32-��Ʈ ��)�� ���Ͽ� ���̴� ����(��� ����)�� �����Ѵ�. 
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
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(Matrix4x4::Multiply(m_xmf4x4InitialRotate, m_xmf4x4Transform), *pxmf4x4Parent) : Matrix4x4::Multiply(m_xmf4x4InitialRotate, m_xmf4x4Transform);

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
	case ShaderType::CAnimationObjectShader:
	{
		CAniamtionObjectsShader* pObjectsShader = new CAniamtionObjectsShader();
		pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		SetShader(pObjectsShader);
		break;
	}
	case ShaderType::C2DObjectShader:
	{
		C2DObjectShader* pObjectsShader = new C2DObjectShader();
		pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		SetShader(pObjectsShader);
		break;
	}
	case ShaderType::CTitleShader:
	{
		CTitleShader* pObjectsShader = new CTitleShader();
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
	CGameObject::SetScale(scale.x, scale.y, scale.z);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

//���� ��ü�� ���� z-�� ���͸� ��ȯ�Ѵ�. 
XMFLOAT3 CGameObject::GetLook()
{
	XMFLOAT3 a = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	return(Vector3::Normalize(a));
}

//���� ��ü�� ���� y-�� ���͸� ��ȯ�Ѵ�. 
XMFLOAT3 CGameObject::GetUp()
{

	XMFLOAT3 a = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	return(Vector3::Normalize(a));
}

//���� ��ü�� ���� x-�� ���͸� ��ȯ�Ѵ�. 
XMFLOAT3 CGameObject::GetRight()
{
	XMFLOAT3 a = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	return(Vector3::Normalize(a));
}

void CGameObject::SetLook(float x, float y, float z)
{
	XMVECTOR lookVector = XMVectorSet(x, y, z, 0.0f);
	lookVector = XMVector3Normalize(lookVector);

	XMVECTOR upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMVECTOR rightVector = XMVector3Cross(upVector, lookVector);
	rightVector = XMVector3Normalize(rightVector);

	upVector = XMVector3Cross(lookVector, rightVector);
	upVector = XMVector3Normalize(upVector);

	float scaleX = sqrtf(m_xmf4x4Transform._11 * m_xmf4x4Transform._11 + m_xmf4x4Transform._12 * m_xmf4x4Transform._12 + m_xmf4x4Transform._13 * m_xmf4x4Transform._13);
	float scaleY = sqrtf(m_xmf4x4Transform._21 * m_xmf4x4Transform._21 + m_xmf4x4Transform._22 * m_xmf4x4Transform._22 + m_xmf4x4Transform._23 * m_xmf4x4Transform._23);
	float scaleZ = sqrtf(m_xmf4x4Transform._31 * m_xmf4x4Transform._31 + m_xmf4x4Transform._32 * m_xmf4x4Transform._32 + m_xmf4x4Transform._33 * m_xmf4x4Transform._33);

	m_xmf4x4Transform._11 = rightVector.m128_f32[0] * scaleX;
	m_xmf4x4Transform._12 = rightVector.m128_f32[1] * scaleX;
	m_xmf4x4Transform._13 = rightVector.m128_f32[2] * scaleX;

	m_xmf4x4Transform._21 = upVector.m128_f32[0] * scaleY;
	m_xmf4x4Transform._22 = upVector.m128_f32[1] * scaleY;
	m_xmf4x4Transform._23 = upVector.m128_f32[2] * scaleY;

	m_xmf4x4Transform._31 = lookVector.m128_f32[0] * scaleZ;
	m_xmf4x4Transform._32 = lookVector.m128_f32[1] * scaleZ;
	m_xmf4x4Transform._33 = lookVector.m128_f32[2] * scaleZ;

	UpdateTransform(nullptr);
}

void CGameObject::SetLook(XMFLOAT3 xmf3Position)
{
	SetLook(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetUp(float x, float y, float z)
{
	m_xmf4x4World._21 = x;
	m_xmf4x4World._22 = y;
	m_xmf4x4World._23 = z;

	UpdateTransform(NULL);
}

void CGameObject::SetUp(XMFLOAT3 xmf3Position)
{
	SetUp(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetRight(float x, float y, float z)
{
	m_xmf4x4World._11 = x;
	m_xmf4x4World._12 = y;
	m_xmf4x4World._13 = z;

	UpdateTransform(NULL);
}

void CGameObject::SetRight(XMFLOAT3 xmf3Position)
{
	SetRight(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

//���� ��ü�� ���� x-�� �������� �̵��Ѵ�. 
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//���� ��ü�� ���� y-�� �������� �̵��Ѵ�. 
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

//���� ��ü�� ���� z-�� �������� �̵��Ѵ�. 
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MovePosition(float x, float y, float z)
{
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position.x += x;
	xmf3Position.y += y;
	xmf3Position.z += z;
	SetPosition(xmf3Position);
}

void CGameObject::MovePosition(XMFLOAT3 xmfPosition)
{
	MovePosition(xmfPosition.x, xmfPosition.y, xmfPosition.z);
}

//���� ��ü�� �־��� ������ ȸ���Ѵ�. 
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

void CGameObject::SetInitialRotate(float fPitch, float fYaw, float fRoll)
{
	// �ʱ� ȸ�� ���� XMFLOAT4X4 �������� �����մϴ�.
	m_xmf4x4InitialRotate = XMFLOAT4X4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	// XMFLOAT4X4 �������� ����� �ʱ� ȸ�� ���� ȸ�� ��ķ� ��ȯ�մϴ�.
	XMMATRIX mtxInitialRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));

	// ȸ�� ����� XMFLOAT4X4 �������� ��ȯ�Ͽ� m_xmf4x4InitialRotate�� �����մϴ�.
	XMStoreFloat4x4(&m_xmf4x4InitialRotate, mtxInitialRotate);

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
		m_pCollider->UpdateColliderWithOBB((m_ppMeshes[i])->GetCollider(), myWorldMat);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////

CFBXObject::CFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype)
	: CGameObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype)
{
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pcbMappedSkinningObject->m_bIsAvailable = false;
}

CFBXObject::~CFBXObject()
{

}

void CFBXObject::SetFbxData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFbx_V3::ObjectData* pObjectData)
{
	for (int i = 0; i < pObjectData->m_vpMeshs.size(); ++i) {
		CFBXMesh* pAnimMesh = new CFBXMesh(pd3dDevice, pd3dCommandList, pObjectData->m_vpMeshs[i]);
		SetMesh(i, pAnimMesh);
	}

	SetPosition(pObjectData->m_xmf3Translate);
	Rotate(pObjectData->m_xmf3Rotate.x, pObjectData->m_xmf3Rotate.y, pObjectData->m_xmf3Rotate.z);
	SetScale(pObjectData->m_xmf3Scale);

	if (pObjectData->m_pSkeleton) {
		m_pSkeletonData = pObjectData->m_pSkeleton;
	}
}


void CFBXObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_SKINNINGOBJECT_INFO) + 255) & ~255); //256�� ���
	m_pd3dcbSkinningObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbSkinningObject->Map(0, NULL, (void**)&m_pcbMappedSkinningObject);
}

void CFBXObject::ReleaseShaderVariables()
{
	if (m_pd3dcbSkinningObject)
	{
		m_pd3dcbSkinningObject->Unmap(0, NULL);
		m_pd3dcbSkinningObject->Release();
	}

	CGameObject::ReleaseShaderVariables();
}

void CFBXObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	static int animVal = 0;
	static int counter = 0;
	if (m_pSkeletonData && m_pSkeletonData->m_vAnimationNames.size() > 0 && true) {
		m_pcbMappedSkinningObject->m_bIsAvailable = true;
		if (m_pSkeletonData->m_mAnimations.size() > 0) {
			CFbx_V3::AnimationClip* currAnim = &m_pSkeletonData->m_mAnimations[m_pSkeletonData->m_vAnimationNames[0]];
			for (int i = 0; i < currAnim->m_vBoneAnimations.size(); ++i) {
				XMFLOAT4X4 mMat = Matrix4x4::Multiply(m_pSkeletonData->m_vxmf4x4BoneOffsetMat[i], currAnim->m_vBoneAnimations[i].m_vKeyFrames[animVal].m_xmf4x4AnimMat);
				XMStoreFloat4x4(&m_pcbMappedSkinningObject->m_xmf4x4BoneMat[i], XMMatrixTranspose(XMLoadFloat4x4(&mMat)));
			}
			if (counter >= 30) {
				animVal++;
				counter = 0;
				if (animVal >= currAnim->m_vBoneAnimations[0].m_vKeyFrames.size()) animVal = 0;
				cout << "Skeleton - AnimVal : " << animVal << "\n";
			}
			counter++;
		}
	}

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pd3dcbSkinningObject->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(10, d3dcbGameObjectGpuVirtualAddress);

	CGameObject::UpdateShaderVariables(pd3dCommandList);
}

void CFBXObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption) {
	CGameObject::Render(pd3dCommandList, pCamera, pRenderOption);
}


///////////////////////////////////////////////////////////////////////////////////////////////////


CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName,
	int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, ShaderType stype)
	: CGameObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype)
{
	//������ ����� ���� ���� ����, ������ ũ���̴�.
	m_nWidth = nWidth;
	m_nLength = nLength;

	/*���� ��ü�� ���� �޽����� �迭�� ���� ���̴�. nBlockWidth, nBlockLength�� ���� �޽� �ϳ��� ����, ���� ũ
	���̴�. cxQuadsPerBlock, czQuadsPerBlock�� ���� �޽��� ���� ����� ���� ���� �簢���� �����̴�.*/

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	//xmf3Scale�� ������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ����. 
	m_xmf3Scale = xmf3Scale;

	//������ ����� ���� ���� �����Ѵ�. 
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	//�������� ���� ����, ���� �������� ���� �޽��� �� ���� �ִ� ���� ��Ÿ����. 
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� �����̴�. 
	m_nMeshes = cxBlocks * czBlocks;

	//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� ���� ������ �迭�� �����Ѵ�. 
	m_ppMeshes = new CMesh * [m_nMeshes];

	for (int i = 0; i < m_nMeshes; i++)m_ppMeshes[i] = NULL;
	CHeightMapGridMesh* pHeightMapGridMesh = NULL;

	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� ���� ��ġ(��ǥ)�̴�. 
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);

			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� �����Ͽ� ���� �޽��� �����Ѵ�. 
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



CCharacterObject::CCharacterObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType shaderType) :CFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, shaderType)
{
	m_pDeck = new CDeckData();
	m_iTurnSpeed = 5.;
	m_fMaxHp = m_fCurHp = 100;
	m_fAtk = 10.f;
	m_iTeamId = 0;
}

CCharacterObject::~CCharacterObject()
{
	if (m_pDeck)
		delete m_pDeck;
}

void CCharacterObject::Reset()
{
	m_fCurHp = m_fMaxHp;
	m_pDeck->InitializeDeck();
}

void CCharacterObject::StartTurn()
{
	m_iKarma = 0;
	// �и� ���� ������.
	m_pDeck->SendHandToUsedAll();
}

void CCharacterObject::TakeDamage(float atk) 
{
	m_fCurHp -= atk;
};
void CCharacterObject::Heal(float ratio)
{
	m_fCurHp += m_fMaxHp * ratio;
	if (m_fCurHp > m_fMaxHp)
		m_fCurHp = m_fMaxHp;
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

	// ����ȭ�� ���ͷ� ��ȯ
	vFrom = XMVector3Normalize(vFrom);
	vTo = XMVector3Normalize(vTo);

	// �� ���� ���
	XMVECTOR axis = XMVector3Cross(vFrom, vTo);

	if (!XMVector3Equal(axis, XMVectorZero()))
	{
		XMFLOAT3 fAxis;
		XMStoreFloat3(&fAxis, axis);

		// �� ���� ���� ���� ���
		float angle = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenNormals(vFrom, vTo)));

		// ����� ����
		XMStoreFloat4x4(&m_xmf4x4Transform, XMMatrixIdentity());
		Rotate(&fAxis, angle);
	}
	// ����� ����
	SetPosition(m_vOriginal);
#ifdef _DEBUG
	//std::cout << "m_xmf3Dir: " << ray.GetDir().x << ", " << ray.GetDir().y << ", " << ray.GetDir().z << std::endl;
	//std::cout << "m_vOriginal: " << ray.GetOriginal().x << ", " << ray.GetOriginal().y << ", " << ray.GetOriginal().z << std::endl;
#endif // _DEBUG
}

CUIObject::CUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CCamera* pCamera, ShaderType stype)
	: CFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype)
{
	m_xmfScale.z = m_xmfScale.y = m_xmfScale.z = 1.0;

	m_fCurrntScale = m_fTargetScale = 1.0f;
	SetCamera(pCamera);
}

CUIObject::~CUIObject()
{
}

void CUIObject::ScreenSpaceToWorldSpace()
{
	XMStoreFloat4x4(&m_xmf4x4Transform, XMMatrixIdentity());// �ʱ�ȭ

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

	// ī�޶��� ������� ������. ȸ��������
	// �̰� ����ϸ� ī�޶����� �ݴ븦 ���Ե�.
	// ī�޶���Look�� �ݴ�, ī�޶�UP���� ���� �ɵ�? �Ƹ�?
	XMStoreFloat4x4(&m_xmf4x4Transform, XMLoadFloat4x4(&viewInv));

	// ������ ����.
	SetScale(m_xmfScale);

	// ī�޶� �ٶ󺸰� ����°͵� ��������

	XMFLOAT3 position;
	XMStoreFloat3(&position, rayW);

	SetPosition(position);


	// ȸ���� �׻� ī�޶� ���ؾ���.


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

void Callback_0(CGameObject* self, std::vector<CCharacterObject*>& target) {
	cout << "Callback_0 : atk 1" << endl;
	CCharacterObject* selfObj = static_cast<CCharacterObject*>(self); //dynamic_cast ���
	// ���� �� ī�尡 ���ڸ����� �����̶�� 
	// 1. self�� ���ݷ��� ������ 
	int selfTeamId = selfObj->GetTeamId();
	float atk = selfObj->GetAtk();
	// 2. target���� ���ظ� ������. �׳� Ȯ�ο����� ������ ����.
	// ���� �ٸ� ��� ������ ���ظ� ���ҵ�
	// �̷� ī��� �൵ �Ⱦ�����
	for (CCharacterObject* targetObj : target)
	{
		if (targetObj->GetTeamId() != selfTeamId)
		{
			cout << "targetObj HP : " << targetObj->GetCurHp() << " -> ";
			targetObj->TakeDamage(atk);
			cout << targetObj->GetCurHp() << endl;
		}
	}
}

void Callback_1(CGameObject* self, std::vector<CCharacterObject*>& target) {
	cout << "Callback_1" << endl;
	// ���� �� ī�尡 �̵� �� �����̶�� 
	// 1. self�� �̵��ؾ��ϴ��� Ȯ��
	// 2. �̵��ؾ��ϸ� �̵��Ѵ�.
	// 3. target���� ���ظ� ������.
	// �� �ൿ ��� �Լ��� ����.
	// �̵��� �ؾ��Ѵٸ� �̵��ؾ��ϴ� ��ġ�� ����� ���� �߰�.
	// �̵��� �������� Ȯ���ؾ���
	// �̵��� �����ٸ� ������.
	// �� �ൿ �� �ִϸ��̼� ���
}

void Callback_2(CGameObject* self, std::vector<CCharacterObject*>& target) {
	cout << "Callback_2 : Heal" << endl;
	CCharacterObject* selfObj = static_cast<CCharacterObject*>(self);
	cout << "self HP : " << selfObj->GetCurHp() << " -> ";
	selfObj->Heal();
	cout << selfObj->GetCurHp() << endl;
}

void Callback_3(CGameObject* self, std::vector<CCharacterObject*>& target) {
	cout << "Callback_3" << endl;
}

void Callback_4(CGameObject* self, std::vector<CCharacterObject*>& target) {
	cout << "Callback_4" << endl;
}

CCardUIObject::CCardUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CCamera* pCamera, ShaderType stype)
	: CUIObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pCamera, stype)
{
	m_Card_Ui_Num = -1;
}

CCardUIObject::CCardUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CCamera* pCamera, int UInum, ShaderType stype)
	: CUIObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pCamera, stype)
{
	m_Card_Ui_Num = UInum;
	//CreateShaderVariables(pd3dDevice, pd3dCommandList);

	// �̰� ��� ���� ����°� �ƴ�?
	CTexture* ppTextures[5];

	ppTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[0]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/card_UV2.jpg", RESOURCE_TEXTURE2D, 0);

	ppTextures[1] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[1]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/attack2.jpg", RESOURCE_TEXTURE2D, 0);


	ppTextures[2] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[2]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/attack3.jpg", RESOURCE_TEXTURE2D, 0);

	ppTextures[3] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[3]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/attack4.jpg", RESOURCE_TEXTURE2D, 0);

	ppTextures[4] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[4]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/attack5.jpg", RESOURCE_TEXTURE2D, 0);
	//
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���


	if (m_pMaterial) {
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
			m_pMaterial->m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
			//m_pMaterial->m_pShader->CreateConstantBufferViews(pd3dDevice, 1, m_pd3dcbGameObject, ncbElementBytes);
			if (m_Card_Ui_Num <= 4 && m_Card_Ui_Num >= 0)
			{
				m_pMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, ppTextures[m_Card_Ui_Num], 0, 4);
				m_pMaterial->SetTexture(ppTextures[m_Card_Ui_Num]);
			}
			else
			{
				m_pMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, ppTextures[0], 0, 4);
				m_pMaterial->SetTexture(ppTextures[0]);
			}
			//SetCbvGPUDescriptorHandle(m_pMaterial->m_pShader->GetGPUCbvDescriptorStartHandle());
		}
	}

}

CCardUIObject::~CCardUIObject()
{
}

void CCardUIObject::Animate(float fTimeTotal, float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
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
	CGameObject::Animate(fTimeTotal, fTimeElapsed, pxmf4x4Parent);
}

void CCardUIObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
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

void CCardUIObject::CursorOverObject(bool flag)
{
	if (flag)
		m_fTargetScale = TARGET_SCALE;
	else
		m_fTargetScale = DEFUALT_SCALE;
}

void CCardUIObject::ButtenDown()
{
	// ���� ���°� �ȴ�.
}

void CCardUIObject::ButtenUp()
{
	// 
}

void CCardUIObject::SetFunc(CardCallbackFunction callback)
{
	m_callbackFunc = callback;
}

void CCardUIObject::CallFunc(CGameObject* self, std::vector<CCharacterObject*>& target)
{
	if (m_callbackFunc)
		m_callbackFunc(self, target);
	else
		cout << "�ݹ����" << endl;
}

void CCardUIObject::UpdateData(int num)
{
	m_Card_Ui_Num = num;
	// �ݹ��Լ��� �ٲ����.
	// �ӽ��ڵ�
	switch (num) {
	case 0: m_callbackFunc = Callback_0; break;
	case 1: m_callbackFunc = Callback_1; break;
	case 2: m_callbackFunc = Callback_2; break;
	case 3: m_callbackFunc = Callback_3; break;
	case 4: m_callbackFunc = Callback_4; break;
	default:
		break;
	}
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

	pSkyBoxTexture->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/pz.jpg", RESOURCE_TEXTURE2D, 0);
	pSkyBoxTexture->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/nz.jpg", RESOURCE_TEXTURE2D, 1);
	pSkyBoxTexture->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/nx.jpg", RESOURCE_TEXTURE2D, 2);
	pSkyBoxTexture->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/px.jpg", RESOURCE_TEXTURE2D, 3);
	pSkyBoxTexture->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/py.jpg", RESOURCE_TEXTURE2D, 4);
	pSkyBoxTexture->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/ny.jpg", RESOURCE_TEXTURE2D, 5);

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

CTreeObject::CTreeObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype) : CFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype)
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

	// ������ �ؽ�ó�� ����------------------------------------------------------------------
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

	// ������ �޽� ���� --------------------------------------------------------------------
	CTexturedVertex pVertices[6];
	pVertices[0] = CTexturedVertex(XMFLOAT3(-400.0f, +400.0f, 1.0f), XMFLOAT2(0.0f, 0.0f));
	pVertices[1] = CTexturedVertex(XMFLOAT3(+400.0f, +400.0f, 1.0f), XMFLOAT2(1.0f, 0.0f));
	pVertices[2] = CTexturedVertex(XMFLOAT3(+400.0f, -400.0f, 1.0f), XMFLOAT2(1.0f, 1.0f));
	pVertices[3] = CTexturedVertex(XMFLOAT3(-400.0f, +400.0f, 1.0f), XMFLOAT2(0.0f, 0.0f));
	pVertices[4] = CTexturedVertex(XMFLOAT3(+400.0f, -400.0f, 1.0f), XMFLOAT2(1.0f, 1.0f));
	pVertices[5] = CTexturedVertex(XMFLOAT3(-400.0f, -400.0f, 1.0f), XMFLOAT2(0.0f, 1.0f));

	// ���ؽ� ���� ���� ���� -> ���� �޽� �����κп����� ���ؽ����� �ϳ��� ��� ������ �ش� ������� �ν��Ͻ� ������Ʈ �̹Ƿ� �ν��Ͻ����� �䵵 ���� �Ѱ��� �� �����ؾ���
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, sizeof(CTexturedVertex) * 6, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = sizeof(CTexturedVertex);
	m_d3dVertexBufferView.SizeInBytes = sizeof(CTexturedVertex) * 6;

	// �ν��Ͻ� ������Ʈ ���� �ʱ�ȭ
	m_nInstances = 25;

	// ������ �ν��Ͻ� ����ü ���� // �ν��Ͻ� ����,���ۺ� ����� ����
	VS_VB_BILLBOARD_INSTANCE* pInstanceInfos = new VS_VB_BILLBOARD_INSTANCE[m_nInstances];

	// �ͷ��� �ҷ��� ���̰� �ʱ�ȭ�ϱ�����
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

//CBillBoardInstanceObject::CBillBoardInstanceObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype, void* pContext, int nMeshes)
//{
//}

CBillBoardInstanceObject::~CBillBoardInstanceObject()
{
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

CAttackRangeObject::CAttackRangeObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype, int nMeshes)
	: CGameObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype, nMeshes)
{
	CTexture* ppTextures[1];

	ppTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[0]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/a.jpg", RESOURCE_TEXTURE2D, 0);

	if (m_pMaterial) {
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
			m_pMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, ppTextures[0], 0, 4);
			m_pMaterial->SetTexture(ppTextures[0]);
		}
	}

	SetMesh(0, new AttackRangeMesh(pd3dDevice, pd3dCommandList, CHASE_DISTANCE-10.0f, CHASE_DISTANCE, 40));
	//SetMesh(0, new AttackRangeCircleMesh(pd3dDevice, pd3dCommandList,CHASE_DISTANCE, 40));
}

CAttackRangeObject::~CAttackRangeObject()
{
}



CMonsterObject::CMonsterObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CPlayer* ptestplayer,ShaderType stype)
	:CCharacterObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype)
{
	m_Monster_State = MonsterState::Default_State;
	m_pTestPlayer = ptestplayer;
	m_HpObject = new CHpbarObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, ShaderType::CTexture_Position_Texcoord_Shader);
	m_AttackRangeObject = new CAttackRangeObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, ShaderType::CTexture_Position_Texcoord_Shader);
}

CMonsterObject::~CMonsterObject()
{
}


void CMonsterObject::Animate(float fTimeTotal, float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution <float> urd(-1, 1);

	XMFLOAT3 player_position = m_pTestPlayer->GetPosition();
	XMFLOAT3 monster_position = GetPosition();

	float distance = sqrt(
		pow(monster_position.x - player_position.x, 2) +
		pow(monster_position.y - player_position.y, 2) +
		pow(monster_position.z - player_position.z, 2));


	// �躤�� ������ ������ ���� ������� �ݴ���ҽ� ������ ���� x
	if (m_HpObject) {
		float x = GetPosition().x;
		float y = GetPosition().y + 500.0f;
		float z = GetPosition().z;
		m_HpObject->SetPosition(x, y, z);
	}

	if (m_AttackRangeObject) {
		float x = GetPosition().x;
		float y = GetPosition().y + 100.0f;
		float z = GetPosition().z;
		m_AttackRangeObject->SetPosition(x, y, z);
	}

	if (m_Monster_State == MonsterState::Default_State) {
		SetSpeed(1.0);	// ��ŸŸ�� ���ؾ����� �����

		if (distance > CHASE_DISTANCE) {
			if ((int)(fTimeTotal / 2.0f) > (int)((fTimeTotal - fTimeElapsed) / 2.0f))
			{
				m_dir.x = urd(dre), m_dir.z = urd(dre);
				SetLook(m_dir);
				if (m_HpObject) {
					m_HpObject->SetLook(m_dir);
					m_HpObject->SetScale(m_CurHp / m_MaxHp, 1.0f, 1.0f);
				}
			}

			XMVECTOR vResult = XMVectorScale(XMLoadFloat3(&m_dir), m_speed);
			XMStoreFloat3(&m_dir, vResult);
			MovePosition(m_dir);
		}

		else if (distance > BATTLE_DISTANCE && distance < CHASE_DISTANCE) {
			SetState(MonsterState::Chase_State);
		}

		else if (distance < BATTLE_DISTANCE) {
			SetState(MonsterState::Battle_State);
		}

	}


	else if (m_Monster_State == MonsterState::Chase_State) {
		if (distance > CHASE_DISTANCE) {
			SetState(MonsterState::Default_State);
		}

		else if (distance > BATTLE_DISTANCE && distance < CHASE_DISTANCE) {
			m_CurHp -= 0.005f;

			SetSpeed(3.0f);
			// ���⺤�� ���Ϳ��� �÷��̾��
			XMFLOAT3 position_difference;
			position_difference.x = player_position.x - monster_position.x;
			position_difference.y = player_position.y - monster_position.y;
			position_difference.z = player_position.z - monster_position.z;

			// �������ͷ� ��ȯ ���� 
			float length = sqrt(position_difference.x * position_difference.x + position_difference.y * position_difference.y + position_difference.z * position_difference.z);
			m_dir.x = position_difference.x / length;
			m_dir.y = position_difference.y / length;
			m_dir.z = position_difference.z / length;

			SetLook(m_dir);
			if (m_HpObject) {
				m_HpObject->SetLook(m_dir);
				m_HpObject->SetScale((float)m_CurHp / m_MaxHp, 1.0f, 1.0f);
			}

			XMVECTOR vResult = XMVectorScale(XMLoadFloat3(&m_dir), m_speed);
			XMStoreFloat3(&m_dir, vResult);
			MovePosition(m_dir);
		}
		else if (distance < BATTLE_DISTANCE) {
			SetState(MonsterState::Battle_State);
		}
	}


	else if (m_Monster_State == MonsterState::Battle_State) {
		if (distance > BATTLE_DISTANCE && distance < CHASE_DISTANCE) {
			SetState(MonsterState::Chase_State);
		}
		else if (distance > CHASE_DISTANCE) {
			SetState(MonsterState::Default_State);
		}
		else if (distance < BATTLE_DISTANCE) {
			// �÷��̾� ���� ����
			//m_pTestPlayer->ChangeCamera(SPACESHIP_CAMERA, 0.0f);;

		}
	}

	CGameObject::Animate(fTimeTotal, fTimeElapsed, pxmf4x4Parent);
}



void CMonsterObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption)
{
	CGameObject::Render(pd3dCommandList, pCamera, pRenderOption);

	if (m_HpObject) {
		m_HpObject->Render(pd3dCommandList, pCamera, pRenderOption);
	}

	if (m_AttackRangeObject) {
		m_AttackRangeObject->Render(pd3dCommandList, pCamera, pRenderOption);
	}

}


CHpbarObject::CHpbarObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype, int nMeshes)
	: CGameObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype, nMeshes)
{
	CTexture* ppTextures[1];

	ppTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[0]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/hphpbar.png", RESOURCE_TEXTURE2D, 0);

	if (m_pMaterial) {
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
			m_pMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, ppTextures[0], 0, 4);
			m_pMaterial->SetTexture(ppTextures[0]);
		}
	}

	SetMesh(0, new CHpBarMesh(pd3dDevice, pd3dCommandList, 500.0f, 30.0f));
}

CHpbarObject:: ~CHpbarObject()
{

}

CTextObject::CTextObject(const WCHAR* text, const D2D1_RECT_F& rect, const WCHAR* fontName, float fontSize, D2D1::ColorF::Enum color)
: m_text(text), m_rect(rect), m_fontName(fontName), m_fontSize(fontSize), m_color(color)
{
}

CTextObject::~CTextObject()
{
}

void CTextObject::SetText(const WCHAR* text)
{
	m_text = text;
}

void CTextObject::SetPosition(const D2D1_RECT_F& rect)
{
	m_rect = rect;
}

void CTextObject::SetFont(const WCHAR* fontName, float fontSize)
{
	m_fontName = fontName;
	m_fontSize = fontSize;
}

void CTextObject::SetColor(D2D1::ColorF::Enum color)
{
	m_color = color;
}

void CTextObject::Render(ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory)
{ 
	if (!m_solidColorBrush)
	{
		DX::ThrowIfFailed(pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(m_color), m_solidColorBrush.GetAddressOf()));
	}

	if (!m_dWriteTextFormat)
	{
		DX::ThrowIfFailed(pdWriteFactory->CreateTextFormat(
			m_fontName.c_str(),
			nullptr,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			m_fontSize,
			L"en-us",
			m_dWriteTextFormat.GetAddressOf()));

		m_dWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_dWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
	pd2dDeviceContext->DrawText(m_text.c_str(), static_cast<UINT32>(m_text.length()), m_dWriteTextFormat.Get(), &m_rect, m_solidColorBrush.Get());
}


CButtonObject::CButtonObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, wchar_t* pszFileName, float x, float y, float width, float height, ShaderType stype, int nMeshes)
	: CGameObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype, nMeshes)
{
	m_x = x;
	m_y = y;    
	m_width = width;
	m_height = height;
	m_IsButton = true;
	m_IsClicked = false;

	CTexture* ppTextures[1];

	ppTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[0]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, pszFileName, RESOURCE_TEXTURE2D, 0);

	if (m_pMaterial) {
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
			m_pMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, ppTextures[0], 0, 4);
			m_pMaterial->SetTexture(ppTextures[0]);
		}
	}

	SetMesh(0, new CHpBarMesh(pd3dDevice, pd3dCommandList, width, height));
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CButtonObject::~CButtonObject()
{
}

void CButtonObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_2D_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
	m_pd3dcb2DGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcb2DGameObject->Map(0, NULL, (void**)&m_pcbMapped2DGameObject);
}

void CButtonObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pcbMapped2DGameObject->m_xmf2Position = XMFLOAT2(m_x, m_y);
	m_pcbMapped2DGameObject->m_xmfSize = XMFLOAT2(m_width, m_height);
	m_pcbMapped2DGameObject->m_IsClicked = m_IsClicked;

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pd3dcb2DGameObject->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(11, d3dcbGameObjectGpuVirtualAddress);

}

bool CButtonObject::IsPointInside(float x, float y)
{
	float halfWidth = m_width * 0.5f;
	float halfHeight = m_height * 0.5f;

	// �߽� ��ǥ���� ���� ũ�⸸ŭ �̵��� ���� ���� �ִ��� Ȯ��
	float left = m_x - halfWidth;
	float right = m_x + halfWidth;
	float top = m_y - halfHeight;
	float bottom = m_y + halfHeight;

	return (x >= left && x <= right && y >= top && y <= bottom);
}

void CButtonObject::ReleaseShaderVariables()
{
	if (m_pd3dcb2DGameObject)
	{
		m_pd3dcb2DGameObject->Unmap(0, NULL);
		m_pd3dcb2DGameObject->Release();
	}
	CGameObject::ReleaseShaderVariables();
}

CTitleObject::CTitleObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype, int nMeshes)
	: CGameObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, stype, nMeshes)
{
	CTexture* ppTextures[1];

	ppTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[0]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/title.png", RESOURCE_TEXTURE2D, 0);

	if (m_pMaterial) {
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
			m_pMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, ppTextures[0], 0, 4);
			m_pMaterial->SetTexture(ppTextures[0]);
		}
	}

	SetMesh(0, new C2DTextureMesh(pd3dDevice, pd3dCommandList, 2.0f, 2.0f));	// ��ũ����ǥ -1 ~ 1 ���� ���� 2
}

CTitleObject::~CTitleObject()
{
}
