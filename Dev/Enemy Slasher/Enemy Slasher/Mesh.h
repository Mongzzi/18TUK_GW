#pragma once
#include "Vertex.h"
#include "Collider.h"
#include "FbxLoader.h"
#include "BoundingBox.h"
#include "Ray.h"

class CMesh
{
public:
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMesh();

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	virtual void ReleaseUploadBuffers();

protected:
	CVertex* m_pVertices = NULL; // ���� mesh�� Virtices

	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;

protected:
	UINT* m_pnIndices = NULL;
	XMFLOAT2** m_pxmfUV = NULL;
	XMFLOAT3** m_pxmfNormal = NULL;

	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;

	UINT m_nIndices = 0;	// �ε��� ���ۿ� ���ԵǴ� �ε����� ����
	UINT m_nStartIndex = 0; // �ε��� ���ۿ��� �޽��� �׸��� ���� ���Ǵ� ���� �ε���
	int m_nBaseVertex = 0;  // �ε��� ������ �ε����� ������ �ε���
	UINT m_nInElementUVCount = 0;
	UINT m_nInElementNormalCount = 0;

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool pRenderOption = false);
};

class CColliderMesh : public CMesh
{
public:
	CColliderMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~CColliderMesh();

protected:
	CAABBColliderWithMesh* m_pCollider = NULL;

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool pRenderAABB = false);

	virtual CAABBColliderWithMesh* GetCollider() { return m_pCollider; }

public:
	virtual bool CollisionCheck(CColliderMesh* pOtherObject) = 0;
};

class CMeshIlluminated : public CMesh
{
public:
	CMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMeshIlluminated();
public:
	void CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices, UINT* pnIndices, int nIndices);
};

class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminated();
};

class CDynamicShapeMesh : public CColliderMesh
{
public:
	CDynamicShapeMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CDynamicShapeMesh();

protected:
	bool m_bAllowCutting = false; // true ��� �ٸ� ������Ʈ�� �ڸ� �� �ִ�.
	bool m_bCuttable = false; // true ��� �ٸ� ������Ʈ�� ���� �߸� �� �ִ�.

protected:
	// ���� ��� - �ٸ� ������Ʈ�� �ڸ����� ������ ������ �־�� �Ѵ�.
	XMFLOAT3 m_xmf3PlaneNormal;
	XMFLOAT3 m_xmf3PlanePoint;

public:
	XMFLOAT3 GetCutPlaneNormal() { return m_xmf3PlaneNormal; }
	XMFLOAT3 GetCutPlanePoint() { return m_xmf3PlanePoint; }
	XMFLOAT3 ProjectVertexToPlane(const XMFLOAT3& vertex, const XMFLOAT3& planeNormal, const XMFLOAT3& planePoint);

protected:
	class DSM_Line {
	public:
		XMFLOAT3 m_xmfStart;
		XMFLOAT3 m_xmfEnd;
		XMFLOAT3 m_xmfDir;
		void MakeLine(XMFLOAT3 xmfStart, XMFLOAT3 xmfEnd);
	};
	class DSM_Triangle {
	public:
		XMFLOAT3 m_xmfPos[3];
		void MakeTriangle(XMFLOAT3 xmfPos1, XMFLOAT3 xmfPos2, XMFLOAT3 xmfPos3);
		bool LineTriangleIntersect(DSM_Line& dsmLine, float& t, XMFLOAT3& xmfIntersectionPoint);
	};

public:
	void SetAllowCutting(bool bState) { m_bAllowCutting = bState; }
	void SetCuttable(bool bState) { m_bCuttable = bState; }
	bool GetAllowCutting() { return m_bAllowCutting; }
	bool GetCuttable() { return m_bCuttable; }

public:
	CVertex* GetVertices() { return m_pVertices; }
	UINT* GetIndices() { return m_pnIndices; }
	UINT GetNumVertices() { return m_nVertices; }
	UINT GetNumIndices() { return m_nIndices; }

	XMFLOAT3 TransformVertex(XMFLOAT3& xmf3Vertex, XMFLOAT4X4& xmf4x4Mat);
	bool IsVertexAbovePlane(const XMFLOAT3& vertex, const XMFLOAT3& planeNormal, const XMFLOAT3& planePoint);
public:
	virtual bool CollisionCheck(CColliderMesh* pOtherMesh);
	virtual bool DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, CDynamicShapeMesh* pOtherMesh, XMFLOAT4X4& mxf4x4ThisMat, XMFLOAT4X4& xmf4x4OtherMat); // true - ���� ���� / false - ���� ����
};

class CBoxMesh : public CDynamicShapeMesh
{
public:
	CBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width = 20.0f, float height = 20.0f, float depth = 20.0f);
	CBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float r, float g, float b, float width = 20.0f, float height = 20.0f, float depth = 20.0f);
	virtual ~CBoxMesh();
};

class CCutterBoxMesh : public CDynamicShapeMesh
{
public:
	CCutterBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width = 20.0f, float height = 20.0f, float depth = 20.0f);
	virtual ~CCutterBoxMesh();
};

class CRayMesh : public CMesh
{
public:
	CRayMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CRay* ray);
	virtual ~CRayMesh();
};

// ------------------------------- FBX -----------------------------------

class CFBXMesh : public CDynamicShapeMesh
{
private:
	//UINT* m_pnIndices;

public:
	CFBXMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CFBXMesh();

	bool LoadMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pNode);

	CVertex* GetVertices() { return m_pVertices; };
	UINT* GetUnit() { return m_pnIndices; };

	CAABB* GetAABB(XMFLOAT4X4 m_xmf4x4World);
};

// ------------------------------- �ͷ��� �� -----------------------------------

class CHeightMapImage
{
private:
	//���� �� �̹��� �ȼ�(8-��Ʈ)���� ������ �迭�̴�. �� �ȼ��� 0~255�� ���� ���´�. 
	BYTE* m_pHeightMapPixels;

	//���� �� �̹����� ���ο� ���� ũ���̴�. 
	int m_nWidth;
	int m_nLength;

	//���� �� �̹����� ������ �� �� Ȯ���Ͽ� ����� ���ΰ��� ��Ÿ���� ������ �����̴�. 
	XMFLOAT3 m_xmf3Scale;

public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);

	//���� �� �̹������� (x, z) ��ġ�� �ȼ� ���� ����� ������ ���̸� ��ȯ�Ѵ�. 
	float GetHeight(float fx, float fz);


	//���� �� �̹������� (x, z) ��ġ�� ���� ���͸� ��ȯ�Ѵ�. 
	XMFLOAT3 GetHeightMapNormal(int x, int z);


	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	BYTE* GetHeightMapPixels() { return(m_pHeightMapPixels); }

	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }

};



class CHeightMapGridMesh : public CMesh
{
protected:
	//������ ũ��(����: x-����, ����: z-����)�̴�. 
	int m_nWidth;
	int m_nLength;

	/*������ ������(����: x-����, ����: z-����, ����: y-����) �����̴�. 
	���� ���� �޽��� �� ������ x-��ǥ, y-��ǥ, z-��ǥ�� ������ ������ x-��ǥ, y-��ǥ, z-��ǥ�� ���� ���� ���´�. 
	��, ���� ������ x-�� ������ ������ 1�� �ƴ϶� ������ ������ x-��ǥ�� �ȴ�. 
	�̷��� �ϸ� ���� ����(���� ����)�� ����ϴ��� ū ũ���� ����(����)�� �����Ҽ� �ִ�.*/
	XMFLOAT3 m_xmf3Scale;


public:
	CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		int xStart, int zStart, int nWidth, int nLength, 
		XMFLOAT3 xmf3Scale =XMFLOAT3(1.0f, 1.0f, 1.0f), 
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void* pContext = NULL);


	virtual ~CHeightMapGridMesh();


	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	//������ ��ǥ�� (x, z)�� �� ����(����)�� ���̸� ��ȯ�ϴ� �Լ��̴�. 
	virtual float OnGetHeight(int x, int z, void *pContext);

	//������ ��ǥ�� (x, z)�� �� ����(����)�� ������ ��ȯ�ϴ� �Լ��̴�. 
	virtual XMFLOAT4 OnGetColor(int x, int z, void *pContext);
};
