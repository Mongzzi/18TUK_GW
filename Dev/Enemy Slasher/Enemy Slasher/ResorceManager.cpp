#include "ResorceManager.h"
#include "Shader.h"
#include <filesystem>
#include <stack>

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
	std::string fName(filePath + fileName + ".fbx");
	CFBXObject* newGameObject = nullptr;

	// 현재 parent가 childe의 정보를 수정하므로 오브젝트의 instance화가 불가능하다.
	//if (m_mLoadedFBXObjectMap.end() == m_mLoadedFBXObjectMap.find(fName)) {
	//	newGameObject = LoadFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pFBXLoader->LoadFbx(filePath, fileName));
	//}
	//else {
	//	newGameObject = m_mLoadedFBXObjectMap[fName];
	//}


	CFbx_V3::CFbxData* newFbxData = nullptr;
	if (m_mLoadedFBXDataMap.end() == m_mLoadedFBXDataMap.find(fName)) {
		newFbxData = m_pFBXLoader->LoadFbx(filePath, fileName);
	}
	else {
		newFbxData = m_mLoadedFBXDataMap[fName];
	}
	return LoadFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pFBXLoader->LoadFbx(filePath, fileName));
}

CFBXObject* CResorceManager::LoadFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFbx_V3::CFbxData* pFbxData)
{
	// 현재 parent가 childe의 정보를 수정하므로 오브젝트의 instance화가 불가능하다.
//	if (m_mLoadedFBXObjectMap.end() == m_mLoadedFBXObjectMap.find(pFbxData->m_sFileName)) {
//		CAniamtionObjectsShader* pShader = new CAniamtionObjectsShader();
//		pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
//		if (pFbxData->m_nTextureCount > 0)
//			pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, pFbxData->m_nTextureCount);
//
//#ifdef _DEBUG
//		std::cout << "Loaded DataCount : " << pFbxData->m_nDataCount << '\n';
//		std::cout << "Loaded TextureCount : " << pFbxData->m_nTextureCount << '\n';
//#endif
//
//		CFBXObject* newGameObject = LoadFBXObjectRecursive(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pFbxData->m_pRootObjectData, pShader);
//		m_mLoadedFBXObjectMap[pFbxData->m_sFileName] = newGameObject;
//	}
//	return m_mLoadedFBXObjectMap[pFbxData->m_sFileName];

	CAniamtionObjectsShader* pShader = new CAniamtionObjectsShader();
	pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	if (pFbxData->m_nTextureCount > 0)
		pShader->CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, pFbxData->m_nTextureCount);

	if (m_mLoadedMeshsMap.contains(pFbxData->m_sFileName) == false) {
#ifdef _DEBUG
		std::cout << "Load New FbxData - FileName : " << pFbxData->m_sFileName << '\n';
		std::cout << "\tLoaded AllDataCount : " << pFbxData->m_nDataCount << '\n';
		std::cout << "\tLoaded DataInstanceCount : " << pFbxData->m_vpMeshs.size() << '\n';
		std::cout << "\tLoaded TextureCount : " << pFbxData->m_nTextureCount << '\n';
#endif

		bool isDifferent = true;
		CFBXMesh* pNewMesh;
		std::vector<CFBXMesh*>* pNewMeshVector = new std::vector<CFBXMesh*>;
		for (int i = 0; i < pFbxData->m_vpMeshs.size(); ++i) {
			pNewMesh = new CFBXMesh(pd3dDevice, pd3dCommandList, pFbxData->m_vpMeshs[i]);
			pNewMeshVector->emplace_back(pNewMesh);

			delete pFbxData->m_vpMeshs[i];
		}
		m_mLoadedMeshsMap[pFbxData->m_sFileName] = pNewMeshVector;

		pFbxData->m_vpMeshs.clear();
	}
#ifdef _DEBUG
	else {
		std::cout << "AlreadyLoaded FbxData - FileName : " << pFbxData->m_sFileName << '\n';
	}
#endif

	m_vpCurrFileMeshs = m_mLoadedMeshsMap[pFbxData->m_sFileName];

	//CFBXObject* newGameObject = LoadFBXObjectRecursive(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pFbxData->m_pRootObjectData, pShader);
	CFBXObject* newGameObject = LoadFBXObjectIterative(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pFbxData->m_pRootObjectData, pShader);

	return newGameObject;
}

CFBXObject* CResorceManager::LoadFBXObjectRecursive(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFbx_V3::ObjectData* pObjectData, CShader* pShader)
{
	CFBXObject* newGameObject;
	newGameObject = new CFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, ShaderType::NON);
	for (int i = 0; i < pObjectData->m_vnMeshIndices.size(); ++i) {
		newGameObject->SetMesh(i, (*m_vpCurrFileMeshs)[pObjectData->m_vnMeshIndices[i]]);
	}
	newGameObject->SetFbxData(pd3dDevice, pd3dCommandList, pObjectData);
	newGameObject->SetShader(pShader);

	if (pObjectData->m_vpMaterials.size() > 0) {
		CFbx_V3::Material* pInputMaterial = pObjectData->m_vpMaterials[0];
		CMaterial* pNewMaterial = newGameObject->GetMaterial();
		pNewMaterial->m_xmf4Albedo = pInputMaterial->DiffuseAlbedo;
		if (pInputMaterial->Name != "") {
			CTexture* pNewTextures = nullptr;
			if (m_mLoadedTextureMap.contains(pInputMaterial->Name) == false) {
				std::string filePath = "Image/" + pInputMaterial->Name;
				if (std::filesystem::exists(filePath)) {
#ifdef _DEBUG
					std::cout << "\tExists and Load : " << filePath << '\n';
#endif // _DEBUG
					int bufferSize = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, NULL, 0);
					std::wstring wideFileFullPath(bufferSize, 0);
					MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, &wideFileFullPath[0], bufferSize);

					pNewTextures = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
					pNewTextures->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, &wideFileFullPath[0], RESOURCE_TEXTURE2D, 0);
					pNewMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, pNewTextures, 0, 4);
					m_mLoadedTextureMap[pInputMaterial->Name] = pNewTextures;
				}
#ifdef _DEBUG
				else {
					std::cout << "\tError - File Not Found : " << filePath << '\n';
				}
#endif // _DEBUG
			}
			pNewTextures = m_mLoadedTextureMap[pInputMaterial->Name];
			if (pNewTextures) {
				pNewMaterial->SetTexture(pNewTextures);
			}
		}

	}

	for (int i = 0; i < pObjectData->m_vChildObjects.size(); ++i) {
		CFBXObject* newChildObject = LoadFBXObjectRecursive(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pObjectData->m_vChildObjects[i], pShader);
		newGameObject->SetChild(newChildObject);
	}

	return newGameObject;
}

CFBXObject* CResorceManager::LoadFBXObjectIterative(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFbx_V3::ObjectData* pObjectData, CShader* pShader) {
	std::stack<std::pair<CFBXObject*, CFbx_V3::ObjectData*>> stack;
	CFBXObject* rootObject = new CFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, ShaderType::NON);
	stack.push({ rootObject, pObjectData });

	while (!stack.empty()) {
		auto [currentObject, currentObjectData] = stack.top();
		stack.pop();

		// Initialize the current object
		for (int i = 0; i < currentObjectData->m_vnMeshIndices.size(); ++i) {
			currentObject->SetMesh(i, (*m_vpCurrFileMeshs)[currentObjectData->m_vnMeshIndices[i]]);
		}
		currentObject->SetFbxData(pd3dDevice, pd3dCommandList, currentObjectData);
		currentObject->SetShader(pShader);

		// Set materials if any
		if (!currentObjectData->m_vpMaterials.empty()) {
			CFbx_V3::Material* pInputMaterial = currentObjectData->m_vpMaterials[0];
			CMaterial* pNewMaterial = currentObject->GetMaterial();
			pNewMaterial->m_xmf4Albedo = pInputMaterial->DiffuseAlbedo;

			if (!pInputMaterial->Name.empty()) {
				CTexture* pNewTextures = nullptr;
				if (!m_mLoadedTextureMap.contains(pInputMaterial->Name)) {
					std::string filePath = "Image/" + pInputMaterial->Name;
					if (std::filesystem::exists(filePath)) {
#ifdef _DEBUG
						std::cout << "\tExists and Load : " << filePath << '\n';
#endif // _DEBUG
						int bufferSize = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, NULL, 0);
						std::wstring wideFileFullPath(bufferSize, 0);
						MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, &wideFileFullPath[0], bufferSize);

						pNewTextures = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
						pNewTextures->LoadTextureFromWICFile(pd3dDevice, pd3dCommandList, &wideFileFullPath[0], RESOURCE_TEXTURE2D, 0);
						pNewMaterial->m_pShader->CreateShaderResourceViews(pd3dDevice, pNewTextures, 0, 4);
						m_mLoadedTextureMap[pInputMaterial->Name] = pNewTextures;
					}
#ifdef _DEBUG
					else {
						std::cout << "\tError - File Not Found : " << filePath << '\n';
					}
#endif // _DEBUG
				}
				pNewTextures = m_mLoadedTextureMap[pInputMaterial->Name];
				if (pNewTextures) {
					pNewMaterial->SetTexture(pNewTextures);
				}
			}
		}

		// Process child objects
		for (int i = 0; i < currentObjectData->m_vChildObjects.size(); ++i) {
			CFBXObject* newChildObject = new CFBXObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, ShaderType::NON);
			currentObject->SetChild(newChildObject);
			stack.push({ newChildObject, currentObjectData->m_vChildObjects[i] });
		}
	}

	return rootObject;
}