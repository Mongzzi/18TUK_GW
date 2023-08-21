#pragma once
#include "Scene.h"

class CSceneManager
{
public:
	CBasicScene*			m_pCurScene = NULL;

	// 여러개의 씬을 관리하기 위한 임시 변수
	std::vector<CBasicScene*>	m_pvScenelist;
	int							m_nSceneCounter = 0;

public:
	CSceneManager();
	~CSceneManager();

	void BuildManager();
	void ChangeScene(CBasicScene* pNextScene);
};

