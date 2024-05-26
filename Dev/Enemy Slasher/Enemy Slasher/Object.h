#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "DeckData.h"

class CShader;
class TestPlayer;

enum class ShaderType : int;

enum class ShaderType : int { // enum class는 int형으로 암시적 변환을 불허함으로 명시적 형변환을 해야 함
	NON,				// mesh 생성 코드부분에 쉐이더 연결을 안 하면 나오는 회색 바둑판
	CObjectsShader,
	CUIObjectsShader,
	CObjectsNormalShader,
	CTerrainShader,
	CTextShader,
	CTextureShader,
	CUITextureShader,
	CTexture_Position_Texcoord_Shader,
	CSkyBoxShader,
	CBillBoardInstanceShader,
	CAnimationObjectShader,
	CTitleShader,
	C2DObjectShader
};

enum class MonsterState : int {
	Default_State,
	Chase_State,
	Battle_State
};


//객체를 렌더링할 때 적용하는 상수 버퍼 데이터
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	UINT	   m_nMaterialID;
};

struct CB_2D_GAMEOBJECT_INFO
{
	XMFLOAT2 m_xmf2Position; // 오브젝트의 화면 좌표 (x, y)
	XMFLOAT2 m_xmfSize;		// 오브젝트의 가로 세로길이  // 절반아님
	bool m_IsClicked;		//	클릭 되었는지
};

struct CB_SKINNINGOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4BoneMat[96];
	bool m_bIsAvailable;
};

#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

class CTexture
{
public:
	CTexture(int nTextureResources, UINT nResourceType, int nSamplers, int nRootParameters);
	virtual ~CTexture();

private:
	int								m_nReferences = 0;

	UINT							m_nTextureType;

	int								m_nTextures = 0;
	ID3D12Resource** m_ppd3dTextures = NULL;
	ID3D12Resource** m_ppd3dTextureUploadBuffers;

	UINT* m_pnResourceTypes = NULL;

	_TCHAR(*m_ppstrTextureNames)[64] = NULL;

	DXGI_FORMAT* m_pdxgiBufferFormats = NULL;
	int* m_pnBufferElements = NULL;

	int								m_nRootParameters = 0;
	int* m_pnRootParameterIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSrvGpuDescriptorHandles = NULL;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void LoadTextureFromWICFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex);
	void LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex);


	void SetRootParameterIndex(int nIndex, UINT nRootParameterIndex);
	void SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);

	int GetRootParameters() { return(m_nRootParameters); }
	int GetTextures() { return(m_nTextures); }
	_TCHAR* GetTextureName(int nIndex) { return(m_ppstrTextureNames[nIndex]); }
	ID3D12Resource* GetResource(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(int nIndex) { return(m_pd3dSrvGpuDescriptorHandles[nIndex]); }
	int GetRootParameter(int nIndex) { return(m_pnRootParameterIndices[nIndex]); }

	UINT GetTextureType() { return(m_nTextureType); }
	UINT GetTextureType(int nIndex) { return(m_pnResourceTypes[nIndex]); }
	DXGI_FORMAT GetBufferFormat(int nIndex) { return(m_pdxgiBufferFormats[nIndex]); }
	int GetBufferElements(int nIndex) { return(m_pnBufferElements[nIndex]); }

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);

	void ReleaseUploadBuffers();
};



class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	//재질의 기본 색상
	XMFLOAT4 m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//재질의 번호
	UINT m_nReflection = 0;

	//재질을 적용하여 렌더링을 하기 위한 쉐이더

	CTexture* m_pTexture = NULL;
	CShader* m_pShader = NULL;


	void SetAlbedo(XMFLOAT4& xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetTexture(CTexture* pTexture);
	void SetShader(CShader* pShader);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();
	void ReleaseUploadBuffers();
};

class CGameObject
{
public:
	CGameObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype = ShaderType::NON, int nMeshes = 0);
	virtual ~CGameObject();

private:
	int m_nReferences = 0;

public:
	float m_fLifeTime = 0.0f;

public:
	void AddRef();
	void Release();

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

protected:
	XMFLOAT4X4						m_xmf4x4World;
	XMFLOAT4X4						m_xmf4x4Transform;
	XMFLOAT4X4						m_xmf4x4InitialRotate;

	ID3D12Resource* m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObject = NULL;

	int								m_nMeshes = 0;
	CMesh** m_ppMeshes = NULL;

	CMaterial* m_pMaterial = NULL;

	CGameObject* m_pParent = NULL;
	CGameObject* m_pChild = NULL;
	CGameObject* m_pSibling = NULL;


protected:
	COBBColliderWithMesh* m_pCollider = NULL;	// 충돌체

	bool							m_bAllowCutting = false;	// true 라면 다른 오브젝트를 자를 수 있다.
	bool							m_bCuttable = false;		// true 라면 다른 오브젝트에 인해 잘릴 수 있다.

public:
	void SetChild(CGameObject* pChild);
	CGameObject* GetChild() { return m_pChild; }
	CGameObject* GetSibling() { return m_pSibling; }

	CGameObject* GetParent() { return(m_pParent); }
	void SetWorldMat(XMFLOAT4X4 xmf4x4World) { m_xmf4x4World = xmf4x4World; }
	void SetTransMat(XMFLOAT4X4 xmf4x4Transform) { m_xmf4x4Transform = xmf4x4Transform; }
	XMFLOAT4X4 GetWorldMat() { return m_xmf4x4World; }
	XMFLOAT4X4 GetTransMat() { return m_xmf4x4Transform; }

	CMesh** GetMeshes() { return m_ppMeshes; }
	int GetNumMeshes() { return m_nMeshes; }

	COBBCollider* GetCollider() { if (m_pCollider) { return m_pCollider->GetCollider(); } return NULL; }

	// true 라면 다른 오브젝트를 자를 수 있다.
	void SetAllowCutting(bool bState) { m_bAllowCutting = bState; }
	// true 라면 다른 오브젝트에 인해 잘릴 수 있다.
	void SetCuttable(bool bState) { m_bCuttable = bState; }

	// true 라면 다른 오브젝트를 자를 수 있다.
	bool GetAllowCutting() { return m_bAllowCutting; }
	// true 라면 다른 오브젝트에 인해 잘릴 수 있다.
	bool GetCuttable() { return m_bCuttable; }

public:
	//상수 버퍼를 생성한다. 
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	//상수 버퍼의 내용을 갱신한다. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void ReleaseShaderVariables();

	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);

	// 쉐이더 생성 함수 ( create shader )
	// 기존 쉐이더 타입 재사용 - 생성자 부분 or Scene에서 CreatShader 호출할 예정
	//						   - SetShaderType함수 없애고 이부분에서만 사용할 예정

	void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype);
	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	void ChangeTexture(ID3D12Device* pd3dDevice,CTexture* a);


	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

public:
	//게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetLook(float x, float y, float z);
	void SetLook(XMFLOAT3 xmf3Position);

	void SetUp(float x, float y, float z);
	void SetUp(XMFLOAT3 xmf3Position);

	void SetRight(float x, float y, float z);
	void SetRight(XMFLOAT3 xmf3Position);

	//게임 객체의 위치를 설정한다. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	//게임 객체의 크기를 조정한다
	virtual void SetScale(float x, float y, float z);
	virtual void SetScale(XMFLOAT3 scale);

	//게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void MovePosition(float x, float y, float z);
	void MovePosition(XMFLOAT3 xmfPosition);

	//게임 객체를 회전(x-축, y-축, z-축)한다. 
	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	virtual void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	virtual void Rotate(XMFLOAT4* pxmf4Quaternion);
	void SetInitialRotate(float fPitch, float fYaw, float fRoll);

	//m_pShader
	void SetShader(CShader* pShader);


	void SetMaterial(CMaterial* pMaterial);
	void SetMaterial(UINT nReflection);
	CMaterial* GetMaterial() { return m_pMaterial; }

public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(int nIndex, CMesh* pMesh, bool bMakeColliderFlag = false);
	virtual void SetMesh(int nIndexSize);
	virtual void Animate(float fTimeTotal, float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption = false);
	virtual void RenderColliderMesh(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render2D() {};

public:
	void MakeCollider(); // 이 함수는 하위 메쉬의 모든 Collider를 포함하는 외부 Collider를 만들어 m_pCollider에 저장한다.

};

class CFBXObject : public CGameObject
{
public:
	CFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype = ShaderType::CAnimationObjectShader);
	virtual ~CFBXObject();

	void SetFbxData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFbx_V3::ObjectData* pObjectData);

	//상수 버퍼를 생성한다. 
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	//상수 버퍼의 내용을 갱신한다. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void ReleaseShaderVariables();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption = false);

private:
	CFbx_V3::Skeleton* m_pSkeletonData = NULL;

	ID3D12Resource* m_pd3dcbSkinningObject = NULL;
	CB_SKINNINGOBJECT_INFO* m_pcbMappedSkinningObject = NULL;

};

class CCharacterObject : public CFBXObject
{
public:
	CCharacterObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType shaderType);
	~CCharacterObject();
private:
	CDeckData* m_pDeck;
	float m_iTurnSpeed; // 속도 관련 기능을 서머너즈워처럼 바꿀것.
	// 속도 게이지가 있어서 m_iTurnSpeed만큼 오르는데 이 게이지가 꽉 차면 행동 가능.
	// 만약 꽉 찬 오브젝트가 2 이상이면게이지가 오르지 않거나 조금만 오른다.
	// 위 같은 경우 먼저 싸우기 시작한 순서대로 행동함.
	// 선빵 선빵친 객채부터 턴을 진행.

	// 체력
	float m_fCurHp;
	float m_fMaxHp;

	float m_fAtk;

	int m_iTeamId;

	int m_iKarma;
	// 이름?
	// 등등
public:
	CDeckData* GetDeckData() { return m_pDeck; };
	float GetSpeed() { return m_iTurnSpeed; };
	float GetAtk() { return m_fAtk; };
	float GetCurHp() { return m_fCurHp; };
	int GetTeamId() { return m_iTeamId; };
	void Reset(); // 전투가 끝나거나 시작될때 불릴 함수
	void StartTurn();// 턴 시작시 불릴 함수
	void SetTeamId(int teamId) { m_iTeamId = teamId; };

	void TakeDamage(float atk);
	void Heal(float ratio = 0.2);
};

class CRayObject : public CGameObject
{
public:
	CRayObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype = ShaderType::CObjectsShader);
	virtual ~CRayObject();

	void Reset(CRay ray);

private:
	XMFLOAT3 m_vOriginal;
	XMFLOAT3 m_xmf3DirOld;
	XMFLOAT3 m_xmf3Dir;

public:
	XMFLOAT3 GetRayOrigin() { return m_vOriginal; }
	XMFLOAT3 GetRayDir() { return m_xmf3Dir; }
};


//  UI
class CUIObject : public CFBXObject
{
public:
	CUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CCamera* pCamera, ShaderType stype = ShaderType::CObjectsShader);
	virtual ~CUIObject();
protected:
	static constexpr float TARGET_SCALE = 1.5f;
	static constexpr float DEFUALT_SCALE = 1.0f;
	static constexpr float TIME_ELAPSE_RATIO = 1.0f;

	float m_fCurrntScale;
	float m_fTargetScale;
	CCamera* m_pCamera = NULL;

	int m_iXPosition;
	int m_iYPosition;

	XMFLOAT3 m_xmfScale;

public:
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; };

	void ScreenSpaceToWorldSpace();

	void SetPositionUI(int x, int y);
	void SetPositionUI(POINT pos);
	void AddPositionUI(int x, int y);
	void AddPositionUI(POINT pos);

	POINT GetPositionUI() { return POINT(m_iXPosition, m_iYPosition); }

	// 계속 불리는 함수
	virtual void CursorOverObject(bool flag) = 0;
	// 한번 불리는 함수
	virtual void ButtenDown() = 0;
	virtual void ButtenUp() = 0;

	virtual void SetScale(float x, float y, float z);
	virtual void SetScale(XMFLOAT3 scale);
};

// 함수포인터 콜백함수
// 반환값을 타겟으로 해도 ㄱㅊ을듯
typedef void (*CardCallbackFunction)(CGameObject*, std::vector<CCharacterObject*>&);

// 카드 사용이 끝날때까지 다른 카드는 사용이 불가능해야함. 아마.
// 
// 같은 공격이라도 애니메이션이 다르면 다른 콜백을 부르게 될 듯?
// 
// 1. 콜백 함수가 처리
// 중복되는 기능이 많아지면 콜백의 배열을 갖게 하여 순서대로 부르
// 아니면 콜백함수를 갖는 연결리스트

// 2. 오브젝트가 각자 행동 큐로 처리
// 아니면 콜백 함수는 self 오브젝트의 행동 큐에 행동을 채우는 용도로만 사용하는것도 ㄱㅊ을듯.
void Callback_0(CGameObject* self, std::vector<CCharacterObject*>& target);
void Callback_1(CGameObject* self, std::vector<CCharacterObject*>& target);
void Callback_2(CGameObject* self, std::vector<CCharacterObject*>& target);
void Callback_3(CGameObject* self, std::vector<CCharacterObject*>& target);
void Callback_4(CGameObject* self, std::vector<CCharacterObject*>& target);

//

class CCardUIObject : public CUIObject
{
public:
	CCardUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		CCamera* pCamera, ShaderType stype = ShaderType::CObjectsShader);

	CCardUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		CCamera* pCamera, int UInum, ShaderType stype = ShaderType::CObjectsShader);

	virtual ~CCardUIObject();
private:
	int m_Card_Ui_Num;	// 카드 목록중 카드의 번호
	CardCallbackFunction m_callbackFunc = NULL;

public:
	virtual void Animate(float fTimeTotal, float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	//virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption = false);

	void CursorOverObject(bool flag) override;
	void ButtenDown() override;
	void ButtenUp() override;
	void SetFunc(CardCallbackFunction);
	void CallFunc(CGameObject* self, std::vector<CCharacterObject*>& target);

	int GetUiNum() { return m_Card_Ui_Num; };

	void UpdateData(int num);
};


//
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength,
		XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, ShaderType stype = ShaderType::CTerrainShader);
	virtual ~CHeightMapTerrain();

private:
	//지형의 높이 맵으로 사용할 이미지이다. 
	CHeightMapImage* m_pHeightMapImage;

	//높이 맵의 가로와 세로 크기이다. 
	int m_nWidth;
	int m_nLength;

	//지형을 실제로 몇 배 확대할 것인가를 나타내는 스케일 벡터이다. 
	XMFLOAT3 m_xmf3Scale;

public:
	//지형의 높이를 계산하는 함수이다(월드 좌표계). 높이 맵의 높이에 스케일의 y를 곱한 값이다. 
	float GetHeight(float x, float z) { return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y); }


	//지형의 법선 벡터를 계산하는 함수이다(월드 좌표계). 높이 맵의 법선 벡터를 사용한다. 
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z /
			m_xmf3Scale.z)));
	}

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	//지형의 크기(가로/세로)를 반환한다. 높이 맵의 크기에 스케일을 곱한 값이다. 
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};

class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype, int nMeshes = 6);
	virtual ~CSkyBox();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, bool pRenderOption = false);
};


class CTreeObject : public CFBXObject
{
public:
	CTreeObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype = ShaderType::CObjectsShader);
	virtual ~CTreeObject();
};


class CHpbarObject : public CGameObject
{
public:
	CHpbarObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype, int nMeshes = 1);
	virtual ~CHpbarObject();
};

class CTitleObject : public CGameObject
{
public:
	CTitleObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype, int nMeshes = 1);
	virtual ~CTitleObject();
};

class CAttackRangeObject : public CGameObject
{
public:
	CAttackRangeObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype, int nMeshes = 1);
	virtual ~CAttackRangeObject();
};


class CMonsterObject : public CCharacterObject
{
public:
	CMonsterObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CPlayer* ptestplayer, ShaderType stype = ShaderType::CObjectsShader);


	virtual ~CMonsterObject();
	virtual void Animate(float fTimeTotal, float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption = false);



	void SetState(MonsterState state) { m_Monster_State = state; };
	void SetSpeed(float speed) { m_speed = speed; };

	MonsterState GetState() { return m_Monster_State; };

private:
	CPlayer* m_pTestPlayer;
	MonsterState m_Monster_State;

	CHpbarObject* m_HpObject;
	CAttackRangeObject* m_AttackRangeObject;

	float m_MaxHp = 100.0f;
	float m_CurHp = 100.0f;

	XMFLOAT3 m_dir = { 0.0f,0.0f,1.0f };
	float m_speed = 100.0f;
};


struct VS_VB_BILLBOARD_INSTANCE
{
	XMFLOAT3						m_xmf3Position;
	XMFLOAT4						m_xmf4BillboardInfo;
};

class CBillBoardInstanceObject : public CGameObject
{
public:
	CBillBoardInstanceObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, ShaderType stype = ShaderType::NON, int nMeshes = 0);
	virtual ~CBillBoardInstanceObject();
	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption = false);

	// 버텍스 버퍼
	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;

	// 인스턴스 오브젝트 수
	int								m_nInstances = 0;

	// 인스턴스 버퍼
	ID3D12Resource* m_pd3dInstancesBuffer = NULL;
	ID3D12Resource* m_pd3dInstanceUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dInstancingBufferView;

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

};

class CTextObject
{
public:
	CTextObject(const WCHAR* text, const D2D1_RECT_F& rect, const WCHAR* fontName = L"Verdana", float fontSize = 25.0f, D2D1::ColorF::Enum color = D2D1::ColorF::White);
	~CTextObject();

	void SetText(const WCHAR* text);
	void SetPosition(const D2D1_RECT_F& rect);
	void SetFont(const WCHAR* fontName, float fontSize);
	void SetColor(D2D1::ColorF::Enum color);

	void Render(ID2D1DeviceContext3* pd2dDeviceContext, IDWriteFactory3* pdWriteFactory);

private:
	std::wstring m_text;
	D2D1_RECT_F m_rect;
	std::wstring m_fontName;
	float m_fontSize;
	D2D1::ColorF::Enum m_color;

	ComPtr<ID2D1SolidColorBrush> m_solidColorBrush;
	ComPtr<IDWriteTextFormat> m_dWriteTextFormat;
};


class CButtonObject : public CGameObject
{
public:
	CButtonObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, wchar_t* pszFileName, float x, float y, float width, float height, ShaderType stype, int nMeshes = 1);
	~CButtonObject();

private:
	float m_x, m_y;				// 오브젝트의 화면 좌표 (x, y)
	float m_width, m_height;	// 오브젝트의 가로길이 , 세로길이

	// m_type -  1번 ( 제목 로고 ) 2번 ( 게임시작 ) 3번 ( 게임종료 )
	int m_type;

	ID3D12Resource* m_pd3dcb2DGameObject = NULL;
	CB_2D_GAMEOBJECT_INFO* m_pcbMapped2DGameObject = NULL;

public:
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	void SetIsButton(bool IsButton) { m_IsButton = IsButton; };
	void SetType(int n) { m_type = n; };
	int GetType() { return m_type; };

public:
	bool m_IsButton;
	bool m_IsClicked;
	bool m_IsAbove;
	bool IsPointInside(float x, float y);

};