#pragma once

#include "stdafx.h"
#include "FbxLoader.h"

class CCardMaster
{
public:
	CCardMaster();
	~CCardMaster();
private:
public:
	bool ProcessInput(HWND hWnd, UCHAR* pKeysBuffer, POINT ptOldCursorPos);
	void AnimateObjects(float fTotalTime, float fTimeElapsed);
	void DynamicShaping(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CFBXLoader* pFBXLoader, float fTimeElapsed);
};

