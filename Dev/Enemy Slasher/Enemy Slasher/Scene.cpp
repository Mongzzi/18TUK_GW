#include "stdafx.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "ShaderManager.h"

CScene::CScene()
{
	m_pObjectManager = new CObjectManager;
	m_pShaderManager = new CShaderManager;
}

CScene::~CScene()
{
	delete m_pObjectManager;
	delete m_pShaderManager;
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_pShaderManager->BuildShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	// ------------------------------------       ť�� �޽�      -------------------------------

	//{
	//	//����x����x���̰� 12x12x12�� ������ü �޽��� �����Ѵ�. 
	//	CBoxMesh* pCubeMesh = new CBoxMesh(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);

	//	/*x-��, y-��, z-�� ���� ������ ��ü �����̴�. �� ���� 1�� �ø��ų� ���̸鼭 ������ �� ������ ����Ʈ�� ���
	//	���ϴ� ���� ���캸�� �ٶ���.*/
	//	int xObjects = 10, yObjects = 10, zObjects = 10, i = 0;

	//	//x-��, y-��, z-������ 21���� �� 21 x 21 x 21 = 9261���� ������ü�� �����ϰ� ��ġ�Ѵ�.
	//	int m_nObjects;
	//	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);

	//	float fxPitch = 12.0f * 2.5f;
	//	float fyPitch = 12.0f * 2.5f;
	//	float fzPitch = 12.0f * 2.5f;

	//	CRotatingObject* pRotatingObject = NULL;
	//	for (int x = -xObjects; x <= xObjects; x++)
	//	{
	//		for (int y = -yObjects; y <= yObjects; y++)
	//		{
	//			for (int z = -zObjects; z <= zObjects; z++)
	//			{
	//				pRotatingObject = new CRotatingObject();
	//				pRotatingObject->SetMesh(0, pCubeMesh);

	//				//�� ������ü ��ü�� ��ġ�� �����Ѵ�. 
	//				pRotatingObject->SetPosition(fxPitch * x, fyPitch * y, fzPitch * z);
	//				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	//				pRotatingObject->SetRotationSpeed(10.0f * (i++ % 10) + 3.0f);
	//				m_pObjectManager->AddObj(pRotatingObject, ObjectLayer::Object);
	//			}
	//		}
	//	}
	//}

	// ------------------------------------       �ͷ���      -------------------------------
	//������ Ȯ���� ������ �����̴�. x-��� z-���� 8��, y-���� 2�� Ȯ���Ѵ�. 
	XMFLOAT3 xmf3Scale(8.0f, 2.0f, 8.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.2f, 0.0f, 0.0f);

	//������ ���� �� �̹��� ����(HeightMap.raw)�� ����Ͽ� �����Ѵ�. ���� ���� ũ��� ����x����(257x257)�̴�. 

#ifdef _WITH_TERRAIN_PARTITION
	/*�ϳ��� ���� �޽��� ũ��� ����x����(17x17)�̴�. ���� ��ü�� ���� �������� 16��, ���� �������� 16�� ���� �޽��� ������. ������ �����ϴ� ���� �޽��� ������ �� 256(16x16)���� �ȴ�.*/
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("HeightMap.raw"), 257, 257, 17, 17, xmf3Scale, xmf4Color);

#else
//������ �ϳ��� ���� �޽�(257x257)�� �����Ѵ�. 
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("HeightMap.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color);

#endif
	float fTerrainWidth = m_pTerrain->GetWidth(), fTerrainLength = m_pTerrain->GetLength();
	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	//������ü�� ���� ǥ�鿡 �׸��� �������� ���� ��ġ�� ������ �������� ��ġ�Ѵ�. 
	int xObjects = int(fTerrainWidth / fxPitch), yObjects = 2, zObjects = int(fTerrainLength / fzPitch);

	CBoxMesh* pCubeMesh = new CBoxMesh(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject* pRotatingObject = NULL;

	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject(1);
				pRotatingObject->SetMesh(0, pCubeMesh);

				float xPosition = x * fxPitch;
				float zPosition = z * fzPitch;
				float fHeight = m_pTerrain->GetHeight(xPosition, zPosition);
				pRotatingObject->SetPosition(xPosition, fHeight + (y * 10.0f * fyPitch) +6.0f, zPosition);
				
				if (y == 0)
				{
					/*������ ǥ�鿡 ��ġ�ϴ� ������ü�� ������ ���⿡ ���� ������ �ٸ��� ��ġ�Ѵ�. ������ü�� ��ġ�� ������ ���� ���� ����� ������ü�� y-���� ��ġ�ϵ��� �Ѵ�.*/
					xmf3SurfaceNormal = m_pTerrain->GetNormal(xPosition, zPosition);

					XMFLOAT3 temp = XMFLOAT3(0.0f, 1.0f, 0.0f);
					xmf3RotateAxis = Vector3::CrossProduct(temp, xmf3SurfaceNormal);

					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);

					float fAngle = acos(Vector3::DotProduct(temp, xmf3SurfaceNormal));

					pRotatingObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
				}
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(36.0f * (i % 10) + 36.0f);
				m_pObjectManager->AddObj(pRotatingObject, ObjectLayer::Object);
			}
		}
	}


	//m_nShaders = 1;
	//m_pShaders = new CObjectsShader[m_nShaders];

	//m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);




	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	ReleaseShaderVariables();

	m_pObjectManager->ReleaseObjects();

	//if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[2];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;	// ���� ������Ʈ
	pd3dRootParameters[1].Constants.Num32BitValues = 16;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
	//m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	//m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	//::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	//::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CScene::ReleaseShaderVariables()
{
	//if (m_pd3dcbLights)
	//{
	//	m_pd3dcbLights->Unmap(0, NULL);
	//	m_pd3dcbLights->Release();
	//}

	//if (m_pTerrain) m_pTerrain->ReleaseShaderVariables();
	//if (m_pSkyBox) m_pSkyBox->ReleaseShaderVariables();
}

void CScene::ReleaseUploadBuffers()
{
	m_pObjectManager->ReleaseUploadBuffers();

	//if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
	//if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();

	//for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	//for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->ReleaseUploadBuffers();
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
			// m_ppGameObjects[0] �� ���� Player Object�� �ƴ�

			//case 'W': m_ppGameObjects[0]->MoveForward(+1.0f); break;
			//case 'S': m_ppGameObjects[0]->MoveForward(-1.0f); break;
			//case 'A': m_ppGameObjects[0]->MoveStrafe(-1.0f); break;
			//case 'D': m_ppGameObjects[0]->MoveStrafe(+1.0f); break;
			//case 'Q': m_ppGameObjects[0]->MoveUp(+1.0f); break;
			//case 'R': m_ppGameObjects[0]->MoveUp(-1.0f); break;

			// �׽�Ʈ�� �����ӵ� �̵� �ڵ�
		//case 'W': m_pPlayer->Move(DIR_FORWARD, 1.25f * 10, false); break;
		//case 'S': m_pPlayer->Move(DIR_BACKWARD, 1.25f * 10, false); break;
		//case 'A': m_pPlayer->Move(DIR_LEFT, 1.25f * 10, false); break;
		//case 'D': m_pPlayer->Move(DIR_RIGHT, 1.25f * 10, false); break;
		//case 'Q': m_pPlayer->Move(DIR_UP, 1.25f * 10, false); break;
		//case 'R': m_pPlayer->Move(DIR_DOWN, 1.25f * 10, false); break;

		//case '1': m_bRenderAABB = !m_bRenderAABB; break;

		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);

	// ������ �����Ӻ� �Է��� ó���� ���̹Ƿ� �����Ӹ��� Ű �Է� ó������ �� ��
	//return(true);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	m_pObjectManager->AnimateObjects(fTimeElapsed);
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	m_pObjectManager->Render(pd3dCommandList, pCamera, m_pShaderManager);



	//D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	//pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights
}