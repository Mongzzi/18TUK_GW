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
	// view�� ��� ����
	view = camera.GetViewMatrix();
	// invView�� ����� ����
	invView = Matrix4x4::Inverse(view);
	// ���� ������� �̿��ؼ� ������ ������� ��ȯ
	XMMATRIX mat = XMLoadFloat4x4(&invView);
	XMStoreFloat3(&r.m_vOriginal, XMVector3TransformCoord(XMLoadFloat3(&r.m_vOriginal), mat));
	// ���� ������� �̿��ؼ� ���⺤�͸� ��ȯ
	XMStoreFloat3(&r.m_vOriginal, XMVector3TransformNormal(XMLoadFloat3(&r.m_vOriginal), mat));
	// ���⺤�͸� ����ȭ
	r.m_xmf3Dir = Vector3::Normalize(r.m_xmf3Dir);
	return r;
}


CRay CRay::RayAtViewSpace(int x, int y, CCamera camera)
{
	D3D12_VIEWPORT viewPort = camera.GetViewport();

	XMFLOAT4X4 projection = camera.GetProjectionMatrix();

	CRay r;

	r.m_xmf3Dir.x = ((2.f * x) / viewPort.Width - 1.f) / projection._11;
	r.m_xmf3Dir.y = ((-2.f * y) / viewPort.Height + 1.f) / projection._22;
	r.m_xmf3Dir.z = 1.0;

	return r;
}

bool CRay::RayIntersectsTriangle(CRay& ray, XMFLOAT3& vertex1, XMFLOAT3& vertex2, XMFLOAT3& vertex3)
{
    // �ﰢ���� ��� ���� ���
    XMFLOAT3 edge1 = Vector3::Subtract(vertex2, vertex1);
    XMFLOAT3 edge2 = Vector3::Subtract(vertex3, vertex1);
    XMFLOAT3 normal;
    XMStoreFloat3(&normal, XMVector3Cross(XMLoadFloat3(&edge1), XMLoadFloat3(&edge2)));

    // ���̿� �ﰢ���� ����� �����̸� �������� ����
    float dotProduct = Vector3::DotProduct(normal, ray.m_xmf3Dir);
    if (fabs(dotProduct) < 1e-6f)
        return false;

    // ������ ��������� �������� �Ÿ� ���
    XMFLOAT3 tmp = Vector3::Subtract(vertex1, ray.m_vOriginal);
    float t = Vector3::DotProduct(tmp, normal) / dotProduct;

    // ���̰� �ﰢ�� �ڿ� ������ �������� ����
    if (t < 0)
        return false;

    // ������ ������ ���
    XMFLOAT3 intersectionPoint;
    XMStoreFloat3(&intersectionPoint, XMLoadFloat3(&ray.m_vOriginal) + t * XMLoadFloat3(&ray.m_xmf3Dir));

    // �������� �ﰢ�� ���ο� �ִ��� Ȯ��
    XMFLOAT3 edge3 = Vector3::Subtract(vertex1, intersectionPoint);
    XMFLOAT3 edge4 = Vector3::Subtract(vertex2, intersectionPoint);
    XMFLOAT3 cross1 = Vector3::CrossProduct(edge1, edge3);
    XMFLOAT3 cross2 = Vector3::CrossProduct(edge2, edge4);
    float dot1 = Vector3::DotProduct(normal, cross1);
    float dot2 = Vector3::DotProduct(normal, cross2);

    // �������� �ﰢ�� ���ο� ������ ����
    return (dot1 >= 0 && dot2 >= 0 && dot1 + dot2 <= XMVectorGetX(XMVector3Dot(XMLoadFloat3(&edge1), XMLoadFloat3(&edge1))));

}