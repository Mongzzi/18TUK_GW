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
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 

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

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	//객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다. 
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
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
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
	//객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다. 
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
	if (m_nMeshes > 0)
	{
		m_ppAABBMeshes = new CAABBMesh * [m_nMeshes + 1];
		for (int i = 0; i < m_nMeshes + 1; i++) m_ppAABBMeshes[i] = NULL;
	}
	CGameObject::SetShaderType(ShaderType::CObjectsShader);
}

CInteractiveObject::~CInteractiveObject()
{
	if (m_ppAABBMeshes)
	{
		for (int i = 0; i < m_nMeshes + 1; i++)
		{
			if (m_ppAABBMeshes[i]) m_ppAABBMeshes[i]->Release();
			m_ppAABBMeshes[i] = NULL;
		}
		delete[] m_ppAABBMeshes;
	}
}

void CInteractiveObject::SetMesh(int nIndex, CMesh* pMesh)
{
	if (m_ppAABBMeshes)
	{
		if (m_ppAABBMeshes[nIndex + 1]) m_ppAABBMeshes[nIndex + 1]->Release();

		//m_ppAABBMeshes[nIndex + 1] = pMesh;
		//if (pMesh) pMesh->AddRef();
	}
}

void CInteractiveObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{

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


void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}


CFBXObject::CFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, const char* fileName) : CGameObject(1)//  모델에 mesh가 한개만 생성되도록
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

bool CFBXObject::IsCursorOverObject()
{
	return false;
}

void CFBXObject::ButtenDown()
{
}

void CFBXObject::ButtenUp()
{
}

CAABB* CFBXObject::GetAABB()
{
	// 만약 하나의 오브젝트에 매쉬가 더 늘어날 경우 수정해야함.
	CFBXMesh* tmp = (CFBXMesh*)m_ppMeshes[0];
	return tmp->GetAABB(m_xmf4x4World);
}

void CFBXObject::Animate(float fTimeElapsed)
{
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
