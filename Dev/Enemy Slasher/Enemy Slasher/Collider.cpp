#include "stdafx.h"
#include "Collider.h"

CCollider::CCollider()
{
}

CCollider::CCollider(CVertex* pVertices, UINT nVertices)
{
}

CCollider::~CCollider()
{
}


CAABBCollider::CAABBCollider()
{
	m_xmf3MaxPos = XMFLOAT3(FLT_MIN, FLT_MIN, FLT_MIN);
	m_xmf3MinPos = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
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

void CAABBCollider::UpdateColliderWithAABB(CAABBCollider* pOtherCollider, XMFLOAT4X4& xmf4x4WorldMat)
{
	XMFLOAT3 xmf3NewVertex[8];
	XMFLOAT3 xmf3OtherMaxPos = pOtherCollider->GetAABBMaxPos();
	XMFLOAT3 xmf3OtherMinPos = pOtherCollider->GetAABBMinPos();

	xmf3NewVertex[0] = XMFLOAT3(xmf3OtherMaxPos.x, xmf3OtherMaxPos.y, xmf3OtherMaxPos.z);
	xmf3NewVertex[1] = XMFLOAT3(xmf3OtherMaxPos.x, xmf3OtherMaxPos.y, xmf3OtherMinPos.z);
	xmf3NewVertex[2] = XMFLOAT3(xmf3OtherMaxPos.x, xmf3OtherMinPos.y, xmf3OtherMaxPos.z);
	xmf3NewVertex[3] = XMFLOAT3(xmf3OtherMinPos.x, xmf3OtherMaxPos.y, xmf3OtherMaxPos.z);
	xmf3NewVertex[4] = XMFLOAT3(xmf3OtherMaxPos.x, xmf3OtherMinPos.y, xmf3OtherMinPos.z);
	xmf3NewVertex[5] = XMFLOAT3(xmf3OtherMinPos.x, xmf3OtherMinPos.y, xmf3OtherMaxPos.z);
	xmf3NewVertex[6] = XMFLOAT3(xmf3OtherMinPos.x, xmf3OtherMaxPos.y, xmf3OtherMinPos.z);
	xmf3NewVertex[7] = XMFLOAT3(xmf3OtherMinPos.x, xmf3OtherMinPos.y, xmf3OtherMinPos.z);

	for (int i = 0; i < 8; ++i) {
		XMStoreFloat3(&xmf3NewVertex[i], XMVector3TransformCoord(XMLoadFloat3(&xmf3NewVertex[i]), XMLoadFloat4x4(&xmf4x4WorldMat)));
		if (xmf3NewVertex[i].x < m_xmf3MinPos.x) m_xmf3MinPos.x = xmf3NewVertex[i].x;
		if (xmf3NewVertex[i].y < m_xmf3MinPos.y) m_xmf3MinPos.y = xmf3NewVertex[i].y;
		if (xmf3NewVertex[i].z < m_xmf3MinPos.z) m_xmf3MinPos.z = xmf3NewVertex[i].z;
		if (xmf3NewVertex[i].x > m_xmf3MaxPos.x) m_xmf3MaxPos.x = xmf3NewVertex[i].x;
		if (xmf3NewVertex[i].y > m_xmf3MaxPos.y) m_xmf3MaxPos.y = xmf3NewVertex[i].y;
		if (xmf3NewVertex[i].z > m_xmf3MaxPos.z) m_xmf3MaxPos.z = xmf3NewVertex[i].z;
	}
}


COBBCollider::COBBCollider()
{
	MakeOBBData();
}

COBBCollider::COBBCollider(CVertex* pVertices, UINT nVertices) : CAABBCollider(pVertices, nVertices)
{
	MakeCollider(pVertices, nVertices);
}

COBBCollider::~COBBCollider()
{
}

void COBBCollider::MakeOBBData()
{
	m_xmf3Center = XMFLOAT3(
		(m_xmf3MaxPos.x + m_xmf3MinPos.x) / 2,
		(m_xmf3MaxPos.y + m_xmf3MinPos.y) / 2,
		(m_xmf3MaxPos.z + m_xmf3MinPos.z) / 2
	);

	m_xmf3HalfExtents = XMFLOAT3(
		(m_xmf3MaxPos.x - m_xmf3MinPos.x) / 2,
		(m_xmf3MaxPos.y - m_xmf3MinPos.y) / 2,
		(m_xmf3MaxPos.z - m_xmf3MinPos.z) / 2
	);

	m_xmf3Orientation[0] = XMFLOAT3(m_xmf3MaxPos.x - m_xmf3Center.x, 0.0f, 0.0f);
	m_xmf3Orientation[1] = XMFLOAT3(0.0f, m_xmf3MaxPos.y - m_xmf3Center.y, 0.0f);
	m_xmf3Orientation[2] = XMFLOAT3(0.0f, 0.0f, m_xmf3MaxPos.z - m_xmf3Center.z);

	for (int i = 0; i < 3; ++i) m_xmf3Orientation[i] = Vector3::Normalize(m_xmf3Orientation[i]);
}

void COBBCollider::MakeCollider(CVertex* pVertices, UINT nVertices)
{
	CAABBCollider::MakeCollider(pVertices, nVertices);

	MakeOBBData();
}

void COBBCollider::UpdateColliderWithOBB(COBBCollider* pOtherCollider, XMFLOAT4X4& xmf4x4WorldMat)
{
	CAABBCollider::UpdateColliderWithAABB((CAABBCollider*)pOtherCollider, xmf4x4WorldMat);
	
	MakeOBBData();
}

bool CollisionCheck(CAABBCollider* pACollider, XMFLOAT4X4& AWorldMat, CAABBCollider* pBCollider, XMFLOAT4X4& BWorldMat)
{
	XMFLOAT3 aMax = pACollider->GetAABBMaxPos();
	XMFLOAT3 aMin = pACollider->GetAABBMinPos();
	XMFLOAT3 bMax = pBCollider->GetAABBMaxPos();
	XMFLOAT3 bMin = pBCollider->GetAABBMinPos();
	XMStoreFloat3(&aMax, XMVector3TransformCoord(XMLoadFloat3(&aMax), XMLoadFloat4x4(&AWorldMat)));
	XMStoreFloat3(&aMin, XMVector3TransformCoord(XMLoadFloat3(&aMin), XMLoadFloat4x4(&AWorldMat)));
	XMStoreFloat3(&bMax, XMVector3TransformCoord(XMLoadFloat3(&bMax), XMLoadFloat4x4(&BWorldMat)));
	XMStoreFloat3(&bMin, XMVector3TransformCoord(XMLoadFloat3(&bMin), XMLoadFloat4x4(&BWorldMat)));

	if (aMax.x < bMin.x || aMin.x > bMax.x) return false;
	if (aMax.y < bMin.y || aMin.y > bMax.y) return false;
	if (aMax.z < bMin.z || aMin.z > bMax.z) return false;

	return true;
}

bool CollisionCheck(COBBCollider* pACollider, XMFLOAT4X4& AWorldMat, COBBCollider* pBCollider, XMFLOAT4X4& BWorldMat)
{
	//return CollisionCheck((CAABBCollider*)pACollider, AWorldMat, (CAABBCollider*)pBCollider, BWorldMat);

	XMFLOAT3 aCenter = pACollider->GetOBBCenter();
	XMFLOAT3 aHalfExtents = pACollider->GetOBBHalfExtents();
	XMFLOAT3 aOrientation[3];
	XMFLOAT3 bCenter = pBCollider->GetOBBCenter();
	XMFLOAT3 bHalfExtents = pBCollider->GetOBBHalfExtents();
	XMFLOAT3 bOrientation[3];
	XMFLOAT3 OriginPoint(0.0f, 0.0f, 0.0f);

	XMStoreFloat3(&aCenter, XMVector3TransformCoord(XMLoadFloat3(&aCenter), XMLoadFloat4x4(&AWorldMat)));
	XMStoreFloat3(&bCenter, XMVector3TransformCoord(XMLoadFloat3(&bCenter), XMLoadFloat4x4(&BWorldMat)));
	for (int i = 0; i < 3; ++i) {
		OriginPoint = XMFLOAT3(0.0f, 0.0f, 0.0f);
		aOrientation[i] = pACollider->GetOBBOrientation()[i];
		XMStoreFloat3(&aOrientation[i], XMVector3TransformCoord(XMLoadFloat3(&aOrientation[i]), XMLoadFloat4x4(&AWorldMat)));
		XMStoreFloat3(&OriginPoint, XMVector3TransformCoord(XMLoadFloat3(&OriginPoint), XMLoadFloat4x4(&AWorldMat)));
		aOrientation[i] = Vector3::Subtract(aOrientation[i], OriginPoint);
		aOrientation[i] = Vector3::Normalize(aOrientation[i]);

		OriginPoint = XMFLOAT3(0.0f, 0.0f, 0.0f);
		bOrientation[i] = pBCollider->GetOBBOrientation()[i];
		XMStoreFloat3(&bOrientation[i], XMVector3TransformCoord(XMLoadFloat3(&bOrientation[i]), XMLoadFloat4x4(&BWorldMat)));
		XMStoreFloat3(&OriginPoint, XMVector3TransformCoord(XMLoadFloat3(&OriginPoint), XMLoadFloat4x4(&BWorldMat)));
		bOrientation[i] = Vector3::Subtract(bOrientation[i], OriginPoint);
		bOrientation[i] = Vector3::Normalize(bOrientation[i]);
	}

	// 각 축에 대한 겹치는지 여부 확인
	for (int i = 0; i < 3; ++i) {
		float t = std::abs((&bCenter.x)[i] -
						   (&aCenter.x)[i]);

		float extent1 = aHalfExtents.x * std::abs(aOrientation[i].x) +
						aHalfExtents.y * std::abs(aOrientation[i].y) +
						aHalfExtents.z * std::abs(aOrientation[i].z);

		float extent2 = bHalfExtents.x * std::abs(bOrientation[i].x) +
						bHalfExtents.y * std::abs(bOrientation[i].y) +
						bHalfExtents.z * std::abs(bOrientation[i].z);

		if (t > extent1 + extent2)
			return false;  // 축에 따라 겹치지 않음
	}

	return true;  // 모든 축에 대해 겹침
}
