#include "Picking.h"

PickInfo::PickInfo() {
    ResetData();
}

void PickInfo::ResetData() {
    m_bHit = false;
    m_fDistance = FLT_MAX;
    m_pGameObject = NULL;
    m_xmf3HitPoint = { 0.0f, 0.0f, 0.0f };
    m_xmf3Normal = { 0.0f, 0.0f, 0.0f };
}



///////////////////////////////////////////////////////////////

CPicker::CPicker() {
    m_pPickInfo = new PickInfo;
}

CPicker::~CPicker() {
    delete m_pPickInfo;
}

PickInfo* CPicker::PerformCPUPicking(HWND hWnd, CCamera* m_pCamera, CScene* m_pScene)
{
    // �浹 ��� �� �浹���� �ʾҴٰ� ���� �� �ʱ�ȭ
    m_pPickInfo->ResetData();

    // hWnd���� ��ũ���� ũ�� ���
    GetClientRect(hWnd, &m_ClientRect);
    m_uScreenWidth = m_ClientRect.right - m_ClientRect.left;
    m_ScreenHeight = m_ClientRect.bottom - m_ClientRect.top;

    // ��ũ�� ��ǥ�迡�� Ŀ�� ��ġ �޾ƿ���
    GetCursorPos(&m_CursorPos);

    // ������ ������ ���
    XMVECTOR nearPoint = XMVectorSet((float)m_CursorPos.x, (float)m_CursorPos.y, 0.0f, 1.0f);
    XMVECTOR farPoint = XMVectorSet((float)m_CursorPos.x, (float)m_CursorPos.y, 1.0f, 1.0f);

    // ��-�������� ��İ� ����� ���
    XMMATRIX viewMatrix = XMLoadFloat4x4(&m_pCamera->GetViewMatrix());
    XMMATRIX projectionMatrix = XMLoadFloat4x4(&m_pCamera->GetProjectionMatrix());
    XMMATRIX viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
    XMMATRIX invViewProjectionMatrix = XMMatrixInverse(nullptr, viewProjectionMatrix);

    // ��-�������� ��ǥ�迡�� ������ �������� ���� ���
    nearPoint = XMVector3Unproject(nearPoint, 0.0f, 0.0f, m_uScreenWidth, m_ScreenHeight, 0.0f, 1.0f, projectionMatrix, viewMatrix, XMMatrixIdentity());
    farPoint = XMVector3Unproject(farPoint, 0.0f, 0.0f, m_uScreenWidth, m_ScreenHeight, 0.0f, 1.0f, projectionMatrix, viewMatrix, XMMatrixIdentity());
    XMVECTOR rayDirection = XMVector3Normalize(farPoint - nearPoint);

    // ������ ��� �޽����� ��ȸ�ϸ� �浹 �˻�
    for (int i = 0; i < m_pScene->m_nGameObjects; ++i)
    {
        m_pGameObject = m_pScene->m_ppGameObjects[i];
        m_xmmWorldMatrix = XMLoadFloat4x4(&m_pGameObject->m_xmf4x4World);
        m_xmmInvWorldMatrix = XMMatrixInverse(nullptr, m_xmmWorldMatrix);

        // ��-�������� ��ǥ�迡�� �� ��ǥ��� ��ȯ�ϴ� ��� ���
        m_xmmToModelSpaceMatrix = XMMatrixMultiply(invViewProjectionMatrix, m_xmmWorldMatrix);

        // ������ �������� ������ �� ��ǥ��� ��ȯ
        m_xmvRayOriginModelSpace = XMVector3TransformCoord(nearPoint, invViewProjectionMatrix);

        // �� ��ǥ�迡�� ������ ������, ������ ���
        m_xmvRayOrigin = XMVector3TransformCoord(m_xmvRayOriginModelSpace, m_xmmToModelSpaceMatrix);
        m_xmvRayDirection = XMVector3Normalize(XMVector3TransformNormal(rayDirection, m_xmmToModelSpaceMatrix));

        // �޽��� AABB�� ������ ���� ���� Ȯ��
        float t = 0.0f;
        //if (m_pGameObject.GetBoundingBox().Intersects(m_xmvRayOrigin, m_xmvRayDirection, t))
        if (false)
        {
            m_pPickInfo->m_bHit = true;

            // ������ ���
            m_xmvHitPoint = m_xmvRayOrigin + m_xmvRayDirection * t;

            // �� ��ǥ�迡�� �븻 ������ ���
            //m_xmvLocalNormal = m_pGameObject->GetMeshNormalAtPoint(m_xmvHitPoint);
            //m_xmvWorldNormal = XMVector3TransformNormal(m_xmvLocalNormal, m_xmmWorldMatrix);
            m_xmvHitPointWorld = XMVector3TransformCoord(m_xmvHitPoint, m_xmmWorldMatrix);

            // �Ÿ��� ���Ͽ� ���� ����� �浹�� ����
            float distance = XMVectorGetX(XMVector3LengthSq(m_xmvHitPointWorld - m_xmvRayOrigin));
            if (distance < m_pPickInfo->m_fDistance)
            {
                m_pPickInfo->m_fDistance = distance;
                m_pPickInfo->m_pGameObject = m_pGameObject;
                XMStoreFloat3(&m_pPickInfo->m_xmf3HitPoint, m_xmvHitPointWorld);
                XMStoreFloat3(&m_pPickInfo->m_xmf3Normal, m_xmvWorldNormal);
            }
        }
    }

    return m_pPickInfo;
}