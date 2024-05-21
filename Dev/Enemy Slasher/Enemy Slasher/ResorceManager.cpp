#include "ResorceManager.h"
#include "Shader.h"

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

		CAniamtionObjectsShader* pShader = new CAniamtionObjectsShader();
		pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

		CFBXObject* newGameObject = LoadFBXObjectRecursive(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, loadData->m_pRootObjectData, pShader);

		m_mLoadedFBXDataMap[fName] = newGameObject;
	}

	return m_mLoadedFBXDataMap[fName];
}

CFBXObject* CResorceManager::LoadFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFbx_V3::CFbxData* pFbxData)
{
	CAniamtionObjectsShader* pShader = new CAniamtionObjectsShader();
	pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	CFBXObject* newGameObject = LoadFBXObjectRecursive(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pFbxData->m_pRootObjectData, pShader);

	return newGameObject;
}

CFBXObject* CResorceManager::LoadFBXObjectRecursive(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFbx_V3::ObjectData* pObjectData, CShader* pShader)
{
	CFBXObject* newGameObject;
	newGameObject = new CFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, ShaderType::NON);
	newGameObject->SetFbxData(pd3dDevice, pd3dCommandList, pObjectData);
	newGameObject->SetShader(pShader);

	for (int i = 0; i < pObjectData->m_vChildObjects.size(); ++i) {
		CFBXObject* newChildObject = LoadFBXObjectRecursive(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pObjectData->m_vChildObjects[i], pShader);
		newGameObject->SetChild(newChildObject);
	}

	return newGameObject;
}
