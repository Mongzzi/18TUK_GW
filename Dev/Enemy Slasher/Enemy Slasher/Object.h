#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "DeckData.h"

class CShader;
class TestPlayer;

enum class ShaderType : int;

enum class ShaderType : int { // enum class�� int������ �Ͻ��� ��ȯ�� ���������� ����� ����ȯ�� �ؾ� ��
	NON,				// mesh ���� �ڵ�κп� ���̴� ������ �� �ϸ� ������ ȸ�� �ٵ���
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


//��ü�� �������� �� �����ϴ� ��� ���� ������
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	UINT	   m_nMaterialID;
};

struct CB_2D_GAMEOBJECT_INFO
{
	XMFLOAT2 m_xmf2Position; // ������Ʈ�� ȭ�� ��ǥ (x, y)
	XMFLOAT2 m_xmfSize;		// ������Ʈ�� ���� ���α���  // ���ݾƴ�
	bool m_IsClicked;		//	Ŭ�� �Ǿ�����
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

	//������ �⺻ ����
	XMFLOAT4 m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//������ ��ȣ
	UINT m_nReflection = 0;

	//������ �����Ͽ� �������� �ϱ� ���� ���̴�

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
	COBBColliderWithMesh* m_pCollider = NULL;	// �浹ü

	bool							m_bAllowCutting = false;	// true ��� �ٸ� ������Ʈ�� �ڸ� �� �ִ�.
	bool							m_bCuttable = false;		// true ��� �ٸ� ������Ʈ�� ���� �߸� �� �ִ�.

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

	// true ��� �ٸ� ������Ʈ�� �ڸ� �� �ִ�.
	void SetAllowCutting(bool bState) { m_bAllowCutting = bState; }
	// true ��� �ٸ� ������Ʈ�� ���� �߸� �� �ִ�.
	void SetCuttable(bool bState) { m_bCuttable = bState; }

	// true ��� �ٸ� ������Ʈ�� �ڸ� �� �ִ�.
	bool GetAllowCutting() { return m_bAllowCutting; }
	// true ��� �ٸ� ������Ʈ�� ���� �߸� �� �ִ�.
	bool GetCuttable() { return m_bCuttable; }

public:
	//��� ���۸� �����Ѵ�. 
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	//��� ������ ������ �����Ѵ�. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void ReleaseShaderVariables();

	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);

	// ���̴� ���� �Լ� ( create shader )
	// ���� ���̴� Ÿ�� ���� - ������ �κ� or Scene���� CreatShader ȣ���� ����
	//						   - SetShaderType�Լ� ���ְ� �̺κп����� ����� ����

	void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype);
	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	void ChangeTexture(ID3D12Device* pd3dDevice,CTexture* a);


	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

public:
	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�. 
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

	//���� ��ü�� ��ġ�� �����Ѵ�. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	//���� ��ü�� ũ�⸦ �����Ѵ�
	virtual void SetScale(float x, float y, float z);
	virtual void SetScale(XMFLOAT3 scale);

	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void MovePosition(float x, float y, float z);
	void MovePosition(XMFLOAT3 xmfPosition);

	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�. 
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
	void MakeCollider(); // �� �Լ��� ���� �޽��� ��� Collider�� �����ϴ� �ܺ� Collider�� ����� m_pCollider�� �����Ѵ�.

};

class CFBXObject : public CGameObject
{
public:
	CFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype = ShaderType::CAnimationObjectShader);
	virtual ~CFBXObject();

	void SetFbxData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFbx_V3::ObjectData* pObjectData);

	//��� ���۸� �����Ѵ�. 
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	//��� ������ ������ �����Ѵ�. 
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
	float m_iTurnSpeed; // �ӵ� ���� ����� ���ӳ����ó�� �ٲܰ�.
	// �ӵ� �������� �־ m_iTurnSpeed��ŭ �����µ� �� �������� �� ���� �ൿ ����.
	// ���� �� �� ������Ʈ�� 2 �̻��̸�������� ������ �ʰų� ���ݸ� ������.
	// �� ���� ��� ���� �ο�� ������ ������� �ൿ��.
	// ���� ����ģ ��ä���� ���� ����.

	// ü��
	float m_fCurHp;
	float m_fMaxHp;

	float m_fAtk;

	int m_iTeamId;

	int m_iKarma;
	// �̸�?
	// ���
public:
	CDeckData* GetDeckData() { return m_pDeck; };
	float GetSpeed() { return m_iTurnSpeed; };
	float GetAtk() { return m_fAtk; };
	float GetCurHp() { return m_fCurHp; };
	int GetTeamId() { return m_iTeamId; };
	void Reset(); // ������ �����ų� ���۵ɶ� �Ҹ� �Լ�
	void StartTurn();// �� ���۽� �Ҹ� �Լ�
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

	// ��� �Ҹ��� �Լ�
	virtual void CursorOverObject(bool flag) = 0;
	// �ѹ� �Ҹ��� �Լ�
	virtual void ButtenDown() = 0;
	virtual void ButtenUp() = 0;

	virtual void SetScale(float x, float y, float z);
	virtual void SetScale(XMFLOAT3 scale);
};

// �Լ������� �ݹ��Լ�
// ��ȯ���� Ÿ������ �ص� ��������
typedef void (*CardCallbackFunction)(CGameObject*, std::vector<CCharacterObject*>&);

// ī�� ����� ���������� �ٸ� ī��� ����� �Ұ����ؾ���. �Ƹ�.
// 
// ���� �����̶� �ִϸ��̼��� �ٸ��� �ٸ� �ݹ��� �θ��� �� ��?
// 
// 1. �ݹ� �Լ��� ó��
// �ߺ��Ǵ� ����� �������� �ݹ��� �迭�� ���� �Ͽ� ������� �θ�
// �ƴϸ� �ݹ��Լ��� ���� ���Ḯ��Ʈ

// 2. ������Ʈ�� ���� �ൿ ť�� ó��
// �ƴϸ� �ݹ� �Լ��� self ������Ʈ�� �ൿ ť�� �ൿ�� ä��� �뵵�θ� ����ϴ°͵� ��������.
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
	int m_Card_Ui_Num;	// ī�� ����� ī���� ��ȣ
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
	//������ ���� ������ ����� �̹����̴�. 
	CHeightMapImage* m_pHeightMapImage;

	//���� ���� ���ο� ���� ũ���̴�. 
	int m_nWidth;
	int m_nLength;

	//������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ���� ������ �����̴�. 
	XMFLOAT3 m_xmf3Scale;

public:
	//������ ���̸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���̿� �������� y�� ���� ���̴�. 
	float GetHeight(float x, float z) { return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y); }


	//������ ���� ���͸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���� ���͸� ����Ѵ�. 
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z /
			m_xmf3Scale.z)));
	}

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	//������ ũ��(����/����)�� ��ȯ�Ѵ�. ���� ���� ũ�⿡ �������� ���� ���̴�. 
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

	// ���ؽ� ����
	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;

	// �ν��Ͻ� ������Ʈ ��
	int								m_nInstances = 0;

	// �ν��Ͻ� ����
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
	float m_x, m_y;				// ������Ʈ�� ȭ�� ��ǥ (x, y)
	float m_width, m_height;	// ������Ʈ�� ���α��� , ���α���

	// m_type -  1�� ( ���� �ΰ� ) 2�� ( ���ӽ��� ) 3�� ( �������� )
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