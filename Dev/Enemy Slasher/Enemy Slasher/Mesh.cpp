#include "stdafx.h"
#include "Mesh.h"
#include <numeric>



CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

}


CMesh::~CMesh()
{
	if (m_pVertices) delete[] m_pVertices;
	if (m_pnIndices) delete[] m_pnIndices;
	if (m_pxmfUV)
	{
		for (int i = 0;i < m_nInElementUVCount;i++)
			delete[]m_pxmfUV[i];
		delete[]m_pxmfUV;
	}
	if (m_pxmfNormal)
	{
		for (int i = 0;i < m_nInElementNormalCount;i++)
			delete[]m_pxmfNormal[i];
		delete[]m_pxmfNormal;
	}

	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}


void CMesh::ReleaseUploadBuffers()
{
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��. 
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;

};

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool pRenderOption)
{
	//�޽��� ������Ƽ�� ������ �����Ѵ�. 
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	//�޽��� ���� ���� �並 �����Ѵ�. 
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);


	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//�ε��� ���۰� ������ �ε��� ���۸� ����������(IA: �Է� ������)�� �����ϰ� �ε����� ����Ͽ� �������Ѵ�. 
	}

	else
	{
		//�޽��� ���� ���� �並 �������Ѵ�(����������(�Է� ������)�� �۵��ϰ� �Ѵ�).
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void CMesh::SetMeshData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nStride, CVertex* pVertices, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	m_nVertices = nVertices;
	m_nStride = nStride;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	if (m_pVertices) delete[] m_pVertices;
	m_pVertices = pVertices;

	// ���ۻ���
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// ���ε����� ���ۺ� �ʱ�ȭ
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = nIndices;
	if (m_pnIndices) delete[] m_pnIndices;
	m_pnIndices = pnIndices;

	//�ε��� ���۸� �����Ѵ�. 
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//�ε��� ���� �並 �����Ѵ�. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}


CColliderMesh::CColliderMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
}

CColliderMesh::~CColliderMesh()
{
	if (m_pCollider) delete m_pCollider;
}

void CColliderMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool pRenderAABB)
{
	//�޽��� ������Ƽ�� ������ �����Ѵ�. 
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	//�޽��� ���� ���� �並 �����Ѵ�. 
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);


	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//�ε��� ���۰� ������ �ε��� ���۸� ����������(IA: �Է� ������)�� �����ϰ� �ε����� ����Ͽ� �������Ѵ�. 
	}

	else
	{
		//�޽��� ���� ���� �並 �������Ѵ�(����������(�Է� ������)�� �۵��ϰ� �Ѵ�).
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}

	if (true == pRenderAABB && nullptr != m_pCollider)
		m_pCollider->RenderCollider(pd3dCommandList);
}

void CColliderMesh::CreateCollider(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pVertices) {
		if (m_pCollider) delete m_pCollider;
		m_pCollider = new COBBColliderWithMesh(pd3dDevice, pd3dCommandList, m_pVertices, m_nVertices);
	}
}



CMeshIlluminated::CMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
}

CMeshIlluminated::~CMeshIlluminated()
{
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals,XMFLOAT3* pxmf3Positions, int nVertices)
{
	int nPrimitives = nVertices / 3;
	UINT nIndex0, nIndex1, nIndex2;
	for (int i = 0; i < nPrimitives; i++)
	{
		nIndex0 = i * 3 + 0;
		nIndex1 = i * 3 + 1;
		nIndex2 = i * 3 + 2;
		XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1],pxmf3Positions[nIndex0]);
		XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2],pxmf3Positions[nIndex0]);
		pxmf3Normals[nIndex0] = pxmf3Normals[nIndex1] = pxmf3Normals[nIndex2] =Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
	}
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals,XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfNormal, xmf3Edge01, xmf3Edge02, xmf3Normal;
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = pnIndices[i * 3 + 0];
			nIndex1 = pnIndices[i * 3 + 1];
			nIndex2 = pnIndices[i * 3 + 2];
			if (pnIndices && ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)))
			{
				xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1],pxmf3Positions[nIndex0]);
				xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2],pxmf3Positions[nIndex0]);
				xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, false);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals,XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfNormal(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);
			if (pnIndices) nIndex0 = pnIndices[nIndex0];
			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);
			if (pnIndices) nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j))
			{
				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1],pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2],pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices, UINT* pnIndices, int nIndices)
{
	switch (m_d3dPrimitiveTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (pnIndices)
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices,pnIndices, nIndices);
		else
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices);
		break;

	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		CalculateTriangleStripVertexNormals(pxmf3Normals, pxmf3Positions, nVertices,pnIndices, nIndices);
		break;

	default:
		break;
	}
}


CCubeMeshIlluminated::CCubeMeshIlluminated(ID3D12Device* pd3dDevice,ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) :CMeshIlluminated(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 8;
	m_nStride = sizeof(CIlluminatedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nIndices = 36;
	UINT pnIndices[36];
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices,sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,&m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	XMFLOAT3 pxmf3Positions[8];
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	pxmf3Positions[0] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[1] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[2] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[3] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[4] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[5] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[6] = XMFLOAT3(+fx, -fy, +fz);
	pxmf3Positions[7] = XMFLOAT3(-fx, -fy, +fz);

	XMFLOAT3 pxmf3Normals[8];
	for (int i = 0; i < 8; i++) pxmf3Normals[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);
	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, pnIndices, m_nIndices);
	CIlluminatedVertex pVertices[8];
	for (int i = 0; i < 8; i++) pVertices[i] = CIlluminatedVertex(pxmf3Positions[i],pxmf3Normals[i]);
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshIlluminated::~CCubeMeshIlluminated()
{
}


CDynamicShapeMesh::CDynamicShapeMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CColliderMesh(pd3dDevice, pd3dCommandList)
{

}

CDynamicShapeMesh::~CDynamicShapeMesh()
{

}

XMFLOAT3 CDynamicShapeMesh::ProjectVertexToPlane(const XMFLOAT3& vertex, const XMFLOAT3& planeNormal, const XMFLOAT3& planePoint)
{
	// ����� �����Ŀ����� ����� ���� ���� �Ÿ��� ���
	XMFLOAT3 toVertex;
	toVertex.x = vertex.x - planePoint.x;
	toVertex.y = vertex.y - planePoint.y;
	toVertex.z = vertex.z - planePoint.z;

	float distance = XMVector3Dot(XMLoadFloat3(&toVertex), XMLoadFloat3(&planeNormal)).m128_f32[0];

	// ����� ��� ���͸� �̿��Ͽ� ������ ��鿡 �������� ����
	XMFLOAT3 projectedVertex;
	projectedVertex.x = vertex.x - distance * planeNormal.x;
	projectedVertex.y = vertex.y - distance * planeNormal.y;
	projectedVertex.z = vertex.z - distance * planeNormal.z;

	return projectedVertex;
}

void CDynamicShapeMesh::DSM_Line::MakeLine(XMFLOAT3 xmfStart, XMFLOAT3 xmfEnd)
{
	m_xmfStart = xmfStart;
	m_xmfEnd = xmfEnd;
	m_xmfDir = XMFLOAT3(xmfEnd.x - xmfStart.x, xmfEnd.y - xmfStart.y, xmfEnd.z - xmfStart.z);
}

void CDynamicShapeMesh::DSM_Triangle::MakeTriangle(XMFLOAT3 xmfPos1, XMFLOAT3 xmfPos2, XMFLOAT3 xmfPos3)
{
	m_xmfPos[0] = xmfPos1;
	m_xmfPos[1] = xmfPos2;
	m_xmfPos[2] = xmfPos3;
}
bool CDynamicShapeMesh::DSM_Triangle::LineTriangleIntersect(DSM_Line& dsmLine, float& t, XMFLOAT3& xmfIntersectionPoint)
{
	XMFLOAT3 e1 = XMFLOAT3(m_xmfPos[1].x - m_xmfPos[0].x, m_xmfPos[1].y - m_xmfPos[0].y, m_xmfPos[1].z - m_xmfPos[0].z);
	XMFLOAT3 e2 = XMFLOAT3(m_xmfPos[2].x - m_xmfPos[0].x, m_xmfPos[2].y - m_xmfPos[0].y, m_xmfPos[2].z - m_xmfPos[0].z);
	XMFLOAT3 h = Vector3::CrossProduct(dsmLine.m_xmfDir, e2);
	float a = Vector3::DotProduct(e1, h);

	if (a > -FLT_EPSILON && a < FLT_EPSILON)
		return false;

	float f = 1.0f / a;
	XMFLOAT3 s = XMFLOAT3(dsmLine.m_xmfStart.x - m_xmfPos[0].x, dsmLine.m_xmfStart.y - m_xmfPos[0].y, dsmLine.m_xmfStart.z - m_xmfPos[0].z);
	float u = f * Vector3::DotProduct(s, h);

	if (u < 0.0f || u > 1.0f)
		return false;

	XMFLOAT3 q = Vector3::CrossProduct(s, e1);
	float v = f * Vector3::DotProduct(dsmLine.m_xmfDir, q);

	if (v < 0.0f || u + v > 1.0f)
		return false;

	t = f * Vector3::DotProduct(e2, q);

	if (t > FLT_EPSILON) {
		xmfIntersectionPoint.x = dsmLine.m_xmfStart.x + t * dsmLine.m_xmfDir.x;
		xmfIntersectionPoint.y = dsmLine.m_xmfStart.y + t * dsmLine.m_xmfDir.y;
		xmfIntersectionPoint.z = dsmLine.m_xmfStart.z + t * dsmLine.m_xmfDir.z;
		
		return true;
	}

	return false;
}

XMFLOAT3 CDynamicShapeMesh::CrossProduct(const XMFLOAT3& O, const XMFLOAT3& A, const XMFLOAT3& B) {
	// O = ���� | A = ����A | B = ����B
	return {
		(A.y - O.y) * (B.z - O.z) - (A.z - O.z) * (B.y - O.y),
		(A.z - O.z) * (B.x - O.x) - (A.x - O.x) * (B.z - O.z),
		(A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x)
	};
}

vector<pair<CVertex, UINT>> CDynamicShapeMesh::ConvexHull(vector<pair<CVertex, UINT>>& vVertex) {
	// Convex Hull�� �����ϴ� Graham's Scan �˰���
	// Vertex�� Index�� �ѽ� vector�� �ް� ����

	int n = vVertex.size();
	if (n <= 1) return vVertex;

	// ������ x, y, z ��ǥ ������� ����
	std::sort(vVertex.begin(), vVertex.end(),
		[](const pair<CVertex, UINT>& a, const pair<CVertex, UINT>& b) {
			return (a.first.m_xmf3Vertex.x < b.first.m_xmf3Vertex.x ||
			(a.first.m_xmf3Vertex.x == b.first.m_xmf3Vertex.x && a.first.m_xmf3Vertex.y < b.first.m_xmf3Vertex.y) ||
				(a.first.m_xmf3Vertex.x == b.first.m_xmf3Vertex.x && a.first.m_xmf3Vertex.y == b.first.m_xmf3Vertex.y && a.first.m_xmf3Vertex.z < b.first.m_xmf3Vertex.z));
		});

	// �ߺ� ����
	auto last = std::unique(vVertex.begin(), vVertex.end(), [](const auto& a, const auto& b) {
		return a.first.m_xmf3Vertex.x == b.first.m_xmf3Vertex.x &&
		a.first.m_xmf3Vertex.y == b.first.m_xmf3Vertex.y &&
		a.first.m_xmf3Vertex.z == b.first.m_xmf3Vertex.z;
		});
	vVertex.erase(last, vVertex.end());
	n = vVertex.size();

	// Lower hull ����
	vector<pair<CVertex, UINT>> lowerHull;
	for (int i = 0; i < n; ++i) {
		while (lowerHull.size() >= 2 && CrossProduct(lowerHull[lowerHull.size() - 2].first.m_xmf3Vertex, lowerHull.back().first.m_xmf3Vertex, vVertex[i].first.m_xmf3Vertex).z <= 0) {
			lowerHull.pop_back();
		}
		lowerHull.emplace_back(vVertex[i]);
	}

	// Upper hull ����
	std::vector<pair<CVertex, UINT>> upperHull;
	for (int i = n - 1; i >= 0; --i) {
		while (upperHull.size() >= 2 && CrossProduct(upperHull[upperHull.size() - 2].first.m_xmf3Vertex, upperHull.back().first.m_xmf3Vertex, vVertex[i].first.m_xmf3Vertex).z <= 0) {
			upperHull.pop_back();
		}
		upperHull.emplace_back(vVertex[i]);
	}

	// Lower hull�� Upper hull ��ġ��
	lowerHull.insert(lowerHull.end(), upperHull.begin() + 1, upperHull.end());
	return lowerHull;
}

XMFLOAT3 CDynamicShapeMesh::TransformVertex(XMFLOAT3& xmf3Vertex, XMFLOAT4X4& xmf4x4Mat)
{
	XMFLOAT3 pos = xmf3Vertex;
	XMStoreFloat3(&pos, XMVector3TransformCoord(XMLoadFloat3(&pos), XMLoadFloat4x4(&xmf4x4Mat)));
	return pos;
}

bool CDynamicShapeMesh::IsVertexAbovePlane(const XMFLOAT3& vertex, const XMFLOAT3& planeNormal, const XMFLOAT3& planePoint)
{
	XMFLOAT3 calNormal = planeNormal;

	// ��� �������� ��� ����
	float A = calNormal.x;
	float B = calNormal.y;
	float C = calNormal.z;
	float D = -XMVectorGetX(XMVector3Dot(XMLoadFloat3(&planePoint), XMLoadFloat3(&calNormal)));

	// ���� ��ǥ�� ��� �����Ŀ� �����Ͽ� ��ȣ �Ǵ�
	float result = A * vertex.x + B * vertex.y + C * vertex.z + D;

	// ��麸�� ���� �ִٸ� true, �ƴϸ� false ��ȯ
	return result > 0.0f;
}

vector<CMesh*> CDynamicShapeMesh::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, XMFLOAT4X4& mxf4x4ThisMat, CDynamicShapeMesh* pCutterMesh, XMFLOAT4X4& xmf4x4CutterMat, CutAlgorithm DynamicShapeAlgorithm)
{
	switch (DynamicShapeAlgorithm)
	{
	case CutAlgorithm::Push:
		return DynamicShaping_Push(pd3dDevice, pd3dCommandList, fTimeElapsed, mxf4x4ThisMat, pCutterMesh, xmf4x4CutterMat);
	case CutAlgorithm::ConvexHull:
		return DynamicShaping_ConvexHull(pd3dDevice, pd3dCommandList, fTimeElapsed, mxf4x4ThisMat, pCutterMesh, xmf4x4CutterMat);
	default:
		break;
	}

	return DynamicShaping_Push(pd3dDevice, pd3dCommandList, fTimeElapsed, mxf4x4ThisMat, pCutterMesh, xmf4x4CutterMat);
}

/*bool CDynamicShapeMesh::DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, CDynamicShapeMesh* pOtherMesh, XMFLOAT4X4& xmf4x4ThisMat, XMFLOAT4X4& xmf4x4OtherMat)
{
	// �� �Լ��� �ڽ��� �ٸ� ������Ʈ�� ���� �߸��� �Լ��̴�.
	// �� �Լ��� ���� �ڽ��� Mesh�� �����ȴ�.

	CVertex*	pCutterVertices = pOtherMesh->GetVertices();
	UINT		nCutterVertices = pOtherMesh->GetNumVertices();
	UINT*		pnCutterIndices = pOtherMesh->GetIndices();
	UINT		nCutterIndices  = pOtherMesh->GetNumIndices();

	// Cutter�� Vertex�� �ڽ��� ��ǥ��� ��ȯ�Ͽ� Vertex ��ȯ ó���� �ϴ� ���� ����.
	// Cutter�� ��ǥ�� -> ���� ��ǥ��(���� ��� ��) -> �ڽ��� ��ǥ��(�ڽ��� ����� ��)

	vector<XMFLOAT3> vCutterVerices;
	XMFLOAT3 xmfVertex;
	XMFLOAT4X4 xmf4x4InvThisMat = Matrix4x4::Inverse(xmf4x4ThisMat);
	for (int i = 0; i < nCutterVertices; ++i) {
		xmfVertex = pCutterVertices[i].GetVertex();
		xmfVertex = TransformVertex(xmfVertex, xmf4x4OtherMat); // World��ȯ
		xmfVertex = TransformVertex(xmfVertex, xmf4x4InvThisMat); // �ڱ� ��ǥ�� ��ȯ
		vCutterVerices.emplace_back(xmfVertex);
	}

	// ��� �޽��� TriangleList�� �����Ǿ��ִٰ� ����


	{ // �� �ڵ�� �浹������ Vertex�� ������� �� ������ �ϰ��� �Ѵ�. ���� ������ �����δ�.

		// ���� ���� �޽��� ������ü�̴�. ������ ������ �浹�� ������ �̰��� ������� üũ����

		XMFLOAT3 xmfAABBMaxPos = pOtherMesh->GetCollider()->GetAABBMaxPos();
		XMFLOAT3 xmfAABBMinPos = pOtherMesh->GetCollider()->GetAABBMinPos();
		//cout << xmfAABBMaxPos.x << '\t' << xmfAABBMaxPos.y << '\t' << xmfAABBMaxPos.z << "\t\t\t";
		//cout << xmfAABBMinPos.x << '\t' << xmfAABBMinPos.y << '\t' << xmfAABBMinPos.z << '\n';

		vector<XMFLOAT3> vCollisionVertex;
		for (XMFLOAT3& xmfCutterVertex : vCutterVerices) {
			if (xmfCutterVertex.x < xmfAABBMinPos.x) continue;
			if (xmfCutterVertex.x > xmfAABBMaxPos.x) continue;
			if (xmfCutterVertex.y < xmfAABBMinPos.y) continue;
			if (xmfCutterVertex.y > xmfAABBMaxPos.y) continue;
			if (xmfCutterVertex.z < xmfAABBMinPos.z) continue;
			if (xmfCutterVertex.z > xmfAABBMaxPos.z) continue;
			vCollisionVertex.emplace_back(xmfCutterVertex);
		}

		DSM_Line dsmLine;
		DSM_Triangle dsmTri;
		float t;
		XMFLOAT3 xmfIntersectionPoint;
		{
			for (int i = 0; i < (nCutterIndices / 3); ++i) {
				for (int j = 0; j < (m_nIndices / 3); ++j) {
					dsmTri.MakeTriangle(
						m_pVertices[m_pnIndices[j * 3 + 0]].GetVertex(),
						m_pVertices[m_pnIndices[j * 3 + 1]].GetVertex(),
						m_pVertices[m_pnIndices[j * 3 + 2]].GetVertex()
					);

					dsmLine.MakeLine(
						pCutterVertices[pnCutterIndices[i * 3 + 0]].GetVertex(),
						pCutterVertices[pnCutterIndices[i * 3 + 1]].GetVertex()
					);
					if (dsmTri.LineTriangleIntersect(dsmLine, t, xmfIntersectionPoint)) {
						vCollisionVertex.push_back(xmfIntersectionPoint);
					}

					dsmLine.MakeLine(
						pCutterVertices[pnCutterIndices[i * 3 + 1]].GetVertex(),
						pCutterVertices[pnCutterIndices[i * 3 + 2]].GetVertex()
					);
					if (dsmTri.LineTriangleIntersect(dsmLine, t, xmfIntersectionPoint)) {
						vCollisionVertex.push_back(xmfIntersectionPoint);
					}

					dsmLine.MakeLine(
						pCutterVertices[pnCutterIndices[i * 3 + 2]].GetVertex(),
						pCutterVertices[pnCutterIndices[i * 3 + 0]].GetVertex()
					);
					if (dsmTri.LineTriangleIntersect(dsmLine, t, xmfIntersectionPoint)) {
						vCollisionVertex.push_back(xmfIntersectionPoint);
					}
				}
			}
		}
		{
			XMFLOAT3 xmfMaxPos(FLT_MIN, FLT_MIN, FLT_MIN);
			XMFLOAT3 xmfMinPos(FLT_MAX, FLT_MAX, FLT_MAX);
			for (XMFLOAT3& a : vCollisionVertex) {
				if (xmfMaxPos.x < a.x) xmfMaxPos.x = a.x;
				if (xmfMinPos.x > a.x) xmfMinPos.x = a.x;
				if (xmfMaxPos.y < a.y) xmfMaxPos.y = a.y;
				if (xmfMinPos.y > a.y) xmfMinPos.y = a.y;
				if (xmfMaxPos.z < a.z) xmfMaxPos.z = a.z;
				if (xmfMinPos.z > a.z) xmfMinPos.z = a.z;
			}
			//cout << "NewCollisionArea\n" <<
			//	"\t\tMax =\t" << xmfMaxPos.x << "\t\t" << xmfMaxPos.y << "\t\t" << xmfMaxPos.z << "\n" <<
			//	"\t\tMin =\t" << xmfMinPos.x << "\t\t" << xmfMinPos.y << "\t\t" << xmfMinPos.z << "\n";
		}

		//m_nVertices = vCollisionVertex.size();				// ������ ����
		//m_nStride = sizeof(CVertex); // x , y, z ��ǥ
		//m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		//std::random_device rd;
		//std::default_random_engine dre(rd());
		//std::uniform_real_distribution <> urd(0.0, 1.0);

		//if (m_pVertices) delete[] m_pVertices;
		//m_pVertices = new CVertex[m_nVertices];
		//for (int i = 0; i < m_nVertices; ++i) {
		//	m_pVertices[i] = CVertex(vCollisionVertex[i], XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
		//}

		//// ���ۻ���
		//if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
		//if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
		//m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

		//// ���ε����� ���ۺ� �ʱ�ȭ
		//m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
		//m_d3dVertexBufferView.StrideInBytes = m_nStride;
		//m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

		//m_nIndices = m_nVertices * 3;
		//if(m_pnIndices) delete[] m_pnIndices;
		//m_pnIndices = new UINT[m_nIndices];

		//vector<UINT> numbers(m_nVertices - 1);
		//std::iota(numbers.begin(), numbers.end(), 1);
		//std::shuffle(numbers.begin(), numbers.end(), dre);
		//for (int i = 0; i < m_nVertices; ++i) {
		//	std::vector<int> selectedNumbers(numbers.begin(), numbers.begin() + 3);
		//	m_pnIndices[i * 3 + 0] = selectedNumbers[0];
		//	m_pnIndices[i * 3 + 1] = selectedNumbers[1];
		//	m_pnIndices[i * 3 + 2] = selectedNumbers[2];
		//}

		//if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
		//if (m_pd3dIndexUploadBuffer)m_pd3dIndexUploadBuffer->Release();
		////�ε��� ���۸� �����Ѵ�. 
		//m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
		////�ε��� ���� �並 �����Ѵ�. 
		//m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
		//m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		//m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

		//m_bCuttable = false;
	}
	return true;
}*/

vector<CMesh*> CDynamicShapeMesh::DynamicShaping_Push(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, XMFLOAT4X4& xmf4x4ThisMat, CDynamicShapeMesh* pCutterMesh, XMFLOAT4X4& xmf4x4CutterMat)
{
	// �� ������ ���� Object�� ��������� ����Ͽ� �����Ѵ�.

	// �� �Լ��� �ڽ��� �ٸ� ������Ʈ�� ���� �߸��� �Լ��̴�.
	// �� �Լ��� ���� �ڽ��� Mesh�� �����ȴ�.
	// ���ܵ� CMesh 2�� �̻��� �����Ѵ�.

	vector<CMesh*> pvNewMeshs; // ���� ���� �޽����� ������ ����

	if (CCutterMesh* pCutter = dynamic_cast<CCutterMesh*>(pCutterMesh)) {

		XMFLOAT3 xmf3CutNormal = pCutter->GetCutPlaneNormal();
		XMFLOAT3 xmf3CutPoint = pCutter->GetCutPlanePoint();

		// Cutter�� Plain ���� �ڽ��� ��ǥ��� ��ȯ�Ͽ� Vertex ��ȯ ó���� �ϴ� ���� ����.
		// Cutter�� ��ǥ�� -> ���� ��ǥ��(���� ��� ��) -> �ڽ��� ��ǥ��(�ڽ��� ����� ��)

		XMFLOAT4X4 xmf4x4InvThisMat = Matrix4x4::Inverse(xmf4x4ThisMat);
		XMFLOAT4X4 xmf4x4InvTransposeThisMat = Matrix4x4::Transpose(xmf4x4InvThisMat);
		XMFLOAT4X4 xmf4x4TransposeCutterMat = Matrix4x4::Transpose(xmf4x4CutterMat);

		// ��� �������� ��ȯ�� ��
		XMFLOAT3 xmf3LPlaneNormal, xmf3LPlanePoint;
		xmf3LPlaneNormal = TransformVertex(xmf3CutNormal, xmf4x4TransposeCutterMat);
		xmf3LPlaneNormal = TransformVertex(xmf3LPlaneNormal, xmf4x4InvTransposeThisMat);
		xmf3LPlaneNormal = Vector3::Normalize(xmf3LPlaneNormal);
		xmf3LPlanePoint = TransformVertex(xmf3CutPoint, xmf4x4CutterMat);
		xmf3LPlanePoint = TransformVertex(xmf3LPlanePoint, xmf4x4InvThisMat);

		vector<pair<vector<CVertex>*, bool>> vvNewVertices; // first = vertex, second = PlainInObject
		int nNewVertices = 0;
		{
			// �̰��� ���ܽ� ����� ��� ������� Ȯ���ϸ� ����.
			// ����� ���� ��� ���� �¿�� �� 2���� ����� ���� �����Ѵ�.
			vvNewVertices.emplace_back(new vector<CVertex>, false);
			vvNewVertices.emplace_back(new vector<CVertex>, false);

			nNewVertices = vvNewVertices.size();
		}

		for (int i = 0; i < m_nVertices; ++i) {
			if (IsVertexAbovePlane(m_pVertices[i].GetVertex(), xmf3LPlaneNormal, xmf3LPlanePoint)) {
				XMFLOAT3 xmf3Vertex = ProjectVertexToPlane(m_pVertices[i].m_xmf3Vertex, xmf3LPlaneNormal, xmf3LPlanePoint);
				XMFLOAT3 xmf33Normal = m_pVertices[i].m_xmf3Normal;
				XMFLOAT4 xmf4Color = m_pVertices[i].m_xmf4Color;
				vvNewVertices[1].first->emplace_back(xmf3Vertex, xmf33Normal, xmf4Color);

				vvNewVertices[0].first->emplace_back(m_pVertices[i]);
				vvNewVertices[0].second = true;
			}
			else {
				XMFLOAT3 xmf3Vertex = ProjectVertexToPlane(m_pVertices[i].m_xmf3Vertex, xmf3LPlaneNormal, xmf3LPlanePoint);
				XMFLOAT3 xmf33Normal = m_pVertices[i].m_xmf3Normal;
				XMFLOAT4 xmf4Color = m_pVertices[i].m_xmf4Color;
				vvNewVertices[0].first->emplace_back(xmf3Vertex, xmf33Normal, xmf4Color);

				vvNewVertices[1].first->emplace_back(m_pVertices[i]);
				vvNewVertices[1].second = true;
			}
		}
		{
			// ���� ����ִ� ���Ͱ� �ִٸ� / ���� ������Ʈ�� ��ġ�� �ʾҴٸ� ����
			auto it = std::remove_if(vvNewVertices.begin(), vvNewVertices.end(),
				[](const pair<vector<CVertex>*, bool> pVector) {
					if (pVector.first->empty() || false == pVector.second) {
						delete pVector.first;
						return true;
					}
			return false;
				});
			vvNewVertices.erase(it, vvNewVertices.end());
			nNewVertices = vvNewVertices.size();
		}

		if (nNewVertices < 2) return pvNewMeshs; // ���ܸ� �������� 2�� �̻����� ���������� ������ ������ ������ ������ ����.


		vector<CVertex*> pvNewVertices;
		vector<UINT*> pvNewIndices;
		for (int i = 0; i < nNewVertices; ++i) {
			pvNewVertices.push_back(new CVertex[m_nVertices]);
			copy(vvNewVertices[i].first->begin(), vvNewVertices[i].first->end(), pvNewVertices[i]);

			pvNewIndices.push_back(new UINT[m_nIndices]);
			memcpy(pvNewIndices[i], m_pnIndices, sizeof(UINT) * m_nIndices);

			delete vvNewVertices[i].first; // �� ����� Vertex vector ����
		}

		for (int i = 0; i < nNewVertices; ++i) {
			pvNewMeshs.push_back(new CDynamicShapeMesh(pd3dDevice, pd3dCommandList));
			pvNewMeshs[i]->SetMeshData(pd3dDevice, pd3dCommandList, m_nStride, pvNewVertices[i], m_nVertices, pvNewIndices[i], m_nIndices);
			((CDynamicShapeMesh*)pvNewMeshs[i])->CreateCollider(pd3dDevice, pd3dCommandList);
			((CDynamicShapeMesh*)pvNewMeshs[i])->SetCuttable(true);
		}
	}
	return pvNewMeshs;
}

vector<CMesh*> CDynamicShapeMesh::DynamicShaping_ConvexHull(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fTimeElapsed, XMFLOAT4X4& xmf4x4ThisMat, CDynamicShapeMesh* pCutterMesh, XMFLOAT4X4& xmf4x4CutterMat)
{
	// �� ������ ���� Object�� ��������� ����Ͽ� �����Ѵ�.
	
	// �� �Լ��� �ڽ��� �ٸ� ������Ʈ�� ���� �߸��� �Լ��̴�.
	// �� �Լ��� ���� �ڽ��� Mesh�� �����ȴ�.
	// ���ܵ� CMesh 2�� �̻��� �����Ѵ�.

	vector<CMesh*> pvNewMeshs; // ���� ���� �޽����� ������ ����

	if (CCutterMesh* pCutter = dynamic_cast<CCutterMesh*>(pCutterMesh)) {

		XMFLOAT3 xmf3CutNormal = pCutter->GetCutPlaneNormal();
		XMFLOAT3 xmf3CutPoint = pCutter->GetCutPlanePoint();

		// Cutter�� Plain ���� �ڽ��� ��ǥ��� ��ȯ�Ͽ� Vertex ��ȯ ó���� �ϴ� ���� ����.
		// Cutter�� ��ǥ�� -> ���� ��ǥ��(���� ��� ��) -> �ڽ��� ��ǥ��(�ڽ��� ����� ��)

		XMFLOAT4X4 xmf4x4InvThisMat = Matrix4x4::Inverse(xmf4x4ThisMat);
		XMFLOAT4X4 xmf4x4InvTransposeThisMat = Matrix4x4::Transpose(xmf4x4InvThisMat);
		XMFLOAT4X4 xmf4x4TransposeCutterMat = Matrix4x4::Transpose(xmf4x4CutterMat);

		// ��� �������� ��ȯ�� ��
		XMFLOAT3 xmf3LPlaneNormal, xmf3LPlanePoint;
		xmf3LPlaneNormal = TransformVertex(xmf3CutNormal, xmf4x4TransposeCutterMat);
		xmf3LPlaneNormal = TransformVertex(xmf3LPlaneNormal, xmf4x4InvTransposeThisMat);
		xmf3LPlaneNormal = Vector3::Normalize(xmf3LPlaneNormal);
		xmf3LPlanePoint = TransformVertex(xmf3CutPoint, xmf4x4CutterMat);
		xmf3LPlanePoint = TransformVertex(xmf3LPlanePoint, xmf4x4InvThisMat);

		vector<pair<vector<CVertex>, bool>> vvNewVertices; // first = vertex, second = PlainInObject | ���ο� ���� vertex
		vector<map<UINT, UINT>> vmIndicesMap;	// ���� Index�� ���ο� ������Ʈ�� Index�� mapping���� map
		vector<vector<UINT>> vvnNewIndices;		// ���� ���� ������Ʈ���� ���ο� Index���� ������ vector

		constexpr int PLANE_UP = 0;
		constexpr int PLANE_DOWN = 1;

		

		int nNewVertices = 0;
		{
			// �̰��� ���ܽ� ����� ��� ������� Ȯ���ϸ� ����.
			// ����� ���� ��� ���� �¿�� �� 2���� ����� ���� �����Ѵ�.
			vvNewVertices.emplace_back();
			vvNewVertices.emplace_back();

			nNewVertices = vvNewVertices.size();
		}

		for (int i = 0; i < nNewVertices; ++i) {
			vvNewVertices[i].second = false;
			vmIndicesMap.emplace_back();
			vvnNewIndices.emplace_back();
		}

		int nPlainAboveCounter = 0;
		int nPlainUnderCounter = 0;
		{
			// Vertex ��� ���� �и�
			for (int i = 0; i < m_nVertices; ++i) {
				if (IsVertexAbovePlane(m_pVertices[i].GetVertex(), xmf3LPlaneNormal, xmf3LPlanePoint)) { // ��� ��
					vvNewVertices[PLANE_UP].first.emplace_back(m_pVertices[i]);
					vmIndicesMap[PLANE_UP][i] = nPlainAboveCounter++;

					vvNewVertices[PLANE_UP].second = true;
				}
				else { // ��� �Ʒ�
					vvNewVertices[PLANE_DOWN].first.emplace_back(m_pVertices[i]);
					vmIndicesMap[PLANE_DOWN][i] = nPlainUnderCounter++;

					vvNewVertices[PLANE_DOWN].second = true;
				}
			}
		}
		
		vector<pair<CVertex, UINT>> vConvexHullVertex;			// ���� ��� �ܰ������� ConvexHull�� �� ������� ������ vector
		vector<vector<pair<CVertex, UINT>>> vvPlaneVertices;	// ���� ��� �ܰ����� Index�� ������ vector
		for (int i = 0; i < nNewVertices; ++i) {
			vConvexHullVertex.emplace_back();
			vvPlaneVertices.emplace_back();
		}
		{
			// IndexList ���п� ���� Vertex ��з�
			for (int i = 0; i < m_nIndices / 3; ++i) {
				bool bVal[3];
				for (int j = 0; j < 3; ++j) bVal[j] = IsVertexAbovePlane(m_pVertices[m_pnIndices[i * 3 + j]].GetVertex(), xmf3LPlaneNormal, xmf3LPlanePoint);

				if (bVal[0] && bVal[1] && bVal[2]) { // ��� ��
					for (int j = 0; j < 3; ++j) vvnNewIndices[PLANE_UP].emplace_back(vmIndicesMap[PLANE_UP][m_pnIndices[i * 3 + j]]);
				}
				else if (bVal[0] || bVal[1] || bVal[2]) { // ���� ��ġ�� ������Ƽ��
					XMFLOAT3 xmf3Vertex;
					for (int j = 0; j < 3; ++j) {
						xmf3Vertex = ProjectVertexToPlane(m_pVertices[m_pnIndices[i * 3 + j]].m_xmf3Vertex, xmf3LPlaneNormal, xmf3LPlanePoint);
						if (IsVertexAbovePlane(m_pVertices[m_pnIndices[i * 3 + j]].GetVertex(), xmf3LPlaneNormal, xmf3LPlanePoint)) { // ���� ��� ��
							vvNewVertices[PLANE_DOWN].first.emplace_back(
								xmf3Vertex,
								m_pVertices[m_pnIndices[i * 3 + j]].m_xmf3Normal,
								m_pVertices[m_pnIndices[i * 3 + j]].m_xmf4Color);
							vmIndicesMap[PLANE_DOWN][m_pnIndices[i * 3 + j]] = nPlainUnderCounter;

							{
								// ConvexHull ����� ���� Vertex�� ���� ����
								vvPlaneVertices[PLANE_DOWN].emplace_back(
									CVertex(xmf3Vertex,
									m_pVertices[m_pnIndices[i * 3 + j]].m_xmf3Normal,
									m_pVertices[m_pnIndices[i * 3 + j]].m_xmf4Color), nPlainUnderCounter);
							}

							nPlainUnderCounter++;
						}
						else { // ���� ��� �Ʒ�
							vvNewVertices[PLANE_UP].first.emplace_back(
								xmf3Vertex,
								m_pVertices[m_pnIndices[i * 3 + j]].m_xmf3Normal,
								m_pVertices[m_pnIndices[i * 3 + j]].m_xmf4Color);
							vmIndicesMap[PLANE_UP][m_pnIndices[i * 3 + j]] = nPlainAboveCounter;

							{
								// ConvexHull ����� ���� Vertex�� ���� ����
								vvPlaneVertices[PLANE_UP].emplace_back(
									CVertex(xmf3Vertex,
									m_pVertices[m_pnIndices[i * 3 + j]].m_xmf3Normal,
									m_pVertices[m_pnIndices[i * 3 + j]].m_xmf4Color), nPlainAboveCounter);
							}

							nPlainAboveCounter++;
						}
						vvnNewIndices[PLANE_UP].emplace_back(vmIndicesMap[PLANE_UP][m_pnIndices[i * 3 + j]]);
						vvnNewIndices[PLANE_DOWN].emplace_back(vmIndicesMap[PLANE_DOWN][m_pnIndices[i * 3 + j]]);

					}
				}
				else { // ��� �Ʒ�
					for (int j = 0; j < 3; ++j) vvnNewIndices[PLANE_DOWN].emplace_back(vmIndicesMap[PLANE_DOWN][m_pnIndices[i * 3 + j]]);
				}
			}
		}

		{
			// ���� ����ִ� ���Ͱ� �ִٸ� / ���� ������Ʈ�� ��ġ�� �ʾҴٸ� ����
			auto itVertices = vvNewVertices.begin();
			auto itIndices = vmIndicesMap.begin();

			while (itVertices != vvNewVertices.end()) {
				if (itVertices->first.empty() || false == itVertices->second) {
					itVertices = vvNewVertices.erase(itVertices);
					itIndices = vmIndicesMap.erase(itIndices);
				}
				else {
					++itVertices;
					++itIndices;
				}
			}
			nNewVertices = vvNewVertices.size();

			if (nNewVertices < 2) return pvNewMeshs; // ���ܸ� �������� 2�� �̻����� ���������� ������ ������ ������ ������ ����.
		}

		{
			// ConvexHull �� ���� ���ܸ� ����
			{
				vConvexHullVertex = ConvexHull(vvPlaneVertices[PLANE_UP]);
				vector<UINT> nNewPlaneIndex;
				for (int i = 2; i < vConvexHullVertex.size(); ++i) {
					nNewPlaneIndex.emplace_back(vConvexHullVertex[0].second);
					nNewPlaneIndex.emplace_back(vConvexHullVertex[i - 1].second);
					nNewPlaneIndex.emplace_back(vConvexHullVertex[i].second);
					nNewPlaneIndex.emplace_back(vConvexHullVertex[0].second);
					nNewPlaneIndex.emplace_back(vConvexHullVertex[i].second);
					nNewPlaneIndex.emplace_back(vConvexHullVertex[i - 1].second);
				}
				vvnNewIndices[PLANE_UP].insert(vvnNewIndices[PLANE_UP].end(), nNewPlaneIndex.begin(), nNewPlaneIndex.end());
			}
			{
				vConvexHullVertex = ConvexHull(vvPlaneVertices[PLANE_DOWN]);
				vector<UINT> nNewPlaneIndex;
				for (int i = 2; i < vConvexHullVertex.size(); ++i) {
					nNewPlaneIndex.emplace_back(vConvexHullVertex[0].second);
					nNewPlaneIndex.emplace_back(vConvexHullVertex[i - 1].second);
					nNewPlaneIndex.emplace_back(vConvexHullVertex[i].second);
					nNewPlaneIndex.emplace_back(vConvexHullVertex[0].second);
					nNewPlaneIndex.emplace_back(vConvexHullVertex[i].second);
					nNewPlaneIndex.emplace_back(vConvexHullVertex[i - 1].second);
				}
				vvnNewIndices[PLANE_DOWN].insert(vvnNewIndices[PLANE_DOWN].end(), nNewPlaneIndex.begin(), nNewPlaneIndex.end());
			}
		}

		{
			CVertex* pNewVertices;
			UINT* pNewIndices;
			for (int i = 0; i < nNewVertices; ++i) {
				pNewVertices = new CVertex[vvNewVertices[i].first.size()];
				copy(vvNewVertices[i].first.begin(), vvNewVertices[i].first.end(), pNewVertices);

				pNewIndices = new UINT[vvnNewIndices[i].size()];
				copy(vvnNewIndices[i].begin(), vvnNewIndices[i].end(), pNewIndices);

				pvNewMeshs.push_back(new CDynamicShapeMesh(pd3dDevice, pd3dCommandList));
				pvNewMeshs[i]->SetMeshData(pd3dDevice, pd3dCommandList, m_nStride, pNewVertices, vvNewVertices[i].first.size(), pNewIndices, vvnNewIndices[i].size());
				((CDynamicShapeMesh*)pvNewMeshs[i])->CreateCollider(pd3dDevice, pd3dCommandList);
				((CDynamicShapeMesh*)pvNewMeshs[i])->SetCuttable(true);
			}
		}
	}
	return pvNewMeshs;
}


CBoxMesh::CBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width, float height, float depth) : CDynamicShapeMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 8;				// ������ ����
	m_nStride = sizeof(CVertex); // x , y, z ��ǥ
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution <> urd(0.0, 1.0);

	float fx = width * 0.5f, fy = height * 0.5f, fz = depth * 0.5f;

	m_pVertices = new CVertex[m_nVertices];
	m_pVertices[0] = CVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[1] = CVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[2] = CVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[3] = CVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[4] = CVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[5] = CVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[6] = CVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
	m_pVertices[7] = CVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));

	// ���ۻ���
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// ���ε����� ���ۺ� �ʱ�ȭ
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 36;
	m_pnIndices = new UINT[m_nIndices];

	//�� �ո�(Front) �簢���� ���� �ﰢ��
	m_pnIndices[0] = 3; m_pnIndices[1] = 1; m_pnIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[3] = 2; m_pnIndices[4] = 1; m_pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��
	m_pnIndices[6] = 0; m_pnIndices[7] = 5; m_pnIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[9] = 1; m_pnIndices[10] = 5; m_pnIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	m_pnIndices[12] = 3; m_pnIndices[13] = 4; m_pnIndices[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[15] = 0; m_pnIndices[16] = 4; m_pnIndices[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	m_pnIndices[18] = 1; m_pnIndices[19] = 6; m_pnIndices[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[21] = 2; m_pnIndices[22] = 6; m_pnIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��
	m_pnIndices[24] = 2; m_pnIndices[25] = 7; m_pnIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[27] = 3; m_pnIndices[28] = 7; m_pnIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��
	m_pnIndices[30] = 6; m_pnIndices[31] = 4; m_pnIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[33] = 7; m_pnIndices[34] = 4; m_pnIndices[35] = 6;

	//�ε��� ���۸� �����Ѵ�. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//�ε��� ���� �並 �����Ѵ�. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	CreateCollider(pd3dDevice, pd3dCommandList);
}

CBoxMesh::CBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float r, float g, float b, float width, float height, float depth) : CDynamicShapeMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 8;				// ������ ����
	m_nStride = sizeof(CVertex); // x , y, z ��ǥ
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = width * 0.5f, fy = height * 0.5f, fz = depth * 0.5f;

	m_pVertices = new CVertex[m_nVertices];
	m_pVertices[0] = CVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(r, g, b, 1.0f));
	m_pVertices[1] = CVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT4(r, g, b, 1.0f));
	m_pVertices[2] = CVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT4(r, g, b, 1.0f));
	m_pVertices[3] = CVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT4(r, g, b, 1.0f));
	m_pVertices[4] = CVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT4(r, g, b, 1.0f));
	m_pVertices[5] = CVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT4(r, g, b, 1.0f));
	m_pVertices[6] = CVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT4(r, g, b, 1.0f));
	m_pVertices[7] = CVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT4(r, g, b, 1.0f));

	// ���ۻ���
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// ���ε����� ���ۺ� �ʱ�ȭ
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 36;
	m_pnIndices = new UINT[m_nIndices];

	//�� �ո�(Front) �簢���� ���� �ﰢ��
	m_pnIndices[0] = 3; m_pnIndices[1] = 1; m_pnIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[3] = 2; m_pnIndices[4] = 1; m_pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��
	m_pnIndices[6] = 0; m_pnIndices[7] = 5; m_pnIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[9] = 1; m_pnIndices[10] = 5; m_pnIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	m_pnIndices[12] = 3; m_pnIndices[13] = 4; m_pnIndices[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[15] = 0; m_pnIndices[16] = 4; m_pnIndices[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	m_pnIndices[18] = 1; m_pnIndices[19] = 6; m_pnIndices[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[21] = 2; m_pnIndices[22] = 6; m_pnIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��
	m_pnIndices[24] = 2; m_pnIndices[25] = 7; m_pnIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[27] = 3; m_pnIndices[28] = 7; m_pnIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��
	m_pnIndices[30] = 6; m_pnIndices[31] = 4; m_pnIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	m_pnIndices[33] = 7; m_pnIndices[34] = 4; m_pnIndices[35] = 6;

	//�ε��� ���۸� �����Ѵ�. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//�ε��� ���� �並 �����Ѵ�. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	CreateCollider(pd3dDevice, pd3dCommandList);
}

CBoxMesh::~CBoxMesh()
{
}


CCutterMesh::CCutterMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CDynamicShapeMesh(pd3dDevice, pd3dCommandList)
{
	m_xmf3PlanePoint = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3PlaneNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

CCutterMesh::~CCutterMesh()
{
}

CCutterBox_NonMesh::CCutterBox_NonMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width, float height, float depth) : CCutterMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 2;			 // ������ ����
	m_nStride = sizeof(CVertex); // x , y, z ��ǥ
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_xmf3PlanePoint = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3PlaneNormal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	m_xmf3PlaneNormal = Vector3::Normalize(m_xmf3PlaneNormal);

	float fx = width * 1.f, fy = height * 1.f, fz = depth * 1.f;

	m_pVertices = new CVertex[m_nVertices];
	m_pVertices[0] = CVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[1] = CVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

	SetCuttable(true);

	CreateCollider(pd3dDevice, pd3dCommandList);
}

CCutterBox_NonMesh::~CCutterBox_NonMesh()
{
}

void CCutterBox_NonMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, bool pRenderAABB)
{
	if (true == pRenderAABB && nullptr != m_pCollider)
		m_pCollider->RenderCollider(pd3dCommandList);
}


CFBXMesh::CFBXMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CDynamicShapeMesh(pd3dDevice, pd3dCommandList)
{
}

CFBXMesh::~CFBXMesh()
{
}


bool CFBXMesh::LoadMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FbxNode* pNode)
{
	FbxMesh* lMesh = (FbxMesh*)pNode->GetNodeAttribute();

#ifdef _DEBUG
	//std::cout << "Mesh Name: " << (char*)pNode->GetName() << std::endl;
#endif // _DEBUG


	//DisplayMetaDataConnections(lMesh);

	//-----------------------------------------------------------------------------------
	//DisplayControlsPoints(lMesh);
	int i, lControlPointsCount = lMesh->GetControlPointsCount();
	FbxVector4* lControlPoints = lMesh->GetControlPoints();

	m_nVertices = lControlPointsCount;
	m_nStride = sizeof(CVertex); // x , y, z ��ǥ
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution <> urd(0.0, 1.0);

	float fx, fy, fz;

	m_pVertices = new CVertex[m_nVertices];

	FbxVector4 fvNormal;

	for (i = 0; i < lControlPointsCount; i++)
	{
		fx = (float)lControlPoints[i][0]* 1;
		fy = (float)lControlPoints[i][1] *1;
		fz = (float)lControlPoints[i][2] *1;

		for (int j = 0; j < lMesh->GetElementNormalCount(); j++)
		{
			FbxGeometryElementNormal* leNormals = lMesh->GetElementNormal(j);
			if (leNormals->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				if (leNormals->GetReferenceMode() == FbxGeometryElement::eDirect)
					fvNormal = leNormals->GetDirectArray().GetAt(i);
			}
		}
		XMFLOAT2 xmfUV;
		for (int k = 0; k < lMesh->GetElementUVCount(); ++k)
		{
			FbxVector2 fvUV;
			FbxGeometryElementUV* leUV = lMesh->GetElementUV(k);
			fvUV = leUV->GetDirectArray().GetAt(i);	// �̰� ���� �ɵ�?
			xmfUV.x = fvUV[0];
			xmfUV.y = fvUV[1];
			break;// m_nInElementUVCount �� �ϳ��� �ִٰ� ����.
		}
		if (lMesh->GetElementUVCount() == 0)
		{
			m_pVertices[i] = CVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT3(fvNormal[0], fvNormal[1], fvNormal[2]), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f));
		}
		else
			m_pVertices[i] = CVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT3(fvNormal[0], fvNormal[1], fvNormal[2]), XMFLOAT4(urd(dre), urd(dre), urd(dre), 1.0f), XMFLOAT2(xmfUV));
		//m_pVertices[i] = CVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT3(fvNormal[0], fvNormal[1], fvNormal[2]), XMFLOAT2(xmfUV));
	}
	////////////////////////////////////////////////////////////// - ���� ������ ����. - //////////////////////////////////////////////////////////////
	// ���ۻ��� 
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// ���ε����� ���ۺ� �ʱ�ȭ
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//DisplayPolygons(lMesh);
	int j, lPolygonCount = lMesh->GetPolygonCount();
	int lPolygonSize = lMesh->GetPolygonSize(i);
	m_nIndices = lPolygonCount * lPolygonSize;
	m_pnIndices = new UINT[m_nIndices];

	m_nInElementUVCount = lMesh->GetElementUVCount();
	if (m_nInElementUVCount)
	{
		m_pxmfUV = new XMFLOAT2*[m_nInElementUVCount];
		for (int i = 0;i < m_nInElementUVCount;i++)
			m_pxmfUV[i] = new XMFLOAT2[m_nIndices];
	}

	m_nInElementNormalCount = lMesh->GetElementNormalCount();
	if (m_nInElementNormalCount)
	{
		m_pxmfNormal = new XMFLOAT3 * [m_nInElementNormalCount];
		for (int i = 0;i < m_nInElementNormalCount;i++)
			m_pxmfNormal[i] = new XMFLOAT3[m_nIndices];
	}

	for (i = 0; i < lPolygonCount; i++)
	{
		for (j = 0; j < lPolygonSize; j++)
		{
			int lControlPointIndex = lMesh->GetPolygonVertex(i, j);
			m_pnIndices[i * 3 + j] = lControlPointIndex;

			for (int k = 0; k < m_nInElementUVCount; ++k)
			{
				XMFLOAT2 xmfUV;
				FbxVector2 fvUV;
				FbxGeometryElementUV* leUV = lMesh->GetElementUV(k);
				switch (leUV->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:	// �� ã�ƺ�����
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						fvUV = leUV->GetDirectArray().GetAt(lControlPointIndex);	// �̰� ���� �ɵ�?
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
						fvUV = leUV->GetDirectArray().GetAt(id);
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					int lTextureUVIndex = lMesh->GetTextureUVIndex(i, j);
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
					{
						fvUV = leUV->GetDirectArray().GetAt(lTextureUVIndex);
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
				xmfUV.x = fvUV[0];
				xmfUV.y = fvUV[1];
				m_pxmfUV[k][i * 3 + j] = xmfUV;
			}

			for (int k = 0; k < m_nInElementNormalCount; ++k)
			{

				XMFLOAT3 xmfNormal;
				FbxVector4 fvNormal;
				FbxGeometryElementNormal* leNormal = lMesh->GetElementNormal(k);

				if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						fvNormal = leNormal->GetDirectArray().GetAt(lControlPointIndex);
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leNormal->GetIndexArray().GetAt(lControlPointIndex);
						fvNormal = leNormal->GetDirectArray().GetAt(id);
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
				xmfNormal.x = fvNormal[0];
				xmfNormal.y = fvNormal[1];
				xmfNormal.z = fvNormal[2];
			}
		}
	}

	//�ε��� ���۸� �����Ѵ�.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//�ε��� ���� �並 �����Ѵ�. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	////////////////////////////////////////////////////////////// - ���� ������ ����. - //////////////////////////////////////////////////////////////

	//DisplayMaterialMapping(lMesh);
	//DisplayMaterial(lMesh);
	//DisplayTexture(lMesh);
	//DisplayMaterialConnections(lMesh);

	//DisplayLink(lMesh);
	{
		int lSkinCount = lMesh->GetDeformerCount(FbxDeformer::eSkin);
		FbxCluster* lCluster;
		string name;

		for (int i = 0;i != lSkinCount;i++)
		{
			m_nICluster = ((FbxSkin*)lMesh->GetDeformer(i, FbxDeformer::eSkin))->GetClusterCount();

			m_skelList = new CSkeleton[m_nICluster];

			for (int j = 0; j != m_nICluster; ++j)
			{
				lCluster = ((FbxSkin*)lMesh->GetDeformer(i, FbxDeformer::eSkin))->GetCluster(j);

				if (lCluster->GetLink() != NULL)
				{
					name = (char*)lCluster->GetLink()->GetName();
				}
				int lIndicesCount = lCluster->GetControlPointIndicesCount();
				int* lIndices = new int[lIndicesCount];
				double* lWeights = new double[lIndicesCount];
				for (int k = 0;k < lIndicesCount;k++)
				{
					lIndices[k] = (lCluster->GetControlPointIndices())[k];
					lWeights[k] = (lCluster->GetControlPointWeights())[k];
				}

				FbxAMatrix transMatrix;
				transMatrix = lCluster->GetTransformMatrix(transMatrix);
				FbxAMatrix transLinkMatrix;
				transLinkMatrix = lCluster->GetTransformLinkMatrix(transLinkMatrix);

				m_skelList[j].SetData(name, lIndicesCount, lIndices, lWeights, transMatrix, transLinkMatrix);
				
			}

		}

	}
	//DisplayShape(lMesh);

	//DisplayCache(lMesh);

	CreateCollider(pd3dDevice, pd3dCommandList);
	return true;
}

CAABB* CFBXMesh::GetAABB(XMFLOAT4X4 m_xmf4x4World)
{
	float xMin = FLT_MAX, yMin = FLT_MAX, zMin = FLT_MAX;
	float xMax = FLT_MIN, yMax = FLT_MIN, zMax = FLT_MIN;

	XMFLOAT3 ver;

	for (int i = 0;i < m_nVertices;i++) {
		ver = m_pVertices[i].GetVertex();
		
		XMMATRIX mat = XMLoadFloat4x4(&m_xmf4x4World);
		XMStoreFloat3(&ver, XMVector3TransformCoord(XMLoadFloat3(&ver), mat));
		float x = ver.x;
		float y = ver.y;
		float z = ver.z;

		if (x < xMin) xMin = x;
		if (y < yMin) yMin = y;
		if (z < zMin) zMin = z;

		if (x > xMax) xMax = x;
		if (y > yMax) yMax = y;
		if (z > zMax) zMax = z;
	}

	CAABB* aabb = new CAABB(XMFLOAT3((xMax + xMin) / 2, (yMax + yMin) / 2, (zMax + zMin) / 2 ), XMFLOAT3((xMax - xMin) / 2, (yMax - yMin) / 2, (zMax - zMin) / 2));
	//CAABB* aabb = new CAABB(XMFLOAT3((xMax + xMin) / 2 + m_xmf4x4World._41, (yMax + yMin) / 2 + m_xmf4x4World._42, (zMax + zMin) / 2 + m_xmf4x4World._43 ), XMFLOAT3((xMax - xMin) / 2, (yMax - yMin) / 2, (zMax - zMin) / 2));
	return aabb;
}

void CFBXMesh::UpdateVerticesBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* offsetMat)
{
	CVertex* tmp = new CVertex[m_nVertices];
	for (int i = 0;i < m_nVertices;i++)
	{
		tmp[i].m_xmf3Normal = m_pVertices[i].m_xmf3Normal;
		tmp[i].m_xmf3Vertex = m_pVertices[i].m_xmf3Vertex;
		tmp[i].m_xmf4Color = m_pVertices[i].m_xmf4Color;
		tmp[i].m_xmf3Vertex = Vector3::TransformNormal(m_pVertices[i].m_xmf3Vertex, offsetMat[i]);
		//m_pVertices[i].m_xmf3Vertex = Vector3::TransformNormal(m_pVertices[i].m_xmf3Vertex, offsetMat[i]);
		//cout << "before : " << m_pVertices[i].m_xmf3Vertex.x << ", " << m_pVertices[i].m_xmf3Vertex.y << ", " << m_pVertices[i].m_xmf3Vertex.z << endl;
		//cout << "after  : " << tmp[i].m_xmf3Vertex.x << ", " << tmp[i].m_xmf3Vertex.y << ", " << tmp[i].m_xmf3Vertex.z << endl;
	}
	// ���ۻ���
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, tmp, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	//m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// ���ε����� ���ۺ� �ʱ�ȭ
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();

	delete[] tmp;
}

// -------------------------------- �ͷ��� �� ------------------------------------


CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	BYTE* pHeightMapPixels = new BYTE[m_nWidth * m_nLength]{};

	//������ ���� �д´�. ���� �� �̹����� ���� ����� ���� RAW �̹����̴�. 
	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);

	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];

	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapPixels[x + ((m_nLength - 1 - y) * m_nWidth)] = pHeightMapPixels[x + (y * m_nWidth)];
		}
	}
	if (pHeightMapPixels) delete[] pHeightMapPixels;
}

CHeightMapImage::~CHeightMapImage(void)
{
	if (m_pHeightMapPixels) delete[] m_pHeightMapPixels;
	m_pHeightMapPixels = NULL;
}


#define _WITH_APPROXIMATE_OPPOSITE_CORNER
float CHeightMapImage::GetHeight(float fx, float fz)
{

	/*������ ��ǥ (fx, fz)�� �̹��� ��ǥ���̴�.
	���� ���� x-��ǥ�� z-��ǥ�� ���� ���� ������ ����� ������ ���̴�0�̴�.*/
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);



	//���� ���� ��ǥ�� ���� �κа� �Ҽ� �κ��� ����Ѵ�. 
	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)m_pHeightMapPixels[x + (z * m_nWidth)];
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z * m_nWidth)];

	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1) * m_nWidth)];
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1) * m_nWidth)];


#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	//z-��ǥ�� 1, 3, 5, ...�� ��� �ε����� �����ʿ��� �������� �����ȴ�. 
	bool bRightToLeft = ((z % 2) != 0);

	if (bRightToLeft)
	{
		/*������ �ﰢ������ �����ʿ��� ���� �������� �����Ǵ� ����̴�. ���� �׸��� �������� (fzPercent < fxPercent)�� ����̴�.
		�� ��� TopLeft�� �ȼ� ���� (fTopLeft = fTopRight + (fBottomLeft - fBottomRight))�� �ٻ��Ѵ�.
		���� �׸��� ������ (fzPercent �� fxPercent)�� ����̴�.
		�� ��� BottomRight�� �ȼ� ���� (fBottomRight = fBottomLeft + (fTopRight - fTopLeft))�� �ٻ��Ѵ�.*/

		// �ﰢ���� �����ʿ��� �������� �����Ǵ� ���
		if (fzPercent >= fxPercent) fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}

	else
	{
		/*������ �ﰢ������ ���ʿ��� ������ �������� �����Ǵ� ����̴�.
		���� �׸��� ������ (fzPercent < (1.0f - fxPercent))�� ����̴�.
		�� ��� TopRight�� �ȼ� ���� (fTopRight = fTopLeft + (fBottomRight - fBottomLeft))�� �ٻ��Ѵ�.
		���� �׸��� �������� (fzPercent �� (1.0f - fxPercent))�� ����̴�.
		�� ��� BottomLeft�� �ȼ� ���� (fBottomLeft = fTopLeft + (fBottomRight - fTopRight))�� �ٻ��Ѵ�.*/

		//�ﰢ���� ���ʿ��� ���������� �����Ǵ� ���
		if (fzPercent < (1.0f - fxPercent)) fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}

#endif
	//�簢���� �� ���� �����Ͽ� ����(�ȼ� ��)�� ����Ѵ�. 
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;
	return(fHeight);

}

XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	//x-��ǥ�� z-��ǥ�� ���� ���� ������ ����� ������ ���� ���ʹ� y-�� ���� �����̴�. 
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength))
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	/*���� �ʿ��� (x, z) ��ǥ�� �ȼ� ���� ������ �� ���� �� (x+1, z), (z, z+1)�� ���� �ȼ� ���� ����Ͽ� ���� ���͸� ����Ѵ�.*/
	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;

	//(x, z), (x+1, z), (z, z+1)�� �ȼ����� ������ ���̸� ���Ѵ�. 
	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;

	//xmf3Edge1�� (0, y3, m_xmf3Scale.z) - (0, y1, 0) �����̴�. 
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);

	//xmf3Edge2�� (m_xmf3Scale.x, y2, 0) - (0, y1, 0) �����̴�. 
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);

	//���� ���ʹ� xmf3Edge1�� xmf3Edge2�� ������ ����ȭ�ϸ� �ȴ�. 
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);
	return(xmf3Normal);
}

CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color, void* pContext) : CMesh(pd3dDevice, pd3dCommandList)
{
	//������ ����(����)�� ������ (nWidth * nLength)�̴�. 
	m_nVertices = nWidth * nLength;
	m_nStride = sizeof(Vertex_Color);

	//���ڴ� �ﰢ�� ��Ʈ������ �����Ѵ�. 
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;
	Vertex_Color* pVertices = new Vertex_Color[m_nVertices];

	/*xStart�� zStart�� ������ ���� ��ġ(x-��ǥ�� z-��ǥ)�� ��Ÿ����.
	Ŀ�ٶ� ������ ���ڵ��� ������ �迭�� ���� �ʿ䰡 �ֱ� ������
	��ü �������� �� ������ ���� ��ġ�� ��Ÿ���� ������ �ʿ��ϴ�.*/

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			//������ ���̿� ������ ���� �����κ��� ���Ѵ�. 
			XMFLOAT3 xmf3Position = XMFLOAT3((x * m_xmf3Scale.x), OnGetHeight(x, z, pContext), (z * m_xmf3Scale.z));
			XMFLOAT4 get_color = OnGetColor(x, z, pContext);
			XMFLOAT4 xmf3Color = Vector4::Add(get_color, xmf4Color);

			pVertices[i] = Vertex_Color(xmf3Position, xmf3Color);
			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	delete[] pVertices;


	// ----------------------------------------------------------�ε��� ����--------------------------------------------------------------------------------

	m_nIndices = ((nWidth * 2) * (nLength - 1)) + ((nLength - 1) - 1);
	UINT* pnIndices = new UINT[m_nIndices];

	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			//Ȧ�� ��° ���̹Ƿ�(z = 0, 2, 4, ...) �ε����� ���� ������ ���ʿ��� ������ �����̴�. 
			for (int x = 0; x < nWidth; x++)
			{
				//ù ��° ���� �����ϰ� ���� �ٲ� ������(x == 0) ù ��° �ε����� �߰��Ѵ�.
				if ((x == 0) && (z > 0)) pnIndices[j++] = (UINT)(x + (z * nWidth));

				//�Ʒ�(x, z), ��(x, z+1)�� ������ �ε����� �߰��Ѵ�. 
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			//¦�� ��° ���̹Ƿ�(z = 1, 3, 5, ...) �ε����� ���� ������ �����ʿ��� ���� �����̴�. 
			for (int x = nWidth - 1; x >= 0; x--)
			{
				//���� �ٲ� ������(x == (nWidth-1)) ù ��° �ε����� �߰��Ѵ�. 
				if (x == (nWidth - 1)) pnIndices[j++] = (UINT)(x + (z * nWidth));
				//�Ʒ�(x, z), ��(x, z+1)�� ������ �ε����� �߰��Ѵ�. 
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	delete[] pnIndices;

}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

float CHeightMapGridMesh::OnGetHeight(int x, int z, void* pContext)
{
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	BYTE* pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	int nWidth = pHeightMapImage->GetHeightMapWidth(); float fHeight = pHeightMapPixels[x + (z * nWidth)] * xmf3Scale.y;
	return(fHeight);
}

XMFLOAT4 CHeightMapGridMesh::OnGetColor(int x, int z, void* pContext)
{
	//������ ���� ����(�������� ��������� ����)�̴�. 
	XMFLOAT3 xmf3LightDirection = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	xmf3LightDirection = Vector3::Normalize(xmf3LightDirection);

	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();

	//������ ����(����, ���)�̴�. 
	XMFLOAT4 xmf4IncidentLightColor(0.9f, 0.8f, 0.4f, 1.0f);

	/*���� (x, z)���� ������ �ݻ�Ǵ� ��(����)�� ���� (x, z)�� ���� ���Ϳ� ������ ���� ������ ����(cos)��
	������ 3���� ���� (x+1, z), (x, z+1), (x+1, z+1)�� ���� ���Ϳ� ������ ���� ������ ������ ����Ͽ� ���Ѵ�.
	���� (x, z)�� ������ ���� ����(����)�� �ݻ�Ǵ� ��(����)�� ���� ���̴�.*/

	//������ ����(������ ���� ���Ϳ� ������ ���� ������ ����)

	XMFLOAT3 height_map_normal = pHeightMapImage->GetHeightMapNormal(x, z);
	float fScale = Vector3::DotProduct(height_map_normal, xmf3LightDirection);

	height_map_normal = pHeightMapImage->GetHeightMapNormal(x + 1, z);
	fScale += Vector3::DotProduct(height_map_normal,xmf3LightDirection);

	height_map_normal = pHeightMapImage->GetHeightMapNormal(x + 1, z+1);
	fScale += Vector3::DotProduct(height_map_normal,xmf3LightDirection);

	height_map_normal = pHeightMapImage->GetHeightMapNormal(x, z+1);
	fScale += Vector3::DotProduct(height_map_normal,xmf3LightDirection);
	
	fScale = (fScale / 4.0f) + 0.05f;
	if (fScale > 1.0f) fScale = 1.0f;
	if (fScale < 0.25f) fScale = 0.25f;

	//fScale�� ���� ����(���)�� �ݻ�Ǵ� �����̴�. 
	XMFLOAT4 xmf4Color = Vector4::Multiply(fScale, xmf4IncidentLightColor);
	return(xmf4Color);
}

CRayMesh::CRayMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CRay* ray) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 2;				// ������ ����
	m_nStride = sizeof(CVertex); // x , y, z ��ǥ
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;	// ����

	XMFLOAT3 Ori = { 0, 0, 0 };
	XMFLOAT3 Dir = { 0, 0, 1 };
	if (ray)
	{
		 Ori = ray->GetOriginal();
		 Dir = ray->GetDir();
	}
	XMFLOAT3 End = Vector3::Add(Ori, Vector3::ScalarProduct(Dir, 5000, false));
	

	m_pVertices = new CVertex[m_nVertices];
	m_pVertices[0] = CVertex(XMFLOAT3(Ori.x, Ori.y, Ori.z), XMFLOAT4(0.f, 1.f, 0.0f, 1.0f));
	m_pVertices[1] = CVertex(XMFLOAT3(End.x, End.y, End.z), XMFLOAT4(1.f, 0.f, 0.0f, 1.0f));

	// ���ۻ���
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// ���ε����� ���ۺ� �ʱ�ȭ
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 2;
	UINT pnIndices[2];
	// ����
	pnIndices[0] = 0;
	// ��
	pnIndices[1] = 1;

	//�ε��� ���۸� �����Ѵ�. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//�ε��� ���� �並 �����Ѵ�. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CRayMesh::~CRayMesh()
{
}

CCubeMeshTextured::CCubeMeshTextured(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 36;
	m_nStride = sizeof(CTexturedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	CTexturedVertex pVertices[36];
	int i = 0;

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = CTexturedVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshTextured::~CCubeMeshTextured()
{
}
