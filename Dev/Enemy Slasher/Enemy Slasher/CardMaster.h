#pragma once

#include "stdafx.h"
#include "ResorceManager.h"

//
// ȭ�鿡 ������ �и� ������ ĳ���� �� ���⼭ ����.
// DeckData���� ����� DRE�� ���?
// ���� ���� ����?
// ������ �ϴ� ������Ʈ���� �޾ƿͼ� ����?
//		�Ƹ� �̷��� ���Ե� ���� ����������?
// ������ �����ϱ� ���� �׻� ���� �ʱ�ȭ�ϰ� �����ؾ���.
// ���� �� ĳ������ ���� ��Ͽ� �ִ� ������Ʈ���� �� ������ ���ؾ���.
//
class CCardMaster
{
public:
	CCardMaster();
	~CCardMaster();
private:
	// ���� �� ĳ������ ����
public:
	bool ProcessInput(HWND hWnd, UCHAR* pKeysBuffer, POINT ptOldCursorPos);
	void AnimateObjects(float fTotalTime, float fTimeElapsed);
	void DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager, float fTimeElapsed);
	// ���� �� ĳ���� �߰��Լ�
};

