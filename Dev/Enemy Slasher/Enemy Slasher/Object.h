#pragma once
#include "Camera.h"
#include "Mesh.h"

class CShader;

enum class ShaderType : int;


//��ü�� �������� �� �����ϴ� ��� ���� ������
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	//��ü�� ����� ���� ��ȣ
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
	
	//������ �⺻ ����
	XMFLOAT4 m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	
	//������ ��ȣ
	UINT m_nReflection = 0;
	
	//������ �����Ͽ� �������� �ϱ� ���� ���̴�
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
	//��� ���۸� �����Ѵ�. 
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	//��� ������ ������ �����Ѵ�. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);


public:
	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	//���� ��ü�� ��ġ�� �����Ѵ�. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�. 
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

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
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

public:
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
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

	//��� ������ ������ �����Ѵ�. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

private:
	ID3D12Resource* m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObject = NULL;
};



class CFBXObject : public CGameObject
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
};

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



///////////////////////////////////////////////////////////

class CTextObject
{
	POINT m_position;

public:
	void Render(ID2D1DeviceContext3* pd2dDeviceContext);
};