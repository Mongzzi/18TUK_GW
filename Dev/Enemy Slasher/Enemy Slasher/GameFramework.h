#pragma once

#include "Timer.h"
#include "Scene.h"
#include "Player.h"
#include "FbxLoader.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void CreateD2DDevice();
	void CreateD3D11On12Device();

	void ChangeSwapChainState();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	void DynamicShaping();
	void UpdateTimer();


	void Render2D();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd;

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;

	IDXGIFactory4*				m_pdxgiFactory = NULL;
	IDXGISwapChain3*			m_pdxgiSwapChain = NULL;
	ID3D12Device*				m_pd3dDevice = NULL;

	// d3d11on12 | connect  d3d11 -> d3d11on12 -> d2d -> dwrite
	ComPtr<ID3D11Device>		m_cd3d11Device;
	ID3D11DeviceContext*		m_pd3d11DeviceContext;
	ComPtr<ID3D11On12Device>	m_cd3d11On12Device;
	ID2D1Factory3*				m_pd2dFactory = NULL;
	ID2D1Device3*				m_pd2dDevice;
	ID2D1DeviceContext3*		m_pd2dDeviceContext;
	ComPtr<IDXGIDevice>			m_cdxgiDevice;
	IDWriteFactory3*			m_pdWriteFactory;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = 2;
	UINT						m_nSwapChainBufferIndex;

	ID3D12Resource*				m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap*		m_pd3dRtvDescriptorHeap = NULL;

	ID3D12Resource*				m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap*		m_pd3dDsvDescriptorHeap = NULL;

	// d3d11on12 | 2D Render Resource
	ComPtr<ID3D11Resource>		m_cpWrappedBackBuffers[m_nSwapChainBuffers];
	ComPtr<ID2D1Bitmap1>		m_cpd2dRenderTargets[m_nSwapChainBuffers];

	ID3D12CommandAllocator*		m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue*			m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList*	m_pd3dCommandList = NULL;

	ID3D12Fence*				m_pd3dFence = NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;
#if defined(_DEBUG)
	ID3D12Debug*				m_pd3dDebugController;
#endif

	CGameTimer					m_GameTimer;

	// 여러개의 씬을 관리하기 위한 임시 변수
	std::vector<CBasicScene*>	m_pvScenelist;
	int							m_nSceneCounter = 0;

	CBasicScene*				m_pScene = NULL;
	CPlayer*					m_pPlayer = NULL;
	CCamera*					m_pCamera = NULL;

	POINT						m_ptOldCursorPos;

	_TCHAR						m_pszFrameRate[75];

	CFBXLoader*					m_pFBXLoader = NULL;
};

