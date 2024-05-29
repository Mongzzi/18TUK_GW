#pragma once
#include "FbxLoader_V3.h"
#include "Object.h"
#include "PhysXManager.h"
#include <map>



class CResorceManager
{
public:
	CResorceManager();
	~CResorceManager();

public:
	CFBXObject* LoadFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const string& filePath, const string& fileName);
	CFBXObject* LoadFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFbx_V3::CFbxData* pFbxData);

private:
	CFBXObject* LoadFBXObjectIterative(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CFbx_V3::ObjectData* pObjectData, CShader* pShader);

private:


private:
	// FbxSdk
	CFbxLoader_V3*												m_pFBXLoader = nullptr;

	std::unordered_map<std::string, CFBXObject*>				m_mLoadedFBXObjectMap;
	std::unordered_map<std::string, CFbx_V3::CFbxData*>			m_mLoadedFBXDataMap;
	std::unordered_map<std::string, CTexture*>					m_mLoadedTextureMap;

	std::unordered_map<std::string, std::vector<CFBXMesh*>*>	m_mLoadedMeshsMap;
	
	std::vector<CFBXMesh*>*										m_vpCurrFileMeshs = nullptr;

private:
	// PhysX
	CPhysXManager* m_pPhysXManager = nullptr;

};

