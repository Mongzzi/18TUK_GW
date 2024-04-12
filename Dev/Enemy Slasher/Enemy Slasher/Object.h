#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "Skeleton.h"

class CShader;

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
	CSkyBoxShader
};

//객체를 렌더링할 때 적용하는 상수 버퍼 데이터
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
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

public:
	//게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

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

	//게임 객체를 회전(x-축, y-축, z-축)한다. 
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
	void MakeCollider(); // 이 함수는 하위 메쉬의 모든 Collider를 포함하는 외부 Collider를 만들어 m_pCollider에 저장한다.
};

class CDynamicShapeObject : public CInteractiveObject
{
	// dynamic_cast 로 처리를 하고 있지만 이것은 런타임시 코스트가 높은 작업이다. 주의할 것
public:
	CDynamicShapeObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ShaderType stype= ShaderType::CObjectsShader, int nMeshes = 1);
	virtual ~CDynamicShapeObject();

protected:
	bool m_bAllowCutting = false;	// true 라면 다른 오브젝트를 자를 수 있다.
	bool m_bCuttable = false;		// true 라면 다른 오브젝트에 인해 잘릴 수 있다.

public:
	void SetAllowCutting(bool bState) { m_bAllowCutting = bState; }
	void SetCuttable(bool bState) { m_bCuttable = bState; }
	bool GetAllowCutting() { return m_bAllowCutting; }
	bool GetCuttable() { return m_bCuttable; }

public:

	vector<CGameObject*> DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		ShaderType stype, float fTimeElapsed, CGameObject* pCutterObject, CDynamicShapeMesh::CutAlgorithm cutAlgorithm = CDynamicShapeMesh::CutAlgorithm::Push); // 절단된 오브젝트 2개를 리턴한다.
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

	//상수 버퍼의 내용을 갱신한다. 
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

	// 애니메이션 관련
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

	// 계속 불리는 함수
	virtual void CursorOverObject(bool flag) = 0;
	// 한번 불리는 함수
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