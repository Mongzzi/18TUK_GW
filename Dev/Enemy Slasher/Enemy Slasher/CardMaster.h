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
// 
// 씬의 OnProcessingKeyboardMessage, OnProcessingMouseMessage, ProcessInput 등등의 함수에서 벌어지는 일들을 다 처리해야함. 이벤트를 받는 곳과 처리하는곳이 다를 가능성 농후함.
// 
// 턴제 게임으로 만들기 위해는 일단 그려야 할 패도 여러개 일 수 있으니 더미 덱 하나 추가해보는것도 ㄱㅊ을지도?
// 카드 사용이라는 행동을 큐로 저장하고 턴 종료시 가져와 실행하고 턴을 넘겨야함.
// 이때 랜덤 처리를 여기서 해야 같은 default_random_engine 사용 가능.
// 턴마다 새로운 default_random_engine를 만들고 각 턴마다 카드 사용 큐와 default_random_engine 시드를 저장한다. 그러면 리플래이도 가능.
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

