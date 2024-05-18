#include "stdafx.h"
#include "Scene.h"
#include "ObjectManager.h"
//#include "ShaderManager.h"
#include "PhysXManager.h"
#include "Ray.h"
#include "FbxLoader_V2.h"
#include "ResorceManager.h"

CBasicScene::CBasicScene()
{
	m_pObjectManager = new CObjectManager;
	m_CurrentTime = 0.0f;
	m_ElapsedTime = 0.0f;
	//m_pShaderManager = new CShaderManager;
}

CBasicScene::~CBasicScene()
{
	delete m_pObjectManager;
	//delete m_pShaderManager;
}

void CBasicScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	//m_pShaderManager->BuildShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pTextShader = new CTextShader();
	m_pTextShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

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

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// 게임 오브젝트
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;



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

void CBasicScene::UpdateTimer(float fTimeCurrent, float fTimeElapsed)
{
	m_CurrentTime = fTimeCurrent;
	m_ElapsedTime = fTimeElapsed;
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

void CBasicScene::AnimateObjects(float fTimeTotal ,float fTimeElapsed)
{
	m_pObjectManager->AnimateObjects(fTimeTotal , fTimeElapsed);
}

void CBasicScene::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager, float fTimeElapsed)
{
	m_pObjectManager->DynamicShaping(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, fTimeElapsed);
}

void CBasicScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	//if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	m_pObjectManager->Render(pd3dCommandList, pCamera);



	//D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	//pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights
}

void CBasicScene::Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	//m_pShaderManager->Render(pd3dCommandList, pCamera, ShaderType::CTextShader);
	m_pTextShader->Render(pd3dCommandList, pCamera);


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
	case WM_LBUTTONUP:
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

void CTitleScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	//m_pShaderManager->BuildShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pTextShader = new CTextShader();
	m_pTextShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	{
		m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		m_pPlayer->ChangeCamera(SPACESHIP_CAMERA, 0.0f);
		m_pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);
	}

	{
		CGameObject* pBackGround = new CGameObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		CBoxMesh* pBox = new CBoxMesh(pd3dDevice, pd3dCommandList, 0.0f, 0.0f, 0.0f, 10000, 10000);
		pBackGround->SetMesh(0, pBox);
		pBackGround->SetPosition(0.0f, 0.0f, 1000);
		m_pObjectManager->AddObj(pBackGround, ObjectLayer::BackGround);
	}

	{
		CBoxMesh* pBox = new CBoxMesh(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f, 100, 100, 1000);
		CCardUIObject* pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pPlayer->GetCamera(), ShaderType::CUIObjectsShader);
		pCardUIObject->SetMesh(0, pBox);
		pCardUIObject->SetPositionUI(480, 300);
		pCardUIObject->SetScale(1, 1, 1);
		//pCardUIObject->SetShaderType(ShaderType::CUIObjectsShader);
		m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);

		pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pPlayer->GetCamera(), ShaderType::CUIObjectsShader);
		pCardUIObject->SetMesh(0, pBox);
		pCardUIObject->SetPositionUI(480, 420);
		pCardUIObject->SetScale(1, 1, 1);
		//pCardUIObject->SetShaderType(ShaderType::CUIObjectsShader);
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

	//m_pShaderManager->Render(pd3dCommandList, pCamera, ShaderType::CTextShader);
	m_pTextShader->Render(pd3dCommandList, pCamera);

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
	static const WCHAR text2[] = L"게임 시작";
	pd2dDeviceContext->DrawText(text2, _countof(text2) - 1, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());

	textRect = D2D1::RectF(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 4 * 3, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	static const WCHAR text3[] = L"게임 종료";
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

ID3D12RootSignature* CTestScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[5];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 0; //t0: gtxtTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 1; //t4: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 2; //t5: gtxtTerrainDetailTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 3; //t6: gtxtTerrainAlphaTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 7;
	pd3dDescriptorRanges[4].BaseShaderRegister = 4; //t7: gtxtBillboardTextures[7]
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;



	D3D12_ROOT_PARAMETER pd3dRootParameters[11];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// 게임 오브젝트
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 2; //Materials
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //Lights
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0];
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1]; //t4: gtxtTerrainBaseTexture
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2]; //t5: gtxtTerrainDetailTexture
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[3]; //t6: gtxtTerrainAlphaTexture
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[8].Descriptor.ShaderRegister = 4; //Time Info
	pd3dRootParameters[8].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[4]; //t7: gtxtBillboardTextures[7]
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// BoneMat
	pd3dRootParameters[10].Constants.ShaderRegister = 5;
	pd3dRootParameters[10].Constants.RegisterSpace = 0;
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[2];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);


	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
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
			// 현재 카드UI 전용코드가 올라가있음.
			// 아래 내용을 CCardUIObject.ButtenUp()에 넣어야함.
			pSelectedUI->ButtenUp();
			if (ptCursorPos.y > (float)clientHeight / 5 * 4)
			{
				// 원위치로 돌아감.
				pSelectedUI->SetPositionUI(pSelectedUI->GetPositionUI().x, (float)clientHeight / 10 * 9);

			}
			else
			{
				// 카드 사용
				CCardUIObject* pcUI = dynamic_cast<CCardUIObject*>(pSelectedUI);
				pcUI->CallFunc(NULL, NULL);
				dynamic_cast<TestPlayer*> (m_pPlayer)->GetDeckData()->SendHandToUsedByNum(pcUI->GetUiNum());
				bCardUpdateFlag = true;
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
	int drawnCard;
	std::vector<int> intVector;
	std::default_random_engine dre(0);
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
		case 'Z':
		case 'z': 
			drawnCard = dynamic_cast<TestPlayer*> (m_pPlayer)->GetDeckData()->Draw(dre); 
			// 오브젝트레이어의 카드 정보를 핸드의 정보로 바꿔줘야함.
			if (drawnCard != -1)
			{
				std::cout << drawnCard << " 드로우" << endl;
				bCardUpdateFlag = true;
			}
			else
				std::cout << " 드로우 실패. 이미 5장이거나 덱이 없음." << endl;
			break;
		case 'X':
		case 'x':
			intVector = dynamic_cast<TestPlayer*> (m_pPlayer)->GetDeckData()->GetHand();
			std::cout << "hand : ";
			for (int card : intVector)
				std::cout << card << ", ";
			std::cout << std::endl;

			intVector = dynamic_cast<TestPlayer*> (m_pPlayer)->GetDeckData()->GetDeck();
			std::cout << "Deck : ";
			for (int card : intVector)
				std::cout << card << ", ";
			std::cout << std::endl;

			intVector = dynamic_cast<TestPlayer*> (m_pPlayer)->GetDeckData()->GetUsed();
			std::cout << "Used : ";
			for (int card : intVector)
				std::cout << card << ", ";
			std::cout << std::endl;
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

void CTestScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CFbxLoader_V2 fLoader;

	m_pTextShader = new CTextShader();
	m_pTextShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);


	m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CTextureShader);
	//CFBXTestMesh* pPlayerMesh = new CFBXTestMesh(pd3dDevice, pd3dCommandList, fLoader.LoadFBX(PEASANT_1_FBX));
	//m_pPlayer->SetMesh(0, pPlayerMesh);

	m_pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	m_pPlayer->SetPosition(XMFLOAT3(2160.0f, 2000.0f, 2340));
	//m_pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pPlayer->SetGravity(XMFLOAT3(0.0f, -10.0f, 0.0f));

	m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);

	//--------------------------------- 조명, 재질 생성 ----------------------------------------

	BuildLightsAndMaterials();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//

	// -------------------------------      스카이 박스     _____________________________________

	CSkyBox* pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CSkyBoxShader, 6);
	m_pObjectManager->AddObj(pSkyBox, ObjectLayer::SkyBox);

	// -------------------------------      스카이 박스 끝    _____________________________________

	// ------------------------------------       터레인      -------------------------------
	//지형을 확대할 스케일 벡터이다. x-축과 z-축은 8배, y-축은 2배 확대한다. 
	XMFLOAT3 xmf3Scale(240.0f, 60.0f, 240.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.0f, 0.0f, 0.0f);

	//지형을 높이 맵 이미지 파일(HeightMap.raw)을 사용하여 생성한다. 높이 맵의 크기는 가로x세로(257x257)이다. 
	CHeightMapTerrain* pTerrain;
#ifdef _WITH_TERRAIN_PARTITION
	/*하나의 격자 메쉬의 크기는 가로x세로(17x17)이다. 지형 전체는 가로 방향으로 16개, 세로 방향으로 16의 격자 메쉬를 가진다. 지형을 구성하는 격자 메쉬의 개수는 총 256(16x16)개가 된다.*/
	pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("HeightMap.raw"), 257, 257, 17, 17, xmf3Scale, xmf4Color);

#else
	//지형을 하나의 격자 메쉬(257x257)로 생성한다. 
	pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("a.raw"), 257, 257, 13, 13, xmf3Scale, xmf4Color, ShaderType::CTerrainShader);
	pTerrain->GetMaterial()->SetReflection(0);
#endif
	m_pObjectManager->AddObj(pTerrain, ObjectLayer::Terrain);

	// --------------------------------      터레인 끝     _____________________________________



	{
		// 플레이어 위치 테레인 위로 이동
		XMFLOAT3 xmfPlayerPos = m_pPlayer->GetPosition();
		xmfPlayerPos.y = pTerrain->GetHeight(xmfPlayerPos.x, xmfPlayerPos.z);
		m_pPlayer->SetPosition(xmfPlayerPos);
	}


	// --------------------------------- 빌보드 인스턴스 오브젝트 ------------------------------


	CBillBoardInstanceObject* pBillBoardObjects = new CBillBoardInstanceObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pTerrain, ShaderType::CBillBoardInstanceShader);
	m_pObjectManager->AddObj(pBillBoardObjects, ObjectLayer::BillBoardObject);

	// ------------------------------ 적 몬스터 오브젝트 --------------------------------------


	//Stone_lit_003

	CTexture* ppTextures[1];
	ppTextures[0] = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ppTextures[0]->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/diffuso.tif", RESOURCE_TEXTURE2D, 0);

	float xPosition = 0.0f;
	float zPosition = 0.0f;

	float terrain_width = pTerrain->GetWidth();
	float terrain_length = pTerrain->GetLength();

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution <float> urd_width(0, terrain_width);
	std::uniform_real_distribution <float> urd_length(0, terrain_length);

	for (int i = 0; i < 1; i++) {
		CMonsterObject* pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature,(TestPlayer*)m_pPlayer, pTerrain, ShaderType::CTextureShader);
		pMonsterObject->SetFbxData(pd3dDevice, pd3dCommandList, fLoader.LoadFBX("fbxsdk/Stonefbx.fbx"));
		pMonsterObject->SetScale(50.0f, 50.0f, 50.0f);
		pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);

		CMaterial* pTreeMaterial = pMonsterObject->GetMaterial();
		if (pTreeMaterial) {
			if (pTreeMaterial->m_pShader) {
				pTreeMaterial->m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
				pTreeMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, ppTextures[0], 0, 4);
				pTreeMaterial->SetTexture(ppTextures[0]);
			}
		}

		xPosition = 3000.0f;
		zPosition = 3000.0f;

		float fHeight = pTerrain->GetHeight(xPosition, zPosition);
		pMonsterObject->SetPosition(xPosition, fHeight, zPosition);
		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::TextureObject);
	}



	// tree
	{
		CTexture* pTreeTextures;
		pTreeTextures = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
		pTreeTextures->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, L"Image/tree_texture.png", RESOURCE_TEXTURE2D, 0);

		float xpitch = 257.0f * 24.0f / 10.0f;
		float zpitch = 257.0f * 24.0f / 7.0f;

		CFBXObject* pTreeObject;// = new CFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CTextureShader);
		CFBXMesh* pTreeMesh = new CFBXMesh(pd3dDevice, pd3dCommandList, fLoader.LoadFBX(TREE3)->m_pvMeshs[0]);
		for (int x = 0; x <5; x++)
		{
			for (int z = 0; z < 5; z++)
			{
				pTreeObject = new CFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CTextureShader);
				pTreeObject->SetMesh(0, pTreeMesh);
				CMaterial* pTreeMaterial = pTreeObject->GetMaterial();

				if (pTreeMaterial) {
					if (pTreeMaterial->m_pShader) {
						pTreeMaterial->m_pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1);
						pTreeMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, pTreeTextures, 0, 4);
						pTreeMaterial->SetTexture(pTreeTextures);
					}
				}

				float xPosition = x * xpitch;
				float zPosition = z * zpitch;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				pTreeObject->SetPosition(xPosition, fHeight, zPosition);
				pTreeObject->Rotate(-90.0f, 0.0f, 0.0f);
				m_pObjectManager->AddObj(pTreeObject, ObjectLayer::TextureObject);


			}
		}
	}

	////// UI
	//{
	//	//카드 UI 테스트용 오브젝트.
	//	//CCubeMeshTextured* pCardMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 412.0f, 582.0f, 1.0f);
	//	CFBXMesh* pCardMesh = new CFBXMesh(pd3dDevice, pd3dCommandList, fLoader.LoadFBX(CARD_FBX)->m_pvMeshs[0]);


	//	CCardUIObject* pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pPlayer->GetCamera(), 0, ShaderType::CUITextureShader);
	//	pCardUIObject->SetPositionUI(100, 100);
	//	pCardUIObject->SetMesh(0, pCardMesh,true);
	//	pCardUIObject->SetScale(2, 2, 1);
	//	pCardUIObject->SetFunc(Callback_0);
	//	m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);

	//	pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pPlayer->GetCamera(), 1, ShaderType::CUIObjectsShader);
	//	pCardUIObject->SetPositionUI(200, 200);
	//	pCardUIObject->SetMesh(0, pCardMesh, true);
	//	pCardUIObject->SetScale(2, 2, 1);
	//	pCardUIObject->SetFunc(Callback_1);
	//	m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);

	//	pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pPlayer->GetCamera(), 2, ShaderType::CUITextureShader);
	//	pCardUIObject->SetPositionUI(300, 300);
	//	pCardUIObject->SetMesh(0, pCardMesh, true);
	//	pCardUIObject->SetScale(2, 2, 1);
	//	pCardUIObject->SetFunc(Callback_2);
	//	m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);

	//	pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pPlayer->GetCamera(), 3, ShaderType::CUITextureShader);
	//	pCardUIObject->SetPositionUI(400, 400);
	//	pCardUIObject->SetMesh(0, pCardMesh, true);
	//	pCardUIObject->SetScale(2, 2, 1);
	//	pCardUIObject->SetFunc(Callback_3);
	//	m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);

	//	pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pPlayer->GetCamera(), 4, ShaderType::CUITextureShader);
	//	pCardUIObject->SetPositionUI(400, 400);
	//	pCardUIObject->SetMesh(0, pCardMesh, true);
	//	pCardUIObject->SetScale(2, 2, 1);
	//	pCardUIObject->SetFunc(Callback_4);
	//	m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);
	//}




	CRay r = r.RayAtWorldSpace(0, 0, m_pPlayer->GetCamera());

	CRayObject* pRayObject = NULL;
	pRayObject = new CRayObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
	pRayObject->SetMesh(0, new CRayMesh(pd3dDevice, pd3dCommandList, NULL));
	m_pObjectManager->AddObj(pRayObject, ObjectLayer::Ray);
}

void CTestScene::BuildLightsAndMaterials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 400.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(5.0f, 5.0f, 5.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(2160.0f, 2700.0f, 2340.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 700.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[1].m_fFalloff = 50.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(35.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_pLights->m_pLights[3].m_bEnable = false;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 200.0f;
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

	//        Ambient ( 주변광 )          //         Diffuse ( 산란광 )           //       Specullar ( 반사광 )         //       Emissive ( 발광 )     //
	//			Ambient : 일반적으로 장면 전반적인 색상과 밝기를 나타내는 데 사용
	//			Diffuse : 물체의 색상과 표면 특성을 나타내는 데 사용
	//			Specular : 빛이 표면에 반사되는 빛의 색상과 강도를 나타내는 데 사용
	//			Emissive : 물체가 발광하는 경우 해당 물체의 색상과 강도를 나타내는 데 사용
	//			r g b a
	m_pMaterials->m_pReflections[0] = { XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[1] = { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[2] = { XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[3] = { XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[4] = { XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 25.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[5] = { XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[6] = { XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 35.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[7] = { XMFLOAT4(1.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 40.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };

}

void CTestScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);

	UINT ncbMaterialBytes = ((sizeof(MATERIALS) + 255) & ~255); //256의 배수
	m_pd3dcbMaterials = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbMaterialBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbMaterials->Map(0, NULL, (void**)&m_pcbMappedMaterials);

	UINT ncbTimeBytes = ((sizeof(CB_TIME_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbTimeInfo = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbTimeBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbTimeInfo->Map(0, NULL, (void**)&m_pcbMappedTimeInfo);
}

void CTestScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbMaterialsGpuVirtualAddress); //Materials

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbLightsGpuVirtualAddress); //Lights

	m_pcbMappedTimeInfo->m_fCurrentTime = m_CurrentTime;
	m_pcbMappedTimeInfo->m_fElapsedTime = m_ElapsedTime;

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbTimeInfo->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(8, d3dGpuVirtualAddress);
}

void CTestScene::ReleaseShaderVariables()
{
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

	for (int lc = 0; lc < (int)ObjectLayer::Count; lc++)
	{
		if (lc == (int)ObjectLayer::InteractiveUIObject)
		{
			for (int i = 0; i < pObjectList[lc].size(); i++) {
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
			if (pCoveredUI)
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
						if (CGameObject* IterObj = dynamic_cast<CGameObject*>(obj)) {
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
				// 선택 카드 드래그.
				if (pSelectedUI)
				{
					pSelectedUI->SetPositionUI(ptCursorPos);
				}
			}
			//
		}
		if (dwDirection) m_pPlayer->Move(dwDirection, 100.0f, true);
	}


	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// 카드 위치지정하는 부분.
	int clientWidth = clientRect.right - clientRect.left;
	int clientHeight = clientRect.bottom - clientRect.top;

	//int count = pObjectList[(int)ObjectLayer::InteractiveUIObject].size();

	TestPlayer* tp = dynamic_cast<TestPlayer*> (m_pPlayer);

	vector hand = tp->GetDeckData()->GetHand();
	int handSize = tp->GetDeckData()->GetCurrentHandSize();

	// 이 레이어가 비어있을 수 있기 떄문에 먼저 포인터를 받아 검사
	std::vector<CGameObject*>* pInteractiveUIObj = &pObjectList[(int)ObjectLayer::InteractiveUIObject];
	if (pInteractiveUIObj->size() > 0) {
		for (int i = 0; i < handSize; i++)
		{
			CUIObject* obj = (CUIObject*)pObjectList[(int)ObjectLayer::InteractiveUIObject][i];
			if (obj != pSelectedUI)
			{
				obj->SetPositionUI(clientWidth / 2 + ((float)i - (float)handSize / 2) * (clientWidth / 12) + (clientWidth / 24), (float)clientHeight / 10 * 9);
				if (bCardUpdateFlag)
					dynamic_cast<CCardUIObject*> (obj)->UpdateData(hand[i]);
			}
		}
		bCardUpdateFlag = false; // 지금은 여기 있지만 다이나믹 쉐이핑 함수를 활성화 하면 거기 마지막으로 가면 됨.
		// 손에 없는 카드 위치
		for (int i = handSize; i < 5; i++)
		{
			CUIObject* obj = (CUIObject*)pObjectList[(int)ObjectLayer::InteractiveUIObject][i];
			if (obj != pSelectedUI)
				obj->SetPositionUI(clientWidth * 1.2, (float)clientHeight / 10 * 9);
		}
	}


	return(true);
}

void CTestScene::AnimateObjects(float fTotalTime ,float fTimeElapsed)
{
	m_pObjectManager->AnimateObjects(fTotalTime, fTimeElapsed);

	std::vector<CGameObject*>* pvObjectList = m_pObjectManager->GetObjectList();

	if (false == pvObjectList[(int)ObjectLayer::Player].empty()) { // Player가 있다면
		CPlayer* pPlayer = (CPlayer*)pvObjectList[(int)ObjectLayer::Player][0];

		if (false == pvObjectList[(int)ObjectLayer::Object].empty()) {
			for (const auto& obj : pvObjectList[(int)ObjectLayer::Object]) {
				if (CGameObject* IterObj = dynamic_cast<CGameObject*>(obj)) {
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

		if (false == pvObjectList[(int)ObjectLayer::Terrain].empty()) { // Terrain과 Player가 있다면
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
	if (m_pLights)
	{
		m_pLights->m_pLights[0].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights->m_pLights[0].m_xmf3Direction = m_pPlayer->GetLookVector();

		m_pLights->m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights->m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}

void CTestScene::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager, float fTimeElapsed)
{
	//if (SelectedUInum != -1) {
	//	float fBoxSize = 200.0f;
	//
	//
	//	CRayObject* pRayObj = ((CRayObject*)(m_pObjectManager->GetObjectList()[(int)ObjectLayer::Ray][0]));
	//	//XMFLOAT3 ray_dir = m_pPlayer->GetLookVector();	// 둘 다 해봐
	//
	//	std::random_device rd;
	//	std::default_random_engine dre(rd());
	//	std::uniform_real_distribution <float> urd(-1.0, 1.0);
	//
	//	XMFLOAT3 playerLook = m_pPlayer->GetLook();
	//	XMFLOAT3 Vector1;
	//	XMFLOAT3 Vector2 = XMFLOAT3(0.0f, 0.0f, 1.0f);
	//	XMFLOAT3 planeNormal;
	//
	//	CGameObject* cutterObject = new CGameObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
	//	CCutterBox_NonMesh* cutterMesh = new CCutterBox_NonMesh(pd3dDevice, pd3dCommandList, fBoxSize, fBoxSize, fBoxSize);	// 박스 안의 오브젝트를 절단한다.
	//
	//
	//	// 외적 계산 (수직인 벡터)
	//	switch (SelectedUInum)
	//	{
	//	case 0:
	//	case 1:
	//		//Vector1 = m_pPlayer->GetRight();
	//		Vector1 = XMFLOAT3(1.0f, 0.0f, 0.0f);
	//		break;
	//	case 2:
	//	case 3:
	//		//Vector1 = m_pPlayer->GetUp();
	//		Vector1 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//		break;
	//	default:
	//		Vector1 = XMFLOAT3(urd(dre), urd(dre), urd(dre));
	//		break;
	//	}
	//	planeNormal = Vector3::CrossProduct(Vector1, Vector2);
	//	cutterMesh->SetCutPlaneNormal(planeNormal); // 절단면의 노멀
	//	cutterObject->SetMesh(0, cutterMesh, true);
	//
	//
	//	XMFLOAT3 pos = m_pPlayer->GetPosition();
	//	pos = Vector3::Add(pos, Vector3::ScalarProduct(playerLook, 500, false));
	//	pos.y = pos.y + 200.f;
	//	cutterObject->SetPosition(pos);
	//
	//	cutterObject->Rotate(0, m_pPlayer->GetYaw(), 0);
	//
	//	cutterObject->SetAllowCutting(true);	// 이게 켜져있어야 자른다?
	//	//cutterObject->SetShaderType(ShaderType::CObjectsShader);
	//
	//	m_pObjectManager->AddObj(cutterObject, ObjectLayer::CutterObject);
	//	SelectedUInum = -1;
	//}

	CBasicScene::DynamicShaping(pd3dDevice, pd3dCommandList, pFBXDataManager, fTimeElapsed);
}

void CTestScene::Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	//m_pShaderManager->Render(pd3dCommandList, pCamera, ShaderType::CTextShader);
	m_pTextShader->Render(pd3dCommandList, pCamera);

	D2D1_RECT_F textRect = D2D1::RectF(0.0f, 0.0f, FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 14);
	WCHAR text[100];
	WCHAR objText[] = L"맵 상의 오브젝트 갯수 : ";
	int textLen = _countof(objText) - 1;
	vector<CGameObject*>* objList = m_pObjectManager->GetObjectList();
	int objCount = objList[(int)ObjectLayer::Object].size();
	if (objCount == 0) textLen++;
	while (objCount > 0) {
		objCount /= 10; textLen++;
	}

	wsprintf(text, L"%s%d", objText, (int)(objList[(int)ObjectLayer::Object].size()));

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
	pd2dDeviceContext->DrawText(text, textLen, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());
}

void CTestScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);


	//if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	m_pObjectManager->Render(pd3dCommandList, pCamera);



	//D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	//pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights
}

void CTestScene::Enter()
{
}

void CTestScene::Exit()
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
	case WM_LBUTTONDOWN:
		if (true == m_bCutterMode) {
			m_bAddMouseCutter = false;
			m_pCutStartPos = { 0,0 };
			m_pCutEndPos = { 0,0 };
			GetCursorPos(&m_pCutStartPos);
			ScreenToClient(hWnd, &m_pCutStartPos);
			m_bStartCutting = true;
		}
		break;
	case WM_LBUTTONUP:
		if (true == m_bCutterMode && true == m_bStartCutting) {
			GetCursorPos(&m_pCutEndPos);
			ScreenToClient(hWnd, &m_pCutEndPos);
			if (m_pCutStartPos.x != m_pCutEndPos.x && m_pCutStartPos.y != m_pCutEndPos.y)
				m_bAddMouseCutter = true;
			m_bStartCutting = false;
		}
		break;
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
			if (CGameObject* pInterObj = dynamic_cast<CGameObject*>(objects)) {
				float tmin, tmax;
				if (true == m_pObjectManager->CollisionCheck_RayWithAABB(&r, pInterObj, tmin, tmax)) {
					if (nearestDist > tmin) { // 가장 가까운 오브젝트 선별
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
		case 'c': case'C': m_bCutterMode = !m_bCutterMode; break;
		case 'p': case'P': m_bCutAlgorithm = !m_bCutAlgorithm; break;
		case 'm': case 'M': m_bResetFlag = true; break;

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

void CTestScene_Slice::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	//m_pShaderManager->BuildShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pTextShader = new CTextShader();
	m_pTextShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	{
		m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		m_pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
		m_pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pPlayer->SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);
	}

	{
		CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		//CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, pFBXLoader, PEASANT_1_FBX, ShaderType::CObjectsShader);
		((CDynamicShapeMesh*)(pFBXObject->GetMeshes()[0]))->SetCuttable(true);
		pFBXObject->SetCuttable(true);
		pFBXObject->SetPosition(50.0f, 0.0f, 100.0f);
		//pFBXObject->SetScale(0.5f, 0.5f, 0.5f);
		m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
	}

	{
		//CBoxMesh* pBoxMesh = new CBoxMesh(pd3dDevice, pd3dCommandList);
		//CDynamicShapeObject * newObject = new CDynamicShapeObject;
		//newObject->SetMesh(0, pBoxMesh);
		//newObject->SetPosition(200.0f, 0.0f, 100.0f);
		//newObject->Rotate(45.0f, 45.0f, 45.0f);
		//newObject->SetShaderType(ShaderType::CObjectsShader);
		//m_pObjectManager->AddObj(newObject, ObjectLayer::Object);

		//newObject = new CDynamicShapeObject;
		//newObject->SetMesh(0, pBoxMesh);
		//newObject->SetPosition(200.0f, 0.0f, 200.0f);
		//newObject->Rotate(45.0f, 45.0f, 45.0f);
		//newObject->SetShaderType(ShaderType::CObjectsShader);
		//m_pObjectManager->AddObj(newObject, ObjectLayer::Object);

		//newObject = new CDynamicShapeObject;
		//newObject->SetMesh(0, pBoxMesh);
		//newObject->SetPosition(200.0f, 0.0f, 300.0f);
		//newObject->Rotate(0.0f, 0.0f, 0.0f);
		//newObject->SetShaderType(ShaderType::CObjectsShader);
		//m_pObjectManager->AddObj(newObject, ObjectLayer::Object);
	}

	{
		CRayObject* pRayObject = NULL;
		pRayObject = new CRayObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
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
				if (false == m_bCutterMode) {
					SetCursor(NULL);
					m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
					SetCursorPos(ptOldCursorPos.x, ptOldCursorPos.y);
				}
			}

			if (pKeysBuffer[VK_RBUTTON] & 0xF0) { // 우클릭시 오브젝트를 잡아서 이동
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

void CTestScene_Slice::AnimateObjects(float fTotalTime, float fTimeElapsed)
{
	m_pObjectManager->AnimateObjects(fTotalTime, fTimeElapsed);

	std::vector<CGameObject*>* pvObjectList = m_pObjectManager->GetObjectList();

#ifdef _DEBUG
	std::vector<CGameObject*> pvOL = pvObjectList[(int)ObjectLayer::Object];
	if (pvOL.size() < 2) return;
	XMFLOAT4X4 MatA, MatB;
	for (int i = 0; i < pvOL.size() - 1; ++i) {
		if (CGameObject* ObjA = dynamic_cast<CGameObject*>(pvOL[i])) {
			for (int j = i + 1; j < pvOL.size(); ++j) {
				if (CGameObject* ObjB = dynamic_cast<CGameObject*>(pvOL[j])) {
					MatA = ObjA->GetWorldMat();
					MatB = ObjB->GetWorldMat();
					if (CollisionCheck(ObjA->GetCollider(), MatA, ObjB->GetCollider(), MatB)) {
						//cout << "Collision! ObjectNum = " << i << '\t' << j << '\n';
					}
				}
			}
		}
	}
#endif // DEBUG
}

void CTestScene_Slice::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager, float fTimeElapsed)
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
		// 외적 계산 (수직인 벡터)
		XMFLOAT3 planeNormal = Vector3::CrossProduct(randomVector, ray_dir);

		CGameObject* cutterObject = new CGameObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		CCutterBox_NonMesh* cutterMesh = new CCutterBox_NonMesh(pd3dDevice, pd3dCommandList, fBoxSize, fBoxSize, fBoxSize);	// 박스 안의 오브젝트를 절단한다.
		cutterMesh->SetCutPlaneNormal(planeNormal); // 절단면의 노멀
		cutterObject->SetMesh(0, cutterMesh, true);
		cutterObject->SetPosition(Vector3::Add(ray_origin, Vector3::ScalarProduct(ray_dir, fBoxSize)));
		cutterObject->SetAllowCutting(true);	// 이게 켜져있어야 자른다?
		//cutterObject->SetShaderType(ShaderType::CObjectsShader);

		m_pObjectManager->AddObj(cutterObject, ObjectLayer::CutterObject);
	}
	if (m_bCutterMode && m_bAddMouseCutter) {
		m_bAddMouseCutter = false;
		float fBoxSize = 100.0f;
		CRay r = r.RayAtWorldSpace(m_pCutStartPos.x, m_pCutStartPos.y, m_pPlayer->GetCamera());
		CRay r2 = r2.RayAtWorldSpace(m_pCutEndPos.x, m_pCutEndPos.y, m_pPlayer->GetCamera());
		XMFLOAT3 ray_origin = r.GetOriginal();
		XMFLOAT3 ray_dir = r.GetDir();
		XMFLOAT3 ray2_dir = r2.GetDir();

		XMFLOAT3 rayVec = Vector3::Subtract(ray_dir, ray2_dir);
		// 외적 계산 (수직인 벡터)
		XMFLOAT3 planeNormal = Vector3::CrossProduct(rayVec, ray_dir);
		rayVec = Vector3::Add(ray_dir, ray2_dir);
		rayVec = Vector3::Normalize(rayVec);

		CGameObject* cutterObject = new CGameObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		CCutterBox_NonMesh* cutterMesh = new CCutterBox_NonMesh(pd3dDevice, pd3dCommandList, fBoxSize, fBoxSize, fBoxSize);
		cutterMesh->SetCutPlaneNormal(planeNormal);
		cutterObject->SetMesh(0, cutterMesh, true);
		cutterObject->SetPosition(Vector3::Add(ray_origin, Vector3::ScalarProduct(rayVec, fBoxSize)));
		cutterObject->SetAllowCutting(true);
		//cutterObject->SetShaderType(ShaderType::CObjectsShader);

		m_pObjectManager->AddObj(cutterObject, ObjectLayer::CutterObject);
	}

	if (m_bResetFlag) {
		m_bResetFlag = false;
		m_pObjectManager->ClearLayer(ObjectLayer::Object);

		CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		((CDynamicShapeMesh*)(pFBXObject->GetMeshes()[0]))->SetCuttable(true);
		pFBXObject->SetCuttable(true);
		pFBXObject->SetPosition(50.0f, 0.0f, 100.0f);
		m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
	}

	if (true == m_bCutAlgorithm)
		m_pObjectManager->DynamicShaping(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, fTimeElapsed, CDynamicShapeMesh::CutAlgorithm::Push);
	else
		m_pObjectManager->DynamicShaping(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, fTimeElapsed, CDynamicShapeMesh::CutAlgorithm::ConvexHull);
}

void CTestScene_Slice::Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	//m_pShaderManager->Render(pd3dCommandList, pCamera, ShaderType::CTextShader);
	m_pTextShader->Render(pd3dCommandList, pCamera);

	D2D1_RECT_F textRect = D2D1::RectF(0.0f, 0.0f, FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 14);
	WCHAR text[100];
	WCHAR objText[] = L"맵 상의 오브젝트 갯수 : ";
	int textLen = _countof(objText) - 1;
	vector<CGameObject*>* objList = m_pObjectManager->GetObjectList();
	int objCount = objList[(int)ObjectLayer::Object].size();
	if (objCount == 0) textLen++;
	while (objCount > 0) {
		objCount /= 10; textLen++;
	}

	wsprintf(text, L"%s%d", objText, (int)(objList[(int)ObjectLayer::Object].size()));

	ComPtr<ID2D1SolidColorBrush> mSolidColorBrush;
	ComPtr<IDWriteTextFormat> mDWriteTextFormat;

	DX::ThrowIfFailed(pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), mSolidColorBrush.GetAddressOf()));
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
	pd2dDeviceContext->DrawText(text, textLen, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());

	textRect = D2D1::RectF(0.0f, FRAME_BUFFER_HEIGHT / 14 * 1, FRAME_BUFFER_WIDTH / 7 * 4, FRAME_BUFFER_HEIGHT / 14 * 2);
	WCHAR objText3[] = L"소멸 대기 오브젝트 갯수 : ";
	textLen = _countof(objText3) - 1;
	objCount = objList[(int)ObjectLayer::TemporaryObject].size();
	if (objCount == 0) textLen++;
	while (objCount > 0) {
		objCount /= 10; textLen++;
	}
	wsprintf(text, L"%s%d", objText3, (int)(objList[(int)ObjectLayer::TemporaryObject].size()));
	pd2dDeviceContext->DrawText(text, textLen, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());



	DX::ThrowIfFailed(pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Aqua), mSolidColorBrush.GetAddressOf()));
	DX::ThrowIfFailed(pdWriteFactory->CreateTextFormat(
		L"Verdana",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20,
		L"en-us",
		mDWriteTextFormat.GetAddressOf()));

	mDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	mDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	textRect = D2D1::RectF(0.0f, FRAME_BUFFER_HEIGHT / 14 * 3, FRAME_BUFFER_WIDTH / 7 * 4, FRAME_BUFFER_HEIGHT / 14 * 4);
	WCHAR objText2[] = L"현재 모드 (C를 눌러 변경) : ";
	textLen = _countof(objText2) - 1;
	if (true == m_bCutterMode)
	{
		wsprintf(text, L"%s%s", objText2, L"절단모드");
		textLen += 5;
	}
	else {
		wsprintf(text, L"%s%s", objText2, L"카메라모드");
		textLen += 6;
	}
	pd2dDeviceContext->DrawText(text, textLen, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());

	textRect = D2D1::RectF(0.0f, FRAME_BUFFER_HEIGHT / 14 * 4, FRAME_BUFFER_WIDTH / 7 * 4, FRAME_BUFFER_HEIGHT / 14 * 5);
	WCHAR objText4[] = L"절단 알고리즘 (P를 눌러 변경) : ";
	textLen = _countof(objText4) - 1;
	if (true == m_bCutAlgorithm)
	{
		wsprintf(text, L"%s%s", objText4, L"Push");
		textLen += 5;
	}
	else {
		wsprintf(text, L"%s%s", objText4, L"ConvexHull");
		textLen += 11;
	}
	pd2dDeviceContext->DrawText(text, textLen, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());

	textRect = D2D1::RectF(0.0f, FRAME_BUFFER_HEIGHT / 14 * 5, FRAME_BUFFER_WIDTH / 14 * 3, FRAME_BUFFER_HEIGHT / 14 * 6);
	WCHAR objText5[] = L"초기화 (M)";
	textLen = _countof(objText5) - 1;
	pd2dDeviceContext->DrawText(objText5, textLen, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());
}

void CTestScene_Slice::Enter()
{
}

void CTestScene_Slice::Exit()
{
}

CTestScene_PhysX::CTestScene_PhysX()
{
	m_pPhysXManager = new CPhysXManager;
	m_pObjectManager->SetPhysXManager(m_pPhysXManager);
}

CTestScene_PhysX::~CTestScene_PhysX()
{
	delete m_pPhysXManager;
}

void CTestScene_PhysX::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	//m_pShaderManager->BuildShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pTextShader = new CTextShader();
	m_pTextShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	{
		m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		m_pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
		m_pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pPlayer->SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);
	}

	{
		CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		((CDynamicShapeMesh*)(pFBXObject->GetMeshes()[0]))->SetCuttable(true);
		pFBXObject->SetCuttable(true);
		pFBXObject->SetPosition(50.0f, 0.0f, 100.0f);
		m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);

		for (int i = 0; i < (10); ++i) {
			pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
			pFBXObject->SetPosition(50.0f, 50.0f * i, 100.0f);
			m_pObjectManager->AddObj(pFBXObject, ObjectLayer::ObjectPhysX);
		}
	}

	{
		CRayObject* pRayObject = NULL;
		pRayObject = new CRayObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		pRayObject->SetMesh(0, new CRayMesh(pd3dDevice, pd3dCommandList, NULL));
		m_pObjectManager->AddObj(pRayObject, ObjectLayer::Ray);
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CTestScene_PhysX::Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	//m_pShaderManager->Render(pd3dCommandList, pCamera, ShaderType::CTextShader);
	m_pTextShader->Render(pd3dCommandList, pCamera);


	D2D1_RECT_F textRect = D2D1::RectF(0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	static const WCHAR text[] = L"PhysXScene의 Render2D 입니다.";

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

void CTestScene_PhysX::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	m_pObjectManager->Render(pd3dCommandList, pCamera);
}



CTestScene_Animation::CTestScene_Animation()
{

}

CTestScene_Animation::~CTestScene_Animation()
{

}

void CTestScene_Animation::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CFbxLoader_V2 fLoader;

	m_pTextShader = new CTextShader();
	m_pTextShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	{
		m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		m_pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
		m_pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pPlayer->SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pPlayer->SetMaxVelocityXZ(50.0f);
		m_pPlayer->SetMaxVelocityY(30.0f);
		m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);
	}

	//--------------------------------- 조명, 재질 생성 ----------------------------------------

	BuildLightsAndMaterials();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//

	// animaition Test Charactor
	{
		CFBXObject* pNewObject = new CFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CAnimationObjectShader);
		CFBXObject* pNewChildObject = pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, "fbxsdk/Test_Walking.fbx");
		//pNewObject->SetAnimData(pFBXDataManager->LoadAnimDataFromFBX("fbxsdk/Test_Walking.fbx"));
		float xPosition = 0;
		float zPosition = 0;
		float fHeight = 0;
		pNewObject->SetPosition(xPosition, fHeight, zPosition);
		pNewObject->Rotate(0.0f, 0.0f, 0.0f);

		pNewChildObject->SetAnimData(pFBXDataManager->LoadAnimDataFromFBX("fbxsdk/Test_Walking.fbx"));
		pNewObject->SetChild(pNewChildObject);

		//m_pObjectManager->AddObj(pAnimObject, ObjectLayer::ObjectNormal);
		m_pObjectManager->AddObj(pNewObject, ObjectLayer::ObjectNormal);
	}

	{
		CRayObject* pRayObject = NULL;
		pRayObject = new CRayObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		pRayObject->SetMesh(0, new CRayMesh(pd3dDevice, pd3dCommandList, NULL));
		m_pObjectManager->AddObj(pRayObject, ObjectLayer::Ray);
	}

}

void CTestScene_Animation::Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	//m_pShaderManager->Render(pd3dCommandList, pCamera, ShaderType::CTextShader);
	m_pTextShader->Render(pd3dCommandList, pCamera);


	D2D1_RECT_F textRect = D2D1::RectF(0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	static const WCHAR text[] = L"AnimationScene의 Render2D 입니다.";

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