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
	CVertex* m_pVertices = NULL; // 본래 mesh의 Virtices

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

	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;

	UINT m_nIndices = 0;	// 인덱스 버퍼에 포함되는 인덱스의 개수
	UINT m_nStartIndex = 0; // 인덱스 버퍼에서 메쉬를 그리기 위해 사용되는 시작 인덱스
	int m_nBaseVertex = 0;  // 인덱스 버퍼의 인덱스에 더해질 인덱스

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
	bool m_bAllowCutting = false; // true 라면 다른 오브젝트를 자를 수 있다.
	bool m_bCuttable = false; // true 라면 다른 오브젝트에 인해 잘릴 수 있다.

protected:
	// 절단 평면 - 다른 오브젝트를 자르려면 무조건 가지고 있어야 한다.
	XMFLOAT3 m_xmf3PlaneNormal;
	XMFLOAT3 m_xmf3PlanePoint;

public:
	XMFLOAT3 GetCutPlaneNormal() { return m_xmf3PlaneNormal; }
	XMFLOAT3 GetCutPlanePoint() { return m_xmf3PlanePoint; }

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
	virtual bool DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, CDynamicShapeMesh* pOtherMesh, XMFLOAT4X4& mxf4x4ThisMat, XMFLOAT4X4& xmf4x4OtherMat); // true - 절단 성공 / false - 절단 실패
};

class CBoxMesh : public CDynamicShapeMesh
{
public:
	CBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width = 20.0f, float height = 20.0f, float depth = 20.0f);
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

	void LoadMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pNode);

	CVertex* GetVertices() { return m_pVertices; };
	UINT* GetUnit() { return m_pnIndices; };

	CAABB* GetAABB(XMFLOAT4X4 m_xmf4x4World);
};

// ------------------------------- 터레인 맵 -----------------------------------

class CHeightMapImage
{
private:
	//높이 맵 이미지 픽셀(8-비트)들의 이차원 배열이다. 각 픽셀은 0~255의 값을 갖는다. 
	BYTE* m_pHeightMapPixels;

	//높이 맵 이미지의 가로와 세로 크기이다. 
	int m_nWidth;
	int m_nLength;

	//높이 맵 이미지를 실제로 몇 배 확대하여 사용할 것인가를 나타내는 스케일 벡터이다. 
	XMFLOAT3 m_xmf3Scale;

public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);

	//높이 맵 이미지에서 (x, z) 위치의 픽셀 값에 기반한 지형의 높이를 반환한다. 
	float GetHeight(float fx, float fz);


	//높이 맵 이미지에서 (x, z) 위치의 법선 벡터를 반환한다. 
	XMFLOAT3 GetHeightMapNormal(int x, int z);


	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	BYTE* GetHeightMapPixels() { return(m_pHeightMapPixels); }

	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }

};



class CHeightMapGridMesh : public CMesh
{
protected:
	//격자의 크기(가로: x-방향, 세로: z-방향)이다. 
	int m_nWidth;
	int m_nLength;

	/*격자의 스케일(가로: x-방향, 세로: z-방향, 높이: y-방향) 벡터이다. 
	실제 격자 메쉬의 각 정점의 x-좌표, y-좌표, z-좌표는 스케일 벡터의 x-좌표, y-좌표, z-좌표로 곱한 값을 갖는다. 
	즉, 실제 격자의 x-축 방향의 간격은 1이 아니라 스케일 벡터의 x-좌표가 된다. 
	이렇게 하면 작은 격자(적은 정점)를 사용하더라도 큰 크기의 격자(지형)를 생성할수 있다.*/
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

	//격자의 좌표가 (x, z)일 때 교점(정점)의 높이를 반환하는 함수이다. 
	virtual float OnGetHeight(int x, int z, void *pContext);

	//격자의 좌표가 (x, z)일 때 교점(정점)의 색상을 반환하는 함수이다. 
	virtual XMFLOAT4 OnGetColor(int x, int z, void *pContext);
};
