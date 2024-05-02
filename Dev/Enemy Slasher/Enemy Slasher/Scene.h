#pragma once
#include "Player.h"
#include "Shader.h"

class CObjectManager;
class CPhysXManager;


struct CB_TIME_INFO
{
	float					m_fCurrentTime;
	float					m_fElapsedTime;
};

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
};

struct MATERIAL
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4				m_xmf4Emissive;
};

struct MATERIALS
{
	MATERIAL				m_pReflections[MAX_MATERIALS];
};

class CBasicScene
{
public:
	CBasicScene();
	virtual ~CBasicScene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader);
	virtual void ReleaseObjects();

	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	virtual bool ProcessInput(HWND hWnd, UCHAR* pKeysBuffer, POINT ptOldCursorPos);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	virtual void Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera= NULL);
	virtual void Enter();
	virtual void Exit();

	void ReleaseUploadBuffers();
	void UpdateTimer(float fTimeCurrent,float fTimeElapsed);

	CPlayer* m_pPlayer = NULL;
	
public:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	ID3D12RootSignature* m_pd3dComputeRootSignature = NULL;
	CObjectManager* m_pObjectManager = NULL;
	CTextShader* m_pTextShader = NULL;

protected:
	float m_CurrentTime;
	float m_ElapsedTime;
};

class CTitleScene : public CBasicScene
{
public:
	CTitleScene();
	~CTitleScene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader);

	virtual bool ProcessInput(HWND hWnd, UCHAR* pKeysBuffer, POINT ptOldCursorPos);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera = NULL);
	virtual void Enter();
	virtual void Exit();

	CTextShader* m_pTextShader = NULL;

private:
	CUIObject* pCoveredUI = NULL;
	CUIObject* pSelectedUI = NULL;

};

class CLoadingScene : public CBasicScene
{

};

class CTestScene : public CBasicScene
{
public:
	CTestScene();
	~CTestScene();


	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);


	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader);
	void BuildLightsAndMaterials();


	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();


	virtual bool ProcessInput(HWND hWnd, UCHAR* pKeysBuffer, POINT ptOldCursorPos);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, float fTimeElapsed);
	virtual void Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	virtual void Enter();
	virtual void Exit();

	CTextShader* m_pTextShader = NULL;


private:
	CUIObject* pCoveredUI = NULL;
	CUIObject* pSelectedUI = NULL;

	// 임시
	int SelectedUInum;

protected:

	LIGHTS* m_pLights = NULL;
	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

	MATERIALS* m_pMaterials = NULL;
	ID3D12Resource* m_pd3dcbMaterials = NULL;
	MATERIAL* m_pcbMappedMaterials = NULL;

	ID3D12Resource* m_pd3dcbTimeInfo = NULL;
	CB_TIME_INFO* m_pcbMappedTimeInfo = NULL;
};

class CTestScene_Card : public CBasicScene
{
public:
	CTestScene_Card();
	~CTestScene_Card();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader);

	void BuildLightsAndMaterials();
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	
	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);


public:
	//CPlayer* m_pPlayer = NULL; -> 조명을 붙이기 위해 새로 초기화 필요
	
	virtual void ReleaseObjects();
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Enter();
	virtual void Exit();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

};

class CTestScene_Slice : public CBasicScene
{
public:
	CTestScene_Slice();
	~CTestScene_Slice();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader);

	virtual bool ProcessInput(HWND hWnd, UCHAR* pKeysBuffer, POINT ptOldCursorPos);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, float fTimeElapsed);
	virtual void Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera = NULL);
	virtual void Enter();
	virtual void Exit();

	CTextShader* m_pTextShader = NULL;


protected:
	bool m_bAddCutter = false;
	bool m_bMoveObj = false;
	bool m_bCutterMode = false;
	bool m_bCutAlgorithm = true;

	bool m_bAddMouseCutter = false;
	bool m_bStartCutting = false;
	POINT m_pCutStartPos;
	POINT m_pCutEndPos;

	bool m_bResetFlag = false;

	POINT m_ptOldCursorPos;
	CInteractiveObject* m_pSelectedObj = NULL;

};


namespace physx {
	class PxActor;
}
class CTestScene_PhysX : public CBasicScene
{
public:
	CTestScene_PhysX();
	~CTestScene_PhysX();

	//virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	//virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader);

	//virtual bool ProcessInput(HWND hWnd, UCHAR* pKeysBuffer, POINT ptOldCursorPos);
	//virtual void AnimateObjects(float fTimeElapsed);
	//virtual void DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, float fTimeElapsed);
	virtual void Render2D(ID3D12GraphicsCommandList* pd3dCommandList, ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory, CCamera* pCamera = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	//virtual void Enter();
	//virtual void Exit();

	CTextShader* m_pTextShader = NULL;


protected:
	CPhysXManager* m_pPhysXManager = nullptr;
	vector<pair<CGameObject*, physx::PxActor*>> physxPairs;
};