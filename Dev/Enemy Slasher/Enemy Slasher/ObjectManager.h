#pragma once

enum class ObjectLayer : int;

enum class ObjectLayer : int { // enum class는 int형으로 암시적 변환을 불허함으로 명시적 형변환을 해야 함
	BackGround,				// 배경 오브젝트
	LightObject,			// 조명 오브젝트
	InteractiveObject,		// 상호작용가능 오브젝트 (ex 문, 버튼)
	Object,					// 중립 오브젝트
	DestroyedObject,		// 파괴된 오브젝트
	Enemy,					// 적 오브젝트
	Player,					// 플레이어 오브젝트
	UIObject,				// 플레이어 UI 오브젝트	 depth/stancil 무시하고 항상 앞에 그려져야 함

	Count					// Layer 개수
};

class CObjectManager
{
private:
	std::vector<CGameObject*> m_vObjectManager[int(ObjectLayer::Count)];
	std::vector<CShader*> m_vShaderManager;

public:
	void AddObj(CGameObject* object, ObjectLayer layer);
	void DelObj(CGameObject* object, ObjectLayer layer);
	void AddShader(CGameObject* object);
	void DelShader(CGameObject* object);

	void Render();
};