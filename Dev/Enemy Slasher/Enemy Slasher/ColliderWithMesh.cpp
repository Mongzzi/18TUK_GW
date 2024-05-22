#include "stdafx.h"
#include "ColliderWithMesh.h"

CColliderWithMesh::CColliderWithMesh()
{
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
	//AABB의 정점 버퍼를 위한 업로드 버퍼를 소멸시킨다. 
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
}

void CColliderWithMesh::RenderCollider(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//메쉬의 프리미티브 유형을 설정한다. 
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	//메쉬의 정점 버퍼 뷰를 설정한다. 
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다. 
	}
	else
	{
		//메쉬의 정점 버퍼 뷰를 렌더링한다(파이프라인(입력 조립기)을 작동하게 한다).
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}



CAABBColliderWithMesh::CAABBColliderWithMesh()
{
	m_pAABBColider = new CAABBCollider;
}

CAABBColliderWithMesh::CAABBColliderWithMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CVertex* pVertices, UINT nVertices, UINT nStride) : CColliderWithMesh(pd3dDevice, pd3dCommandList, pVertices, nVertices)
{
	MakeCollider(pd3dDevice, pd3dCommandList, pVertices, nVertices, nStride);
}

CAABBColliderWithMesh::~CAABBColliderWithMesh()
{
	if (m_pAABBColider) delete m_pAABBColider;
}

void CAABBColliderWithMesh::MakeCollider(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CVertex* pVertices, UINT nVertices, UINT nStride)
{
	if (m_pVertices) { // 이미 AABB가 있다면 지우고 다시 만든다.
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

	m_nVertices = 8;			 // 꼭지점 개수
	m_nStride = sizeof(CVertex); // x , y, z 좌표
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

	// 버퍼생성
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// 바인딩위해 버퍼뷰 초기화
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 24;
	int Indexer = 0;
	m_pnIndices = new UINT[m_nIndices];
	// 윗면
	m_pnIndices[Indexer++] = 0; m_pnIndices[Indexer++] = 1;
	m_pnIndices[Indexer++] = 1; m_pnIndices[Indexer++] = 2;
	m_pnIndices[Indexer++] = 2; m_pnIndices[Indexer++] = 3;
	m_pnIndices[Indexer++] = 3; m_pnIndices[Indexer++] = 0;

	// 아랫면
	m_pnIndices[Indexer++] = 4; m_pnIndices[Indexer++] = 5;
	m_pnIndices[Indexer++] = 5; m_pnIndices[Indexer++] = 6;
	m_pnIndices[Indexer++] = 6; m_pnIndices[Indexer++] = 7;
	m_pnIndices[Indexer++] = 7; m_pnIndices[Indexer++] = 4;

	// 기둥
	m_pnIndices[Indexer++] = 0; m_pnIndices[Indexer++] = 4;
	m_pnIndices[Indexer++] = 1; m_pnIndices[Indexer++] = 5;
	m_pnIndices[Indexer++] = 2; m_pnIndices[Indexer++] = 6;
	m_pnIndices[Indexer++] = 3; m_pnIndices[Indexer++] = 7;


	//인덱스 버퍼를 생성한다. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//인덱스 버퍼 뷰를 생성한다. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

void CAABBColliderWithMesh::UpdateColliderWithAABB(CAABBColliderWithMesh* pOtherCollider, XMFLOAT4X4& xmf4x4WorldMat)
{
	m_pAABBColider->UpdateColliderWithAABB(pOtherCollider->GetCollider(), xmf4x4WorldMat);
}

void CAABBColliderWithMesh::UpdateColliderWithAABB(CAABBCollider* pOtherCollider, XMFLOAT4X4& xmf4x4WorldMat)
{
	m_pAABBColider->UpdateColliderWithAABB(pOtherCollider, xmf4x4WorldMat);
}

COBBColliderWithMesh::COBBColliderWithMesh()
{
	m_pOBBCollider = new COBBCollider;
}

COBBColliderWithMesh::COBBColliderWithMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CVertex* pVertices, UINT nVertices, UINT nStride) : CColliderWithMesh(pd3dDevice, pd3dCommandList, pVertices, nVertices)
{
	MakeCollider(pd3dDevice, pd3dCommandList, pVertices, nVertices, nStride);
}

COBBColliderWithMesh::~COBBColliderWithMesh()
{
	if (m_pOBBCollider) delete m_pOBBCollider;
}

void COBBColliderWithMesh::MakeCollider(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CVertex* pVertices, UINT nVertices, UINT nStride)
{
	if (m_pVertices) { // 이미 OBB가 있다면 지우고 다시 만든다.
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

	m_nVertices = 8;			 // 꼭지점 개수
	m_nStride = sizeof(CVertex); // x , y, z 좌표
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	float min_x = FLT_MAX, max_x = FLT_MIN,
		min_y = FLT_MAX, max_y = FLT_MIN,
		min_z = FLT_MAX, max_z = FLT_MIN;

	if (nStride == sizeof(CVertex_Skining)) {
		m_nStride = nStride;
	}

	XMFLOAT3 vertex;
	char* bytePointer = reinterpret_cast<char*>(pVertices);
	for (int i = 0; i < nVertices; ++i) {
		// CVertex 는 상속구조이므로 pointer로 vertex 접근
		vertex = ((CVertex*)bytePointer)->m_xmf3Vertex;
		if (vertex.x < min_x) min_x = vertex.x;
		if (vertex.y < min_y) min_y = vertex.y;
		if (vertex.z < min_z) min_z = vertex.z;
		if (vertex.x > max_x) max_x = vertex.x;
		if (vertex.y > max_y) max_y = vertex.y;
		if (vertex.z > max_z) max_z = vertex.z;

		bytePointer += m_nStride;
	}

	CVertex OBBVertex[2];
	OBBVertex[0].m_xmf3Vertex = XMFLOAT3(max_x, max_y, max_z);
	OBBVertex[1].m_xmf3Vertex = XMFLOAT3(min_x, min_y, min_z);
	if (!m_pOBBCollider) m_pOBBCollider = new COBBCollider(OBBVertex, 2);
	else m_pOBBCollider->MakeCollider(OBBVertex, 2);

	m_pVertices = new CVertex[m_nVertices];
	m_pVertices[0] = CVertex(XMFLOAT3(min_x, max_y, min_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[1] = CVertex(XMFLOAT3(max_x, max_y, min_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[2] = CVertex(XMFLOAT3(max_x, max_y, max_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[3] = CVertex(XMFLOAT3(min_x, max_y, max_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[4] = CVertex(XMFLOAT3(min_x, min_y, min_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[5] = CVertex(XMFLOAT3(max_x, min_y, min_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[6] = CVertex(XMFLOAT3(max_x, min_y, max_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pVertices[7] = CVertex(XMFLOAT3(min_x, min_y, max_z), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

	// 버퍼생성
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	// 바인딩위해 버퍼뷰 초기화
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 24;
	int Indexer = 0;
	m_pnIndices = new UINT[m_nIndices];
	// 윗면
	m_pnIndices[Indexer++] = 0; m_pnIndices[Indexer++] = 1;
	m_pnIndices[Indexer++] = 1; m_pnIndices[Indexer++] = 2;
	m_pnIndices[Indexer++] = 2; m_pnIndices[Indexer++] = 3;
	m_pnIndices[Indexer++] = 3; m_pnIndices[Indexer++] = 0;

	// 아랫면
	m_pnIndices[Indexer++] = 4; m_pnIndices[Indexer++] = 5;
	m_pnIndices[Indexer++] = 5; m_pnIndices[Indexer++] = 6;
	m_pnIndices[Indexer++] = 6; m_pnIndices[Indexer++] = 7;
	m_pnIndices[Indexer++] = 7; m_pnIndices[Indexer++] = 4;

	// 기둥
	m_pnIndices[Indexer++] = 0; m_pnIndices[Indexer++] = 4;
	m_pnIndices[Indexer++] = 1; m_pnIndices[Indexer++] = 5;
	m_pnIndices[Indexer++] = 2; m_pnIndices[Indexer++] = 6;
	m_pnIndices[Indexer++] = 3; m_pnIndices[Indexer++] = 7;


	//인덱스 버퍼를 생성한다. 
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	//인덱스 버퍼 뷰를 생성한다. 
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

void COBBColliderWithMesh::UpdateColliderWithOBB(COBBColliderWithMesh* pOtherCollider, XMFLOAT4X4& xmf4x4WorldMat)
{
	m_pOBBCollider->UpdateColliderWithOBB(pOtherCollider->GetCollider(), xmf4x4WorldMat);
}

void COBBColliderWithMesh::UpdateColliderWithOBB(COBBCollider* pOtherCollider, XMFLOAT4X4& xmf4x4WorldMat)
{
	m_pOBBCollider->UpdateColliderWithOBB(pOtherCollider, xmf4x4WorldMat);
}

bool CollisionCheck(CAABBColliderWithMesh* pACollider, XMFLOAT4X4& AWorldMat, CAABBColliderWithMesh* pBCollider, XMFLOAT4X4& BWorldMat)
{
	return CollisionCheck(pACollider->GetCollider(), AWorldMat, pBCollider->GetCollider(), BWorldMat);
}

bool CollisionCheck(COBBColliderWithMesh* pACollider, XMFLOAT4X4& AWorldMat, COBBColliderWithMesh* pBCollider, XMFLOAT4X4& BWorldMat)
{
	return CollisionCheck(pACollider->GetCollider(), AWorldMat, pBCollider->GetCollider(), BWorldMat);
}
