#include "ResorceManager.h"

CResorceManager::CResorceManager()
{
	m_pFBXLoader = new CFbxLoader_V3();
}

CResorceManager::~CResorceManager()
{
	if (m_pFBXLoader) delete m_pFBXLoader;
}

CFBXObject* CResorceManager::LoadFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const string& filePath, const string& fileName)
{
	std::string fName(filePath + fileName);

	if (m_mLoadedFBXDataMap.end() == m_mLoadedFBXDataMap.find(fName)) {
		CFbx_V3::CFbxData* loadData = nullptr;

		loadData = m_pFBXLoader->LoadFbx(filePath, fileName);

		CFBXObject* newGameObject = LoadFBXObjectRecursive(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, loadData->m_pRootObjectData);

		m_mLoadedFBXDataMap[fName] = newGameObject;
	}

	return m_mLoadedFBXDataMap[fName];
}

CFBXObject* CResorceManager::LoadFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFbx_V3::CFbxData* pFbxData)
{
	CFBXObject* newGameObject = LoadFBXObjectRecursive(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pFbxData->m_pRootObjectData);

	return newGameObject;
}

CFBXObject* CResorceManager::LoadFBXObjectRecursive(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFbx_V3::ObjectData* pObjectData)
{
	CFBXObject* newGameObject;
	newGameObject = new CFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, ShaderType::CAnimationObjectShader);
	newGameObject->SetFbxData(pd3dDevice, pd3dCommandList, pObjectData);

	for (int i = 0; i < pObjectData->m_vChildObjects.size(); ++i) {
		CFBXObject* newChildObject = LoadFBXObjectRecursive(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pObjectData->m_vChildObjects[i]);
		newGameObject->SetChild(newChildObject);
	}

	return newGameObject;
}
