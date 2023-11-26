#pragma once
#include "Vertex.h"

class CCollider
{
public:
	CCollider();
	CCollider(CVertex* pVertices, UINT nVertices);
	virtual ~CCollider();

protected:

public:
	virtual void MakeCollider(CVertex* pVertices, UINT nVertices) = 0;
};

class CAABBCollider : public CCollider
{
public:
	CAABBCollider();
	CAABBCollider(CVertex* pVertices, UINT nVertices);
	virtual ~CAABBCollider();

protected:
	XMFLOAT3 m_xmf3MinPos;
	XMFLOAT3 m_xmf3MaxPos;

public:
	virtual void MakeCollider(CVertex* pVertices, UINT nVertices);
	void UpdateColliderWithAABB(CAABBCollider* pOtherCollider, XMFLOAT4X4& xmf4x4WorldMat);

	XMFLOAT3 GetAABBMinPos() { return m_xmf3MinPos; }
	XMFLOAT3 GetAABBMaxPos() { return m_xmf3MaxPos; }
};

class COBBCollider : public CAABBCollider
{
public:
	COBBCollider();
	COBBCollider(CVertex* pVertices, UINT nVertices);
	virtual ~COBBCollider();

protected:
	XMFLOAT3 m_xmf3Center;
	XMFLOAT3 m_xmf3HalfExtents;
	XMFLOAT3 m_xmf3Orientation[3]; // ∑Œƒ√ √‡

	void MakeOBBData();
public:
	virtual void MakeCollider(CVertex* pVertices, UINT nVertices);
	void UpdateColliderWithOBB(COBBCollider* pOtherCollider, XMFLOAT4X4& xmf4x4WorldMat);

	XMFLOAT3 GetOBBCenter() { return m_xmf3Center; }
	XMFLOAT3 GetOBBHalfExtents() { return m_xmf3HalfExtents; }
	XMFLOAT3* GetOBBOrientation() { return m_xmf3Orientation; }
};

bool CollisionCheck(CAABBCollider* pACollider, XMFLOAT4X4& AWorldMat, CAABBCollider* pBCollider, XMFLOAT4X4& BWorldMat);
bool CollisionCheck(COBBCollider* pACollider, XMFLOAT4X4& AWorldMat, COBBCollider* pBCollider, XMFLOAT4X4& BWorldMat);