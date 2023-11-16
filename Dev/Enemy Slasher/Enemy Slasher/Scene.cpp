#include "stdafx.h"
#include "Scene.h"
#include "FbxLoader.h"
#include "ObjectManager.h"
#include "ShaderManager.h"
#include "Ray.h"

CBasicScene::CBasicScene()
{
	m_pObjectManager = new CObjectManager;
	m_pShaderManager = new CShaderManager;
}

CBasicScene::~CBasicScene()
{
	delete m_pObjectManager;
	delete m_pShaderManager;
}

void CBasicScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_pShaderManager->BuildShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CBasicScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	ReleaseShaderVariables();

	m_pObjectManager->ReleaseObjects();

	//if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature* CBasicScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[2];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;	// 게임 오브젝트
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

void CBasicScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	//m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	//m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void CBasicScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	//::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	//::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CBasicScene::ReleaseShaderVariables()
{
	//if (m_pd3dcbLights)
	//{
	//	m_pd3dcbLights->Unmap(0, NULL);
	//	m_pd3dcbLights->Release();
	//}

	//if (m_pTerrain) m_pTerrain->ReleaseShaderVariables();
	//if (m_pSkyBox) m_pSkyBox->ReleaseShaderVariables();
}

void CBasicScene::ReleaseUploadBuffers()
{
	m_pObjectManager->ReleaseUploadBuffers();

	//if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
	//if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();

	//for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	//for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->ReleaseUploadBuffers();
}

bool CBasicScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CBasicScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CBasicScene::ProcessInput(HWND hWnd, UCHAR* pKeysBuffer, POINT ptOldCursorPos)
{
	return(false);

	// 씬에서 프레임별 입력을 처리할 것이므로 프레임마다 키 입력 처리하지 말 것
	//return(true);
}

void CBasicScene::AnimateObjects(float fTimeElapsed)
{
	m_pObjectManager->AnimateObjects(fTimeElapsed);
}

void CBasicScene::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	m_pObjectManager->DynamicShaping(pd3dDevice, pd3dCommandList, fTimeElapsed);
}

void CBasicScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	//if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	m_pObjectManager->Render(pd3dCommandList, pCamera, m_pShaderManager);



	//D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	//pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights
}

void CBasicScene::Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	m_pShaderManager->Render(pd3dCommandList, pCamera, ShaderType::CTextShader);

	D2D1_RECT_F textRect = D2D1::RectF(0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	static const WCHAR text[] = L"BasicScene의 Render2D 입니다.";

	ComPtr<ID2D1SolidColorBrush> mSolidColorBrush;
	ComPtr<IDWriteTextFormat> mDWriteTextFormat;

	DX::ThrowIfFailed(pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Aqua), mSolidColorBrush.GetAddressOf()));
	DX::ThrowIfFailed(pdWriteFactory->CreateTextFormat(
		L"Verdana",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		25,
		L"en-us",
		mDWriteTextFormat.GetAddressOf()));

	mDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	mDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
	pd2dDeviceContext->DrawText(text, _countof(text) - 1, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());
}

void CBasicScene::Enter()
{
}

void CBasicScene::Exit()
{
}



CTestScene::CTestScene()
{
}

CTestScene::~CTestScene()
{
}

bool CTestScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
		if (pCoveredUI)
		{
			pSelectedUI = pCoveredUI;
			pSelectedUI->ButtenDown();
		}
		break;
	case WM_RBUTTONUP:
		if (pSelectedUI)
		{
			pSelectedUI->ButtenUp();
			pSelectedUI = NULL;
		}
		break;
	default:
		break;
	}
	return false;
}

bool CTestScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		//case 'W': m_pPlayer->Move(DIR_FORWARD, m_pPlayer->GetMoveSpeed(), true); break;
		//case 'S': m_pPlayer->Move(DIR_BACKWARD, m_pPlayer->GetMoveSpeed(), true); break;
		//case 'A': m_pPlayer->Move(DIR_LEFT, m_pPlayer->GetMoveSpeed(), true); break;
		//case 'D': m_pPlayer->Move(DIR_RIGHT, m_pPlayer->GetMoveSpeed(), true); break;
		case 'Q': m_pPlayer->Move(DIR_UP, m_pPlayer->GetMoveSpeed(), true); break;
		case 'E': m_pPlayer->Move(DIR_DOWN, m_pPlayer->GetMoveSpeed(), true); break;
		case 'R': m_pPlayer->Rotate(0.0f, 20.0f, 0.0f);	break;
		case 'T': m_pPlayer->Rotate(0.0f, -20.0f, 0.0f); break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

void CTestScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_pShaderManager->BuildShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList);
	m_pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
	m_pPlayer->SetPosition(XMFLOAT3(0.0f, 2000.0f, 0.0f));
	m_pPlayer->SetGravity(XMFLOAT3(0.0f, -10.0f, 0.0f));
	m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);

	// ------------------------------------       큐브 메쉬      -------------------------------

//{
//	//가로x세로x높이가 12x12x12인 정육면체 메쉬를 생성한다. 
//	CBoxMesh* pCubeMesh = new CBoxMesh(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);

//	/*x-축, y-축, z-축 양의 방향의 객체 개수이다. 각 값을 1씩 늘리거나 줄이면서 실행할 때 프레임 레이트가 어떻게
//	변하는 가를 살펴보기 바란다.*/
//	int xObjects = 10, yObjects = 10, zObjects = 10, i = 0;

//	//x-축, y-축, z-축으로 21개씩 총 21 x 21 x 21 = 9261개의 정육면체를 생성하고 배치한다.
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

//				//각 정육면체 객체의 위치를 설정한다. 
//				pRotatingObject->SetPosition(fxPitch * x, fyPitch * y, fzPitch * z);
//				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
//				pRotatingObject->SetRotationSpeed(10.0f * (i++ % 10) + 3.0f);
//				m_pObjectManager->AddObj(pRotatingObject, ObjectLayer::Object);
//			}
//		}
//	}
//}

// ------------------------------------       터레인      -------------------------------
//지형을 확대할 스케일 벡터이다. x-축과 z-축은 8배, y-축은 2배 확대한다. 
	XMFLOAT3 xmf3Scale(24.0f, 6.0f, 24.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.2f, 0.0f, 0.0f);

	//지형을 높이 맵 이미지 파일(HeightMap.raw)을 사용하여 생성한다. 높이 맵의 크기는 가로x세로(257x257)이다. 
	CHeightMapTerrain* pTerrain;
#ifdef _WITH_TERRAIN_PARTITION
	/*하나의 격자 메쉬의 크기는 가로x세로(17x17)이다. 지형 전체는 가로 방향으로 16개, 세로 방향으로 16의 격자 메쉬를 가진다. 지형을 구성하는 격자 메쉬의 개수는 총 256(16x16)개가 된다.*/
	pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("HeightMap.raw"), 257, 257, 17, 17, xmf3Scale, xmf4Color);

#else
	//지형을 하나의 격자 메쉬(257x257)로 생성한다. 
	pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("a.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color);

#endif
	m_pObjectManager->AddObj(pTerrain, ObjectLayer::Terrain);

	//float fTerrainWidth = pTerrain->GetWidth(), fTerrainLength = pTerrain->GetLength();
	//float fxPitch = 12.0f * 3.5f;
	//float fyPitch = 12.0f * 3.5f;
	//float fzPitch = 12.0f * 3.5f;

	////직육면체를 지형 표면에 그리고 지형보다 높은 위치에 일정한 간격으로 배치한다. 
	//int xObjects = int(fTerrainWidth / fxPitch), yObjects = 1, zObjects = int(fTerrainLength / fzPitch);

	//CBoxMesh* pCubeMesh = new CBoxMesh(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);

	////CFBXMesh* pFBXMesh = new CFBXMesh(pd3dDevice, pd3dCommandList, plSdkManager, plScene, "fbxsdk/Stone_lit_001.fbx");
	////CFBXMesh* pFBXMesh2 = new CFBXMesh(pd3dDevice, pd3dCommandList, plSdkManager, plScene, "fbxsdk/box.fbx");

	//XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	////CRotatingObject* pRotatingObject = NULL;
	//CFBXObject* pRotatingObject = NULL;
	////CRotatingObject* pRotatingObject = NULL;

	////CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, STONE_LIT_001_FBX);	// box.fbx는 mesh가 3개라 지금 코드로는 버그 생김.
	////m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);

	//for (int i = 0, x = 0; x < 1; x++)
	//{
	//	for (int z = 0; z < zObjects; z++)
	//	{
	//		for (int y = 0; y < yObjects; y++)
	//		{
	//			if(x%2)pRotatingObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, AA2);
	//			else pRotatingObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, AA2);
	//			//if(x%2) pRotatingObject->SetMesh(0, pCubeMesh);
	//			//else pRotatingObject->SetMesh(0, pCubeMesh);

	//			float xPosition = x * fxPitch;
	//			float zPosition = z * fzPitch;
	//			float fHeight = pTerrain->GetHeight(xPosition, zPosition);
	//			pRotatingObject->SetPosition(xPosition, fHeight + (y * 10.0f * fyPitch) + 6.0f, zPosition);

	//			if (y == 0)
	//			{
	//				/*지형의 표면에 위치하는 직육면체는 지형의 기울기에 따라 방향이 다르게 배치한다. 직육면체가 위치할 지형의 법선 벡터 방향과 직육면체의 y-축이 일치하도록 한다.*/
	//				xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);

	//				XMFLOAT3 temp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//				xmf3RotateAxis = Vector3::CrossProduct(temp, xmf3SurfaceNormal);

	//				if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);

	//				float fAngle = acos(Vector3::DotProduct(temp, xmf3SurfaceNormal));

	//				pRotatingObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
	//			}
	//			//pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	//			//pRotatingObject->SetRotationSpeed(36.0f * (i % 10) + 36.0f);
	//			pRotatingObject->SetShaderType(ShaderType::CObjectsShader);
	//			m_pObjectManager->AddObj(pRotatingObject, ObjectLayer::Object);
	//		}
	//	}
	//}
	
	// house
	{
		CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, HOUSE_FBX);
		float xPosition = 257.0f / 4.0f * 24.0f;
		float zPosition = 257.0f / 4.0f * 24.0f;
		float fHeight = pTerrain->GetHeight(xPosition, zPosition);

		pFBXObject->SetPosition(xPosition, fHeight, zPosition);
		pFBXObject->SetShaderType(ShaderType::CObjectsShader);
		m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
	}

	// tree
	{
		CFBXObject* pFBXObject = NULL;


		float xpitch = 257.0f * 24.0f / 10.0f;
		float zpitch = 257.0f * 24.0f / 7.0f;

		for (int x = 0; x < 10; x++)
		{
			for (int z = 0; z < 20; z++)
			{
				if (x % 5 == 0) {
					CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, TREE1);
					float xPosition = x * xpitch;
					float zPosition = z * zpitch;
					float fHeight = pTerrain->GetHeight(xPosition, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->Rotate(90.0f, 0.0f, 0.0f);

					pFBXObject->SetShaderType(ShaderType::CObjectsShader);
					m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
				}
				else if (x % 5 == 1) {
					CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, TREE1);
					float xPosition = x * xpitch;
					float zPosition = z * zpitch;
					float fHeight = pTerrain->GetHeight(xPosition, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->Rotate(90.0f, 0.0f, 0.0f);

					pFBXObject->SetShaderType(ShaderType::CObjectsShader);
					m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
				}
				else if (x % 5 == 2) {
					CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, TREE5);
					float xPosition = x * xpitch;
					float zPosition = z * zpitch;
					float fHeight = pTerrain->GetHeight(xPosition, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->Rotate(90.0f, 0.0f, 0.0f);

					pFBXObject->SetShaderType(ShaderType::CObjectsShader);
					m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
				}
				else if (x % 5 == 3) {
					CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, TREE2);
					float xPosition = x * xpitch;
					float zPosition = z * zpitch;
					float fHeight = pTerrain->GetHeight(xPosition, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->Rotate(90.0f, 0.0f, 0.0f);

					pFBXObject->SetShaderType(ShaderType::CObjectsShader);
					m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
				}
				else if (x % 5 == 4) {
					CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, TREE3);
					float xPosition = x * xpitch;
					float zPosition = z * zpitch;
					float fHeight = pTerrain->GetHeight(xPosition, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->Rotate(90.0f, 0.0f, 0.0f);

					pFBXObject->SetShaderType(ShaderType::CObjectsShader);
					m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
				}
				else if (x % 5 == 0) {
					CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, TREE4);
					float xPosition = x * xpitch;
					float zPosition = z * zpitch;
					float fHeight = pTerrain->GetHeight(xPosition, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->Rotate(90.0f, 0.0f, 0.0f);
					pFBXObject->SetShaderType(ShaderType::CObjectsShader);
					m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
				}
			}
		}
	}


	{
		//카드 UI 테스트용 오브젝트.
		CCardUIObject* pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, m_pPlayer->GetCamera(), CARD_FBX);
		pCardUIObject->SetPositionUI(100, 100);
		pCardUIObject->SetShaderType(ShaderType::CObjectsShader);
		m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::UIObject);

		pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, m_pPlayer->GetCamera(), CARD_FBX);
		pCardUIObject->SetPositionUI(200, 200);
		pCardUIObject->SetShaderType(ShaderType::CObjectsShader);
		m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::UIObject);

		pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, m_pPlayer->GetCamera(), CARD_FBX);
		pCardUIObject->SetPositionUI(300, 300);
		pCardUIObject->SetShaderType(ShaderType::CObjectsShader);
		m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::UIObject);

		pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, m_pPlayer->GetCamera(), CARD_FBX);
		pCardUIObject->SetPositionUI(400, 400);
		pCardUIObject->SetShaderType(ShaderType::CObjectsShader);
		m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::UIObject);
	}

	CRay r = r.RayAtWorldSpace(0, 0, m_pPlayer->GetCamera());

	CRayObject* pRayObject = NULL;
	pRayObject = new CRayObject();
	pRayObject->SetMesh(0, new CRayMesh(pd3dDevice, pd3dCommandList, NULL));
	m_pObjectManager->AddObj(pRayObject, ObjectLayer::Ray);

	//m_nShaders = 1;
	//m_pShaders = new CObjectsShader[m_nShaders];

	//m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

bool CTestScene::ProcessInput(HWND hWnd, UCHAR* pKeysBuffer, POINT ptOldCursorPos)
{
	DWORD dwDirection = 0;
	if (pKeysBuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
	if (pKeysBuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
	if (pKeysBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
	if (pKeysBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;

	float cxDelta = 0.0f, cyDelta = 0.0f;
	int xDelta = 0, yDelta = 0;
	POINT ptCursorPos{ 0,0 }; //초기화를 하지 않을 시 낮은 확률로 아래의 if문에 진입하지 못하여 초기화되지 않은 값을 사용하게 된다.
	if (GetCapture() == hWnd)
	{
		//SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		xDelta = ptCursorPos.x - ptOldCursorPos.x;
		yDelta = ptCursorPos.y - ptOldCursorPos.y;
		cxDelta = (float)(xDelta) / 3.0f;
		cyDelta = (float)(yDelta) / 3.0f;
		//SetCursorPos(ptOldCursorPos.x, ptOldCursorPos.y);
	}

	GetCursorPos(&ptCursorPos);
	ScreenToClient(hWnd, &ptCursorPos);
	//-------------- 피킹
	CRay r = r.RayAtWorldSpace(ptCursorPos.x, ptCursorPos.y, m_pPlayer->GetCamera());

	std::vector<CGameObject*>* pObjectList = m_pObjectManager->GetObjectList();

	pCoveredUI = NULL;

	for (int lc = 0; lc < (int)ObjectLayer::Count;lc++)
	{
		if (lc == (int)ObjectLayer::UIObject)
		{
			for (int i = 0;i < pObjectList[lc].size();i++) {
				CUIObject* obj = (CUIObject*)pObjectList[lc][i];

				XMFLOAT3 aabbMax = obj->GetAABBMaxPos(0);
				XMFLOAT3 aabbMin = obj->GetAABBMinPos(0);
				XMFLOAT3 ray_dir = r.GetDir();
				XMFLOAT3 ray_origin = r.GetOriginal();
				XMFLOAT3 invDirection = XMFLOAT3(1.0f / ray_dir.x, 1.0f / ray_dir.y, 1.0f / ray_dir.z);

				float t1 = (aabbMin.x - ray_origin.x) * invDirection.x;
				float t2 = (aabbMax.x - ray_origin.x) * invDirection.x;
				float t3 = (aabbMin.y - ray_origin.y) * invDirection.y;
				float t4 = (aabbMax.y - ray_origin.y) * invDirection.y;
				float t5 = (aabbMin.z - ray_origin.z) * invDirection.z;
				float t6 = (aabbMax.z - ray_origin.z) * invDirection.z;

				float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
				float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

				// 교차하지 않으면 tmax < 0
				// tmin > tmax는 뒤집힌 AABB와의 교차를 피하기 위한 조건
				bool result = tmax > 0 && tmin <= tmax;
				if (result) {
					pCoveredUI = obj;
					pCoveredUI->CursorOverObject(true);
#ifdef _DEBUG
					cout << "Collision With Ray! \t\t ObjectNum = " << i << '\n';
#endif // _DEBUG
				}
				else
					obj->CursorOverObject(false);
			}
		}
		else if (lc == (int)ObjectLayer::Ray)
		{
			CRayObject* rayOb = (CRayObject*)pObjectList[lc][0];
			rayOb->Reset(r);
		}
	}
	//----------------
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{

		if (cxDelta || cyDelta)
		{
			if (pKeysBuffer[VK_LBUTTON] & 0xF0)
			{
				SetCursor(NULL);
				SetCursorPos(ptOldCursorPos.x, ptOldCursorPos.y);
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);

			}
			else if (pKeysBuffer[VK_RBUTTON] & 0xF0)
			{
				if (pSelectedUI)
				{
					pSelectedUI->SetPositionUI(ptCursorPos);
				}
			}
			//
		}
		if (dwDirection) m_pPlayer->Move(dwDirection, 10.0f, true);
	}

	return(true);
}

void CTestScene::AnimateObjects(float fTimeElapsed)
{
	m_pObjectManager->AnimateObjects(fTimeElapsed);

	std::vector<CGameObject*>* pvObjectList = m_pObjectManager->GetObjectList();
	if (!pvObjectList[(int)ObjectLayer::Terrain].empty() && !pvObjectList[(int)ObjectLayer::Player].empty()) { // Terrain과 Player가 있다면
		CPlayer* pPlayer = (CPlayer*)pvObjectList[(int)ObjectLayer::Player][0];
		XMFLOAT3 xmfPlayerPos = pPlayer->GetPosition();
		float fHeight = ((CHeightMapTerrain*)pvObjectList[(int)ObjectLayer::Terrain][0])->GetHeight(xmfPlayerPos.x, xmfPlayerPos.z);

		if (xmfPlayerPos.y < fHeight) {
			xmfPlayerPos.y = fHeight;
			pPlayer->SetPosition(xmfPlayerPos);
			XMFLOAT3 xmfVelocity = pPlayer->GetVelocity();
			xmfVelocity.y = 0.0f;
			pPlayer->SetVelocity(xmfVelocity);
		}
	}
}

void CTestScene::Enter()
{
}

void CTestScene::Exit()
{
}




CTestScene_Card::CTestScene_Card()
{
}

CTestScene_Card::~CTestScene_Card()
{
}

bool CTestScene_Card::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}

	return false;
}

bool CTestScene_Card::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
			// 테스트용 빠른속도 이동 코드
		case 'W': m_pPlayer->Move(DIR_FORWARD, 1.25f * 10, false); break;
		case 'S': m_pPlayer->Move(DIR_BACKWARD, 1.25f * 10, false); break;
		case 'A': m_pPlayer->Move(DIR_LEFT, 1.25f * 10, false); break;
		case 'D': m_pPlayer->Move(DIR_RIGHT, 1.25f * 10, false); break;
		case 'Q': m_pPlayer->Move(DIR_UP, 1.25f * 10, false); break;
		case 'E': m_pPlayer->Move(DIR_DOWN, 1.25f * 10, false); break;
		case 'R': m_pPlayer->Rotate(0.0f, 20.0f, 0.0f);	break;
		case 'T': m_pPlayer->Rotate(0.0f, -20.0f, 0.0f); break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

void CTestScene_Card::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_pShaderManager->BuildShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList);
	m_pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
	m_pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);

	BuildLightsAndMaterials();



	// ------------------------------------       큐브 메쉬      -------------------------------

	{
		//가로x세로x높이가 12x12x12인 정육면체 메쉬를 생성한다. 
		CCubeMeshIlluminated* pCubeMesh = new CCubeMeshIlluminated(pd3dDevice,pd3dCommandList, 12.0f, 12.0f, 12.0f);

		int xObjects = 2, yObjects = 2, zObjects = 2, i = 0;

		int m_nObjects;
		m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);

		float fxPitch = 12.0f * 2.5f;
		float fyPitch = 12.0f * 2.5f;
		float fzPitch = 12.0f * 2.5f;

		CRotatingNormalObject* pRotatingNormalObject = NULL;
		for (int x = -xObjects; x <= xObjects; x++)
		{
			for (int y = -yObjects; y <= yObjects; y++)
			{
				for (int z = -zObjects; z <= zObjects; z++)
				{
					pRotatingNormalObject = new CRotatingNormalObject();
					pRotatingNormalObject->SetMesh(0, pCubeMesh);
					pRotatingNormalObject->SetMaterial(i % MAX_MATERIALS);

					pRotatingNormalObject->SetPosition(fxPitch * x, fyPitch * y, fzPitch * z);
					pRotatingNormalObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
					pRotatingNormalObject->SetRotationSpeed(10.0f * (i++ % 10) + 3.0f);
					pRotatingNormalObject->CreateShaderVariables(pd3dDevice, pd3dCommandList);

					m_pObjectManager->AddObj(pRotatingNormalObject, ObjectLayer::ObjectNormal);
				}
			}
		}
	}
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CTestScene_Card::BuildLightsAndMaterials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 100.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(130.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 50.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 60.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(-150.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 8.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f)); 
	m_pLights->m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));


	m_pMaterials = new MATERIALS;
	::ZeroMemory(m_pMaterials, sizeof(MATERIALS));
	m_pMaterials->m_pReflections[0] = { XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f,0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[1] = { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f,1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[2] = { XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f,0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[3] = { XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f,0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[4] = { XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f,0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 25.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[5] = { XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.0f,0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[6] = { XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f,0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 35.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[7] = { XMFLOAT4(1.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f,0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 40.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };

}

void CTestScene_Card::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);


	UINT ncbMaterialBytes = ((sizeof(MATERIALS) + 255) & ~255); //256의 배수
	m_pd3dcbMaterials = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,ncbMaterialBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbMaterials->Map(0, NULL, (void**)&m_pcbMappedMaterials);



}

void CTestScene_Card::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));
}

void CTestScene_Card::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
	if (m_pd3dcbMaterials)
	{
		m_pd3dcbMaterials->Unmap(0, NULL);
		m_pd3dcbMaterials->Release();
	}
}

ID3D12RootSignature* CTestScene_Card::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[4];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	//pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;	// 게임 오브젝트
	//pd3dRootParameters[1].Constants.Num32BitValues = 16;
	//pd3dRootParameters[1].Constants.ShaderRegister = 1;
	//pd3dRootParameters[1].Constants.RegisterSpace = 0;
	//pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 4; //GameObject
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 2; //Materials
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //Lights
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;



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

void CTestScene_Card::ReleaseObjects()
{
	CBasicScene::ReleaseObjects();
	if (m_pLights) delete m_pLights;
	if (m_pMaterials) delete m_pMaterials;
}

void CTestScene_Card::AnimateObjects(float fTimeElapsed)
{
	CBasicScene::AnimateObjects(fTimeElapsed);
	if (m_pLights)
	{
		m_pLights->m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights->m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}

void CTestScene_Card::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	
	//재질 리소스에 대한 상수 버퍼 뷰를 쉐이더 변수에 연결(바인딩)한다. 
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2,d3dcbMaterialsGpuVirtualAddress);

	//조명 리소스에 대한 상수 버퍼 뷰를 쉐이더 변수에 연결(바인딩)한다. 
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbLightsGpuVirtualAddress);
	
	
	m_pObjectManager->Render(pd3dCommandList, pCamera, m_pShaderManager);


}

void CTestScene_Card::Enter()
{
}

void CTestScene_Card::Exit()
{
}


////////////////////////////////////////////////////////////////////////////////////////////

CTestScene_Slice::CTestScene_Slice()
{
}

CTestScene_Slice::~CTestScene_Slice()
{
}

bool CTestScene_Slice::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CTestScene_Slice::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
			//case 'W': m_pPlayer->Move(DIR_FORWARD, m_pPlayer->GetMoveSpeed(), true); break;
			//case 'S': m_pPlayer->Move(DIR_BACKWARD, m_pPlayer->GetMoveSpeed(), true); break;
			//case 'A': m_pPlayer->Move(DIR_LEFT, m_pPlayer->GetMoveSpeed(), true); break;
			//case 'D': m_pPlayer->Move(DIR_RIGHT, m_pPlayer->GetMoveSpeed(), true); break;
		case 'Q': m_pPlayer->Move(DIR_UP, m_pPlayer->GetMoveSpeed(), true); break;
		case 'E': m_pPlayer->Move(DIR_DOWN, m_pPlayer->GetMoveSpeed(), true); break;
		case 'R': m_pPlayer->Rotate(0.0f, 20.0f, 0.0f);	break;
		case 'T': m_pPlayer->Rotate(0.0f, -20.0f, 0.0f); break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

void CTestScene_Slice::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_pShaderManager->BuildShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	{
		m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList);
		m_pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
		m_pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pPlayer->SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);
	}

	{
		CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, STONE_LIT_001_FBX);
		((CDynamicShapeMesh*)(pFBXObject->GetMeshes()[0]))->SetCuttable(true);
		pFBXObject->SetCuttable(true);
		pFBXObject->SetPosition(50.0f, 40.0f, 100.0f);
		pFBXObject->SetShaderType(ShaderType::CObjectsShader);
		m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
	}

	{
		CBoxMesh* pCubeMesh = new CBoxMesh(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);
		pCubeMesh->SetAllowCutting(true);
		CDynamicShapeObject* pDynamicShapeObject = NULL;

		pDynamicShapeObject = new CDynamicShapeObject();
		pDynamicShapeObject->SetAllowCutting(true);
		pDynamicShapeObject->SetMesh(0, pCubeMesh);
		pDynamicShapeObject->SetPosition(-50.0f, 40.0f, 100.0f);
		pDynamicShapeObject->SetShaderType(ShaderType::CObjectsShader);
		m_pObjectManager->AddObj(pDynamicShapeObject, ObjectLayer::Object);
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

bool CTestScene_Slice::ProcessInput(HWND hWnd, UCHAR* pKeysBuffer, POINT ptOldCursorPos)
{
	DWORD dwDirection = 0;
	if (pKeysBuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
	if (pKeysBuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
	if (pKeysBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
	if (pKeysBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;

	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	if (GetCapture() == hWnd)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - ptOldCursorPos.y) / 3.0f;
		SetCursorPos(ptOldCursorPos.x, ptOldCursorPos.y);
	}

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			if (pKeysBuffer[VK_LBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
		if (dwDirection) m_pPlayer->Move(dwDirection, 0.5f, false);
	}

	return(true);
}

void CTestScene_Slice::AnimateObjects(float fTimeElapsed)
{
	m_pObjectManager->AnimateObjects(fTimeElapsed);

	std::vector<CGameObject*>* pvObjectList = m_pObjectManager->GetObjectList();
	if (!pvObjectList[(int)ObjectLayer::Object].empty()) {
		CFBXObject* pObject_stone = (CFBXObject*)pvObjectList[(int)ObjectLayer::Object][0];
		CDynamicShapeObject* pObject_cuttur = (CDynamicShapeObject*)pvObjectList[(int)ObjectLayer::Object][1];
		pObject_cuttur->MoveStrafe(0.04);

		
		if (pObject_stone->CollisionCheck(pObject_cuttur)) {
			//cout << "Collision\n";
		}

	}
	//if (!pvObjectList[(int)ObjectLayer::Terrain].empty() && !pvObjectList[(int)ObjectLayer::Player].empty()) { // Terrain과 Player가 있다면
	//	CPlayer* pPlayer = (CPlayer*)pvObjectList[(int)ObjectLayer::Player][0];
	//	XMFLOAT3 xmfPlayerPos = pPlayer->GetPosition();
	//	float fHeight = ((CHeightMapTerrain*)pvObjectList[(int)ObjectLayer::Terrain][0])->GetHeight(xmfPlayerPos.x, xmfPlayerPos.z);

	//	if (xmfPlayerPos.y < fHeight) {
	//		xmfPlayerPos.y = fHeight;
	//		pPlayer->SetPosition(xmfPlayerPos);
	//		XMFLOAT3 xmfVelocity = pPlayer->GetVelocity();
	//		xmfVelocity.y = 0.0f;
	//		pPlayer->SetVelocity(xmfVelocity);
	//	}
	//}
}

void CTestScene_Slice::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	CBasicScene::DynamicShaping(pd3dDevice, pd3dCommandList, fTimeElapsed);
}

void CTestScene_Slice::Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	m_pShaderManager->Render(pd3dCommandList, pCamera, ShaderType::CTextShader);

	D2D1_RECT_F textRect = D2D1::RectF(0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	static const WCHAR text[] = L"CTestScene_Slice의 Render2D 입니다.";

	ComPtr<ID2D1SolidColorBrush> mSolidColorBrush;
	ComPtr<IDWriteTextFormat> mDWriteTextFormat;

	DX::ThrowIfFailed(pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Aqua), mSolidColorBrush.GetAddressOf()));
	DX::ThrowIfFailed(pdWriteFactory->CreateTextFormat(
		L"Verdana",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		25,
		L"en-us",
		mDWriteTextFormat.GetAddressOf()));

	mDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	mDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
	pd2dDeviceContext->DrawText(text, _countof(text) - 1, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());
}

void CTestScene_Slice::Enter()
{
}

void CTestScene_Slice::Exit()
{
}