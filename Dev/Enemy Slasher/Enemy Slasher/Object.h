#pragma once
#include "Camera.h"
#include "Mesh.h"

class CShader;

enum class ShaderType : int;


//객체를 렌더링할 때 적용하는 상수 버퍼 데이터
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	//객체에 적용될 재질 번호
	UINT m_nMaterial;
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
	ShaderType m_ShaderType;

	void SetAlbedo(XMFLOAT4& xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetShaderType(ShaderType type) { m_ShaderType = type; }

};

class CGameObject
{
public:
	CGameObject(int nMeshes = 1);
	virtual ~CGameObject();

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected:
	XMFLOAT4X4						m_xmf4x4World;
	XMFLOAT4X4						m_xmf4x4Transform;

	CMesh** m_ppMeshes = NULL;
	int m_nMeshes = 0;

	CMaterial* m_pMaterial = NULL;

public:
	//상수 버퍼를 생성한다. 
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	//상수 버퍼의 내용을 갱신한다. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);


public:
	//게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	//게임 객체의 위치를 설정한다. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	//게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//게임 객체를 회전(x-축, y-축, z-축)한다. 
	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	//m_pShader
	void SetShaderType(ShaderType shaderType);

	void SetMaterial(CMaterial* pMaterial);
	void SetMaterial(UINT nReflection);
	CMaterial* GetMaterial() { return m_pMaterial; }

public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(int nIndex, CMesh* pMesh);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool pRenderOption = false);
	virtual void Render2D() {};

public:
	virtual void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
};


class CInteractiveObject : public CGameObject
{
protected:
	XMFLOAT4X4 m_xmf4x4Rotate;

public:
	CInteractiveObject(int nMeshes = 1);
	virtual ~CInteractiveObject();

public:
	//게임 객체를 회전(x-축, y-축, z-축)한다. 
	virtual void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	virtual void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	XMFLOAT3 GetAABBMaxPos(int nIndex);
	XMFLOAT3 GetAABBMinPos(int nIndex);

	int GetNumMeshes() { return m_nMeshes; }

	virtual bool CollisionCheck(CGameObject* pOtherObject);
};

class CDynamicShapeObject : public CInteractiveObject
{
public:
	CDynamicShapeObject(int nMeshes = 1);
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

	virtual bool CollisionCheck(CGameObject* pOtherObject);
};


class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes = 1);
	virtual ~CRotatingObject();
protected:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
	virtual void Animate(float fTimeElapsed);
};

class CRotatingNormalObject : public CRotatingObject
{
public:
	CRotatingNormalObject(int nmeshes = 1);
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
	CRayObject();
	virtual ~CRayObject();

	void Reset(CRay ray);
private:
	XMFLOAT3 m_vOriginal;
	XMFLOAT3 m_xmf3DirOld;
};


class CFBXObject : public CInteractiveObject
{
public:
	CFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, const char* fileName = "fbxsdk/Box001.fbx");
	virtual ~CFBXObject();
private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:	
	void LoadContent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, const char* fileName);
	void LoadContent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pNode);
	virtual void Animate(float fTimeElapsed);

	//bool IsCursorOverObject();
	//void ButtenDown();
	//void ButtenUp();
	CAABB* GetAABB();
};
//  UI
class CUIObject : public CFBXObject
{
public:
	CUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName = "fbxsdk/Box001.fbx");
	virtual ~CUIObject();
protected:
	float m_fCurrntScale;	
	float m_fTargetScale;
	CCamera* m_pCamera = NULL;

	int m_iXPosition;
	int m_iYPosition;

public:
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; };

	void ScreenSpaceToWorldSpace();

	void SetPositionUI(int x, int y);
	void SetPositionUI(POINT pos);
	void AddPositionUI(int x, int y);
	void AddPositionUI(POINT pos);

	// 계속 불리는 함수
	virtual void CursorOverObject(bool flag) = 0;
	// 한번 불리는 함수
	virtual void ButtenDown() = 0;
	virtual void ButtenUp() = 0;
};

class CCardUIObject : public CUIObject
{
public:
	CCardUIObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, CCamera* pCamera, const char* fileName = "fbxsdk/Box001.fbx");
	virtual ~CCardUIObject();
protected:


public:
	virtual void Animate(float fTimeElapsed);

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



///////////////////////////////////////////////////////////

class CTextObject
{
	POINT m_position;

public:
	void Render(ID2D1DeviceContext3* pd2dDeviceContext);
};