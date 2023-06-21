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
    // 충돌 계산 전 충돌하지 않았다고 가정 및 초기화
    m_pPickInfo->ResetData();

    // hWnd에서 스크린의 크기 계산
    GetClientRect(hWnd, &m_ClientRect);
    m_uScreenWidth = m_ClientRect.right - m_ClientRect.left;
    m_ScreenHeight = m_ClientRect.bottom - m_ClientRect.top;

    // 스크린 좌표계에서 커서 위치 받아오기
    GetCursorPos(&m_CursorPos);

    // 레이의 시작점 계산
    XMVECTOR nearPoint = XMVectorSet((float)m_CursorPos.x, (float)m_CursorPos.y, 0.0f, 1.0f);
    XMVECTOR farPoint = XMVectorSet((float)m_CursorPos.x, (float)m_CursorPos.y, 1.0f, 1.0f);

    // 뷰-프로젝션 행렬과 역행렬 계산
    XMMATRIX viewMatrix = XMLoadFloat4x4(&m_pCamera->GetViewMatrix());
    XMMATRIX projectionMatrix = XMLoadFloat4x4(&m_pCamera->GetProjectionMatrix());
    XMMATRIX viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
    XMMATRIX invViewProjectionMatrix = XMMatrixInverse(nullptr, viewProjectionMatrix);

    // 뷰-프로젝션 좌표계에서 레이의 시작점과 방향 계산
    nearPoint = XMVector3Unproject(nearPoint, 0.0f, 0.0f, m_uScreenWidth, m_ScreenHeight, 0.0f, 1.0f, projectionMatrix, viewMatrix, XMMatrixIdentity());
    farPoint = XMVector3Unproject(farPoint, 0.0f, 0.0f, m_uScreenWidth, m_ScreenHeight, 0.0f, 1.0f, projectionMatrix, viewMatrix, XMMatrixIdentity());
    XMVECTOR rayDirection = XMVector3Normalize(farPoint - nearPoint);

    // 씬에서 모든 메쉬들을 순회하며 충돌 검사
    for (int i = 0; i < m_pScene->m_nGameObjects; ++i)
    {
        m_pGameObject = m_pScene->m_ppGameObjects[i];
        m_xmmWorldMatrix = XMLoadFloat4x4(&m_pGameObject->m_xmf4x4World);
        m_xmmInvWorldMatrix = XMMatrixInverse(nullptr, m_xmmWorldMatrix);

        // 뷰-프로젝션 좌표계에서 모델 좌표계로 변환하는 행렬 계산
        m_xmmToModelSpaceMatrix = XMMatrixMultiply(invViewProjectionMatrix, m_xmmWorldMatrix);

        // 레이의 시작점과 방향을 모델 좌표계로 변환
        m_xmvRayOriginModelSpace = XMVector3TransformCoord(nearPoint, invViewProjectionMatrix);

        // 모델 좌표계에서 광선의 시작점, 방향을 계산
        m_xmvRayOrigin = XMVector3TransformCoord(m_xmvRayOriginModelSpace, m_xmmToModelSpaceMatrix);
        m_xmvRayDirection = XMVector3Normalize(XMVector3TransformNormal(rayDirection, m_xmmToModelSpaceMatrix));

        // 메쉬의 AABB와 광선의 교차 여부 확인
        float t = 0.0f;
        //if (m_pGameObject.GetBoundingBox().Intersects(m_xmvRayOrigin, m_xmvRayDirection, t))
        if (false)
        {
            m_pPickInfo->m_bHit = true;

            // 교차점 계산
            m_xmvHitPoint = m_xmvRayOrigin + m_xmvRayDirection * t;

            // 모델 좌표계에서 노말 방향을 계산
            //m_xmvLocalNormal = m_pGameObject->GetMeshNormalAtPoint(m_xmvHitPoint);
            //m_xmvWorldNormal = XMVector3TransformNormal(m_xmvLocalNormal, m_xmmWorldMatrix);
            m_xmvHitPointWorld = XMVector3TransformCoord(m_xmvHitPoint, m_xmmWorldMatrix);

            // 거리를 비교하여 가장 가까운 충돌점 선택
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