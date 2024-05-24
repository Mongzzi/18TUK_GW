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
	XMFLOAT3 GetOriginal() { return m_vOriginal; };
	XMFLOAT3 GetDir() { return m_xmf3Dir; };

	void SetOrigin(XMFLOAT3 xmf3Origin) { m_vOriginal = xmf3Origin; }
	void SetDir(XMFLOAT3 xmf3Dir) { m_xmf3Dir = xmf3Dir; }
private:

protected:
	XMFLOAT3 m_vOriginal;
	XMFLOAT3 m_xmf3Dir;	// CCamera::m_xmf3Look.  GetLookVector()

};

