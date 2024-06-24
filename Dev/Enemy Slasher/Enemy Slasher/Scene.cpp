#include "stdafx.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "PhysXManager.h"
#include "Ray.h"
#include "FbxLoader_V3.h"
#include "ResorceManager.h"
#include "GameFramework.h"

CBasicScene::CBasicScene(CGameFramework* GameFramwork)
{
	m_pObjectManager = new CObjectManager;
	m_CurrentTime = 0.0f;
	m_ElapsedTime = 0.0f;
	m_pGameFramework = GameFramwork;
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

void CBasicScene::BuildLightsAndMaterials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 900.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(20.0f, 20.0f, 20.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(20.0f, 20.0f, 20.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(2160.0f, 2700.0f, 2340.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_pLights->m_pLights[1].m_bEnable = false;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 5000.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[1].m_fFalloff = 200.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(10.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(10.0f));

	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);


	m_pMaterials = new MATERIALS;
	::ZeroMemory(m_pMaterials, sizeof(MATERIALS));

	//        Ambient ( 주변광 )          //         Diffuse ( 산란광 )           //       Specullar ( 반사광 )         //       Emissive ( 발광 )     //
	//			Ambient : 일반적으로 장면 전반적인 색상과 밝기를 나타내는 데 사용
	//			Diffuse : 물체의 색상과 표면 특성을 나타내는 데 사용
	//			Specular : 빛이 표면에 반사되는 빛의 색상과 강도를 나타내는 데 사용
	//			Emissive : 물체가 발광하는 경우 해당 물체의 색상과 강도를 나타내는 데 사용
	//			r g b a
	m_pMaterials->m_pReflections[0] = { XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[1] = { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[2] = { XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[3] = { XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[4] = { XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 25.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[5] = { XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[6] = { XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 35.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[7] = { XMFLOAT4(1.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 40.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };

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

void CBasicScene::AnimateObjects(float fTimeTotal, float fTimeElapsed)
{
	m_pObjectManager->AnimateObjects(fTimeTotal, fTimeElapsed);
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

CTitleScene::CTitleScene(CGameFramework* GameFramwork) :CBasicScene(GameFramwork)
{
}

CTitleScene::~CTitleScene()
{
}

bool CTitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	POINT ptCursorPos{ 0,0 };
	GetCursorPos(&ptCursorPos);
	ScreenToClient(hWnd, &ptCursorPos);
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	int clientWidth = clientRect.right - clientRect.left;
	int clientHeight = clientRect.bottom - clientRect.top;

	// 프레임 버퍼 크기를 기준으로 마우스 좌표 변환
	float mouseX = static_cast<float>(ptCursorPos.x) / (clientWidth)*FRAME_BUFFER_WIDTH;
	float mouseY = static_cast<float>(ptCursorPos.y) / (clientHeight)*FRAME_BUFFER_HEIGHT;

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	{
		auto buttonList = m_pObjectManager->GetObjectList(ObjectLayer::ButtonObject);
		for (auto& pObject : buttonList)
		{
			CButtonObject* pButton = dynamic_cast<CButtonObject*>(pObject);
			if (pButton && pButton->IsPointInside(mouseX, mouseY))
			{
				pButton->m_IsClicked = true;
				// m_type -  1번 ( 제목 로고 ) 2번 ( 게임시작 ) 3번 ( 게임종료 )
				if (pButton->GetType() == 2) {
					std::cout << "게임시작" << std::endl;

					// 0 = 타이틀씬 , 1 = 로비씬 , 2 = 메인씬
					m_pGameFramework->SwitchScene(1);
					return true;
				}
				else if (pButton->GetType() == 3) {
					std::cout << "게임종료" << std::endl;
					exit(1);
				}

			}
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		auto buttonList = m_pObjectManager->GetObjectList(ObjectLayer::ButtonObject);
		for (auto& pObject : buttonList)
		{
			CButtonObject* pButton = dynamic_cast<CButtonObject*>(pObject);
			if (pButton && pButton->m_IsClicked)
			{
				if (pButton->IsPointInside(mouseX, mouseY))
				{
					pButton->m_IsClicked = false;
				}
			}
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{

	}
	break;
	case WM_RBUTTONUP:
	{

	}
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

ID3D12RootSignature* CTitleScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
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



	D3D12_ROOT_PARAMETER pd3dRootParameters[12];

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

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[11].Constants.ShaderRegister = 6;
	pd3dRootParameters[11].Constants.RegisterSpace = 0;
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


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

	BuildLightsAndMaterials();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	// 타이틀 객체 생성
	CTitleObject* pBackGround = new CTitleObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CTitleShader);
	pBackGround->SetPosition(0.0f, 0.0f, 0.0f);
	m_pObjectManager->AddObj(pBackGround, ObjectLayer::BackGround);

	// 타이틀 로고 오브젝트 생성 ------------------------------------
	CButtonObject* pButtonObject = new CButtonObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"Image/llogo.png",
		FRAME_BUFFER_WIDTH * 2 / 3, FRAME_BUFFER_HEIGHT / 7, FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 9, ShaderType::C2DObjectShader);
	pButtonObject->SetIsButton(false);
	pButtonObject->SetType(1);	// 타이틀로고버튼
	m_pObjectManager->AddObj(pButtonObject, ObjectLayer::ButtonObject);


	// 게임시작 버튼 오브젝트 생성------------------------------------
	pButtonObject = new CButtonObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"Image/gamestart.png",
		FRAME_BUFFER_WIDTH * 4 / 5, FRAME_BUFFER_HEIGHT * 5 / 8, FRAME_BUFFER_WIDTH / 4, FRAME_BUFFER_HEIGHT / 11, ShaderType::C2DObjectShader);
	pButtonObject->SetType(2);	//게임시작버튼
	m_pObjectManager->AddObj(pButtonObject, ObjectLayer::ButtonObject);


	// 게임종료 버튼 오브젝트 생성
	pButtonObject = new CButtonObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"Image/gameexit.png",
		FRAME_BUFFER_WIDTH * 4 / 5, FRAME_BUFFER_HEIGHT * 6.5 / 8, FRAME_BUFFER_WIDTH / 4, FRAME_BUFFER_HEIGHT / 11, ShaderType::C2DObjectShader);
	pButtonObject->SetType(3);	//게임종료버튼
	m_pObjectManager->AddObj(pButtonObject, ObjectLayer::ButtonObject);



	//m_textObjects.push_back(new CTextObject(L"Enemy Slasher", D2D1::RectF(0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT / 2), L"Verdana", 60.0f, D2D1::ColorF::RosyBrown));
	//m_textObjects.push_back(new CTextObject(L"게임 시작", D2D1::RectF(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT / 4 * 3), L"Verdana", 40.0f, D2D1::ColorF::RosyBrown));
	//m_textObjects.push_back(new CTextObject(L"게임 종료", D2D1::RectF(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 4 * 3, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT), L"Verdana", 40.0f, D2D1::ColorF::RosyBrown));


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

	m_pTextShader->Render(pd3dCommandList, pCamera);

	// 기존의 텍스트 렌더링 코드를 CTextObject로 변경
	for (auto textObject : m_textObjects)
	{
		textObject->Render(pd2dDeviceContext, pdWriteFactory);
	}

}

void CTitleScene::Enter()
{
}

void CTitleScene::Exit()
{
}

void CTitleScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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

void CTitleScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
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

void CTitleScene::ReleaseShaderVariables()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





CTestScene::CTestScene(CGameFramework* GameFramwork) :CBasicScene(GameFramwork)
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



	D3D12_ROOT_PARAMETER pd3dRootParameters[12];

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

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[11].Constants.ShaderRegister = 6;
	pd3dRootParameters[11].Constants.RegisterSpace = 0;
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


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

	// 프레임 버퍼 크기를 기준으로 마우스 좌표 변환
	float mouseX = static_cast<float>(ptCursorPos.x) / (clientWidth)*FRAME_BUFFER_WIDTH;
	float mouseY = static_cast<float>(ptCursorPos.y) / (clientHeight)*FRAME_BUFFER_HEIGHT;

	// 현재 차례인 오브젝트의 팀
	int teamID = m_pvEngagedObjects[m_iTurnFlag]->GetTeamId();

	auto buttonList = m_pObjectManager->GetObjectList(ObjectLayer::ButtonObject);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	{
		if (m_iOverFlag == 0)
			if (teamID == 0)
			{
				for (auto& pObject : buttonList)
				{
					CButtonObject* pButton = dynamic_cast<CButtonObject*>(pObject);
					if (pButton && pButton->m_IsButton && pButton->IsPointInside(mouseX, mouseY))
					{
						pButton->m_IsClicked = true;
					}
				}
			}
	}
	break;
	case WM_LBUTTONUP:
	{
		if (m_iOverFlag == 0)
			if (teamID == 0)
			{
				for (auto& pObject : buttonList)
				{
					CButtonObject* pButton = dynamic_cast<CButtonObject*>(pObject);
					if (pButton && pButton->m_IsButton && pButton->m_IsClicked)
					{
						if (pButton->IsPointInside(mouseX, mouseY))
						{
							// 지금은 턴 종료 버튼밖에 없음.
							IncreaseTurnFlag();

							pButton->m_IsClicked = false;
						}
					}
				}
			}
	}
	break;
	case WM_RBUTTONDOWN:
		if(m_iOverFlag == 0)
			if(teamID == 0)
				if (pCoveredUI)
				{
					SetSelectedUI(pCoveredUI);
					//m_pSelectedUI->ButtenDown();
				}
		break;

	case WM_RBUTTONUP:
		if (m_iOverFlag == 0)
			if (teamID == 0)
				if (m_pSelectedUI)
				{
					// 현재 카드UI 전용코드가 올라가있음.
					// 아래 내용을 CCardUIObject.ButtenUp()에 넣어야함.
					//m_pSelectedUI->ButtenUp();
					if (ptCursorPos.y > (float)clientHeight / 5 * 4)
					{
						// 원위치로 돌아감.
						m_pSelectedUI->SetPositionUI(m_pSelectedUI->GetPositionUI().x, (float)clientHeight / 10 * 9);
						SetSelectedUI(nullptr);
					}
					else
					{
						if (m_pPlayer->GetCharacterState() == CharacterState::IdleState)
							UseSelectedCard();
						else
						{
							m_pSelectedUI->SetPositionUI(m_pSelectedUI->GetPositionUI().x, (float)clientHeight / 10 * 9);
							SetSelectedUI(nullptr);
						}
					}
				}
		break;


	default:
		break;
	}
	return false;
}

bool CTestScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	//int drawnCard;
	int tmp = 0;
	std::vector<int> intVector;
	std::vector<CGameObject*>* pObjectList = m_pObjectManager->GetObjectList();
	std::vector<CGameObject*>* pInteractiveUIObj = &pObjectList[(int)ObjectLayer::CardButtonObject];
	std::vector<CGameObject*> ButtonObjectLayer = pObjectList[(int)ObjectLayer::ButtonObject];
	CButtonObject* turnEnd = dynamic_cast<CButtonObject*>(ButtonObjectLayer[3]);

	// 현재 차례인 오브젝트의 팀
	int teamID = m_pvEngagedObjects[m_iTurnFlag]->GetTeamId();

	//std::default_random_engine dre(0);
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
			//case 'W': m_pPlayer->Move(DIR_FORWARD, m_pPlayer->GetMoveSpeed(), true); break;
			//case 'S': m_pPlayer->Move(DIR_BACKWARD, m_pPlayer->GetMoveSpeed(), true); break;
			//case 'A': m_pPlayer->Move(DIR_LEFT, m_pPlayer->GetMoveSpeed(), true); break;
			//case 'D': m_pPlayer->Move(DIR_RIGHT, m_pPlayer->GetMoveSpeed(), true); break;
		case 'Q': m_pPlayer->Move(DIR_UP, 5000.0f, true); break;
		case 'E': m_pPlayer->Move(DIR_DOWN, 5000.0f, true); break;
		case 'R': m_pPlayer->Rotate(0.0f, 20.0f, 0.0f);	break;
		case 'T': m_pPlayer->Rotate(0.0f, -20.0f, 0.0f); break;
		case 'Z':
		case 'z':
			break;
		case 'X':
		case 'x':
			//std::cout << "m_currentPhase : " << (int)m_currentPhase << std::endl;
			/*intVector = m_pvEngagedObjects[m_iTurnFlag]->GetDeckData()->GetHand();
			std::cout << "hand : ";
			for (int card : intVector)
				std::cout << card << ", ";
			std::cout << std::endl;

			intVector = m_pvEngagedObjects[m_iTurnFlag]->GetDeckData()->GetDeck();
			std::cout << "Deck : ";
			for (int card : intVector)
				std::cout << card << ", ";
			std::cout << std::endl;

			intVector = m_pvEngagedObjects[m_iTurnFlag]->GetDeckData()->GetUsed();
			std::cout << "Used : ";
			for (int card : intVector)
				std::cout << card << ", ";
			std::cout << std::endl;*/
			break;
		case 'C':
		case 'c':
			break;
		case VK_SPACE:
			if (m_iOverFlag == 0)
				if (teamID == 0)
					if (m_currentPhase == TurnPhase::PlayPhase)
						turnEnd->m_IsClicked = true;;
						//IncreaseTurnFlag();
			//cout << m_pvEngagedObjects.size() << endl;
			break;

		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			if (m_iOverFlag == 0)
				if (teamID == 0)
					{
						tmp = wParam - 0x30 - 1;
						if (m_currentPhase == TurnPhase::PlayPhase)
						{
							if (pInteractiveUIObj->size() > 0) {
								if (tmp < m_pvEngagedObjects[m_iTurnFlag]->GetDeckData()->GetHand().size())
								{
									if (m_pSelectedUI == (CCardButton*)pObjectList[(int)ObjectLayer::CardButtonObject][tmp])
									{
										if (m_pPlayer->GetCharacterState() == CharacterState::IdleState)
											UseSelectedCard();
										else
											SetSelectedUI(nullptr);
									}
									else
									{
										SetSelectedUI((CCardButton*)pObjectList[(int)ObjectLayer::CardButtonObject][tmp]);
									}
								}
							}
						}
					}
			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_SPACE:
			if (m_iOverFlag == 0)
				if (teamID == 0)
					if (m_currentPhase == TurnPhase::PlayPhase)
					{
						turnEnd->m_IsClicked = false;;
						IncreaseTurnFlag();
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

void CTestScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CFbxLoader_V3 fLoader;
	m_pObjectManager->SetPhysXManager(pFBXDataManager->GetPhysXManager());

	m_pTextShader = new CTextShader();
	m_pTextShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	// Collider Shader 등록
	CObjectsShader* pColliderShader = new CObjectsShader();
	pColliderShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pObjectManager->SetColliderShader(pColliderShader);



	// 37 : 24 270					11.25
	CFbx_V3::CFbxData* player1 = fLoader.LoadFbx("fbxsdk/", "Player1");
	fLoader.LoadAnim(player1->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "SwordAndShieldIdle");
	fLoader.LoadAnim(player1->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "SwordAndShieldAttack");
	fLoader.LoadAnim(player1->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "SwordAndShieldMove");
	fLoader.LoadAnim(player1->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "SwordAndShieldDie");
	fLoader.LoadAnim(player1->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "SwordAndShieldBuff");
	
	//fLoader.LoadAnim(player1->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Player_Idle");
	//fLoader.LoadAnim(player1->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Player_Running");
	m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CTextureShader);
	m_pPlayer->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, player1));
	//m_pPlayer->SetScale(3.0f, 3.0f, 3.0f);
	//CFBXTestMesh* pPlayerMesh = new CFBXTestMesh(pd3dDevice, pd3dCommandList, fLoader.LoadFBX(PEASANT_1_FBX));
	//m_pPlayer->SetMesh(0, pPlayerMesh);

	m_pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	//m_pPlayer->SetPosition(XMFLOAT3(2160.0f, 2000.0f, 2340));
	m_pPlayer->SetPosition(XMFLOAT3(0.0f, 50.0f, 0.0f));
	//m_pPlayer->SetMaxVelocityXZ(270.0f);
	m_pPlayer->SetMaxVelocityXZ(1500.0f);
	m_pPlayer->SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pPlayer->SetCharacterByName("SwordAndShield");
	m_pPlayer->SetAnimNum(1);
	m_pPlayer->CCharacterObject::SetCamera(m_pPlayer->GetCamera());
	m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);

	//--------------------------------- 조명, 재질 생성 ----------------------------------------

	BuildLightsAndMaterials();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//

	// -------------------------------      스카이 박스     _____________________________________

	CSkyBox* pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CSkyBoxShader, 6);
	m_pObjectManager->AddObj(pSkyBox, ObjectLayer::SkyBox);

	// -------------------------------      스카이 박스 끝    _____________________________________



	// -------------------------------   맵 추가 ----------------------------------------------

	CFBXObject* pMapObject = new CFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CTextureShader);

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution <> urd(0, 6);
	int random_number = 0;
	//int random_number = urd(dre);
	if (random_number == 0) {
		pMapObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "final_map"));
	}
	else if (random_number == 1) {
		pMapObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "map2"));
		m_pPlayer->SetPosition(XMFLOAT3(0.0f, 100.0f, 0.0f));
	}
	else if (random_number == 2) {
		pMapObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "map2"));
		m_pPlayer->SetPosition(XMFLOAT3(11000.0f, 100.0f, -2200.0f));
	}
	else if (random_number == 3) {
		pMapObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "map2"));
		m_pPlayer->SetPosition(XMFLOAT3(-15000.0f, 100.0f, -17800.0f));
	}
	else if (random_number == 4) {
		pMapObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "map3"));
		m_pPlayer->SetPosition(XMFLOAT3(0.0f, 100.0f, 0.0f));
	}
	else if (random_number == 5) {
		pMapObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "map3"));
		m_pPlayer->SetPosition(XMFLOAT3(-30300.0f, 100.0f, 0.0f));
	}
	else if (random_number == 6) {
		pMapObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "map3"));
		m_pPlayer->SetPosition(XMFLOAT3(-17000.0f, 100.0f, -9000.0f));
	}

	pMapObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pObjectManager->AddObj(pMapObject, ObjectLayer::Map);


	// ----------------- 버튼 오브젝트 ------------------
	//CButtonObject* pButtonObject = new CButtonObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, 320.0f, 240.f, 100.0f, 100.0f, ShaderType::C2DObjectShader);
	//m_pObjectManager->AddObj(pButtonObject, ObjectLayer::ButtonObject);


	// --------------------------------- 빌보드 인스턴스 오브젝트 ------------------------------


	//CBillBoardInstanceObject* pBillBoardObjects = new CBillBoardInstanceObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pTerrain, ShaderType::CBillBoardInstanceShader);
	//m_pObjectManager->AddObj(pBillBoardObjects, ObjectLayer::BillBoardObject);

	// ------------------------------ 적 몬스터 오브젝트 --------------------------------------



	// monster
	{
		// 아마 좀비끼리는 섞어쓸 수 있을?지도
		// overdrive : 50일때 24프레임(1초)에 키가 160일때 270를 감.		
		// 50일때를 270이라고하면											11.25
		// 25일때 202.5일거임. 아마											8.4375


		//zombie4
		// 50 : 121 프레임에 135.   키 180 아마 ㄱㅊ을거임					1.033057851239669
		// 100 : 61 135.													2.213114754098361
		//

		//boss
		// 55 : 24 200.														8.333333333333333

		CMonsterObject* pMonsterObject;
		CFbx_V3::CFbxData* zombie[5];
		zombie[0] = fLoader.LoadFbx("fbxsdk/", "Zombie1");
		fLoader.LoadAnim(zombie[0]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie1Idle");
		fLoader.LoadAnim(zombie[0]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie1Attack");
		fLoader.LoadAnim(zombie[0]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie1Move");
		fLoader.LoadAnim(zombie[0]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie1Die");
		zombie[1] = fLoader.LoadFbx("fbxsdk/", "Zombie2");
		fLoader.LoadAnim(zombie[1]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie2Idle");
		fLoader.LoadAnim(zombie[1]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie2Attack");
		fLoader.LoadAnim(zombie[1]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie2Move");
		fLoader.LoadAnim(zombie[1]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie2Die");
		fLoader.LoadAnim(zombie[1]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie2Buff");
		zombie[2] = fLoader.LoadFbx("fbxsdk/", "Zombie3");
		fLoader.LoadAnim(zombie[2]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie3Idle");
		fLoader.LoadAnim(zombie[2]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie3Attack");
		fLoader.LoadAnim(zombie[2]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie3Move");
		fLoader.LoadAnim(zombie[2]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie3Die");
		fLoader.LoadAnim(zombie[2]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie3Buff");
		zombie[3] = fLoader.LoadFbx("fbxsdk/", "Zombie4");
		fLoader.LoadAnim(zombie[3]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie4Idle");
		fLoader.LoadAnim(zombie[3]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie4Attack");
		fLoader.LoadAnim(zombie[3]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie4Move");
		fLoader.LoadAnim(zombie[3]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Zombie4Die");
		zombie[4] = fLoader.LoadFbx("fbxsdk/", "ZombieBoss");
		fLoader.LoadAnim(zombie[4]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "ZombieBossIdle");
		fLoader.LoadAnim(zombie[4]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "ZombieBossAttack");
		fLoader.LoadAnim(zombie[4]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "ZombieBossMove");
		fLoader.LoadAnim(zombie[4]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "ZombieBossDie");
		fLoader.LoadAnim(zombie[4]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "ZombieBossBuff");

		//for (int i = 0; i < 1; ++i) {
		//	string name = "Zombie" + i + 1;
		//	if (i == 4)
		//		name = "ZombieBoss";
		//	fLoader.LoadAnim(zombie[i]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", name + "Idle");
		//	fLoader.LoadAnim(zombie[i]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", name + "Attack");
		//	fLoader.LoadAnim(zombie[i]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", name + "Move");
		//	fLoader.LoadAnim(zombie[i]->m_pRootObjectData->m_pSkeleton, "fbxsdk/", name + "Die");
		//}
		if (random_number == 0)
		{
			// ---------------- A 섹터 --------------------- 
			{
				//---------------------------  좀비 1 -------------------------------------------
				pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
				pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[0]));
				pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
				//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
				pMonsterObject->SetPosition(12765.0f, 0.0f, 3186.0f);
				pMonsterObject->SetTeamId(1);
				pMonsterObject->SetCharacterByName("Zombie1");
				m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
				//---------------------------  좀비 2 -------------------------------------------
				pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
				pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
				pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
				//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
				pMonsterObject->SetPosition(14593.0f, 0.0f, -432.0f);
				pMonsterObject->SetTeamId(1);
				pMonsterObject->SetCharacterByName("Zombie2");
				m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
				//---------------------------  좀비 3 -------------------------------------------
				pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
				pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
				pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
				//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
				pMonsterObject->SetPosition(16566.0f, 0.0f, -1952.0f);
				pMonsterObject->SetTeamId(1);
				pMonsterObject->SetCharacterByName("Zombie3");
				m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
				//---------------------------  좀비 4 -------------------------------------------
				pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
				pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[3]));
				pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
				//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
				pMonsterObject->SetPosition(14859.0f, 0.0f, 4636.0f);
				pMonsterObject->SetTeamId(1);
				pMonsterObject->SetCharacterByName("Zombie4");
				m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

			}
			// ---------------- B 섹터 ---------------------
		//	{
		//		//---------------------------  좀비 1 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[0]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(27650.0f, 0.0f, 9271.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie1");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//		//---------------------------  좀비 2 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(28928.0f, 0.0f, 8129.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie2");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//		//---------------------------  좀비 3 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(25602.0f, 0.0f, 4795.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie3");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	}
		//	// ---------------- C 섹터 ---------------------
		//	{
		//		//---------------------------  좀비 1 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[0]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(26856.0f, 0.0f, -5531.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie1");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//		//---------------------------  좀비 2 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(24037.0f, 0.0f, -8650.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie2");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//		//---------------------------  좀비 3 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(25858.0f, 0.0f, -14250.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie3");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	}
		//	// ---------------- D 섹터 ---------------------
		//	{
		//		//---------------------------  좀비 1 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[0]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(30818.0f, 0.0f, -2297.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie1");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//		//---------------------------  좀비 2 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(38175.0f, 0.0f, -2542.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie2");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//		//---------------------------  좀비 3 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(41804.0f, 0.0f, -3246.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie3");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	}
		//	// ---------------- E 섹터 ---------------------
		//	{
		//		//---------------------------  좀비 1 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[0]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(41104.0f, 0.0f, 7827.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie1");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//		//---------------------------  좀비 2 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(44483.0f, 0.0f, 7231.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie2");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//		//---------------------------  좀비 3 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(44051.0f, 0.0f, 4317.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie3");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	}
		//	// ---------------- F 섹터 ---------------------
		//	{
		//		//---------------------------  좀비 1 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[0]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(36135.0f, 0.0f, -13805.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie1");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//		//---------------------------  좀비 2 -------------------------------------------
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//		pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(42683.0f, 0.0f, -13215.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("Zombie2");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	}
		//	// ---------------- G 섹터 ---------------------
		//	{
		//		pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//		pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[4]));
		//		pMonsterObject->SetScale(3.0f, 3.0f, 3.0f);
		//		//pMonsterObject->SetInitialRotate(-90.0f, 180.0f, 0.0f);
		//		pMonsterObject->SetPosition(51383.0f, 0.0f, -3133.0f);
		//		pMonsterObject->SetTeamId(1);
		//		pMonsterObject->SetCharacterByName("ZombieBoss");
		//		m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//		m_pBoss = pMonsterObject;
		//	}
		//}
		//else if (random_number == 1 || random_number == 2 || random_number == 3)
		//{
		//	//---------------------------  좀비 1 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[0]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-5900, 100, -2500);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie1");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	//---------------------------  좀비 2 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-7500, 100, -3900);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie2");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	//---------------------------  좀비 3 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-3100, 100, -1700);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie3");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	//---------------------------  좀비 4 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[3]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-6350, 100, -4600);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie4");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	//---------------------------  좀비 1 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[0]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-14200, 100, -2600);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie1");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	//---------------------------  좀비 2 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-16200, 100, -6600);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie2");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	//---------------------------  좀비 3 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-14000, 100, -4600);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie3");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);


		//	//---------------------------  좀비 4 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[3]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-7000, 100, -18200);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie4");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	//---------------------------  좀비 2 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-5000, 100, -16400);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie2");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	//---------------------------  좀비 3 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-9000, 100, -19000);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie3");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	//--------------------------보스------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[4]));
		//	pMonsterObject->SetScale(3.0f, 3.0f, 3.0f);
		//	pMonsterObject->SetPosition(10600, 100, -17844);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("ZombieBoss");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	m_pBoss = pMonsterObject;

		//	//---------------------------  좀비 1 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[0]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(300, 100, -6100);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie1");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	//---------------------------  좀비 2 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(0, 100, -8100);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie2");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	//---------------------------  좀비 3 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(1500, 100, -4100);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie3");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	//---------------------------  좀비 4 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[3]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(15300, 100, -10000);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie4");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	//---------------------------  좀비 2 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(17300, 100, -8000);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie2");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	//---------------------------  좀비 3 -------------------------------------------
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(14300, 100, -6000);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie3");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//}
		//else
		//{
		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[0]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(0, 100, -6100);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie1");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-1000, 100, -9100);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie2");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[3]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-500, 100, -12100);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie4");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[0]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-300, 100, -20200);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie1");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-500, 100, -23200);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie2");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-800, 100, -24200);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie3");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[3]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-7900, 100, -26100);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie4");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-14800, 100, -18400);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie3");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-23400, 100, -15600);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie2");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-27400, 100, -16000);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie3");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[4]));
		//	pMonsterObject->SetScale(3.0f, 3.0f, 3.0f);
		//	pMonsterObject->SetPosition(-24000, 100, -26300);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("ZombieBoss");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		//	m_pBoss = pMonsterObject;

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-30500, 100, -10200);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie2");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-30500, 100, -13200);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie3");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[1]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-18200, 100, -2000);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie2");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);

		//	pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
		//	pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, zombie[2]));
		//	pMonsterObject->SetScale(1.5f, 1.5f, 1.5f);
		//	pMonsterObject->SetPosition(-18200, 100, -4000);
		//	pMonsterObject->SetTeamId(1);
		//	pMonsterObject->SetCharacterByName("Zombie3");
		//	m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::Enemy);
		}
	}
	m_isBossExist = (m_pBoss != nullptr);


	//// UI
	
	//{
	//	//카드 UI 테스트용 오브젝트.
	//	//CCubeMeshTextured* pCardMesh = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, 412.0f, 582.0f, 1.0f);
	//	//CFBXMesh* pCardMesh = new CFBXMesh(pd3dDevice, pd3dCommandList, fLoader.LoadFbx("fbxsdk/", "Card")->m_vpMeshs[0]);
	//	//CFBXMesh* pCardMesh = new CFBXMesh(pd3dDevice, pd3dCommandList, fLoader.LoadFBX(CARD_FBX)->m_pvMeshs[0]);
	//
	//	CCardUIObject* pCardUIObject;
	//	CCardUIObject::InitializeTexture(pd3dDevice, pd3dCommandList);
	//	for (int i = 0; i < m_iMaxHandCount; i++)
	//	{
	//		pCardUIObject = new CCardUIObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, m_pPlayer->GetCamera(), ShaderType::CUITextureShader);
	//		pCardUIObject->SetPositionUI(100, 100);
	//		// 순서 중요
	//		pCardUIObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "Card2"));
	//		pCardUIObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "CardFace2"));
	//		pCardUIObject->InitializeMaterial(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	//		//
	//		pCardUIObject->SetScale(1, 1, 1);
	//		m_pObjectManager->AddObj(pCardUIObject, ObjectLayer::InteractiveUIObject);
	//	}
	//}

	{
		CCardButton* pCardButtonObject;
		CCardButton::InitializeTexture(pd3dDevice, pd3dCommandList);
		for (int i = 0; i < m_iMaxHandCount; i++)
		{
			pCardButtonObject = new CCardButton(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature,
				FRAME_BUFFER_WIDTH * 2 / 3 + i*100, FRAME_BUFFER_HEIGHT / 7, FRAME_BUFFER_WIDTH / 10, FRAME_BUFFER_HEIGHT / 5, ShaderType::C2DObjectShader);
			// 위치xy, 크기 xy
			pCardButtonObject->InitializeMaterial(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
			//
			//pCardButtonObject->SetScale(2, 2, 2);
			//m_pObjectManager->AddObj(pCardButtonObject, ObjectLayer::InteractiveUIObject);
			m_pObjectManager->AddObj(pCardButtonObject, ObjectLayer::CardButtonObject);
		}
	}
	///  버튼 오브젝트 m_iOverFlag의 역순으로 추가.
	{
		// 종료 관련
		CButtonObject* pButtonObject = new CButtonObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"Image/gameClear.png",
			FRAME_BUFFER_WIDTH *2 , FRAME_BUFFER_HEIGHT * 2, FRAME_BUFFER_WIDTH/2, FRAME_BUFFER_HEIGHT/4, ShaderType::C2DObjectShader);
		pButtonObject->SetIsButton(false);
		m_pObjectManager->AddObj(pButtonObject, ObjectLayer::ButtonObject);

		pButtonObject = new CButtonObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"Image/gameOver.png",
			FRAME_BUFFER_WIDTH * 2, FRAME_BUFFER_HEIGHT * 2, FRAME_BUFFER_WIDTH/2, FRAME_BUFFER_HEIGHT/4, ShaderType::C2DObjectShader);
		pButtonObject->SetIsButton(false);
		m_pObjectManager->AddObj(pButtonObject, ObjectLayer::ButtonObject);

		pButtonObject = new CButtonObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"Image/black.png",
			FRAME_BUFFER_WIDTH * 2, FRAME_BUFFER_HEIGHT * 2, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, ShaderType::C2DObjectShader);
		pButtonObject->SetIsButton(false);
		m_pObjectManager->AddObj(pButtonObject, ObjectLayer::ButtonObject);

		// 그 외
		pButtonObject = new CButtonObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"Image/TurnEnd.png",
			FRAME_BUFFER_WIDTH * 2, FRAME_BUFFER_HEIGHT * 2, FRAME_BUFFER_WIDTH/5, FRAME_BUFFER_HEIGHT/12, ShaderType::C2DObjectShader);
		pButtonObject->SetIsButton(true);
		m_pObjectManager->AddObj(pButtonObject, ObjectLayer::ButtonObject);
	}

	//turn 턴 관련 초기화

	m_iTurnFlag = 0;
	m_currentPhase = TurnPhase::NON;
	//m_currentPhase = TurnPhase::StartPhase; // 테스트용
	Engage(m_pPlayer);

	CRay r = r.RayAtWorldSpace(0, 0, m_pPlayer->GetCamera());

	CRayObject* pRayObject = NULL;
	pRayObject = new CRayObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
	pRayObject->SetMesh(0, new CRayMesh(pd3dDevice, pd3dCommandList, NULL));
	m_pObjectManager->AddObj(pRayObject, ObjectLayer::Ray);
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
	m_ptCenterCursorPos = POINT{ FRAME_BUFFER_WIDTH / 2,FRAME_BUFFER_HEIGHT / 2 };
	//GetCursorPos(&m_ptCenterCursorPos);
	ScreenToClient(hWnd, &m_ptCenterCursorPos);

	if (m_iOverFlag != 0) 
		return(true);

	DWORD dwDirection = 0;
	if (m_currentPhase == TurnPhase::NON)
	{
		if (pKeysBuffer['W'] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeysBuffer['S'] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeysBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeysBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;
	
	}

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

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	int clientWidth = clientRect.right - clientRect.left;
	int clientHeight = clientRect.bottom - clientRect.top;
	//-------------- 피킹
	//CRay pkRay = pkRay.RayAtWorldSpace(ptCursorPos.x, ptCursorPos.y, m_pPlayer->GetCamera());

	std::vector<CGameObject*>* pObjectList = m_pObjectManager->GetObjectList();

	pCoveredUI = NULL;

	float mouseX = static_cast<float>(ptCursorPos.x) / (clientWidth)*FRAME_BUFFER_WIDTH;
	float mouseY = static_cast<float>(ptCursorPos.y) / (clientHeight)*FRAME_BUFFER_HEIGHT;

	for (int lc = 0; lc < (int)ObjectLayer::Count; lc++)
	{
		if (lc == (int)ObjectLayer::CardButtonObject)
		{
			for (int i = 0; i < pObjectList[lc].size(); i++) {
				CCardButton* obj = (CCardButton*)pObjectList[lc][i];
				if (!obj)
					continue;

				//float tmin, tmax;
				//if (true == m_pObjectManager->CollisionCheck_RayWithOBB(&r, obj, tmin, tmax)) {
				if(obj->IsPointInside(mouseX, mouseY)){
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
		//else if (lc == (int)ObjectLayer::Ray)
		//{
		//	CRayObject* rayOb = (CRayObject*)pObjectList[lc][0];
		//	rayOb->Reset(pkRay);
		//}
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

				if (m_pPlayer->GetCharacterState() != CharacterState::MoveState)
					m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);

				// 여기는 뭐야?
				//std::vector<CGameObject*>* pvObjectList = m_pObjectManager->GetObjectList();
				//if (false == pvObjectList[(int)ObjectLayer::Object].empty()) {
				//	for (const auto& obj : pvObjectList[(int)ObjectLayer::Object]) {
				//		if (CGameObject* IterObj = dynamic_cast<CGameObject*>(obj)) {
				//			XMFLOAT4X4 playerMat = m_pPlayer->GetWorldMat();
				//			XMFLOAT4X4 objMat = IterObj->GetWorldMat();
				//			if (true == CollisionCheck(m_pPlayer->GetCollider(), playerMat, IterObj->GetCollider(), objMat)) {
				//				m_pPlayer->Rotate(-cyDelta, -cxDelta, 0.0f);
				//			}
				//		}
				//	}
				//}
			}
			else if (pKeysBuffer[VK_RBUTTON] & 0xF0)
			{
				//// 선택 카드 드래그.
				//if (m_pSelectedUI)
				//{
				//	m_pSelectedUI->SetPositionUI(ptCursorPos);
				//}
			}
			//
		}
		if (dwDirection) m_pPlayer->Move(dwDirection, 100.0f, true);
	}
	// 선택 카드 드래그.
	if (m_pSelectedUI)
	{
		if (m_pvEngagedObjects[m_iTurnFlag]->GetTeamId() == 0)
			m_pSelectedUI->SetPositionUI(ptCursorPos);
		else if (m_pvEngagedObjects[m_iTurnFlag]->GetTeamId() == 1) // 부드러운 움직임을 구현하면 이건 필요없어질듯.
			m_pSelectedUI->CursorOverObject(true);
	}


	// 카드 위치지정하는 부분.

	// 이 레이어가 비어있을 수 있기 떄문에 먼저 포인터를 받아 검사
	std::vector<CGameObject*>* pInteractiveUIObj = &pObjectList[(int)ObjectLayer::CardButtonObject];
	if (pInteractiveUIObj->size() > 0) {
		if (m_pvEngagedObjects.size() >= 2)
		{
			CCharacterObject* turnedObj = m_pvEngagedObjects[m_iTurnFlag];

			vector hand = turnedObj->GetDeckData()->GetHand();
			int handSize = turnedObj->GetDeckData()->GetCurrentHandSize();


			for (int i = 0; i < handSize; i++)
			{
				CCardButton* obj = (CCardButton*)pObjectList[(int)ObjectLayer::CardButtonObject][i];
				if (obj != m_pSelectedUI)
				{
					obj->SetPositionUI(clientWidth / 2 + ((float)i - (float)handSize / 2) * (clientWidth / 12) + (clientWidth / 24), (float)clientHeight / 10 * 9);
				}
				//if (turnedObj->GetTeamId() == 1)
				//	obj->Rotate(0, 180, 0);
			}
			// 손에 없는 카드 위치
			for (int i = handSize; i < m_iMaxHandCount; i++)
			{
				CCardButton* obj = (CCardButton*)pObjectList[(int)ObjectLayer::CardButtonObject][i];
				if (obj != m_pSelectedUI)
					obj->SetPositionUI(clientWidth * 1.2, (float)clientHeight / 10 * 9);
			}
		}
		else
		{
			for (int i = 0; i < m_iMaxHandCount; i++)
			{
				CCardButton* obj = (CCardButton*)pObjectList[(int)ObjectLayer::CardButtonObject][i];
				if (obj != m_pSelectedUI)
					obj->SetPositionUI(clientWidth * 1.2, (float)clientHeight / 10 * 9);
			}
		}
	}



	return(true);
}

void CTestScene::AnimateObjects(float fTotalTime, float fTimeElapsed)
{
	m_pObjectManager->AnimateObjects(fTotalTime, fTimeElapsed);

	std::vector<CGameObject*>* pvObjectList = m_pObjectManager->GetObjectList();

	if (false == pvObjectList[(int)ObjectLayer::Player].empty()) { // Player가 있다면
		CPlayer* pPlayer = (CPlayer*)pvObjectList[(int)ObjectLayer::Player][0];

		//if (false == pvObjectList[(int)ObjectLayer::Object].empty()) {
		//	for (const auto& obj : pvObjectList[(int)ObjectLayer::Object]) {
		//		if (CGameObject* IterObj = dynamic_cast<CGameObject*>(obj)) {
		//			XMFLOAT3 xmfPlayerPos = pPlayer->GetPosition();
		//			XMFLOAT4X4 playerMat = pPlayer->GetWorldMat();
		//			XMFLOAT4X4 objMat = IterObj->GetWorldMat();
		//			if (CollisionCheck(pPlayer->GetCollider(), playerMat, IterObj->GetCollider(), objMat)) {
		//				XMFLOAT3 playerVal = pPlayer->GetVelocity();
		//				xmfPlayerPos.x -= (playerVal.x * fTimeElapsed);
		//				xmfPlayerPos.z -= (playerVal.z * fTimeElapsed);
		//				pPlayer->SetPosition(xmfPlayerPos);
		//			}
		//		}
		//	}
		//}

		//if (false == pvObjectList[(int)ObjectLayer::Terrain].empty()) { // Terrain과 Player가 있다면
		//	XMFLOAT3 xmfPlayerPos = pPlayer->GetPosition();
		//	float fHeight = ((CHeightMapTerrain*)pvObjectList[(int)ObjectLayer::Terrain][0])->GetHeight(xmfPlayerPos.x, xmfPlayerPos.z);
		//
		//	if (xmfPlayerPos.y < fHeight) {
		//		xmfPlayerPos.y = fHeight;
		//		pPlayer->SetPosition(xmfPlayerPos);
		//		XMFLOAT3 xmfVelocity = pPlayer->GetVelocity();
		//		xmfVelocity.y = 0.0f;
		//		pPlayer->SetVelocity(xmfVelocity);
		//	}
		//}
	}
	if (m_pLights)
	{
		float px = m_pPlayer->GetPosition().x;
		float py = m_pPlayer->GetPosition().y + 500.0f;
		float pz = m_pPlayer->GetPosition().z;

		float lx = m_pPlayer->GetLookVector().x;
		float ly = m_pPlayer->GetLookVector().y;
		float lz = m_pPlayer->GetLookVector().z;

		float falloff = 500.0f;
		XMFLOAT3 end_position = XMFLOAT3(px + lx * falloff, py + ly * falloff, pz + lz * falloff);

		m_pLights->m_pLights[0].m_xmf3Position = end_position;
		m_pLights->m_pLights[0].m_xmf3Direction = m_pPlayer->GetLookVector();

		m_pLights->m_pLights[1].m_xmf3Position = end_position;
		m_pLights->m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}

	// 전투 관련
	CCharacterObject* turnObj = m_pvEngagedObjects[m_iTurnFlag];
	std::vector<CGameObject*> TextureObjectLayer = pvObjectList[(int)ObjectLayer::Enemy];
	//std::vector<CGameObject*> TextureObjectLayer = pvObjectList[(int)ObjectLayer::TextureObject];

	// 버튼
	std::vector<CGameObject*> ButtonObjectLayer = pvObjectList[(int)ObjectLayer::ButtonObject];
	CButtonObject* clear = dynamic_cast<CButtonObject*>(ButtonObjectLayer[0]);
	CButtonObject* over = dynamic_cast<CButtonObject*>(ButtonObjectLayer[1]);
	CButtonObject* black = dynamic_cast<CButtonObject*>(ButtonObjectLayer[2]);
	CButtonObject* turnEnd = dynamic_cast<CButtonObject*>(ButtonObjectLayer[3]);

	// 버튼
	turnEnd->SetXY(FRAME_BUFFER_WIDTH * 2, FRAME_BUFFER_HEIGHT * 2);

	// 종료조건
	if (m_pPlayer)
		if (m_pPlayer->GetCurHp() <= 0)
			m_iOverFlag = 1;// game over
	if (m_pBoss)
		if (m_pBoss->GetCurHp() <= 0)
			m_iOverFlag = 2;// clear
	if (m_iOverFlag != 0)
	{
		black->SetXY(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2);
		switch (m_iOverFlag)
		{
		case 1:
			over->SetXY(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2);
			break;
		case 2:
			clear->SetXY(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2);
			break;
		}
	}
	else
	{
		//for (CGameObject* obj : ButtonObjectLayer)
		//{
		//	CButtonObject* bObj = dynamic_cast<CButtonObject*>(obj);
		//	if (bObj)
		//		bObj->SetXY(FRAME_BUFFER_WIDTH * 2, FRAME_BUFFER_HEIGHT * 2);
		//}
	}
	 // 종료 함수에 m_iOverFlag를 넣어 넘기거나 그 함수가 m_iOverFlag를 확인해 종료화면 출력.

	//몬스터의 state가 Battle_State이면 추가.
	for (CGameObject* obj : TextureObjectLayer)
	{
		CMonsterObject* monsterObj = dynamic_cast<CMonsterObject*>(obj);

		if (monsterObj) {
			if (monsterObj->CMonsterObject::GetState() == MonsterState::Battle_State)
				if (monsterObj->GetCurHp() > 0)
					Engage((CCharacterObject*)obj);
		}
		else;
	}


	if (m_currentPhase != TurnPhase::NON)
	{
		// 버튼
		turnEnd->SetXY(FRAME_BUFFER_WIDTH / 10 * 9, FRAME_BUFFER_HEIGHT / 2);

		// 체력이 적은 오브젝트 벡터에서 제거. 이렇게 하면 안되지 않나
		for (int i = 0; i < m_pvEngagedObjects.size();)
		{
			CCharacterObject* cobj = m_pvEngagedObjects[i];
			if (cobj->GetCurHp() <= 0)
			{
				// cobj의 위치에 절단 오브젝트 생성. 가능?
				cobj->SetCuttable(true);
				CRay r = r.RayAtWorldSpace(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 20 * 5, m_pPlayer->GetCamera());
				((CRayObject*)(m_pObjectManager->GetObjectList()[(int)ObjectLayer::Ray][0]))->Reset(r);

				m_pvDeadObjects.push_back(cobj);
				//m_bAddCutter = true;

				m_pvEngagedObjects.erase(m_pvEngagedObjects.begin() + i);
				if (m_iTurnFlag > i)
					m_iTurnFlag--;
			}
			else
			{
				i++; // Only increment if element is not removed
			}
		}
		// 제거 후 초기화? 초기화도 여기서 해야하나?
		//if (m_pvEngagedObjects.size() < 2)
		//	m_currentPhase = TurnPhase::NON;
		// 이걸 여기서 해야하는가?
	}

	if (m_currentPhase == TurnPhase::StartPhase)
	{
		turnObj->StartTurn();

		for (int i = 0; i < m_iMaxHandCount; i++)
			turnObj->GetDeckData()->Draw(dre);
		// 손패에 변경사항이 있을 경우 true로 바꿔줘야함.
		bCardUpdateFlag = true;

		m_currentPhase = TurnPhase::PlayPhase;
	}
	else if (m_currentPhase == TurnPhase::PlayPhase)
	{
		if (turnObj->GetTeamId() == 0) {
			//플레이어의 턴
		}
		else if (turnObj->GetTeamId() == 1) {
			CMonsterObject* MonsterObj = dynamic_cast<CMonsterObject*>(turnObj);
			if (MonsterObj)
			{
				int flag = MonsterObj->TurnPlay(dre);
				// AI의 턴.
				// 팀을 검사하는것과 다이나믹 캐스트하는 방법이 있을듯.
				// 이 작업을 계속 하는건 비효율적일지도
				// 카드를 적당히 사용한다.
				// 사용한다는 함수를 부르면 해당 객체가 카드를 사용함
				// 카드를 사용한다 라는 함수가 사용할 핸드의 번호를 리턴하며 특정 값을 리턴하면 턴 종료 함수를 호출.
				// 아무것도 안 하는 값도 있어야함.
				if (flag == -1);
				else if (flag == -2)
				{
					if(!(turnEnd->m_IsClicked))
						turnEnd->m_IsClicked = true;
					else
					{
						turnEnd->m_IsClicked = false;
						IncreaseTurnFlag();
					}
				}
				else {
					if(m_pSelectedUI == NULL)
						m_pSelectedUI = (CCardButton*)pvObjectList[(int)ObjectLayer::CardButtonObject][flag];
					//if (m_pSelectedUI != (CUIObject*)pvObjectList[(int)ObjectLayer::InteractiveUIObject][flag])
					else
						UseSelectedCard();
				}
			}
		}
		// 카드를 뽑는다는 효과를 지닌 카드가 있을경우 여기서 처리.

		// IncreaseTurnFlag() 함수를 통해 EndPhase로 넘어감.
		// 벡터에 객체가 하나만 남으면 EndBattle로 넘어감.
		if (m_pvEngagedObjects.size() < 2)
			m_currentPhase = TurnPhase::EndBattle;
	}
	else if (m_currentPhase == TurnPhase::EndPhase)
	{
		// 처리할것을 다 처리하면 StartPhase로 변경.
		m_currentPhase = TurnPhase::StartPhase;
	}
	else if (m_currentPhase == TurnPhase::EndBattle)
	{
		// 처리할것을 다 처리하면 NON으로 변경.
		m_iCurrentTurnCount = 0;
		m_iTurnFlag = 0;
		m_currentPhase = TurnPhase::NON;

		for (CCharacterObject* cobj : m_pvEngagedObjects)
		{
			cobj->AfterEngage();
		}
	}
	else if (m_currentPhase == TurnPhase::Engage)
	{
		m_pEngageObj->BeforeEngage();
		m_pEngageObj->GetDeckData()->ShuffleDeck(dre);
		m_currentPhase = m_lastPhase;
		m_pEngageObj = nullptr;
	}
	else if (m_currentPhase == TurnPhase::StartBattle)
	{
		m_iCurrentTurnCount = 1;
		dre.seed(m_CurrentTime);
		for (CCharacterObject* cobj : m_pvEngagedObjects)
		{
			cobj->BeforeEngage();
			cobj->GetDeckData()->ShuffleDeck(dre);
		}
		m_currentPhase = TurnPhase::StartPhase;
	}
}

void CTestScene::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager, float fTimeElapsed)
{
	std::vector<CGameObject*>* pObjectList = m_pObjectManager->GetObjectList();
	// 카드패 관련
	std::vector<CGameObject*>* pInteractiveUIObj = &pObjectList[(int)ObjectLayer::CardButtonObject];
	if (pInteractiveUIObj->size() > 0)
	{
		CCharacterObject* turnedObj = m_pvEngagedObjects[m_iTurnFlag];

		vector hand = turnedObj->GetDeckData()->GetHand();
		int handSize = turnedObj->GetDeckData()->GetCurrentHandSize();

		if (bCardUpdateFlag)
		{
			for (int i = 0; i < handSize; i++)
			{
				CCardButton* obj = (CCardButton*)pObjectList[(int)ObjectLayer::CardButtonObject][i];

				dynamic_cast<CCardButton*> (obj)->UpdateData(hand[i]);
				dynamic_cast<CCardButton*> (obj)->UpdateTexture(pd3dDevice, hand[i]);
				//dynamic_cast<CCardUIObject*> (obj)->UpdateTexture(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, hand[i]);
			}
			bCardUpdateFlag = false;
		}
	}

	// 절단
	XMFLOAT3 cutterPos{ 0,0,0 };
	bool flag = false;
	CRayObject* pRayObj = nullptr;
	XMFLOAT3 ray_dir{ 0,0,0 };
	XMFLOAT3 ray_origin{ 0,0,0 };
	XMFLOAT3 playerLook = m_pPlayer->GetLook();

	if (m_pvDeadObjects.size() > 0)
	{
		//cutterPos = m_pvDeadObjects[0]->GetPosition();
		//m_pvDeadObjects[0]->SetPosition(cutterPos);
		m_pvDeadObjects.erase(m_pvDeadObjects.begin());

		flag = true;
		m_bAddCutter = true;
	}

	if (m_bAddCutter) {
		m_bAddCutter = false;
		float fBoxSize = 100.0f;
		pRayObj = ((CRayObject*)(m_pObjectManager->GetObjectList()[(int)ObjectLayer::Ray][0]));
		ray_dir = pRayObj->GetRayDir();
		ray_origin = pRayObj->GetRayOrigin();

		std::random_device rd;
		std::default_random_engine dre(rd());
		std::uniform_real_distribution <float> urd(-1.0, 1.0);

		XMFLOAT3 randomVector(urd(dre), urd(dre), urd(dre));
		// 외적 계산 (수직인 벡터)
		XMFLOAT3 planeNormal = Vector3::CrossProduct(playerLook, randomVector);

		CGameObject* cutterObject = new CGameObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		CCutterBox_NonMesh* cutterMesh = nullptr;

		if (flag)
			cutterMesh = new CCutterBox_NonMesh(pd3dDevice, pd3dCommandList, fBoxSize * 2, fBoxSize * 2, fBoxSize * 2);	// 박스 안의 오브젝트를 절단한다.
		else
			cutterMesh = new CCutterBox_NonMesh(pd3dDevice, pd3dCommandList, fBoxSize, fBoxSize, fBoxSize);	// 박스 안의 오브젝트를 절단한다.

		//if (!cutterMesh) return;

		cutterMesh->SetCutPlaneNormal(planeNormal); // 절단면의 노멀
		cutterObject->SetMesh(0, cutterMesh, true);
		if (flag)
			cutterObject->SetPosition(Vector3::Add(ray_origin, Vector3::ScalarProduct(ray_dir, fBoxSize * 13.5)));
		else
			cutterObject->SetPosition(Vector3::Add(ray_origin, Vector3::ScalarProduct(ray_dir, fBoxSize * 3)));
		cutterObject->SetAllowCutting(true);	// 이게 켜져있어야 자른다?
		//cutterObject->SetShaderType(ShaderType::CObjectsShader);

		cutterObject->Rotate(0, m_pPlayer->GetYaw(), 0);

		m_pObjectManager->AddObj(cutterObject, ObjectLayer::CutterObject);
	}
	if (flag)
		flag = false;
	m_pObjectManager->DynamicShaping(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, fTimeElapsed, CDynamicShapeMesh::CutAlgorithm::Graph);
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
	WCHAR objText[] = L"Remaining Enemies : ";
	int textLen = _countof(objText) - 1;
	vector<CGameObject*>* objList = m_pObjectManager->GetObjectList();

	int objCount = 0;
	int textLength;
	std::vector<CGameObject*> EnemyLayer = objList[(int)ObjectLayer::Enemy];

	for (auto obj : EnemyLayer)
	{
		CMonsterObject* monsterObj = dynamic_cast<CMonsterObject*>(obj);

		if (monsterObj->GetCurHp() > 0)
			objCount++;
	}
	textLength = objCount;
	if (textLength == 0) textLen++;
	while (textLength > 0) {
		textLength /= 10; textLen++;
	}

	wsprintf(text, L"%s%d", objText, objCount);

	ComPtr<ID2D1SolidColorBrush> mSolidColorBrush;
	ComPtr<IDWriteTextFormat> mDWriteTextFormat;

	DX::ThrowIfFailed(pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0xefe300), mSolidColorBrush.GetAddressOf()));
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
	if(m_iOverFlag == 0)
		pd2dDeviceContext->DrawText(text, textLen, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());


	// 턴 표시
	DX::ThrowIfFailed(pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0xefe300), mSolidColorBrush.GetAddressOf()));
	DX::ThrowIfFailed(pdWriteFactory->CreateTextFormat(
		L"Verdana",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		40,
		L"en-us",
		mDWriteTextFormat.GetAddressOf()));

	mDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	mDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	CCharacterObject* turnObj = m_pvEngagedObjects[m_iTurnFlag];

	textRect = D2D1::RectF(0, 0, FRAME_BUFFER_WIDTH*19/20, FRAME_BUFFER_HEIGHT / 14);
	WCHAR objText1[] = L"Turn ";
	// 턴숫자.
	WCHAR objText2[] = L". Player's turn";
	WCHAR objText3[] = L". Enemy's turn";
	textLen = _countof(objText1) - 1;
	if (turnObj->GetTeamId() == 0)
	{
		wsprintf(text, L"%s%d%s", objText1, m_iCurrentTurnCount, objText2);
		textLen += _countof(objText2) - 1;
	}
	else
	{
		wsprintf(text, L"%s%d%s", objText1, m_iCurrentTurnCount, objText3);
		textLen += _countof(objText3) - 1;
	}
	textLength = m_iCurrentTurnCount;
	if (textLength == 0) textLen++;
	while (textLength > 0) {
		textLength /= 10; textLen++;
	}
	if (m_iOverFlag == 0 && m_currentPhase != TurnPhase::NON)
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

void CTestScene::Engage(CCharacterObject* obj)
{
	// 객체가 이미 벡터에 있는지 검사
	auto it = std::find(m_pvEngagedObjects.begin(), m_pvEngagedObjects.end(), obj);

	// 객체가 벡터에 없으면 추가
	if (it == m_pvEngagedObjects.end()) {
		m_pvEngagedObjects.push_back(obj);

		if (m_pvEngagedObjects.size() >= 2)
		{
			if (m_currentPhase == TurnPhase::NON)// 이 조건을 밖으로 빼는것도 ㄱㅊ을지도?
			{
				m_currentPhase = TurnPhase::StartBattle;
			}
			else //TurnPhase::PlayPhase;TurnPhase::StartPhase;TurnPhase::EndPhase;
			{
				m_lastPhase = m_currentPhase;
				m_currentPhase = TurnPhase::Engage;
				m_pEngageObj = obj;
			}
		}

		// 객체 전투 준비.
		//cout << obj->GetName() << " Engage" << endl;
	}
	else
		;
	//// 속도 순서로 정렬
	//std::sort(m_pvEngagedObjects.begin(), m_pvEngagedObjects.end(), [](CCharacterObject* a, CCharacterObject* b) {
	//	return a->GetSpeed() > b->GetSpeed(); // 내림차순 정렬
	//	});
}

void CTestScene::IncreaseTurnFlag()
{
	std::cout << "IncreaseTurnFlag" << std::endl;
	std::cout << "m_iTurnFlag : " << m_iTurnFlag << " to ";
	m_iTurnFlag++;
	if (m_iTurnFlag >= m_pvEngagedObjects.size())
		m_iTurnFlag = 0;
	std::cout << m_iTurnFlag << std::endl;
	m_iCurrentTurnCount++;
	// 턴 종료 버튼을 눌렀다는 신호 
	m_currentPhase = TurnPhase::EndPhase;
}

void CTestScene::UseSelectedCard()
{
	// 카드 사용
	CCardButton* pcUI = dynamic_cast<CCardButton*>(m_pSelectedUI);
	pcUI->CallFunc(m_pvEngagedObjects[m_iTurnFlag], m_pvEngagedObjects);
	m_pvEngagedObjects[m_iTurnFlag]->GetDeckData()->SendHandToUsedByNum(pcUI->GetUiNum());
	bCardUpdateFlag = true;
	SetSelectedUI(nullptr);
}

void CTestScene::SetSelectedUI(CCardButton* selected)
{
	if (selected)
	{
		if (m_pSelectedUI)
			SetSelectedUI(nullptr);
		m_pSelectedUI = selected;
		m_pSelectedUI->m_IsClicked = true;
	}
	else
	{
		m_pSelectedUI->m_IsClicked = false;
		m_pSelectedUI = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

CTestScene_Slice::CTestScene_Slice(CGameFramework* GameFramwork) :CTestScene(GameFramwork)
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
				if (true == m_pObjectManager->CollisionCheck_RayWithOBB(&r, pInterObj, tmin, tmax)) {
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

	m_pObjectManager->SetPhysXManager(pFBXDataManager->GetPhysXManager());

	// Collider Shader 등록
	CObjectsShader* pColliderShader = new CObjectsShader();
	pColliderShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pObjectManager->SetColliderShader(pColliderShader);

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

	//--------------------------------- 조명, 재질 생성 ----------------------------------------
	BuildLightsAndMaterials();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//

	{
		CFBXObject* pFBXObject = pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "Zombie1");
		((CDynamicShapeMesh*)(pFBXObject->GetMeshes()[0]))->SetCuttable(true);
		pFBXObject->SetCuttable(true);
		pFBXObject->SetPosition(50.0f, 0.0f, 100.0f);
		m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Enemy);
	}

	{
		CRayObject* pRayObject = NULL;
		pRayObject = new CRayObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
		pRayObject->SetMesh(0, new CRayMesh(pd3dDevice, pd3dCommandList, NULL));
		m_pObjectManager->AddObj(pRayObject, ObjectLayer::Ray);
	}
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
	//std::vector<CGameObject*> pvOL = pvObjectList[(int)ObjectLayer::Object];
	//if (pvOL.size() < 2) return;
	//XMFLOAT4X4 MatA, MatB;
	//for (int i = 0; i < pvOL.size() - 1; ++i) {
	//	if (CGameObject* ObjA = dynamic_cast<CGameObject*>(pvOL[i])) {
	//		for (int j = i + 1; j < pvOL.size(); ++j) {
	//			if (CGameObject* ObjB = dynamic_cast<CGameObject*>(pvOL[j])) {
	//				MatA = ObjA->GetWorldMat();
	//				MatB = ObjB->GetWorldMat();
	//				if (CollisionCheck(ObjA->GetCollider(), MatA, ObjB->GetCollider(), MatB)) {
	//					//cout << "Collision! ObjectNum = " << i << '\t' << j << '\n';
	//				}
	//			}
	//		}
	//	}
	//}
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

	if (m_bResetFlag) { // 초기화
		m_bResetFlag = false;
		m_pObjectManager->ClearLayer(ObjectLayer::Object);

		CFBXObject* pFBXObject = pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "Stone_lit_003");
		((CDynamicShapeMesh*)(pFBXObject->GetMeshes()[0]))->SetCuttable(true);
		pFBXObject->SetCuttable(true);
		pFBXObject->SetPosition(50.0f, 0.0f, 100.0f);
		m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);
	}

	m_pObjectManager->DynamicShaping(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, fTimeElapsed, CDynamicShapeMesh::CutAlgorithm::Graph);
	//m_pObjectManager->DynamicShaping(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, fTimeElapsed, CDynamicShapeMesh::CutAlgorithm::ConvexHull);
	//if (true == m_bCutAlgorithm)
	//	m_pObjectManager->DynamicShaping(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, fTimeElapsed, CDynamicShapeMesh::CutAlgorithm::Push);
	//else
	//	m_pObjectManager->DynamicShaping(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, fTimeElapsed, CDynamicShapeMesh::CutAlgorithm::ConvexHull);
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
	int objCount = objList[(int)ObjectLayer::TextureObject].size();
	if (objCount == 0) textLen++;
	while (objCount > 0) {
		objCount /= 10; textLen++;
	}

	wsprintf(text, L"%s%d", objText, (int)(objList[(int)ObjectLayer::TextureObject].size()));

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

CTestScene_PhysX::CTestScene_PhysX(CGameFramework* GameFramwork) :CTestScene(GameFramwork)
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

	CFbxLoader_V3 fLoader;

	// Collider Shader 등록
	CObjectsShader* pColliderShader = new CObjectsShader();
	pColliderShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pObjectManager->SetColliderShader(pColliderShader);

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
		CFBXObject* pFBXObject;
		//pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		//((CDynamicShapeMesh*)(pFBXObject->GetMeshes()[0]))->SetCuttable(true);
		//pFBXObject->SetCuttable(true);
		//pFBXObject->SetPosition(50.0f, 0.0f, 100.0f);
		//m_pObjectManager->AddObj(pFBXObject, ObjectLayer::Object);

		for (int i = 0; i < (10); ++i) {
			//pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
			pFBXObject = pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "Card");
			pFBXObject->SetPosition(50.0f, 50.0f * i, 100.0f);
			m_pObjectManager->AddObj(pFBXObject, ObjectLayer::ObjectNormal);
		}
	}

	{
		CRayObject* pRayObject = NULL;
		pRayObject = new CRayObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		pRayObject->SetMesh(0, new CRayMesh(pd3dDevice, pd3dCommandList, NULL));
		m_pObjectManager->AddObj(pRayObject, ObjectLayer::Ray);
	}

	BuildLightsAndMaterials();
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



CTestScene_Animation::CTestScene_Animation(CGameFramework* GameFramwork) : CTestScene(GameFramwork)
{
}

CTestScene_Animation::~CTestScene_Animation()
{
}

void CTestScene_Animation::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CFbxLoader_V3 fLoader;
	m_pObjectManager->SetPhysXManager(pFBXDataManager->GetPhysXManager());

	// Collider Shader 등록
	CObjectsShader* pColliderShader = new CObjectsShader();
	pColliderShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pObjectManager->SetColliderShader(pColliderShader);


	m_pTextShader = new CTextShader();
	m_pTextShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	{
		m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		m_pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
		m_pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pPlayer->SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pPlayer->SetMaxVelocityXZ(550.0f);
		m_pPlayer->SetMaxVelocityY(100.0f);
		m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);
	}

	//--------------------------------- 조명, 재질 생성 ----------------------------------------

	BuildLightsAndMaterials();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//

	CFBXObject* pFBXObject = new CFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pFBXObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "final_map"));
	pFBXObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pObjectManager->AddObj(pFBXObject, ObjectLayer::ObjectPhysX);

	// animaition Test Charactor
	if (true) {
		//	//fLoader.LoadFbx("fbxsdk/", "box");
		//	//fLoader.LoadAnim(pFbxData->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Test_Walking");
		//	//pFbxData = fLoader.LoadFbx("fbxsdk/", "Tree_temp_climate_003");
		//	//pFbxData = fLoader.LoadFbx("fbxsdk/", "peasant_1");

		CFbx_V3::CFbxData* pFbxData;
		CFBXObject* pNewChildObject;

		//pFbxData = fLoader.LoadFbx("fbxsdk/", "Player1");
		//fLoader.LoadAnim(pFbxData->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Player_Idle");
		//fLoader.LoadAnim(pFbxData->m_pRootObjectData->m_pSkeleton, "fbxsdk/", "Player_Running");

		CFBXObject* pNewObject = new CFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CAnimationObjectShader);
		//	//CFBXObject* pNewChildObject = pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, "fbxsdk/Test_Walking.fbx");
		//	//CFBXObject* pNewChildObject = pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "Tree_temp_climate_003");

		//	//pFbxData = fLoader.LoadFbxScene("fbxsdk/", "citymap");
		//	//pNewChildObject = pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pFbxData);
		//	//pNewObject->SetChild(pNewChildObject);

		//	//pFbxData = fLoader.LoadFbx("fbxsdk/", "Test_Walking");
		//	//pNewChildObject = pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pFbxData);
		//	//pNewObject->SetChild(pNewChildObject);
		//	
		//	//pNewChildObject = pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "peasant_1");
		//	//pNewObject->SetChild(pNewChildObject);

		//pNewChildObject = pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "Card");
		pNewObject = pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "Card");
		//pNewChildObject = pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pFbxData);
		//pNewObject->SetChild(pNewChildObject);

		//pNewObject->SetAnimData(pFBXDataManager->LoadAnimDataFromFBX("fbxsdk/Test_Walking.fbx"));
		float xPosition = 0;
		float zPosition = 500;
		float fHeight = 0;
		pNewObject->SetScale(0.3f, 0.3f, 0.3f);
		pNewObject->Rotate(0.0f, 45.0f, 0.0f);
		pNewObject->SetPosition(xPosition, fHeight, zPosition);

		m_pObjectManager->AddObj(pNewObject, ObjectLayer::ObjectPhysX);
	}

	{
		CRayObject* pRayObject = NULL;
		pRayObject = new CRayObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
		pRayObject->SetMesh(0, new CRayMesh(pd3dDevice, pd3dCommandList, NULL));
		m_pObjectManager->AddObj(pRayObject, ObjectLayer::Ray);
	}

}

void CTestScene_Animation::AnimateObjects(float fTotalTime, float fTimeElapsed)
{
	std::vector<CGameObject*> obData = m_pObjectManager->GetObjectList(ObjectLayer::ObjectNormal);
	std::vector<CGameObject*> rData = m_pObjectManager->GetObjectList(ObjectLayer::Ray);
	CRay r;
	r.SetDir(((CRayObject*)rData[0])->GetRayDir());
	r.SetOrigin(((CRayObject*)rData[0])->GetRayOrigin());
	//std::cout << r.GetOriginal().x << ", " << r.GetOriginal().y << ", " << r.GetOriginal().z << "\t\t";
	//std::cout << r.GetDir().x << ", " << r.GetDir().y << ", " << r.GetDir().z << "\t";
	for (auto& a : obData) {
		//r.SetDir(XMFLOAT3(-1.f, 0.f, 0.f));
		//r.SetOrigin(XMFLOAT3(100.f, 0.f, 0.f));
		float tmin, tmax;
		//if (m_pObjectManager->CollisionCheck_RayWithOBB(&r, a, tmin, tmax)) {
		//	std::cout << tmin << ", " << tmax << '\t';
		//	std::cout << "Collision!\n";
		//}
		//else {
		//	std::cout << "\n";
		//}
	}
	m_pObjectManager->AnimateObjects(fTotalTime, fTimeElapsed);
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

CLobbyScene::CLobbyScene(CGameFramework* GameFramwork) :CTestScene(GameFramwork)
{
}

bool CLobbyScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	POINT ptCursorPos{ 0,0 };
	GetCursorPos(&ptCursorPos);
	ScreenToClient(hWnd, &ptCursorPos);
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	int clientWidth = clientRect.right - clientRect.left;
	int clientHeight = clientRect.bottom - clientRect.top;

	// 프레임 버퍼 크기를 기준으로 마우스 좌표 변환
	float mouseX = static_cast<float>(ptCursorPos.x) / (clientWidth)*FRAME_BUFFER_WIDTH;
	float mouseY = static_cast<float>(ptCursorPos.y) / (clientHeight)*FRAME_BUFFER_HEIGHT;

	auto UIList = m_pObjectManager->GetObjectList(ObjectLayer::LobbyButtonObject1);
	for (auto& pObject : UIList) {
		CLobbyUI1Object* pUI = dynamic_cast<CLobbyUI1Object*>(pObject);
		int case_num = pUI->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		if (case_num == 2) {
			m_pGameFramework->SwitchScene(2);
			return true;
		}
	}

	auto UIList2 = m_pObjectManager->GetObjectList(ObjectLayer::LobbyButtonObject2);
	for (auto& pObject : UIList2) {
		CLobbyUI1Object* pUI = dynamic_cast<CLobbyUI2Object*>(pObject);
		int case_num = pUI->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		// case_num 1= 파티원2 초대 //  2= 파티원2 추방 // 3= 파티원3 초대 //4= 파티원3 추방 //5= 파티원4 초대 // 6= 파티원4 추방
		if (case_num == 1) {
		}
		else if (case_num == 2) {
		}
		else if (case_num == 3) {
		}
		else if (case_num == 4) {
		}
		else if (case_num == 5) {
		}
		else if (case_num == 6) {
		}
	}

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	{
	}
	break;
	case WM_LBUTTONUP:
	{
	}
	break;
	case WM_RBUTTONDOWN:
	{
	}
	break;
	case WM_RBUTTONUP:
	{
	}
	break;
	default:
		break;
	}
	return false;
}

bool CLobbyScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'Q': m_pPlayer->Move(DIR_UP, 5000.0f, true); break;
		case 'E': m_pPlayer->Move(DIR_DOWN, 5000.0f, true); break;
		case 'R': m_pPlayer->Rotate(0.0f, 20.0f, 0.0f);	break;
		case 'T': m_pPlayer->Rotate(0.0f, -20.0f, 0.0f); break;
		case 'P':
		{
			m_Drawing_Trigger = !m_Drawing_Trigger;
			auto UIList = m_pObjectManager->GetObjectList(ObjectLayer::LobbyButtonObject2);
			for (auto& pObject : UIList) {
				CLobbyUI2Object* pUI = dynamic_cast<CLobbyUI2Object*>(pObject);
				pUI->SetDrawingOn(m_Drawing_Trigger);
			}
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

bool CLobbyScene::ProcessInput(HWND hWnd, UCHAR* pKeysBuffer, POINT ptOldCursorPos)
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
		GetCursorPos(&ptCursorPos);
		xDelta = ptCursorPos.x - ptOldCursorPos.x;
		yDelta = ptCursorPos.y - ptOldCursorPos.y;
		cxDelta = (float)(xDelta) / 3.0f;
		cyDelta = (float)(yDelta) / 3.0f;
	}

	GetCursorPos(&ptCursorPos);
	ScreenToClient(hWnd, &ptCursorPos);
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
		}
		if (dwDirection) m_pPlayer->Move(dwDirection, 100.0f, true);
	}
	return(true);
}

void CLobbyScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	CFbxLoader_V3 fLoader;

	m_pTextShader = new CTextShader();
	m_pTextShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	// Collider Shader 등록
	CObjectsShader* pColliderShader = new CObjectsShader();
	pColliderShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pObjectManager->SetColliderShader(pColliderShader);

	m_pPlayer = new TestPlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CObjectsShader);
	m_pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
	m_pPlayer->SetPosition(XMFLOAT3(200.0f, 1000.0f, -8900.0f));
	m_pPlayer->SetMaxVelocityXZ(1000.0f);
	m_pObjectManager->AddObj(m_pPlayer, ObjectLayer::Player);

	BuildLightsAndMaterials();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CSkyBox* pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CSkyBoxShader, 6);
	m_pObjectManager->AddObj(pSkyBox, ObjectLayer::SkyBox);


	CFBXObject* pMapObject = new CFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, ShaderType::CTextureShader);
	pMapObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "GameObject"));
	pMapObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pObjectManager->AddObj(pMapObject, ObjectLayer::Map);


	CLobbyUI1Object* pUiObject1 = new CLobbyUI1Object(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pUiObject1->SetDrawingOn(false);
	m_pObjectManager->AddObj(pUiObject1, ObjectLayer::LobbyButtonObject1);

	CLobbyUI2Object* pUiObject2 = new CLobbyUI2Object(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pObjectManager->AddObj(pUiObject2, ObjectLayer::LobbyButtonObject2);


	//std::wstring name = L"플레이어1 이름";
	//m_textObjects.push_back(new CTextObject((L"파티모집 - " + name).c_str(), D2D1::RectF(0.0f, 310.0f, 1280.0f, 310.0f), L"Impact", 60.0f, D2D1::ColorF::DarkRed));

	m_textObjects.push_back(new CTextObject(L"Enemy Slasher", D2D1::RectF(470.0f, 250.0f, 1280.0f, 250.0f), L"Impact", 60.0f, D2D1::ColorF::DarkRed));
	m_textObjects.push_back(new CTextObject(L"파티모집", D2D1::RectF(580.0f, 310.0f, 1280.0f, 310.0f), L"Impact", 25.0f, D2D1::ColorF::DarkRed));
	m_textObjects.push_back(new CTextObject(L"* Player2 - ", D2D1::RectF(370.0f, 370.0f, 800.0f, 370.0f), L"Impact", 25.0f, D2D1::ColorF::White));
	m_textObjects.push_back(new CTextObject(L"* Player3 - ", D2D1::RectF(370.0f, 460.0f, 800.0f, 460.0f), L"Impact", 25.0f, D2D1::ColorF::White));
	m_textObjects.push_back(new CTextObject(L"* Player4 - ", D2D1::RectF(370.0f, 550.0f, 800.0f, 550.0f), L"Impact", 25.0f, D2D1::ColorF::White));


	//CMonsterObject* pMonsterObject;
	////---------------------------  좀비 1 -------------------------------------------
	//pMonsterObject = new CMonsterObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, (TestPlayer*)m_pPlayer, ShaderType::CTextureShader);
	//pMonsterObject->SetChild(pFBXDataManager->LoadFBXObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "fbxsdk/", "Zombie1"));
	//pMonsterObject->SetScale(1.0f, 1.0f, 1.0f);
	//pMonsterObject->SetPosition(0.0f, 0.0f, 0.0f);
	//pMonsterObject->SetTeamId(1);
	//m_pObjectManager->AddObj(pMonsterObject, ObjectLayer::TextureObject);

}

void CLobbyScene::AnimateObjects(float fTotalTime, float fTimeElapsed)
{
	float xpos = m_pPlayer->GetPosition().x;
	float zpos = m_pPlayer->GetPosition().z;

	// 문 앞에 위치하면 게임시작 창 렌더
	if (-2500.0f < xpos && xpos < 2500.0f && -17000.0f < zpos && zpos < -14000.0f) {
		auto UIList = m_pObjectManager->GetObjectList(ObjectLayer::LobbyButtonObject1);
		for (auto& pObject : UIList) {
			CLobbyUI1Object* pUI = dynamic_cast<CLobbyUI1Object*>(pObject);
			pUI->SetDrawingOn(true);
		}
	}

	// 문앞에서 멀어지면 게임시작 창 렌더 x
	else {
		auto UIList = m_pObjectManager->GetObjectList(ObjectLayer::LobbyButtonObject1);
		for (auto& pObject : UIList) {
			CLobbyUI1Object* pUI = dynamic_cast<CLobbyUI1Object*>(pObject);
			pUI->SetDrawingOn(false);
		}
	}


}

void CLobbyScene::Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera)
{
	// 파티 초대 UI 텍스트 렌더
	bool drawing = false;
	auto UIList = m_pObjectManager->GetObjectList(ObjectLayer::LobbyButtonObject2);
	for (auto& pObject : UIList) {
		CLobbyUI2Object* pUI = dynamic_cast<CLobbyUI2Object*>(pObject);
		//	파티 모집 UI가 존재하고 해당 UI가 렌더 가능한 상태면 렌더
		if (pUI) {
			drawing = pUI->GetDrawingOn();
		}
		else std::cout << "pUI 존재 x" << std::endl;
	}

	if (drawing)
	{
		if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

		pCamera->SetViewportsAndScissorRects(pd3dCommandList);
		pCamera->UpdateShaderVariables(pd3dCommandList);

		UpdateShaderVariables(pd3dCommandList);

		m_pTextShader->Render(pd3dCommandList, pCamera);

		for (auto textObject : m_textObjects)
		{
			textObject->Render(pd2dDeviceContext, pdWriteFactory);
		}
	}
}

void CLobbyScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	m_pObjectManager->Render(pd3dCommandList, pCamera);
}

