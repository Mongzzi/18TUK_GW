#pragma once
#include "Vertex.h"

class CCollider
{
public:
	CCollider(CVertex* pVertices, UINT nVertices);
	virtual ~CCollider();

protected:

public:
	virtual void MakeCollider(CVertex* pVertices, UINT nVertices) = 0;
};

class CAABBCollider : public CCollider
{
public:
	CAABBCollider(CVertex* pVertices, UINT nVertices);
	virtual ~CAABBCollider();

protected:
	XMFLOAT3 m_xmf3MinPos;
	XMFLOAT3 m_xmf3MaxPos;

public:
	virtual void MakeCollider(CVertex* pVertices, UINT nVertices);

	XMFLOAT3 GetAABBMinPos() { return m_xmf3MinPos; }
	XMFLOAT3 GetAABBMaxPos() { return m_xmf3MaxPos; }
};




class CColliderWithMesh
{
public:
	CColliderWithMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CVertex* pVertices, UINT nVertices);
	virtual ~CColliderWithMesh();

public:
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

	UINT* m_pnIndices = NULL;

	UINT m_nIndices = 0;	// 인덱스 버퍼에 포함되는 인덱스의 개수
	UINT m_nStartIndex = 0; // 인덱스 버퍼에서 메쉬를 그리기 위해 사용되는 시작 인덱스
	int m_nBaseVertex = 0;  // 인덱스 버퍼의 인덱스에 더해질 인덱스

public:
	virtual void MakeCollider(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CVertex* pVertices, UINT nVertices) = 0;
	virtual void RenderCollider(ID3D12GraphicsCommandList* pd3dCommandList);
};

class CAABBColliderWithMesh : public CColliderWithMesh
{
public:
	CAABBColliderWithMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CVertex* pVertices, UINT nVertices);
	virtual ~CAABBColliderWithMesh();

protected:
	CAABBCollider* m_pAABBColider;

public:
	virtual void MakeCollider(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CVertex* pVertices, UINT nVertices);

	XMFLOAT3 GetAABBMinPos() { return m_pAABBColider->GetAABBMinPos(); }
	XMFLOAT3 GetAABBMaxPos() { return m_pAABBColider->GetAABBMaxPos(); }
};