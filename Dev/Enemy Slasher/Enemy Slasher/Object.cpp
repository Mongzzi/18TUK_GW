#include "Object.h"
#include "stdafx.h"
#include "Shader.h"

CMaterial::CMaterial()
{
	m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

CMaterial::~CMaterial()
{
}


CGameObject::CGameObject(int nMeshes)
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
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
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();

		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}

void CGameObject::ReleaseUploadBuffers()
{
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��. 

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}

}

void CGameObject::Animate(float fTimeElapsed)
{
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption)
{
	OnPrepareRender();

	//��ü�� ������ ���̴� ����(��� ����)�� �����Ѵ�. 
	UpdateShaderVariables(pd3dCommandList);


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
	//��ü�� ���� ��ȯ ����� ��Ʈ ���(32-��Ʈ ��)�� ���Ͽ� ���̴� ����(��� ����)�� �����Ѵ�. 
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}


void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
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

//���� ��ü�� �־��� ������ ȸ���Ѵ�. 
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
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

//���� ��ü�� �־��� ������ ȸ���Ѵ�. 
void CInteractiveObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
	m_xmf4x4Rotate = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Rotate);
}

void CInteractiveObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
	m_xmf4x4Rotate = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Rotate);
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
	if (CInteractiveObject* pInteractiveObject = dynamic_cast<CInteractiveObject*>(pOtherObject)) { // OtherObject�� InteractiveObject ���

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
		// InteractiveObject�� �ƴ϶�� �浹üũ�� ���� �ʴ´�.
		return false;
	}
}


bool CDynamicShapeObject::CollisionCheck(CGameObject* pOtherObject)
{
	if (CInteractiveObject* pInteractiveObject = dynamic_cast<CInteractiveObject*>(pOtherObject)) { // OtherObject�� InteractiveObject ���
		if (CDynamicShapeObject* pDynamicShapeObject = dynamic_cast<CDynamicShapeObject*>(pOtherObject)) { // OtherObject�� DynamicShapeObject ���
			// ������ �� �ִ� �浹üũ�� �Ѵ�.
			if (m_bAllowCutting && pDynamicShapeObject->GetCuttable()) {

			}
			if (m_bCuttable && pDynamicShapeObject->GetAllowCutting()) {

			}

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
			// DynamicShapeObject�� �ƴ϶�� �ܼ� �浹üũ�� �Ѵ�.
			return CInteractiveObject::CollisionCheck(pOtherObject);
		}
	}
	else {
		// InteractiveObject�� �ƴ϶�� �浹üũ�� ���� �ʴ´�.
		return false;
	}
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
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbGameObjectBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbGameObject->Map(0, NULL, (void**)&m_pcbMappedGameObject);
}

void CRotatingNormalObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���

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


void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}


CFBXObject::CFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, const char* fileName) : CInteractiveObject(1)//  �𵨿� mesh�� �Ѱ��� �����ǵ���
{
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
		FBXSDK_printf("\n\nAn error occurred while loading the scene...");
#endif // _DEBUG

	}
	else if (lResult == LoadResult::First)
	{
		LoadContent(pd3dDevice, pd3dCommandList, pFBXLoader, lFilePath.Buffer());
	}
	else if (lResult == LoadResult::Overlapping)
	{
		LoadContent(pd3dDevice, pd3dCommandList, pFBXLoader, lFilePath.Buffer());
	}

	//------------------------------------------------------------------------------------------
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
			LoadContent(pd3dDevice, pd3dCommandList, lNode->GetChild(i));
		}
	}
}

void CFBXObject::LoadContent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pNode)
{
	FbxNodeAttribute::EType lAttributeType;
	int i;
	CFBXMesh* pFBXMesh = NULL;

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
			pFBXMesh->LoadMesh(pd3dDevice, pd3dCommandList, pNode);
			SetMesh(0, pFBXMesh);
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

	for (i = 0; i < pNode->GetChildCount(); i++)
	{
		LoadContent(pd3dDevice, pd3dCommandList, pNode->GetChild(i));
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
	// ���� �ϳ��� ������Ʈ�� �Ž��� �� �þ ��� �����ؾ���.
	CFBXMesh* tmp = (CFBXMesh*)m_ppMeshes[0];
	return tmp->GetAABB(m_xmf4x4World);
}

void CFBXObject::Animate(float fTimeElapsed)
{
}


CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName,
	int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(0)
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

	CGameObject::SetShaderType(ShaderType::CObjectsShader);
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
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
		Rotate(&fAxis, angle);
	}
	// ����� ����
	SetPosition(m_vOriginal);
#ifdef _DEBUG
	//std::cout << "m_xmf3Dir: " << ray.GetDir().x << ", " << ray.GetDir().y << ", " << ray.GetDir().z << std::endl;
	//std::cout << "m_vOriginal: " << ray.GetOriginal().x << ", " << ray.GetOriginal().y << ", " << ray.GetOriginal().z << std::endl;
#endif // _DEBUG
}

CUIObject::CUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName) : CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, fileName)
{
	m_fCurrntScale = m_fTargetScale = 1.0f;
	SetCamera(pCamera);
}

CUIObject::~CUIObject()
{
}

void CUIObject::ScreenSpaceToWorldSpace()
{
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

CCardUIObject::CCardUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName) : CUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, pCamera, fileName)
{
}

CCardUIObject::~CCardUIObject()
{
}

void CCardUIObject::Animate(float fTimeElapsed)
{
	ScreenSpaceToWorldSpace();

	if (std::fabs(m_fTargetScale - m_fTargetScale) > EPSILON)
	{
		if (m_fTargetScale > m_fTargetScale)
			m_fTargetScale += fTimeElapsed * 0.5;
		else
			m_fTargetScale -= fTimeElapsed * 0.5;
	}
}

void CCardUIObject::CursorOverObject(bool flag)
{
	if (flag)
	{
		m_fTargetScale = 1.2f;
	}
	else
	{
		m_fTargetScale = 1.0f;
	}
}

void CCardUIObject::ButtenDown()
{
	// ���� ���°� �ȴ�.
}

void CCardUIObject::ButtenUp()
{
	// ���� ���û��¶��
		// ���� ��ġ�� �˻��� �۵��Ѵ�.

		// Hand Area

		// Play Area
	
		
	// �׻� ���� ���°� �����ȴ�.
}
