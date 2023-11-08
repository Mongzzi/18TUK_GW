#include "Ray.h"

CRay::CRay()
{
}

CRay::~CRay()
{
}

CRay CRay::RayAtWorldSpace(int x, int y, CCamera camera)
{
	CRay r = CRay::RayAtViewSpace(x, y, camera);

	XMFLOAT4X4 view, invView;
	// view에 행렬 저장
	view = camera.GetViewMatrix();
	// invView에 역행렬 저장
	invView = Matrix4x4::Inverse(view);
	// 뷰의 역행렬을 이용해서 광선의 출발점을 변환
	XMMATRIX mat = XMLoadFloat4x4(&invView);
	XMStoreFloat3(&r.m_vOriginal, XMVector3TransformCoord(XMLoadFloat3(&r.m_vOriginal), mat));
	// 뷰의 역행렬을 이용해서 방향벡터를 변환
    XMStoreFloat3(&r.m_xmf3Dir, XMVector3TransformNormal(XMLoadFloat3(&r.m_xmf3Dir), mat));
    auto tmp = r.m_xmf3Dir.z;
    r.m_xmf3Dir.z = r.m_xmf3Dir.x;
    r.m_xmf3Dir.x = -tmp;
	// 방향벡터를 정규화
	r.m_xmf3Dir = Vector3::Normalize(r.m_xmf3Dir);
#ifdef _DEBUG
    //std::cout << "m_vOriginal: " << int(r.m_vOriginal.x) << ", " << int(r.m_vOriginal.y) << ", " << int(r.m_vOriginal.z) << ", " << "m_xmf3Dir: " << r.m_xmf3Dir.x << ", " << r.m_xmf3Dir.y << ", " << r.m_xmf3Dir.z << ", " << std::endl;
#endif // _DEBUG
	return r;
}


CRay CRay::RayAtViewSpace(int x, int y, CCamera camera)
{
	D3D12_VIEWPORT viewPort = camera.GetViewport();

	XMFLOAT4X4 projection = camera.GetProjectionMatrix();

	CRay r;
    // 이거 축이 조 ㅁ이상한데? y, z
	r.m_xmf3Dir.x = ((2.f * x) / viewPort.Width - 1.f) / projection._11;
	r.m_xmf3Dir.y = ((-2.f * y) / viewPort.Height + 1.f) / projection._22;
	r.m_xmf3Dir.z = 1.0;
    //
    r.m_vOriginal.x = ((2.f * x) / viewPort.Width - 1.f);
    r.m_vOriginal.y = ((-2.f * y) / viewPort.Height + 1.f);
    r.m_vOriginal.z = 0;

#ifdef _DEBUG
    /*std::cout << "projection._11: " << projection._11 << std::endl;
    std::cout << "projection._22: " << projection._22 << std::endl;
    std::cout << "m_xmf3Dir: " << r.m_xmf3Dir.x << ", " << r.m_xmf3Dir.y << ", " << r.m_xmf3Dir.z << ", " << std::endl;*/
#endif // _DEBUG
	return r;
}

bool CRay::RayIntersectsTriangle(CRay& ray, XMFLOAT3& vertex1, XMFLOAT3& vertex2, XMFLOAT3& vertex3)
{
    // 삼각형의 노멀 벡터 계산
    XMFLOAT3 edge1 = Vector3::Subtract(vertex2, vertex1);
    XMFLOAT3 edge2 = Vector3::Subtract(vertex3, vertex1);
    XMFLOAT3 normal;
    XMStoreFloat3(&normal, XMVector3Cross(XMLoadFloat3(&edge1), XMLoadFloat3(&edge2)));

    // 레이와 삼각형의 평면이 평행이면 교차하지 않음
    float dotProduct = Vector3::DotProduct(normal, ray.m_xmf3Dir);
    if (fabs(dotProduct) < 1e-6f)
        return false;

    // 레이의 출발점에서 평면까지의 거리 계산
    XMFLOAT3 tmp = Vector3::Subtract(vertex1, ray.m_vOriginal);
    float t = Vector3::DotProduct(tmp, normal) / dotProduct;

    // 레이가 삼각형 뒤에 있으면 교차하지 않음
    if (t < 0)
        return false;

    // 레이의 교차점 계산
    XMFLOAT3 intersectionPoint;
    XMStoreFloat3(&intersectionPoint, XMLoadFloat3(&ray.m_vOriginal) + t * XMLoadFloat3(&ray.m_xmf3Dir));

    // 교차점이 삼각형 내부에 있는지 확인
    XMFLOAT3 edge3 = Vector3::Subtract(vertex1, intersectionPoint);
    XMFLOAT3 edge4 = Vector3::Subtract(vertex2, intersectionPoint);
    XMFLOAT3 cross1 = Vector3::CrossProduct(edge1, edge3);
    XMFLOAT3 cross2 = Vector3::CrossProduct(edge2, edge4);
    float dot1 = Vector3::DotProduct(normal, cross1);
    float dot2 = Vector3::DotProduct(normal, cross2);

    // 교차점이 삼각형 내부에 있으면 교차
    return (dot1 >= 0 && dot2 >= 0 && dot1 + dot2 <= XMVectorGetX(XMVector3Dot(XMLoadFloat3(&edge1), XMLoadFloat3(&edge1))));

}

bool CRay::IntersectsAABB(CAABB& aabb)
{
#ifdef _DEBUG
    //std::cout << "m_vOriginal: " << int(m_vOriginal.x) << ", " << int(m_vOriginal.y) << ", " << int(m_vOriginal.z) << ", " << "m_xmf3Dir: " << m_xmf3Dir.x << ", " << m_xmf3Dir.y << ", " << m_xmf3Dir.z << ", " << std::endl;
#endif // _DEBUG
    XMFLOAT3 center = aabb.GetCenter();
    XMFLOAT3 edgeDist = aabb.GetEdgeDistances();
    XMFLOAT3 aabbMin = Vector3::Subtract(center, edgeDist);
    XMFLOAT3 aabbMax = Vector3::Add(center, edgeDist);

    XMFLOAT3 x(1, 0, 0);
    XMFLOAT3 y(0, 1, 0);
    XMFLOAT3 z(0, 0, 1);
    XMFLOAT3 dist = Vector3::Subtract(center, m_vOriginal);
    
    float s1, s2;
    float dot = Vector3::DotProduct(x, m_xmf3Dir);
    if (dot > FLT_EPSILON)
    {
        s1 = (Vector3::DotProduct(x, dist) + edgeDist.x) / dot;
        s2 = (Vector3::DotProduct(x, dist) - edgeDist.x) / dot;
#ifdef _DEBUG
        //std::cout <<"x: " << s1 << ", " << s2 << std::endl;
#endif // _DEBUG
    }
    else 
    {
#ifdef _DEBUG
        std::cout << "x exit: " << -(Vector3::DotProduct(x, dist)) - edgeDist.x << ", " << -(Vector3::DotProduct(x, dist)) + edgeDist.x << std::endl;
#endif // _DEBUG
        if ((-(Vector3::DotProduct(x, dist)) - edgeDist.x > 0) || (-(Vector3::DotProduct(x, dist)) + edgeDist.x < 0))
            return false;
        else
            return true;
    }
    float s3 =0 , s4=0;
    dot = Vector3::DotProduct(y, m_xmf3Dir);
    if (dot > FLT_EPSILON)
    {
        s3 = (Vector3::DotProduct(y, dist) + edgeDist.y) / dot;
        s4 = (Vector3::DotProduct(y, dist) - edgeDist.y) / dot;
#ifdef _DEBUG
        //std::cout <<"y: " << s3 << ", " << s4 << std::endl;
#endif // _DEBUG
    }
    else
    {
#ifdef _DEBUG
        //std::cout << "y exit: " << -(Vector3::DotProduct(y, dist)) - edgeDist.y << ", " << -(Vector3::DotProduct(y, dist)) + edgeDist.y << std::endl;
#endif // _DEBUG
        if ((-(Vector3::DotProduct(y, dist)) - edgeDist.y > 0) || (-(Vector3::DotProduct(x, dist)) + edgeDist.y < 0))
            return false;
        else
            return true;
    }
    float s5=0, s6=0;
    dot = Vector3::DotProduct(z, m_xmf3Dir);
    if (dot > FLT_EPSILON)
    {
        s5 = (Vector3::DotProduct(z, dist) + edgeDist.z) / dot;
        s6 = (Vector3::DotProduct(z, dist) - edgeDist.z) / dot;
#ifdef _DEBUG
        //std::cout <<"z: " << s5 << ", " << s6 << std::endl;
#endif // _DEBUG
    }
    else
    {
#ifdef _DEBUG
        //std::cout << "z exit: " << -(Vector3::DotProduct(z, dist)) - edgeDist.z << ", " << -(Vector3::DotProduct(z, dist)) + edgeDist.z << std::endl;
#endif // _DEBUG
        if ((-(Vector3::DotProduct(z, dist)) - edgeDist.z > 0) || (-(Vector3::DotProduct(x, dist)) + edgeDist.z < 0))
            return false;
        else
            return true;
    }

    float tmin = max(max(min(s1, s2), min(s3, s4)), min(s5, s6));
    float tmax = min(min(max(s1, s2), max(s3, s4)), max(s5, s6));

    //float t1 = (aabbMin.x - m_vOriginal.x) / m_xmf3Dir.x;
    //float t2 = (aabbMax.x - m_vOriginal.x) / m_xmf3Dir.x;
    //float t3 = (aabbMin.y - m_vOriginal.y) / m_xmf3Dir.y;
    //float t4 = (aabbMax.y - m_vOriginal.y) / m_xmf3Dir.y;
    //float t5 = (aabbMin.z - m_vOriginal.z) / m_xmf3Dir.z;
    //float t6 = (aabbMax.z - m_vOriginal.z) / m_xmf3Dir.z;

    //float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    //float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

#ifdef _DEBUG
    //std::cout<<"inax" << tmin << ", " << tmax << std::endl;
#endif // _DEBUG

    if (tmax < 0) // Ray의 방향이 AABB 뒤에 있는 경우
        return false;

    if (tmin > tmax) // Ray와 AABB가 평행하면서 겹치지 않는 경우
        return false;

    if (tmin < 0) // Ray의 시작점이 AABB 내부에 있는 경우
        return true;

    return true; // Ray와 AABB가 교차하는 경우
}
