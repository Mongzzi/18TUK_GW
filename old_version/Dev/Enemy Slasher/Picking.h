#pragma once
#include "stdafx.h"
#include "Camera.h"
#include "Scene.h"

class PickInfo
{
public:
    PickInfo();
private:

public:
    bool            m_bHit;             // 충돌 여부
    float           m_fDistance;        // 충돌 지점까지의 거리
    CGameObject*    m_pGameObject;      // 충돌한 오브젝트의 포인터
    XMFLOAT3        m_xmf3HitPoint;     // 충돌 지점
    XMFLOAT3        m_xmf3Normal;       // 충돌 지점의 법선 벡터

public:
    void ResetData();
};

class CPicker
{
private:
    PickInfo* m_pPickInfo;

    // 클라이언트 크기에 관련된 변수
    RECT m_ClientRect;
    UINT m_uScreenWidth;
    UINT m_ScreenHeight;

    // 커서위치 변수
    POINT m_CursorPos;

    // 오브젝트와 충돌검사를 위한 변수들
    CGameObject* m_pGameObject;
    XMMATRIX m_xmmWorldMatrix;
    XMMATRIX m_xmmInvWorldMatrix;

    XMMATRIX m_xmmToModelSpaceMatrix;
    XMVECTOR m_xmvRayOriginModelSpace;

    XMVECTOR m_xmvRayOrigin;
    XMVECTOR m_xmvRayDirection;

    XMVECTOR m_xmvHitPoint;

    XMVECTOR m_xmvLocalNormal;
    XMVECTOR m_xmvWorldNormal;
    XMVECTOR m_xmvHitPointWorld;
public:
    CPicker();
    ~CPicker();

    // CPU 기반 피킹 함수
    PickInfo* PerformCPUPicking(HWND hWnd, CCamera* m_pCamera, CScene* m_pScene);
};