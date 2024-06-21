#pragma once
#include "Vertex.h"
#include "ColliderWithMesh.h"
#include "BoundingBox.h"
#include "Ray.h"
#include "FbxLoader_V3.h"
#include <unordered_map>
#include <unordered_set>
#include <cmath>

// ��ǥ�� �ݿø��ϴ� �Լ�
float RoundToPrecision(float value, int precision);
// ��ǥ�� ���� �� �ݿø��Ͽ� ��
bool AreVerticesEqual(const XMFLOAT3& v1, const XMFLOAT3& v2, int precision = 6);

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

protected:
	COBBColliderWithMesh* m_pCollider = nullptr;

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool pRenderOption = false);
	virtual void RenderCollider(ID3D12GraphicsCommandList* pd3dCommandList);

	void SetMeshData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nStride, CVertex* pVertices, UINT nVertices, UINT* pnIndices, UINT nIndices);

public:
	CVertex* GetVertices() { return m_pVertices; }
	UINT* GetIndices() { return m_pnIndices; }
	UINT GetNumVertices() { return m_nVertices; }
	UINT GetNumIndices() { return m_nIndices; }

	UINT GetStride() { return m_nStride; }

	virtual COBBCollider* GetCollider() { if (m_pCollider) { return m_pCollider->GetCollider(); } return NULL; };

public:
	virtual void CreateCollider(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
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

class CDynamicShapeMesh : public CMesh
{
public:
	CDynamicShapeMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CDynamicShapeMesh();

protected:
	bool m_bCuttable = false; // true ��� �ٸ� ������Ʈ�� ���� �߸� �� �ִ�.

public:
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
	XMFLOAT3 CrossProduct(const XMFLOAT3& O, const XMFLOAT3& A, const XMFLOAT3& B);
	vector<pair<CVertex, UINT>> ConvexHull(vector<pair<CVertex, UINT>>& vVertex);

public:
	void SetCuttable(bool bState) { m_bCuttable = bState; }
	bool GetCuttable() { return m_bCuttable; }

public:
	XMFLOAT3 TransformVertex(XMFLOAT3& xmf3Vertex, XMFLOAT4X4& xmf4x4Mat);
	bool IsVertexAbovePlane(const XMFLOAT3& vertex, const XMFLOAT3& planeNormal, const XMFLOAT3& planePoint);

public:
	enum class CutAlgorithm : int {
		Push,
		ConvexHull,
		Graph
	};

	vector<CMesh*> DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, XMFLOAT4X4& xmf4x4ThisMat, CDynamicShapeMesh* pCutterMesh, XMFLOAT4X4& xmf4x4CutterMat, CutAlgorithm DynamicShapeAlgorithm = CutAlgorithm::Push); // ���ܵ� CMesh 2�� �迭�� �����Ѵ�.
	vector<CMesh*> DynamicShaping_Push(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, XMFLOAT4X4& xmf4x4ThisMat, CDynamicShapeMesh* pCutterMesh, XMFLOAT4X4& xmf4x4CutterMat); // ���ܵ� CMesh 2�� �迭�� �����Ѵ�.
	vector<CMesh*> DynamicShaping_ConvexHull(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, XMFLOAT4X4& xmf4x4ThisMat, CDynamicShapeMesh* pCutterMesh, XMFLOAT4X4& xmf4x4CutterMat); // ���ܵ� CMesh 2�� �迭�� �����Ѵ�.
	vector<CMesh*> DynamicShaping_Graph(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, XMFLOAT4X4& xmf4x4ThisMat, CDynamicShapeMesh* pCutterMesh, XMFLOAT4X4& xmf4x4CutterMat); // ���ܵ� CMesh 2�� �迭�� �����Ѵ�.
	vector<CMesh*> DynamicShaping_Graph_Meshs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, XMFLOAT4X4& xmf4x4ThisMat, CDynamicShapeMesh* pCutterMesh, XMFLOAT4X4& xmf4x4CutterMat); // ���ܵ� CMesh 2�� �̻��� �迭�� �����Ѵ�.

	// DynamicShaping_Graph�� ���� ��� �Լ� ����
private:
	// XMFLOAT3 ���� == operator�� �����Ƿ� ����� ���� class �ۼ� (unorderd_map ����� ����)
	class CEdge {
	public:
		CEdge() {
			x = 0.f;
			y = 0.f;
			z = 0.f;
		}
		CEdge(XMFLOAT3 val) {
			x = val.x;
			y = val.y;
			z = val.z;
		}
		CEdge(float x, float y, float z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}
	public:
		int x, y, z;

		bool IsNearlyEqual(float a, float b, float epsilon = EPSILON) const {
			return std::fabs(a - b) <= epsilon;
		}

		bool operator==(const CEdge& rhs) const {
			return IsNearlyEqual(x, rhs.x) && IsNearlyEqual(y, rhs.y) && IsNearlyEqual(z, rhs.z);
		}
	};

	// unorderd map ����� ���� hash ����
	struct hash_fn {
		//std::size_t operator()(const CEdge& v) const {
		//	std::size_t h1 = std::hash<float>{}(RoundToPrecision(v.x, 0));
		//	std::size_t h2 = std::hash<float>{}(RoundToPrecision(v.y, 0));
		//	std::size_t h3 = std::hash<float>{}(RoundToPrecision(v.z, 0));
		//	return h1 ^ h2 ^ h3;
		//}
		std::size_t operator()(const CEdge& v) const {
			std::size_t h1 = std::hash<int>{}(v.x);
			std::size_t h2 = std::hash<int>{}(v.y);
			std::size_t h3 = std::hash<int>{}(v.z);
			return h1 ^ h2 ^ h3;
		}
	};

	struct equal_fn {
		bool operator()(const CEdge& v1, const CEdge& v2) const {
			return AreVerticesEqual(XMFLOAT3(v1.x, v1.y, v1.z), XMFLOAT3(v2.x, v2.y, v2.z), 6);
		}
	};

	// Graph Edge�� Map ���� ����
	using EdgeMap = std::unordered_map<CEdge, std::unordered_set<CEdge, hash_fn>, hash_fn>;

	// ���� ��� ����� ���� �Լ�
	void FindBoundaryEdges(const std::vector<std::pair<CEdge, CEdge>>& edges, EdgeMap& edgeMap);
	std::vector<CEdge> ExtractBoundaryLoop(EdgeMap& edgeMap);
	void TriangulateBoundary(const std::vector<CEdge>& boundary, std::vector<CVertex>& fillVertices, std::vector<UINT>& fillIndices, XMFLOAT3& normal);

	// ��� ���� ���Ʒ� ���� ������Ʈ ���� �Լ� (���ܸ��� ����ִ�)
	void HandleTriangleCut(std::vector<CVertex>& vertices, std::vector<UINT>& indices, int i, XMFLOAT3& planeNormal, XMFLOAT3& planePoint, std::vector<CVertex>& newVerticesUp, std::vector<CVertex>& newVerticesDown, std::vector<UINT>& newIndicesUp, std::vector<UINT>& newIndicesDown, std::vector<std::pair<CEdge, CEdge>>& edges);
	std::pair<CVertex, bool> CalculateIntersection(CVertex& v1, CVertex& v2, XMFLOAT3& planeNormal, XMFLOAT3& planePoint);

private:
	// �ﰢ���� normal�� ����ϴ� �Լ�
	XMFLOAT3 CalculateNormal(const XMFLOAT3& v1, const XMFLOAT3& v2, const XMFLOAT3& v3) {
		XMFLOAT3 u = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };
		XMFLOAT3 v = { v3.x - v1.x, v3.y - v1.y, v3.z - v1.z };
		XMFLOAT3 normal = {
			u.y * v.z - u.z * v.y,
			u.z * v.x - u.x * v.z,
			u.x * v.y - u.y * v.x
		};
		return normal;
	}

	// �ﰢ���� �����߽����� ����ϴ� �Լ�
	XMFLOAT3 CalculateCentroid(const XMFLOAT3& v1, const XMFLOAT3& v2, const XMFLOAT3& v3) {
		return { (v1.x + v2.x + v3.x) / 3, (v1.y + v2.y + v3.y) / 3, (v1.z + v2.z + v3.z) / 3 };
	}

	// ������ �����߽����� Normal ���� �̿��� CCW�� �����ϴ� �Լ�
	void SortVerticesCCW(std::vector<CVertex>& vertices, const XMFLOAT3& centroid, const XMFLOAT3& normal) {
		std::sort(vertices.begin(), vertices.end(), [&centroid, &normal](const CVertex& a, const CVertex& b) {
			XMFLOAT3 va = { a.m_xmf3Vertex.x - centroid.x, a.m_xmf3Vertex.y - centroid.y, a.m_xmf3Vertex.z - centroid.z };
			XMFLOAT3 vb = { b.m_xmf3Vertex.x - centroid.x, b.m_xmf3Vertex.y - centroid.y, b.m_xmf3Vertex.z - centroid.z };
			return (va.x * vb.y - va.y * vb.x) > 0;
			});
	}

	// �ﰢ���� CCW�� vector�� �������ִ� �Լ�
	void AddTriangle(std::vector<CVertex>& vertices, std::vector<UINT>& indices, const CVertex& v1, const CVertex& v2, const CVertex& v3, XMFLOAT3& oriNormal) {
		XMFLOAT3 newNormal = CalculateNormal(v1.m_xmf3Vertex, v2.m_xmf3Vertex, v3.m_xmf3Vertex);
		vertices.push_back(v1);
		if (Vector3::DotProduct(oriNormal, newNormal) > 0) {
			vertices.push_back(v2);
			vertices.push_back(v3);
		}
		else {
			vertices.push_back(v3);
			vertices.push_back(v2);
		}
		indices.push_back(vertices.size() - 3);
		indices.push_back(vertices.size() - 2);
		indices.push_back(vertices.size() - 1);
	}

private:
	// �׷����� ������ ������ ǥ���ϴ� ����ü
	struct Graph {
		std::unordered_map<int, std::vector<int>> adjList; // ���� ����Ʈ
	};

	class MyFloat3 {
	public:
		MyFloat3() { x = 0.f, y = 0.f, z = 0.f; }
		MyFloat3(XMFLOAT3 val) { x = val.x, y = val.y, z = val.z; }

		float x, y, z;

		bool operator==(const MyFloat3& rhs) const {
			return x == rhs.x && y == rhs.y && z == rhs.z;
		}
	};

	// ���� ���� Ž���� ���� hash �Լ�
	struct Vector3Hash {
		std::size_t operator()(const MyFloat3& v) const {
			std::size_t h1 = std::hash<float>{}(v.x);
			std::size_t h2 = std::hash<float>{}(v.y);
			std::size_t h3 = std::hash<float>{}(v.z);
			return h1 ^ h2 ^ h3;
		}
	};

	// �׷������� ����� ������Ʈ�� ã�� �Լ� (BFS ���)
	std::vector<std::vector<int>> findConnectedComponents(const Graph& graph, int vertexCount);
	// ���ܵ� ������Ʈ�� �׷����� ��ȯ�ϴ� �Լ�
	Graph createGraph(const std::vector<UINT>& indices);
	// ���� ������ ã�� �ε����� ������ϴ� �Լ�
	void remapVerticesAndIndices(
		const std::vector<CVertex>& vertices, const std::vector<UINT>& indices,
		std::vector<CVertex>& uniqueVertices, std::vector<UINT>& remappedIndices);

};

class CBoxMesh : public CDynamicShapeMesh
{
public:
	CBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width = 20.0f, float height = 20.0f, float depth = 20.0f);
	CBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float r, float g, float b, float width = 20.0f, float height = 20.0f, float depth = 20.0f);
	virtual ~CBoxMesh();
};

class CCutterMesh : public CDynamicShapeMesh // Cutter�� ����Ŭ����
{
public:
	CCutterMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CCutterMesh();

protected:
	// ���� ��� - �ٸ� ������Ʈ�� �ڸ����� ������ ������ �־�� �Ѵ�.
	XMFLOAT3 m_xmf3PlaneNormal;
	XMFLOAT3 m_xmf3PlanePoint;

public:
	void SetCutPlaneNormal(XMFLOAT3 xmf3PlaneNormal) { m_xmf3PlaneNormal = Vector3::Normalize(xmf3PlaneNormal); }
	void SetCutPlanePoint(XMFLOAT3 xmf3PlanePoint) { m_xmf3PlanePoint = xmf3PlanePoint; }
	XMFLOAT3 GetCutPlaneNormal() { return m_xmf3PlaneNormal; }
	XMFLOAT3 GetCutPlanePoint() { return m_xmf3PlanePoint; }
};

class CCutterBox_NonMesh : public CCutterMesh
{
public:
	CCutterBox_NonMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width = 20.0f, float height = 20.0f, float depth = 20.0f);
	virtual ~CCutterBox_NonMesh();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, bool pRenderAABB = false);
};

class CRayMesh : public CMesh
{
public:
	CRayMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CRay* ray);
	virtual ~CRayMesh();
};

// ------------------------------- FBX V3 -----------------------------------
class CFBXMesh : public CDynamicShapeMesh
{
public:
	CFBXMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFbx_V3::MeshData* pMeshData, UINT nStride = sizeof(CVertex_Skining));
	virtual ~CFBXMesh();
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

class CTexturedVertex
{
public:
	XMFLOAT3						m_xmf3Position;
	XMFLOAT2						m_xmf2TexCoord;

public:
	CTexturedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); }
	CTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf2TexCoord = xmf2TexCoord; }
	CTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f)) { m_xmf3Position = xmf3Position; m_xmf2TexCoord = xmf2TexCoord; }
	~CTexturedVertex() { }
};

class CCubeMeshTextured : public CMesh
{
public:
	CCubeMeshTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshTextured();
};


class CTexturedRectMesh : public CMesh
{
public:
	CTexturedRectMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f, float fxPosition = 0.0f, float fyPosition = 0.0f, float fzPosition = 0.0f);
	virtual ~CTexturedRectMesh();
};

class CHpBarMesh : public CMesh
{
public:
	CHpBarMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight);
	virtual ~CHpBarMesh();

};

class C2DTextureMesh : public CMesh
{
public:
	C2DTextureMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight);
	virtual ~C2DTextureMesh();
};

class AttackRangeMesh : public CMesh
{
public:
	AttackRangeMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fInnerRadius, float fOuterRadius, int nSlices);
	virtual ~AttackRangeMesh();

};

class AttackRangeCircleMesh : public CMesh
{
public:
	AttackRangeCircleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fRadius, int nSlices);
	virtual ~AttackRangeCircleMesh();

};