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
}


CGameObject::CGameObject(int nMeshes)
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


void CGameObject::SetMesh(int nIndex, CMesh* pMesh)
{
	if (m_ppMeshes == NULL || nIndex >= m_nMeshes)
		SetMesh(nIndex + 1);
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();

		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
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
	UpdateShaderVariables(pd3dCommandList, &m_xmf4x4World);


	//if (m_pMaterial)
	//{
	//	if (m_pMaterial->m_pShader)
	//	{
	//		m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
	//		m_pMaterial->m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
	//	}
	//}


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
}

void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	//객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다. 
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
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

void CGameObject::SetShaderType(ShaderType shaderType)
{
	if (!m_pMaterial)
	{
		m_pMaterial = new CMaterial();
		m_pMaterial->AddRef();
	}

	if (m_pMaterial) m_pMaterial->SetShaderType(shaderType); 
}



CInteractiveObject::CInteractiveObject(int nMeshes) : CGameObject(nMeshes)
{
	XMStoreFloat4x4(&m_xmf4x4Rotate, XMMatrixIdentity());
	CGameObject::SetShaderType(ShaderType::CObjectsShader);
}

CInteractiveObject::~CInteractiveObject()
{
}


XMFLOAT3 CInteractiveObject::GetAABBMaxPos(int nIndex) {
	XMFLOAT3 pos = ((CColliderMesh*)(m_ppMeshes[nIndex]))->GetCollider()->GetAABBMaxPos();
	XMStoreFloat3(&pos, XMVector3TransformCoord(XMLoadFloat3(&pos), XMLoadFloat4x4(&m_xmf4x4World)));
	return pos;
}

XMFLOAT3 CInteractiveObject::GetAABBMinPos(int nIndex) {
	XMFLOAT3 pos = ((CColliderMesh*)(m_ppMeshes[nIndex]))->GetCollider()->GetAABBMinPos();
	XMStoreFloat3(&pos, XMVector3TransformCoord(XMLoadFloat3(&pos), XMLoadFloat4x4(&m_xmf4x4World)));
	return pos;
}

bool CInteractiveObject::CollisionCheck(CGameObject* pOtherObject)
{
	if (CInteractiveObject* pInteractiveObject = dynamic_cast<CInteractiveObject*>(pOtherObject)) { // OtherObject가 InteractiveObject 라면

		XMFLOAT3 aMax = GetAABBMaxPos(0);
		XMFLOAT3 aMin = GetAABBMinPos(0);
		XMFLOAT3 bMax = pInteractiveObject->GetAABBMaxPos(0);
		XMFLOAT3 bMin = pInteractiveObject->GetAABBMinPos(0);

		if (aMax.x < bMin.x || aMin.x > bMax.x) return false;
		if (aMax.y < bMin.y || aMin.y > bMax.y) return false;
		if (aMax.z < bMin.z || aMin.z > bMax.z) return false;

		return true;
	}
	else {
		// InteractiveObject가 아니라면 충돌체크를 하지 않는다.
		return false;
	}
}


CDynamicShapeObject::CDynamicShapeObject(int nMeshes) : CInteractiveObject(nMeshes)
{
}

CDynamicShapeObject::~CDynamicShapeObject()
{
}

bool CDynamicShapeObject::CollisionCheck(CGameObject* pOtherObject)
{
	// 충돌체크만 한다면 아래의 복잡한 처리가 필요 없다.
	return CInteractiveObject::CollisionCheck(pOtherObject);

	if (CInteractiveObject* pInteractiveObject = dynamic_cast<CInteractiveObject*>(pOtherObject)) { // OtherObject가 InteractiveObject 라면
		if (CDynamicShapeObject* pDynamicShapeObject = dynamic_cast<CDynamicShapeObject*>(pOtherObject)) { // OtherObject가 DynamicShapeObject 라면
			// 절단할 수 있는 충돌체크를 한다.
			if (m_bAllowCutting && pDynamicShapeObject->GetCuttable()) {
				// 내가 다른 오브젝트를 자르는 처리는 나중에
				return true;
			}
			else if (m_bCuttable && pDynamicShapeObject->GetAllowCutting()) {
				// 다른 오브젝트에 의해 내가 잘릴 수 있을 때
				return true; // 이 함수는 충돌 체크 함수이니 외부에서 처리하자
			}
			else {
				// 둘다 절단이 일어날 수 없다면 단순 충돌체크만 한다.
				return CInteractiveObject::CollisionCheck(pOtherObject);
			}
		}
		else {
			// DynamicShapeObject가 아니라면 단순 충돌체크만 한다.
			return CInteractiveObject::CollisionCheck(pOtherObject);
		}
	}
	else {
		// InteractiveObject가 아니라면 충돌체크를 하지 않는다.
		return false;
	}
}

CGameObject** CDynamicShapeObject::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, CGameObject* pCutterObject)
{
	CDynamicShapeObject* pDynamicShapeObject = static_cast<CDynamicShapeObject*>(pCutterObject);

	int nCutterMeshes = pDynamicShapeObject->GetNumMeshes();
	CCutterMesh** ppCutterMeshes = (CCutterMesh**)pDynamicShapeObject->GetMeshes();
	CCutterMesh** ppMeshes = (CCutterMesh**)m_ppMeshes;
	XMFLOAT4X4 pxmfCutterMat = pDynamicShapeObject->GetWorldMat();

	// 절단 후 생성될 Mesh들을 받기위한 데이터 정의
	vector<CMesh**> newMeshs;
	XMFLOAT4X4 newWorldMat = Matrix4x4::Identity();
	XMFLOAT4X4 newTransformMat = Matrix4x4::Identity();

	if (m_bCuttable && pDynamicShapeObject->GetAllowCutting()) {
		// 다른 오브젝트에 의해 내가 잘릴 수 있을 때

		for (int i = 0; i < m_nMeshes; ++i) {
			for (int j = 0; j < nCutterMeshes; ++j) {
				if (ppMeshes[i]->CollisionCheck(ppCutterMeshes[j])) { // 두 오브젝트가 충돌하면 DynamicShaping을 시도한다.
					CMesh** m = ppMeshes[i]->DynamicShaping(pd3dDevice, pd3dCommandList, fTimeElapsed, m_xmf4x4World, ppCutterMeshes[j], pxmfCutterMat);
					if (NULL != m) newMeshs.push_back(m);
				}
			}
		}
		if (!newMeshs.empty()) {
			newWorldMat = GetWorldMat();
			newTransformMat = GetTransMat();
		}
	}

	// 만약 절단이 일어나 새로운 Mesh가 생겼다면 2개의 오브젝트를 생성한다.
	if (newMeshs.size() > 0) {
		CGameObject** newGameObjects = new CGameObject * [2];
		newGameObjects[0] = new CDynamicShapeObject();
		newGameObjects[1] = new CDynamicShapeObject();

		int LoopCounter = 0;
		for (auto& a : newMeshs) {
			newGameObjects[0]->SetMesh(LoopCounter, a[0]);
			newGameObjects[1]->SetMesh(LoopCounter, a[1]);
		}

		newGameObjects[0]->SetWorldMat(newWorldMat);
		newGameObjects[0]->SetTransMat(newTransformMat);
		newGameObjects[0]->SetShaderType(ShaderType::CObjectsShader);
		((CDynamicShapeObject*)newGameObjects[0])->SetCuttable(true);

		newGameObjects[1]->SetWorldMat(newWorldMat);
		newGameObjects[1]->SetTransMat(newTransformMat);
		newGameObjects[1]->SetShaderType(ShaderType::CObjectsShader);
		((CDynamicShapeObject*)newGameObjects[1])->SetCuttable(true);

		return newGameObjects;
	}
	return NULL;
}


CRotatingObject::CRotatingObject(int nMeshes) : CGameObject(nMeshes)
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
	CGameObject::SetShaderType(ShaderType::CObjectsShader);

}

CRotatingObject::~CRotatingObject()
{
}

CRotatingNormalObject::CRotatingNormalObject(int nmeshes)
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
	CGameObject::SetShaderType(ShaderType::CObjectNormalShader);
}

CRotatingNormalObject::~CRotatingNormalObject()
{
}

void CRotatingNormalObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbGameObjectBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbGameObject->Map(0, NULL, (void**)&m_pcbMappedGameObject);
}

void CRotatingNormalObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	m_pcbMappedGameObject->m_nMaterial = m_pMaterial->m_nReflection;

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbGameObject->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(1, d3dGpuVirtualAddress);
}

void CRotatingNormalObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}
}


void CRotatingObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);

	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}


CFBXObject::CFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, const char* fileName, ShaderType shaderType) : CDynamicShapeObject(0)//  모델에 mesh가 있으면 LoadContent에서 증가시킨다.
{
	if (NULL == fileName) {
		SetShaderType(shaderType);
		return;
	}
	//------------------------------------------------------------------------------------------
	//FbxManager* plSdkManager = NULL;
	//FbxScene* plScene = NULL;
	LoadResult lResult;

	//InitializeSdkObjects(plSdkManager, plScene);

	FbxString lFilePath(fileName);

	if (lFilePath.IsEmpty())
	{
		lResult = LoadResult::False;
#ifdef _DEBUG
		FBXSDK_printf("\n\nUsage: ImportScene <FBX file name>\n\n");
#endif // _DEBUG
	}
	else
	{
#ifdef _DEBUG
		//FBXSDK_printf("\n\nFile: %s\n\n", lFilePath.Buffer());
#endif // _DEBUG
		lResult = pFBXLoader->LoadScene(lFilePath.Buffer(), pFBXLoader);
	}


	if (lResult == LoadResult::False)
	{
#ifdef _DEBUG
		FBXSDK_printf("\n\nAn error occurred while loading the scene...\n");
#endif // _DEBUG

	}
	else if (lResult == LoadResult::First)
	{
		LoadContent(pd3dDevice, pd3dCommandList, pFBXLoader, lFilePath.Buffer());
		LoadHierarchy(pFBXLoader, lFilePath.Buffer()); // 이 함수가 true 를 반환해야 본이 있는것. false를 반환하면 없어야함.
		LoadHierarchyFromMesh();
	}
	else if (lResult == LoadResult::Overlapping)
	{
		// 일단 아직 위와 동일한 코드를 넣어놓는다.
		LoadContent(pd3dDevice, pd3dCommandList, pFBXLoader, lFilePath.Buffer());
	}

	//------------------------------------------------------------------------------------------

	SetShaderType(shaderType);
}

CFBXObject::~CFBXObject()
{
}

void CFBXObject::LoadContent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, const char* filePath)
{
	int i;
	FbxNode* lNode = pFBXLoader->GetNode(filePath);
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
	int p=0;

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
	FbxNode* lNode = pFBXLoader->GetNode(fileName);

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
		if(m_skelRoot)
			m_skelRoot->LoadHierarchyFromMesh(pMesh);
	}

}

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

void CFBXObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	CGameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}


CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName,
	int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(0)
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

	CGameObject::SetShaderType(ShaderType::CTerrainShader);
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

CRayObject::CRayObject() : CInteractiveObject()
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

CUIObject::CUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName, ShaderType shaderType) : CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, fileName, shaderType)
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

CCardUIObject::CCardUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName, ShaderType shaderType) : CUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, pCamera, fileName, shaderType)
{
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
			m_fCurrntScale += fTimeElapsed* TIME_ELAPSE_RATIO;
			if(m_fTargetScale < m_fCurrntScale)
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
	// 만약 선택상태라면
		// 현재 위치를 검사해 작동한다.

		// Hand Area

		// Play Area
	
		
	// 항상 선택 상태가 해제된다.
}
