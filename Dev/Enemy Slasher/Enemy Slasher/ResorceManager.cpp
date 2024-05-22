#include "ResorceManager.h"
#include "Shader.h"
#include <filesystem>

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
		if(loadData->m_nTextureCount > 0)
			pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, loadData->m_nTextureCount);

#ifdef _DEBUG
		std::cout << "Loaded DataCount : " << loadData->m_nDataCount << '\n';
		std::cout << "Loaded TextureCount : " << loadData->m_nTextureCount << '\n';
#endif

		CFBXObject* newGameObject = LoadFBXObjectRecursive(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, loadData->m_pRootObjectData, pShader);

		m_mLoadedFBXDataMap[fName] = newGameObject;
	}

	return m_mLoadedFBXDataMap[fName];
}

CFBXObject* CResorceManager::LoadFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFbx_V3::CFbxData* pFbxData)
{
	CAniamtionObjectsShader* pShader = new CAniamtionObjectsShader();
	pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, pFbxData->m_nTextureCount);

#ifdef _DEBUG
	std::cout << "Loaded DataCount : " << pFbxData->m_nDataCount << '\n';
	std::cout << "Loaded TextureCount : " << pFbxData->m_nTextureCount << '\n';
#endif

	CFBXObject* newGameObject = LoadFBXObjectRecursive(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pFbxData->m_pRootObjectData, pShader);

	return newGameObject;
}

CFBXObject* CResorceManager::LoadFBXObjectRecursive(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFbx_V3::ObjectData* pObjectData, CShader* pShader)
{
	CFBXObject* newGameObject;
	newGameObject = new CFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, ShaderType::NON);
	newGameObject->SetFbxData(pd3dDevice, pd3dCommandList, pObjectData);
	newGameObject->SetShader(pShader);

	if (pObjectData->m_vpMaterials.size() > 0) {
		CFbx_V3::Material* pInputMaterial = pObjectData->m_vpMaterials[0];
		CMaterial* pNewMaterial = newGameObject->GetMaterial();
		pNewMaterial->m_xmf4Albedo = pInputMaterial->DiffuseAlbedo;
		if (pInputMaterial->Name != "") {
			std::string filePath = "Image/" + pInputMaterial->Name;
			if (std::filesystem::exists(filePath)) {
				std::cout << "Exists : " << filePath << '\n';
				CTexture* pNewTextures = nullptr;
				if (m_mLoadedTextureMap.contains(pInputMaterial->Name) == false) {
					int bufferSize = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, NULL, 0);
					std::wstring wideFileFullPath(bufferSize, 0);
					MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, &wideFileFullPath[0], bufferSize);

					pNewTextures = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
					pNewTextures->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, &wideFileFullPath[0], RESOURCE_TEXTURE2D, 0);
					pNewMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, pNewTextures, 0, 4);
					m_mLoadedTextureMap[pInputMaterial->Name] = pNewTextures;
				}
				pNewTextures = m_mLoadedTextureMap[pInputMaterial->Name];
				if (pNewTextures) {
					pNewMaterial->SetTexture(pNewTextures);
				}
			}
			else {
				std::cout << "Non : " << filePath << '\n';
			}
		}
	}

	for (int i = 0; i < pObjectData->m_vChildObjects.size(); ++i) {
		CFBXObject* newChildObject = LoadFBXObjectRecursive(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pObjectData->m_vChildObjects[i], pShader);
		newGameObject->SetChild(newChildObject);
	}

	return newGameObject;
}
