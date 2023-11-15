#include "stdafx.h"
#include "Collider.h"

CCollider::CCollider(CVertex* pVertices, UINT nVertices)
{
}

CCollider::~CCollider()
{
}


CAABBCollider::CAABBCollider(CVertex* pVertices, UINT nVertices) : CCollider(pVertices, nVertices)
{
	MakeCollider(pVertices, nVertices);
}

CAABBCollider::~CAABBCollider()
{
}

void CAABBCollider::MakeCollider(CVertex* pVertices, UINT nVertices)
{
	float min_x = FLT_MAX, max_x = FLT_MIN,
		min_y = FLT_MAX, max_y = FLT_MIN,
		min_z = FLT_MAX, max_z = FLT_MIN;

	XMFLOAT3 vertex;
	for (int i = 0; i < nVertices; ++i) {
		vertex = pVertices[i].m_xmf3Vertex;
		if (vertex.x < min_x) min_x = vertex.x;
		if (vertex.y < min_y) min_y = vertex.y;
		if (vertex.z < min_z) min_z = vertex.z;
		if (vertex.x > max_x) max_x = vertex.x;
		if (vertex.y > max_y) max_y = vertex.y;
		if (vertex.z > max_z) max_z = vertex.z;
	}

	m_xmf3MaxPos = XMFLOAT3(max_x, max_y, max_z);
	m_xmf3MinPos = XMFLOAT3(min_x, min_y, min_z);
}


CColliderWithMesh::CColliderWithMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CVertex* pVertices, UINT nVertices)
{

}

CColliderWithMesh::~CColliderWithMesh()
{
	if (m_pVertices) delete[] m_pVertices;
	if (m_pnIndices) delete[] m_pnIndices;
}

void CColliderWithMesh::ReleaseUploadBuffers()
{
	//AABB�� ���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��. 
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
}

void CColliderWithMesh::RenderCollider(ID3D12GraphicsCommandList* pd3dCommandList)
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



CAABBColliderWithMesh::CAABBColliderWithMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CVertex* pVertices, UINT nVertices) : CColliderWithMesh(pd3dDevice, pd3dCommandList, pVertices, nVertices)
{
	MakeCollider(pd3dDevice, pd3dCommandList, pVertices, nVertices);
}

CAABBColliderWithMesh::~CAABBColliderWithMesh()
{
	if (m_pAABBColider) delete m_pAABBColider;
}

void CAABBColliderWithMesh::MakeCollider(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CVertex* pVertices, UINT nVertices)
{
	if (m_pVertices) { // �̹� AABB�� �ִٸ� ����� �ٽ� �����.
		delete[] m_pVertices;

		if (m_pnIndices) delete[] m_pnIndices;

		if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
		m_pd3dVertexBuffer = NULL;

		if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
		m_pd3dIndexBuffer = NULL;

		if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
		m_pd3dVertexUploadBuffer = NULL;

		if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
		m_pd3dIndexUploadBuffer = NULL;
	}

	m_nVertices = 8;			 // ������ ����
	m_nStride = sizeof(CVertex); // x , y, z ��ǥ
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	float min_x = FLT_MAX, max_x = FLT_MIN,
		min_y = FLT_MAX, max_y = FLT_MIN,
		min_z = FLT_MAX, max_z = FLT_MIN;

	XMFLOAT3 vertex;
	for (int i = 0; i < nVertices; ++i) {
		vertex = pVertices[i].m_xmf3Vertex;
		if (vertex.x < min_x) min_x = vertex.x;
		if (vertex.y < min_y) min_y = vertex.y;
		if (vertex.z < min_z) min_z = vertex.z;
		if (vertex.x > max_x) max_x = vertex.x;
		if (vertex.y > max_y) max_y = vertex.y;
		if (vertex.z > max_z) max_z = vertex.z;
	}


	CVertex AABBVertex[2];
	AABBVertex[0].m_xmf3Vertex = XMFLOAT3(max_x, max_y, max_z);
	AABBVertex[1].m_xmf3Vertex = XMFLOAT3(min_x, min_y, min_z);
	if (!m_pAABBColider) m_pAABBColider = new CAABBCollider(AABBVertex, 2);
	else m_pAABBColider->MakeCollider(AABBVertex, 2);

	m_pVertices = new CVertex[m_nVertices];
	m_pVertices[0] = CVertex(XMFLOAT3(min_x, max_y, min_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[1] = CVertex(XMFLOAT3(max_x, max_y, min_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[2] = CVertex(XMFLOAT3(max_x, max_y, max_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[3] = CVertex(XMFLOAT3(min_x, max_y, max_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[4] = CVertex(XMFLOAT3(min_x, min_y, min_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[5] = CVertex(XMFLOAT3(max_x, min_y, min_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[6] = CVertex(XMFLOAT3(max_x, min_y, max_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[7] = CVertex(XMFLOAT3(min_x, min_y, max_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

	// ���ۻ���
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// ���ε����� ���ۺ� �ʱ�ȭ
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 24;
	int Indexer = 0;
	m_pnIndices = new UINT[m_nIndices];
	// ����
	m_pnIndices[Indexer++] = 0; m_pnIndices[Indexer++] = 1;
	m_pnIndices[Indexer++] = 1; m_pnIndices[Indexer++] = 2;
	m_pnIndices[Indexer++] = 2; m_pnIndices[Indexer++] = 3;
	m_pnIndices[Indexer++] = 3; m_pnIndices[Indexer++] = 0;

	// �Ʒ���
	m_pnIndices[Indexer++] = 4; m_pnIndices[Indexer++] = 5;
	m_pnIndices[Indexer++] = 5; m_pnIndices[Indexer++] = 6;
	m_pnIndices[Indexer++] = 6; m_pnIndices[Indexer++] = 7;
	m_pnIndices[Indexer++] = 7; m_pnIndices[Indexer++] = 4;

	// ���
	m_pnIndices[Indexer++] = 0; m_pnIndices[Indexer++] = 4;
	m_pnIndices[Indexer++] = 1; m_pnIndices[Indexer++] = 5;
	m_pnIndices[Indexer++] = 2; m_pnIndices[Indexer++] = 6;
	m_pnIndices[Indexer++] = 3; m_pnIndices[Indexer++] = 7;


	//�ε��� ���۸� �����Ѵ�. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//�ε��� ���� �並 �����Ѵ�. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}