#pragma once
#include "FbxLoader.h"
#include "BoundingBox.h"

class CVertex
{
private:

public:
	XMFLOAT3 m_xmf3Vertex;
	XMFLOAT4 m_xmf4Color;
public:
	CVertex(XMFLOAT3 v, XMFLOAT4 c) : m_xmf3Vertex{ v }, m_xmf4Color{ c } {}

	CVertex() {
		m_xmf3Vertex = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf4Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CVertex(float x, float y, float z, XMFLOAT4 c) {
		m_xmf3Vertex = XMFLOAT3(x, y, z);
		m_xmf4Color = c;
	}
	~CVertex() { }

	XMFLOAT3 GetVertex() { return m_xmf3Vertex; };
};


/////////////////////////////////////////////////////////////////////////////////////////////


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
	CVertex* m_pVertices = NULL;

	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;

protected:
	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;

	UINT m_nIndices = 0;	// �ε��� ���ۿ� ���ԵǴ� �ε����� ����
	UINT m_nStartIndex = 0; // �ε��� ���ۿ��� �޽��� �׸��� ���� ���Ǵ� ���� �ε���
	int m_nBaseVertex = 0;  // �ε��� ������ �ε����� ������ �ε���

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void RenderAABB(ID3D12GraphicsCommandList* pd3dCommandList) {};
};


class Vertex_Color
{
private:
	XMFLOAT3 vertex;
	XMFLOAT4 color;
public:
	Vertex_Color(XMFLOAT3 v, XMFLOAT4 c) : vertex{ v }, color{ c } {}

	Vertex_Color() {
		vertex = XMFLOAT3(0.0f, 0.0f, 0.0f);
		color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	Vertex_Color(float x, float y, float z, XMFLOAT4 c) {
		vertex = XMFLOAT3(x, y, z);
		color = c;
	}
	~Vertex_Color() { }

	XMFLOAT3 GetVertex() { return vertex; };
};

class CIlluminatedVertex 
{
protected:
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT3 m_xmf3Position;
public:
	CIlluminatedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); 
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}CIlluminatedVertex(float x, float y, float z, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f,0.0f, 0.0f)) {
		m_xmf3Position = XMFLOAT3(x, y, z); 
		m_xmf3Normal = xmf3Normal;
	}
	CIlluminatedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f,0.0f)) {
		m_xmf3Position = xmf3Position; 
		m_xmf3Normal = xmf3Normal;
	}
	~CIlluminatedVertex() { }
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

class CAABBMesh : public CMesh
{
public:
	CAABBMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CAABBMesh();

public:
	virtual void ReleaseUploadBuffers();

protected:
	XMFLOAT3 m_xmf3MinPos;
	XMFLOAT3 m_xmf3MaxPos;

	CVertex* m_pAABBVertices = NULL;

	ID3D12Resource* m_pd3dAABBVertexBuffer = NULL;
	ID3D12Resource* m_pd3dAABBVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW m_d3dAABBVertexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY m_d3dAABBPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT m_nAABBSlot = 0;
	UINT m_nAABBVertices = 0;
	UINT m_nAABBStride = 0;
	UINT m_nAABBOffset = 0;

protected:
	ID3D12Resource* m_pd3dAABBIndexBuffer = NULL;
	ID3D12Resource* m_pd3dAABBIndexUploadBuffer = NULL;
	D3D12_INDEX_BUFFER_VIEW m_d3dAABBIndexBufferView;

	UINT* m_pnIndices = NULL;

	UINT m_nAABBIndices = 0;	// �ε��� ���ۿ� ���ԵǴ� �ε����� ����
	UINT m_nAABBStartIndex = 0; // �ε��� ���ۿ��� �޽��� �׸��� ���� ���Ǵ� ���� �ε���
	int m_nAABBBaseVertex = 0;  // �ε��� ������ �ε����� ������ �ε���

public:
	virtual void MakeAABB(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void RenderAABB(ID3D12GraphicsCommandList* pd3dCommandList);

	XMFLOAT3 GetAABBMinPos() { return m_xmf3MinPos; }
	XMFLOAT3 GetAABBMaxPos() { return m_xmf3MaxPos; }
};

class CBoxMesh : public CAABBMesh
{

public:
	CBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width = 20.0f, float height = 20.0f, float depth = 20.0f);
	virtual ~CBoxMesh();
};

// ------------------------------- FBX -----------------------------------

class CFBXMesh : public CAABBMesh
{
private:
	UINT* pnIndices;

public:
	CFBXMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CFBXMesh();

	void LoadMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pNode);

	CVertex* GetVertices() { return m_pVertices; };
	UINT* GetUnit() { return pnIndices; };

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
