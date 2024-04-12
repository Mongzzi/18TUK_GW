#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "Skeleton.h"

class CShader;

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
	CSkyBoxShader
};

//��ü�� �������� �� �����ϴ� ��� ���� ������
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
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











struct MATERIAL
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4 m_xmf4Emissive;
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
	CGameObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype = ShaderType::NON, int nMeshes = 1);
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

	ID3D12Resource					*m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO				*m_pcbMappedGameObject = NULL;

	int								m_nMeshes = 0;
	CMesh** m_ppMeshes = NULL;

	int								m_nMaterial = 0;
	CMaterial* m_pMaterial = NULL;

	CGameObject* m_pParent = NULL;
	CGameObject* m_pChild = NULL;
	CGameObject* m_pSibling = NULL;

public:
	void SetChild(CGameObject* pChild);

	CGameObject* GetParent() { return(m_pParent); }
	void SetWorldMat(XMFLOAT4X4 xmf4x4World) { m_xmf4x4World = xmf4x4World; }
	void SetTransMat(XMFLOAT4X4 xmf4x4Transform) { m_xmf4x4Transform = xmf4x4Transform; }
	XMFLOAT4X4 GetWorldMat() { return m_xmf4x4World; }
	XMFLOAT4X4 GetTransMat() { return m_xmf4x4Transform; }

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
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

public:
	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

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

	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�. 
	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	virtual void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	virtual void Rotate(XMFLOAT4* pxmf4Quaternion);

	//m_pShader
	void SetShader(CShader* pShader);


	void SetMaterial(CMaterial* pMaterial);
	void SetMaterial(UINT nReflection);
	CMaterial* GetMaterial() { return m_pMaterial; }

public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(int nIndex, CMesh* pMesh);
	virtual void SetMesh(int nIndexSize);
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption = false);
	virtual void Render2D() {};
};


class CInteractiveObject : public CGameObject
{
public:
	CInteractiveObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype = ShaderType::CObjectsShader, int nMeshes = 1);
	virtual ~CInteractiveObject();

public:
	COBBColliderWithMesh* m_pCollider = NULL;

	CMesh** GetMeshes() { return m_ppMeshes; }
	int GetNumMeshes() { return m_nMeshes; }
	COBBCollider* GetCollider() { return m_pCollider->GetCollider(); }

	virtual void SetMesh(int nIndex, CMesh* pMesh);
public:
	void MakeCollider(); // �� �Լ��� ���� �޽��� ��� Collider�� �����ϴ� �ܺ� Collider�� ����� m_pCollider�� �����Ѵ�.
};

class CDynamicShapeObject : public CInteractiveObject
{
	// dynamic_cast �� ó���� �ϰ� ������ �̰��� ��Ÿ�ӽ� �ڽ�Ʈ�� ���� �۾��̴�. ������ ��
public:
	CDynamicShapeObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype= ShaderType::CObjectsShader, int nMeshes = 1);
	virtual ~CDynamicShapeObject();

protected:
	bool m_bAllowCutting = false;	// true ��� �ٸ� ������Ʈ�� �ڸ� �� �ִ�.
	bool m_bCuttable = false;		// true ��� �ٸ� ������Ʈ�� ���� �߸� �� �ִ�.

public:
	void SetAllowCutting(bool bState) { m_bAllowCutting = bState; }
	void SetCuttable(bool bState) { m_bCuttable = bState; }
	bool GetAllowCutting() { return m_bAllowCutting; }
	bool GetCuttable() { return m_bCuttable; }

public:

	vector<CGameObject*> DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		ShaderType stype, float fTimeElapsed, CGameObject* pCutterObject, CDynamicShapeMesh::CutAlgorithm cutAlgorithm = CDynamicShapeMesh::CutAlgorithm::Push); // ���ܵ� ������Ʈ 2���� �����Ѵ�.
};


class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype = ShaderType::CObjectsShader, int nMeshes = 1);
	virtual ~CRotatingObject();
protected:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

class CRotatingNormalObject : public CRotatingObject
{
public:
	CRotatingNormalObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype = ShaderType::CObjectsNormalShader, int nMeshes = 1);
	virtual ~CRotatingNormalObject();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	//��� ������ ������ �����Ѵ�. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

private:
	ID3D12Resource* m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObject = NULL;
};


class CRayObject : public CInteractiveObject
{
public:
	CRayObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype= ShaderType::CObjectsShader);
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


class CFBXObject : public CDynamicShapeObject
{
public:
	CFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFBXLoader* pFBXLoader, const char* fileName, ShaderType stype= ShaderType::CObjectsShader);
	virtual ~CFBXObject();
private:
	XMFLOAT3 m_xmf3RotationAxis = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float m_fRotationSpeed = 0.0f;

	CSkeleton* m_skelRoot = NULL;

	string m_sFileName;
public:
	void LoadContent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, const char* fileName);
	void LoadContent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pNode, int childId);

	bool LoadHierarchy(CFBXLoader* pFBXLoader, const char* fileName);
	bool LoadHierarchy(FbxNode* pNode);
	void LoadHierarchyFromMesh();

	//void LoadAnimation(CFBXLoader* pFBXLoader, const char* fileName);
	//void LoadAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, bool isSwitcher = false);
	//void LoadAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode, bool isSwitcher = false);

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);

	//bool IsCursorOverObject();
	//void ButtenDown();
	//void ButtenUp();
	CAABB* GetAABB();
	string GetFileName() { return m_sFileName; };

	// �ִϸ��̼� ����
private:
	float  m_fProgressedFrame;
	bool m_bCurrentLoofFlag;
	bool m_bOldLoofFlag;
	CAnimationData* m_adOldAnimationData = NULL;
	CAnimationData* m_adCurrentAnimationData = NULL;

public:
	void SetAnimation(CAnimationData* ani, bool loofFlag);
	void RestoreAnimation();
	CAnimationData* GetCurrentAnimationData() { return m_adCurrentAnimationData; };
};
//  UI
class CUIObject : public CFBXObject
{
public:
	CUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName, ShaderType stype= ShaderType::CObjectsShader);
	CUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName, int UInum, ShaderType stype = ShaderType::CObjectsShader);
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

	int m_iUInum;
public:
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; };

	void ScreenSpaceToWorldSpace();

	void SetPositionUI(int x, int y);
	void SetPositionUI(POINT pos);
	void AddPositionUI(int x, int y);
	void AddPositionUI(POINT pos);

	POINT GetPositionUI() { return POINT(m_iXPosition, m_iYPosition); }
	int GetUInum() { return m_iUInum; };

	// ��� �Ҹ��� �Լ�
	virtual void CursorOverObject(bool flag) = 0;
	// �ѹ� �Ҹ��� �Լ�
	virtual void ButtenDown() = 0;
	virtual void ButtenUp() = 0;

	virtual void SetScale(float x, float y, float z);
	virtual void SetScale(XMFLOAT3 scale);
};

class CCardUIObject : public CUIObject
{
public:
	CCardUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, 
		 CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName, ShaderType stype= ShaderType::CObjectsShader);

	CCardUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, 
		 CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName,  int UInum, ShaderType stype= ShaderType::CObjectsShader);

	virtual ~CCardUIObject();
private:
	int m_Card_Ui_Num;


public:
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	//virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption = false);

	void CursorOverObject(bool flag) override;
	void ButtenDown() override;
	void ButtenUp() override;
};


//
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength,
		XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
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
	CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,ShaderType stype, int nMeshes = 6);
	virtual ~CSkyBox();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, bool pRenderOption = false);
};


class CTreeObject: public CFBXObject
{
public:
	CTreeObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFBXLoader* pFBXLoader, const char* fileName, ShaderType stype = ShaderType::CObjectsShader);
	virtual ~CTreeObject();


};


///////////////////////////////////////////////////////////

class CTextObject
{
	POINT m_position;

public:
	void Render(ID2D1DeviceContext3* pd2dDeviceContext);
};