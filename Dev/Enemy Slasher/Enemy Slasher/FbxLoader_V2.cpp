#include "FbxLoader_V2.h"

CFbxLoader_V2::CFbxLoader_V2()
{
	InitializeSDK();
}

CFbxLoader_V2::~CFbxLoader_V2()
{
	DestroySDK();
}

void CFbxLoader_V2::InitializeSDK()
{
	if (m_plSdkManager == nullptr)
		m_plSdkManager = FbxManager::Create();

	if (!m_plSdkManager) {
		exit(-1);
	}
	else {
#ifdef _DEBUG
		FBXSDK_printf("Autodesk FBX SDK version %s\n", m_plSdkManager->GetVersion());
#endif // _DEBUG
	}

	FbxIOSettings* ios = FbxIOSettings::Create(m_plSdkManager, IOSROOT);
	m_plSdkManager->SetIOSettings(ios);
}

void CFbxLoader_V2::DestroySDK()
{
	m_plSdkManager->Destroy();
}

CFbxData* CFbxLoader_V2::LoadFBX(const char* fileName)
{
	std::string fName(fileName);
	CFbxData* loadData = nullptr;

	if (m_mLoadedDataMap.contains(fName) == false) {
		FbxScene* lScene = FbxScene::Create(m_plSdkManager, "myScene");
		FbxImporter* pFbxImporter = FbxImporter::Create(m_plSdkManager, "");
		pFbxImporter->Initialize(fileName, -1, m_plSdkManager->GetIOSettings());

		pFbxImporter->Import(lScene);
		pFbxImporter->Destroy();

		FbxNode* lRootNode = lScene->GetRootNode();
		if (lRootNode)
			loadData = LoadNode(lRootNode);

		m_mLoadedDataMap[fName] = loadData;
	}

	return m_mLoadedDataMap[fName];
}

CFbxData* CFbxLoader_V2::LoadNode(FbxNode* node)
{
	CFbxData* newNode = new CFbxData;

	LoadContent(node, newNode);

	newNode->m_nChildCount = node->GetChildCount();
	newNode->m_ppChilds = new CFbxData * [newNode->m_nChildCount];
	for (unsigned int i = 0; i < newNode->m_nChildCount; ++i) {
		newNode->m_ppChilds[i] = LoadNode(node->GetChild(i));
	}

	return newNode;
}

void CFbxLoader_V2::LoadContent(FbxNode* node, CFbxData* pData)
{
	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
	FbxNodeAttribute::EType lAttributeType;

	if (nodeAttribute)
	{
		lAttributeType = nodeAttribute->GetAttributeType();

		switch (lAttributeType)
		{
		default:
			break;
		case FbxNodeAttribute::eMarker:
			break;

		case FbxNodeAttribute::eSkeleton:
			break;

		case FbxNodeAttribute::eMesh:
			break;

		case FbxNodeAttribute::eNurbs:
			break;

		case FbxNodeAttribute::ePatch:
			break;

		case FbxNodeAttribute::eCamera:
			break;

		case FbxNodeAttribute::eLight:
			break;

		case FbxNodeAttribute::eLODGroup:
			break;
		}
	}
}
