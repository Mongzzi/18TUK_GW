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
    bool            m_bHit;             // �浹 ����
    float           m_fDistance;        // �浹 ���������� �Ÿ�
    CGameObject*    m_pGameObject;      // �浹�� ������Ʈ�� ������
    XMFLOAT3        m_xmf3HitPoint;     // �浹 ����
    XMFLOAT3        m_xmf3Normal;       // �浹 ������ ���� ����

public:
    void ResetData();
};

class CPicker
{
private:
    PickInfo* m_pPickInfo;

    // Ŭ���̾�Ʈ ũ�⿡ ���õ� ����
    RECT m_ClientRect;
    UINT m_uScreenWidth;
    UINT m_ScreenHeight;

    // Ŀ����ġ ����
    POINT m_CursorPos;

    // ������Ʈ�� �浹�˻縦 ���� ������
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

    // CPU ��� ��ŷ �Լ�
    PickInfo* PerformCPUPicking(HWND hWnd, CCamera* m_pCamera, CScene* m_pScene);
};