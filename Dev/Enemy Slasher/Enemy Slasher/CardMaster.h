#pragma once

#include "stdafx.h"
#include "ResorceManager.h"

//
// 화면에 보여줄 패를 보여줄 캐릭터 등 여기서 설정.
// DeckData에서 사용할 DRE는 어디서?
// 씬은 새로 만들어서?
// 전투를 하는 오브젝트들을 받아와서 진행?
//		아마 이러면 난입도 구현 가능할지도?
// 전투를 시작하기 전에 항상 덱을 초기화하고 시작해야함.
// 전투 할 캐릭터의 벡터 목록에 있는 오브젝트들의 턴 순서도 정해야함.
//
class CCardMaster
{
public:
	CCardMaster();
	~CCardMaster();
private:
	// 전투 할 캐릭터의 벡터
public:
	bool ProcessInput(HWND hWnd, UCHAR* pKeysBuffer, POINT ptOldCursorPos);
	void AnimateObjects(float fTotalTime, float fTimeElapsed);
	void DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CResorceManager* pFBXDataManager, float fTimeElapsed);
	// 전투 할 캐릭터 추가함수
};

