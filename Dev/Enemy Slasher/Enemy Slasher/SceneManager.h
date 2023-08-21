#pragma once
#include "Scene.h"

class CSceneManager
{
public:
	CBasicScene*			m_pCurScene = NULL;

	// �������� ���� �����ϱ� ���� �ӽ� ����
	std::vector<CBasicScene*>	m_pvScenelist;
	int							m_nSceneCounter = 0;

public:
	CSceneManager();
	~CSceneManager();

	void BuildManager();
	void ChangeScene(CBasicScene* pNextScene);
};

