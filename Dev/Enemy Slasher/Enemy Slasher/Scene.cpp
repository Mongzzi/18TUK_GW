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

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;	// ���� ������Ʈ
	pd3dRootParameters[1].Constants.Num32BitValues = 16;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;



	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.MipLODBias = 0;
	d3dSamplerDesc.MaxAnisotropy = 1;
	d3dSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc.ShaderRegister = 0;
	d3dSamplerDesc.RegisterSpace = 0;
	d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 1;
	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDesc;
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
	//UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
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

	// ������ �����Ӻ� �Է��� ó���� ���̹Ƿ� �����Ӹ��� Ű �Է� ó������ �� ��
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
	static const WCHAR text[] = L"BasicScene�� Render2D �Դϴ�.";

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTitleScene::CTitleScene()
{
}

CTitleScene::~CTitleScene()
{
}

bool CTitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_RBUTTONUP:
		break;
	default:
		break;
	}
	return false;
}

bool CTitleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

void CTitleScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_pShaderManager->BuildShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	{
		m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList, pFBXLoader, NULL, ShaderType::CObjectsShader);
		m_pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
		m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);
	}

	{
		CGameObject* pBackGround = new CGameObject();
		CBoxMesh* pBox = new CBoxMesh(pd3dDevice, pd3dCommandList, 0.0f, 0.0f, 0.0f, 10000, 10000);
		pBackGround->SetMesh(0, pBox);
		pBackGround->SetPosition(0.0f, 0.0f, 1000);
		pBackGround->SetShaderType(ShaderType::CObjectsShader);
		m_pObjectManager->AddObj(pBackGround, ObjectLayer::BackGround);
	}

	{
		CBoxMesh* pBox = new CBoxMesh(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f, 100, 100, 1000);
		CCardUIObject* pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, m_pPlayer->GetCamera(), NULL, ShaderType::CUIObjectsShader);
		pCardUIObject->SetMesh(0, pBox);
		pCardUIObject->SetPositionUI(480, 300);
		pCardUIObject->SetScale(1, 1, 1);
		pCardUIObject->SetShaderType(ShaderType::CUIObjectsShader);
		m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);

		pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, m_pPlayer->GetCamera(), NULL, ShaderType::CUIObjectsShader);
		pCardUIObject->SetMesh(0, pBox);
		pCardUIObject->SetPositionUI(480, 420);
		pCardUIObject->SetScale(1, 1, 1);
		pCardUIObject->SetShaderType(ShaderType::CUIObjectsShader);
		m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);
	}

	{
		//CRayObject* pRayObject = NULL;
		//pRayObject = new CRayObject();
		//pRayObject->SetMesh(0, new CRayMesh(pd3dDevice, pd3dCommandList, NULL));
		//m_pObjectManager->AddObj(pRayObject, ObjectLayer::Ray);
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

bool CTitleScene::ProcessInput(HWND hWnd, UCHAR* pKeysBuffer, POINT ptOldCursorPos)
{
	DWORD dwDirection = 0;
	if (pKeysBuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
	if (pKeysBuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
	if (pKeysBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
	if (pKeysBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;

	float cxDelta = 0.0f, cyDelta = 0.0f;
	int xDelta = 0, yDelta = 0;
	POINT ptCursorPos{ 0,0 }; //�ʱ�ȭ�� ���� ���� �� ���� Ȯ���� �Ʒ��� if���� �������� ���Ͽ� �ʱ�ȭ���� ���� ���� ����ϰ� �ȴ�.
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

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			if (pKeysBuffer[VK_LBUTTON] & 0xF0)
			{
			}
			else if (pKeysBuffer[VK_RBUTTON] & 0xF0)
			{
			}
		}
	}

	//vector<CGameObject*>* pObjectList = m_pObjectManager->GetObjectList();

	//RECT clientRect;
	//GetClientRect(hWnd, &clientRect);

	//int clientWidth = clientRect.right - clientRect.left;
	//int clientHeight = clientRect.bottom - clientRect.top;

	//int count = pObjectList[(int)ObjectLayer::InteractiveUIObject].size();

	//for (int i = 0; i < count; i++)
	//{
	//	CUIObject* obj = (CUIObject*)pObjectList[(int)ObjectLayer::InteractiveUIObject][i];
	//	obj->SetPositionUI(clientWidth / 2 + ((float)i - (float)count / 2) * (clientWidth / 12) + (clientWidth / 24), (float)clientHeight / 10 * 9);
	//}

	return(true);
}

void CTitleScene::AnimateObjects(float fTimeElapsed)
{
}

void CTitleScene::Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	m_pShaderManager->Render(pd3dCommandList, pCamera, ShaderType::CTextShader);

	D2D1_RECT_F textRect = D2D1::RectF(0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT / 2);
	static const WCHAR text[] = L"Enemy Slasher";

	ComPtr<ID2D1SolidColorBrush> mSolidColorBrush;
	ComPtr<IDWriteTextFormat> mDWriteTextFormat;

	DX::ThrowIfFailed(pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::RosyBrown), mSolidColorBrush.GetAddressOf()));
	DX::ThrowIfFailed(pdWriteFactory->CreateTextFormat(
		L"Verdana",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		60,
		L"en-us",
		mDWriteTextFormat.GetAddressOf()));

	mDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	mDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
	pd2dDeviceContext->DrawText(text, _countof(text) - 1, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());


	DX::ThrowIfFailed(pdWriteFactory->CreateTextFormat(
		L"Verdana",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		40,
		L"en-us",
		mDWriteTextFormat.GetAddressOf()));

	mDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	mDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	textRect = D2D1::RectF(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT / 4 * 3);
	static const WCHAR text2[] = L"���� ����";
	pd2dDeviceContext->DrawText(text2, _countof(text2) - 1, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());

	textRect = D2D1::RectF(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 4 * 3, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	static const WCHAR text3[] = L"���� ����";
	pd2dDeviceContext->DrawText(text3, _countof(text3) - 1, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());
}

void CTitleScene::Enter()
{
}

void CTitleScene::Exit()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





CTestScene::CTestScene()
{
}

CTestScene::~CTestScene()
{
}

bool CTestScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	POINT ptCursorPos{ 0,0 }; 
	GetCursorPos(&ptCursorPos);
	ScreenToClient(hWnd, &ptCursorPos);
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	int clientWidth = clientRect.right - clientRect.left;
	int clientHeight = clientRect.bottom - clientRect.top;


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
			if (ptCursorPos.y > (float)clientHeight / 5 * 4)
			{
				// ����ġ�� ���ư�.
				cout << "����ġ " << ptCursorPos.y << ", " << (float)clientHeight / 5 * 4 << endl;
				pSelectedUI->SetPositionUI(pSelectedUI->GetPositionUI().x, (float)clientHeight / 10 * 9);

			}
			else
			{
				// ī�� ���
				// �ڽ� ����
				m_pObjectManager->DelObj((CGameObject*)pSelectedUI, ObjectLayer::InteractiveUIObject);
				cout << "���� " << ptCursorPos.y << ", " << (float)clientHeight / 5 * 4 << endl;
			}
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

	m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList, pFBXLoader, PEASANT_1_FBX, ShaderType::CObjectsShader);
	m_pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	m_pPlayer->SetPosition(XMFLOAT3(2160.0f, 2000.0f, 2340.0f));
	m_pPlayer->SetGravity(XMFLOAT3(0.0f, -10.0f, 0.0f));
	m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);



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
	XMFLOAT3 xmf3Scale(24.0f, 6.0f, 24.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.2f, 0.0f, 0.0f);

	//������ ���� �� �̹��� ����(HeightMap.raw)�� ����Ͽ� �����Ѵ�. ���� ���� ũ��� ����x����(257x257)�̴�. 
	CHeightMapTerrain* pTerrain;
#ifdef _WITH_TERRAIN_PARTITION
	/*�ϳ��� ���� �޽��� ũ��� ����x����(17x17)�̴�. ���� ��ü�� ���� �������� 16��, ���� �������� 16�� ���� �޽��� ������. ������ �����ϴ� ���� �޽��� ������ �� 256(16x16)���� �ȴ�.*/
	pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("HeightMap.raw"), 257, 257, 17, 17, xmf3Scale, xmf4Color);

#else
	//������ �ϳ��� ���� �޽�(257x257)�� �����Ѵ�. 
	pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("a.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color);

#endif
	m_pObjectManager->AddObj(pTerrain, ObjectLayer::Terrain);

	{
		// �÷��̾� ��ġ �׷��� ���� �̵�
		XMFLOAT3 xmfPlayerPos = m_pPlayer->GetPosition();
		xmfPlayerPos.y = pTerrain->GetHeight(xmfPlayerPos.x, xmfPlayerPos.z);
		m_pPlayer->SetPosition(xmfPlayerPos);
	}


	{
		//float fTerrainWidth = pTerrain->GetWidth(), fTerrainLength = pTerrain->GetLength();
		//float fxPitch = 12.0f * 3.5f;
		//float fyPitch = 12.0f * 3.5f;
		//float fzPitch = 12.0f * 3.5f;

		////������ü�� ���� ǥ�鿡 �׸��� �������� ���� ��ġ�� ������ �������� ��ġ�Ѵ�. 
		//int xObjects = int(fTerrainWidth / fxPitch), yObjects = 1, zObjects = int(fTerrainLength / fzPitch);

		//CBoxMesh* pCubeMesh = new CBoxMesh(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);

		////CFBXMesh* pFBXMesh = new CFBXMesh(pd3dDevice, pd3dCommandList, plSdkManager, plScene, "fbxsdk/Stone_lit_001.fbx");
		////CFBXMesh* pFBXMesh2 = new CFBXMesh(pd3dDevice, pd3dCommandList, plSdkManager, plScene, "fbxsdk/box.fbx");

		//XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
		////CRotatingObject* pRotatingObject = NULL;
		//CFBXObject* pRotatingObject = NULL;
		////CRotatingObject* pRotatingObject = NULL;

		////CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, STONE_LIT_001_FBX);	// box.fbx�� mesh�� 3���� ���� �ڵ�δ� ���� ����.
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
		//				/*������ ǥ�鿡 ��ġ�ϴ� ������ü�� ������ ���⿡ ���� ������ �ٸ��� ��ġ�Ѵ�. ������ü�� ��ġ�� ������ ���� ���� ����� ������ü�� y-���� ��ġ�ϵ��� �Ѵ�.*/
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
	}
	// house
	{
		CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, HOUSE_FBX, ShaderType::CObjectsShader);
		float xPosition = 257.0f / 4.0f * 24.0f;
		float zPosition = 257.0f / 4.0f * 24.0f;
		float fHeight = pTerrain->GetHeight(xPosition, zPosition);

		pFBXObject->SetPosition(xPosition, fHeight, zPosition);
		m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
	}

	// tree
	{
		CFBXObject* pFBXObject = NULL;


		float xpitch = 257.0f * 24.0f / 10.0f;
		float zpitch = 257.0f * 24.0f / 7.0f;

		for (int x = 0; x < 10; x++)
		{
			for (int z = 0; z < 7; z++)
			{
				if (x % 5 == 0) {
					CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, TREE1, ShaderType::CObjectsShader);
					float xPosition = x * xpitch;
					float zPosition = z * zpitch;
					float fHeight = pTerrain->GetHeight(xPosition, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->Rotate(90.0f, 0.0f, 0.0f);
					m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
				}
				else if (x % 5 == 1) {
					CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, TREE1, ShaderType::CObjectsShader);
					float xPosition = x * xpitch;
					float zPosition = z * zpitch;
					float fHeight = pTerrain->GetHeight(xPosition, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->Rotate(90.0f, 0.0f, 0.0f);

					m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
				}
				else if (x % 5 == 2) {
					CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, TREE5, ShaderType::CObjectsShader);
					float xPosition = x * xpitch;
					float zPosition = z * zpitch;
					float fHeight = pTerrain->GetHeight(xPosition, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->Rotate(90.0f, 0.0f, 0.0f);

					m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
				}
				else if (x % 5 == 3) {
					CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, TREE2, ShaderType::CObjectsShader);
					float xPosition = x * xpitch;
					float zPosition = z * zpitch;
					float fHeight = pTerrain->GetHeight(xPosition, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->Rotate(90.0f, 0.0f, 0.0f);

					m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
				}
				else if (x % 5 == 4) {
					CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, TREE3, ShaderType::CObjectsShader);
					float xPosition = x * xpitch;
					float zPosition = z * zpitch;
					float fHeight = pTerrain->GetHeight(xPosition, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->Rotate(90.0f, 0.0f, 0.0f);

					m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
				}
				else if (x % 5 == 0) {
					CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, TREE4, ShaderType::CObjectsShader);
					float xPosition = x * xpitch;
					float zPosition = z * zpitch;
					float fHeight = pTerrain->GetHeight(xPosition, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->SetPosition(xPosition, fHeight, zPosition);
					pFBXObject->Rotate(90.0f, 0.0f, 0.0f);

					m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
				}
			}
		}
	}

	// UI
	{
		//ī�� UI �׽�Ʈ�� ������Ʈ.
		CCardUIObject* pUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, m_pPlayer->GetCamera(), TREE4, ShaderType::CUIObjectsShader);
		CMesh* pBoxMesh = new CBoxMesh(pd3dDevice, pd3dCommandList, 0.2f, 0.5f, 0.2f, 10.0f, 10.0f, 10.0f);
		pUIObject->SetMesh(0, pBoxMesh);
		pUIObject->SetPositionUI(320, 440);
		pUIObject->SetScale(1000, 100, 1);
		m_pObjectManager->AddObj(pUIObject, ObjectLayer::UIObject);

		CCardUIObject* pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, m_pPlayer->GetCamera(), CARD_FBX, ShaderType::CUIObjectsShader);
		pCardUIObject->SetPositionUI(100, 100);
		pCardUIObject->SetScale(2, 2, 1);
		m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);

		pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, m_pPlayer->GetCamera(), CARD_FBX, ShaderType::CUIObjectsShader);
		pCardUIObject->SetPositionUI(200, 200);
		pCardUIObject->SetScale(2, 2, 1);
		m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);

		pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, m_pPlayer->GetCamera(), CARD_FBX, ShaderType::CUIObjectsShader);
		pCardUIObject->SetPositionUI(300, 300);
		pCardUIObject->SetScale(2, 2, 1);
		m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);

		pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, m_pPlayer->GetCamera(), CARD_FBX, ShaderType::CUIObjectsShader);
		pCardUIObject->SetPositionUI(400, 400);
		pCardUIObject->SetScale(2, 2, 1);
		m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);

		pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, pFBXLoader, m_pPlayer->GetCamera(), CARD_FBX, ShaderType::CUIObjectsShader);
		pCardUIObject->SetPositionUI(400, 400);
		pCardUIObject->SetScale(2, 2, 1);
		m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);
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
	POINT ptCursorPos{ 0,0 }; //�ʱ�ȭ�� ���� ���� �� ���� Ȯ���� �Ʒ��� if���� �������� ���Ͽ� �ʱ�ȭ���� ���� ���� ����ϰ� �ȴ�.
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
	//-------------- ��ŷ
	CRay r = r.RayAtWorldSpace(ptCursorPos.x, ptCursorPos.y, m_pPlayer->GetCamera());

	std::vector<CGameObject*>* pObjectList = m_pObjectManager->GetObjectList();

	pCoveredUI = NULL;

	for (int lc = 0; lc < (int)ObjectLayer::Count;lc++)
	{
		if (lc == (int)ObjectLayer::InteractiveUIObject)
		{
			for (int i = 0;i < pObjectList[lc].size();i++) {
				CUIObject* obj = (CUIObject*)pObjectList[lc][i];
				if (!obj)
					continue;

				float tmin, tmax;
				if (true == m_pObjectManager->CollisionCheck_RayWithAABB(&r, obj, tmin, tmax)) {
					pCoveredUI = obj;
#ifdef _DEBUG
					//cout << "Collision With Ray! \t\t ObjectNum = " << i << '\n';
#endif // _DEBUG
				}
				else
					;//obj->CursorOverObject(false);
				obj->CursorOverObject(false);
			}
			if(pCoveredUI)
				pCoveredUI->CursorOverObject(true);
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

				std::vector<CGameObject*>* pvObjectList = m_pObjectManager->GetObjectList();
				if (false == pvObjectList[(int)ObjectLayer::Object].empty()) {
					for (const auto& obj : pvObjectList[(int)ObjectLayer::Object]) {
						if (CInteractiveObject* IterObj = dynamic_cast<CInteractiveObject*>(obj)) {
							XMFLOAT4X4 playerMat = m_pPlayer->GetWorldMat();
							XMFLOAT4X4 objMat = IterObj->GetWorldMat();
							if (true == CollisionCheck(m_pPlayer->GetCollider(), playerMat, IterObj->GetCollider(), objMat)) {
								m_pPlayer->Rotate(-cyDelta, -cxDelta, 0.0f);
							}
						}
					}
				}
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


	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	int clientWidth = clientRect.right - clientRect.left;
	int clientHeight = clientRect.bottom - clientRect.top;
	
	int count = pObjectList[(int)ObjectLayer::InteractiveUIObject].size();

	for (int i = 0;i < count;i++)
	{
		CUIObject* obj = (CUIObject*)pObjectList[(int)ObjectLayer::InteractiveUIObject][i];
		if(obj != pSelectedUI)
			obj->SetPositionUI(clientWidth / 2 + ((float)i - (float)count / 2) * (clientWidth / 12)+ (clientWidth / 24), (float)clientHeight / 10 * 9);
	}



	return(true);
}

void CTestScene::AnimateObjects(float fTimeElapsed)
{
	m_pObjectManager->AnimateObjects(fTimeElapsed);

	std::vector<CGameObject*>* pvObjectList = m_pObjectManager->GetObjectList();
	
	if (false == pvObjectList[(int)ObjectLayer::Player].empty()) { // Player�� �ִٸ�
		CPlayer* pPlayer = (CPlayer*)pvObjectList[(int)ObjectLayer::Player][0];

		if (false == pvObjectList[(int)ObjectLayer::Object].empty()) {
			for (const auto& obj : pvObjectList[(int)ObjectLayer::Object]) {
				if (CInteractiveObject* IterObj = dynamic_cast<CInteractiveObject*>(obj)) {
					XMFLOAT3 xmfPlayerPos = pPlayer->GetPosition();
					XMFLOAT4X4 playerMat = pPlayer->GetWorldMat();
					XMFLOAT4X4 objMat = IterObj->GetWorldMat();
					if (CollisionCheck(pPlayer->GetCollider(), playerMat, IterObj->GetCollider(), objMat)) {
						XMFLOAT3 playerVal = pPlayer->GetVelocity();
						xmfPlayerPos.x -= (playerVal.x * fTimeElapsed);
						xmfPlayerPos.z -= (playerVal.z * fTimeElapsed);
						pPlayer->SetPosition(xmfPlayerPos);
					}
				}
			}
		}

		if (false == pvObjectList[(int)ObjectLayer::Terrain].empty()) { // Terrain�� Player�� �ִٸ�
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
			// �׽�Ʈ�� �����ӵ� �̵� �ڵ�
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

	m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList, pFBXLoader, PEASANT_1_FBX, ShaderType::CObjectsShader);
	//m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList);
	m_pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
	m_pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);

	BuildLightsAndMaterials();



	// ------------------------------------       ť�� �޽�      -------------------------------

	{
		//����x����x���̰� 12x12x12�� ������ü �޽��� �����Ѵ�. 
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
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
	
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);


	UINT ncbMaterialBytes = ((sizeof(MATERIALS) + 255) & ~255); //256�� ���
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

	//pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;	// ���� ������Ʈ
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

	
	//���� ���ҽ��� ���� ��� ���� �並 ���̴� ������ ����(���ε�)�Ѵ�. 
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2,d3dcbMaterialsGpuVirtualAddress);

	//���� ���ҽ��� ���� ��� ���� �並 ���̴� ������ ����(���ε�)�Ѵ�. 
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
	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
		{
			m_pSelectedObj = NULL;

			POINT ptCursorPos{ 0,0 };
			GetCursorPos(&ptCursorPos);
			ScreenToClient(hWnd, &ptCursorPos);
			CRay r = r.RayAtWorldSpace(ptCursorPos.x, ptCursorPos.y, m_pPlayer->GetCamera());

			float nearestDist = FLT_MAX;

			vector<CGameObject*>* pvObjectList = m_pObjectManager->GetObjectList();
			for (const auto& objects : pvObjectList[(int)ObjectLayer::Object]) {
				if (CInteractiveObject* pInterObj = dynamic_cast<CInteractiveObject*>(objects)) {
					float tmin, tmax;
					if (true == m_pObjectManager->CollisionCheck_RayWithAABB(&r, pInterObj, tmin, tmax)) {
						if (nearestDist > tmin) { // ���� ����� ������Ʈ ����
							nearestDist = tmin;
							m_pSelectedObj = pInterObj;
						}
					}
				}
			}
			if (NULL != m_pSelectedObj) m_bMoveObj = true;
		}
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_RBUTTONUP:
		m_bMoveObj = false;
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
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

		case VK_SPACE:
			if (false == m_bAddCutter) {
				m_bAddCutter = true;

				POINT ptCursorPos{ 0,0 };
				GetCursorPos(&ptCursorPos);
				ScreenToClient(hWnd, &ptCursorPos);
				CRay r = r.RayAtWorldSpace(ptCursorPos.x, ptCursorPos.y, m_pPlayer->GetCamera());
				((CRayObject*)(m_pObjectManager->GetObjectList()[(int)ObjectLayer::Ray][0]))->Reset(r);
			}
			break;
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
		m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList, pFBXLoader, NULL, ShaderType::CObjectsShader);
		m_pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
		m_pPlayer->SetPosition(XMFLOAT3(0.0f, -100.0f, 0.0f));
		m_pPlayer->SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);
	}

	{
		//CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, STONE_LIT_001_FBX, ShaderType::CObjectsShader);
		//((CDynamicShapeMesh*)(pFBXObject->GetMeshes()[0]))->SetCuttable(true);
		//pFBXObject->SetCuttable(true);
		//pFBXObject->SetPosition(50.0f, 40.0f, 100.0f);
		//m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
	}

	{
		CBoxMesh* pBoxMesh = new CBoxMesh(pd3dDevice, pd3dCommandList);
		CDynamicShapeObject * newObject = new CDynamicShapeObject;
		newObject->SetMesh(0, pBoxMesh);
		newObject->SetPosition(200.0f, 40.0f, 100.0f);
		newObject->Rotate(45.0f, 45.0f, 45.0f);
		newObject->SetShaderType(ShaderType::CObjectsShader);
		m_pObjectManager->AddObj(newObject, ObjectLayer::Object);

		newObject = new CDynamicShapeObject;
		newObject->SetMesh(0, pBoxMesh);
		newObject->SetPosition(200.0f, 40.0f, 200.0f);
		newObject->Rotate(45.0f, 45.0f, 45.0f);
		newObject->SetShaderType(ShaderType::CObjectsShader);
		m_pObjectManager->AddObj(newObject, ObjectLayer::Object);

		newObject = new CDynamicShapeObject;
		newObject->SetMesh(0, pBoxMesh);
		newObject->SetPosition(200.0f, 40.0f, 300.0f);
		newObject->Rotate(0.0f, 0.0f, 0.0f);
		newObject->SetShaderType(ShaderType::CObjectsShader);
		m_pObjectManager->AddObj(newObject, ObjectLayer::Object);
	}

	{
		CRayObject* pRayObject = NULL;
		pRayObject = new CRayObject();
		pRayObject->SetMesh(0, new CRayMesh(pd3dDevice, pd3dCommandList, NULL));
		m_pObjectManager->AddObj(pRayObject, ObjectLayer::Ray);
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
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - ptOldCursorPos.y) / 3.0f;
	}

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			if (pKeysBuffer[VK_LBUTTON] & 0xF0) {
				SetCursor(NULL);
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
				SetCursorPos(ptOldCursorPos.x, ptOldCursorPos.y);
			}

			if (pKeysBuffer[VK_RBUTTON] & 0xF0) { // ��Ŭ���� ������Ʈ�� ��Ƽ� �̵�
				if (m_bMoveObj) {
					if (NULL != m_pSelectedObj) {
						XMFLOAT3 t0((float)(ptCursorPos.x - m_ptOldCursorPos.x), (float)(ptCursorPos.y - m_ptOldCursorPos.y), 0.0f);

						m_pObjectManager->ScreenBasedObjectMove(m_pSelectedObj, m_pPlayer->GetCamera(), t0.x, t0.y);

						m_ptOldCursorPos = ptCursorPos;
					}
				}
			}
		}
		if (dwDirection) m_pPlayer->Move(dwDirection, 0.5f, false);
	}

	return(true);
}

void CTestScene_Slice::AnimateObjects(float fTimeElapsed)
{
	m_pObjectManager->AnimateObjects(fTimeElapsed);

	std::vector<CGameObject*>* pvObjectList = m_pObjectManager->GetObjectList();

#ifdef _DEBUG
	std::vector<CGameObject*> pvOL = pvObjectList[(int)ObjectLayer::Object];
	if (pvOL.size() < 2) return;
	XMFLOAT4X4 MatA, MatB;
	for (int i = 0; i < pvOL.size() - 1; ++i) {
		if (CInteractiveObject* ObjA = dynamic_cast<CInteractiveObject*>(pvOL[i])) {
			for (int j = i + 1; j < pvOL.size(); ++j) {
				if (CInteractiveObject* ObjB = dynamic_cast<CInteractiveObject*>(pvOL[j])) {
					MatA = ObjA->GetWorldMat();
					MatB = ObjB->GetWorldMat();
					if (CollisionCheck(ObjA->GetCollider(), MatA, ObjB->GetCollider(), MatB)) {
						cout << "Collision! ObjectNum = " << i << '\t' << j << '\n';
					}
				}
			}
		}
	}
#endif // DEBUG
}

void CTestScene_Slice::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed)
{
	if (m_bAddCutter) {
		m_bAddCutter = false;
		float fBoxSize = 100.0f;
		CRayObject* pRayObj = ((CRayObject*)(m_pObjectManager->GetObjectList()[(int)ObjectLayer::Ray][0]));
		XMFLOAT3 ray_dir = pRayObj->GetRayDir();
		XMFLOAT3 ray_origin = pRayObj->GetRayOrigin();

		std::random_device rd;
		std::default_random_engine dre(rd());
		std::uniform_real_distribution <float> urd(-1.0, 1.0);

		XMFLOAT3 randomVector(urd(dre), urd(dre), urd(dre));
		// ���� ��� (������ ����)
		XMFLOAT3 planeNormal = Vector3::CrossProduct(randomVector, ray_dir);

		CDynamicShapeObject* cutterObject = new CDynamicShapeObject;
		CCutterBox_NonMesh* cutterMesh = new CCutterBox_NonMesh(pd3dDevice, pd3dCommandList, fBoxSize, fBoxSize, fBoxSize);
		cutterMesh->SetCutPlaneNormal(planeNormal);
		cutterObject->SetMesh(0, cutterMesh);
		cutterObject->SetPosition(Vector3::Add(ray_origin, Vector3::ScalarProduct(ray_dir, fBoxSize)));
		cutterObject->SetAllowCutting(true);
		cutterObject->SetShaderType(ShaderType::CObjectsShader);

		m_pObjectManager->AddObj(cutterObject, ObjectLayer::CutterObject);
	}
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
	static const WCHAR text[] = L"CTestScene_Slice�� Render2D �Դϴ�.";

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