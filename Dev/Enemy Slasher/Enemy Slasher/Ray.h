#pragma once
#include "stdafx.h"
#include "Camera.h"
#include "BoundingBox.h"

class CRay
{
public:
	CRay();
	~CRay();

	CRay RayAtViewSpace(int x, int y, CCamera camera);
	CRay RayAtWorldSpace(int x, int y, CCamera camera);
	bool RayIntersectsTriangle(CRay& ray, XMFLOAT3& vertex1, XMFLOAT3& vertex2, XMFLOAT3& vertex3);
	bool IntersectsAABB(CAABB& aabb);
private:

protected:
	XMFLOAT3 m_vOriginal;
	XMFLOAT3 m_xmf3Dir;	// CCamera::m_xmf3Look.  GetLookVector()

};

