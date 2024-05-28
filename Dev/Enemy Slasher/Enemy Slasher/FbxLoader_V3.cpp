#include "FbxLoader_V3.h"
#include <filesystem>
#include <stack>
#include <fstream>

CFbxLoader_V3::CFbxLoader_V3()
{
}

CFbxLoader_V3::~CFbxLoader_V3()
{
}

CFbx_V3::CFbxData* CFbxLoader_V3::LoadFbx(const std::string& filePath, const std::string& fileName)
{
	CFbx_V3::CFbxData* loadData = nullptr;
	loadData = FileLoadFBXData(fileName);

	if (loadData == nullptr) {

		if (m_plSdkManager == nullptr) {
			m_plSdkManager = FbxManager::Create();

			FbxIOSettings* ios = FbxIOSettings::Create(m_plSdkManager, IOSROOT);
			m_plSdkManager->SetIOSettings(ios);
		}


		FbxScene* lScene = FbxScene::Create(m_plSdkManager, "myScene");
		FbxImporter* pFbxImporter = FbxImporter::Create(m_plSdkManager, "");
		std::string fileFullName = filePath + fileName + ".fbx";
		bool res = pFbxImporter->Initialize(fileFullName.c_str(), -1, m_plSdkManager->GetIOSettings());

		if (res == false) {
			// Convert std::string to std::wstring using MultiByteToWideChar
			int bufferSize = MultiByteToWideChar(CP_UTF8, 0, fileFullName.c_str(), -1, NULL, 0);

			std::wstring wideFileFullName(bufferSize, 0);
			MultiByteToWideChar(CP_UTF8, 0, fileFullName.c_str(), -1, &wideFileFullName[0], bufferSize);

			// Display the message box with the file name
			std::wstring message = L"File not found: " + wideFileFullName;
			MessageBox(0, message.c_str(), L"Error", 0);
		}

		pFbxImporter->Import(lScene);
		pFbxImporter->Destroy();

		FbxGeometryConverter geometryConverter(m_plSdkManager);
		geometryConverter.Triangulate(lScene, true);

		FbxNode* lRootNode = lScene->GetRootNode();
		if (lRootNode)
		{
			loadData = new CFbx_V3::CFbxData;
			loadData->m_sFileName = fileName;

			CFbx_V3::ObjectData* pRootObject;
			pRootObject = ProgressNodes(lRootNode, lScene, fileName);
			loadData->m_pRootObjectData = pRootObject;

			loadData->RecursiveCountAll(loadData->m_pRootObjectData);

			loadData->m_vpMeshs = m_vpAllMeshs;
			m_vpAllMeshs.clear();
		}

		ExportFBXData(loadData, fileName);
	}

	return loadData;
}

void CFbxLoader_V3::LoadAnim(CFbx_V3::Skeleton* pTargetSkeleton, const std::string& filePath, const std::string& fileName)
{
	// Load data to this Skeleton pointer
	m_pSkeleton = pTargetSkeleton;

	// 동일한 이름의 애니메이션이 없다면 로드
	if (m_pSkeleton->m_mAnimations.end() == m_pSkeleton->m_mAnimations.find(fileName)) {
		CFbx_V3::AnimationClip* pAnimClip = FlieLoadAnimationClip(fileName);

		if (pAnimClip != nullptr) {
			m_pSkeleton->m_vAnimationNames.push_back(fileName);
			m_pSkeleton->m_mAnimations[fileName] = pAnimClip;
		}
		else {
			if (m_plSdkManager == nullptr) {
				m_plSdkManager = FbxManager::Create();

				FbxIOSettings* ios = FbxIOSettings::Create(m_plSdkManager, IOSROOT);
				m_plSdkManager->SetIOSettings(ios);
			}


			FbxScene* lScene = FbxScene::Create(m_plSdkManager, "myScene");
			FbxImporter* pFbxImporter = FbxImporter::Create(m_plSdkManager, "");
			std::string fileFullName = filePath + fileName + ".fbx";
			pFbxImporter->Initialize(fileFullName.c_str(), -1, m_plSdkManager->GetIOSettings());

			pFbxImporter->Import(lScene);
			pFbxImporter->Destroy();

			FbxGeometryConverter geometryConverter(m_plSdkManager);
			geometryConverter.Triangulate(lScene, true);

			FbxNode* lRootNode = lScene->GetRootNode();
			if (lRootNode)
			{
				// Animation Data
				for (int i = 0; i < lRootNode->GetChildCount(); i++)
				{
					FbxNode* pFbxChildNode = lRootNode->GetChild(i);
					FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();
					FbxNodeAttribute::EType AttributeType = pMesh->GetAttributeType();
					if (!pMesh || !AttributeType) { continue; }

					if (AttributeType == FbxNodeAttribute::eMesh)
					{
						// Get Animation Clip
						LoadAnimation(pFbxChildNode, lScene, fileName, true);
					}
				}
			}
			ExportAnimationClip(m_pSkeleton->m_mAnimations[fileName], fileName);
		}
	}
}

CFbx_V3::CFbxData* CFbxLoader_V3::LoadFbxScene(const std::string& filePath, const std::string& fileName)
{
	CFbx_V3::CFbxData* loadData = nullptr;
	loadData = FileLoadFBXData(fileName);

	if (loadData == nullptr) {
		if (m_plSdkManager == nullptr) {
			m_plSdkManager = FbxManager::Create();

			FbxIOSettings* ios = FbxIOSettings::Create(m_plSdkManager, IOSROOT);
			m_plSdkManager->SetIOSettings(ios);
		}


		FbxScene* lScene = FbxScene::Create(m_plSdkManager, "myScene");
		FbxImporter* pFbxImporter = FbxImporter::Create(m_plSdkManager, "");
		std::string fileFullName = filePath + fileName + ".fbx";
		bool res = pFbxImporter->Initialize(fileFullName.c_str(), -1, m_plSdkManager->GetIOSettings());

		if (res == false) {
			// Convert std::string to std::wstring using MultiByteToWideChar
			int bufferSize = MultiByteToWideChar(CP_UTF8, 0, fileFullName.c_str(), -1, NULL, 0);

			std::wstring wideFileFullName(bufferSize, 0);
			MultiByteToWideChar(CP_UTF8, 0, fileFullName.c_str(), -1, &wideFileFullName[0], bufferSize);

			// Display the message box with the file name
			std::wstring message = L"File not found: " + wideFileFullName;
			MessageBox(0, message.c_str(), L"Error", 0);
		}

		pFbxImporter->Import(lScene);
		pFbxImporter->Destroy();

		FbxGeometryConverter geometryConverter(m_plSdkManager);
		geometryConverter.Triangulate(lScene, true);

		FbxNode* lRootNode = lScene->GetRootNode();
		if (lRootNode)
		{
			loadData = new CFbx_V3::CFbxData;
			loadData->m_sFileName = fileName;


			// Make return object
			CFbx_V3::ObjectData* pRootObject = new CFbx_V3::ObjectData();

			// input translate data
			{
				FbxDouble3 translation = lRootNode->LclTranslation.Get();
				FbxDouble3 rotation = lRootNode->LclRotation.Get();
				FbxDouble3 scaling = lRootNode->LclScaling.Get();
				pRootObject->m_xmf3Translate.x = static_cast<float>(translation.mData[0]);
				pRootObject->m_xmf3Translate.y = static_cast<float>(translation.mData[1]);
				pRootObject->m_xmf3Translate.z = static_cast<float>(translation.mData[2]);
				pRootObject->m_xmf3Rotate.x = static_cast<float>(rotation.mData[0]);
				pRootObject->m_xmf3Rotate.y = static_cast<float>(rotation.mData[1]);
				pRootObject->m_xmf3Rotate.z = static_cast<float>(rotation.mData[2]);
				pRootObject->m_xmf3Scale.x = static_cast<float>(scaling.mData[0]);
				pRootObject->m_xmf3Scale.y = static_cast<float>(scaling.mData[1]);
				pRootObject->m_xmf3Scale.z = static_cast<float>(scaling.mData[2]);
			}

			// loop for all childs and make object Hierarch
			for (int i = 0; i < lRootNode->GetChildCount(); ++i) {
				FbxNode* pChildNode = lRootNode->GetChild(i);
				if (pChildNode->GetNodeAttribute() == NULL) {
					CFbx_V3::ObjectData* pNewChildObject = ProgressNodes_LoadOnlyMesh(lRootNode->GetChild(i));
					pRootObject->m_vChildObjects.push_back(pNewChildObject);
					continue;
				}
				FbxNodeAttribute::EType AttributeType = pChildNode->GetNodeAttribute()->GetAttributeType();
				if (AttributeType != FbxNodeAttribute::eSkeleton) {
					CFbx_V3::ObjectData* pNewChildObject = ProgressNodes_LoadOnlyMesh(lRootNode->GetChild(i));
					pRootObject->m_vChildObjects.push_back(pNewChildObject);
				}
			}

			loadData->m_pRootObjectData = pRootObject;

			loadData->RecursiveCountAll(loadData->m_pRootObjectData);

			loadData->m_vpMeshs = m_vpAllMeshs;
			m_vpAllMeshs.clear();
		}
		ExportFBXData(loadData, fileName);
	}
	return loadData;
}

CFbx_V3::ObjectData* CFbxLoader_V3::ProgressNodes(FbxNode* lRootNode, FbxScene* lScene, const std::string& fileName)
{
	// Make return object
	CFbx_V3::ObjectData* pNewObject = new CFbx_V3::ObjectData();
	
	// input translate data
	{
		FbxDouble3 translation = lRootNode->LclTranslation.Get();
		FbxDouble3 rotation = lRootNode->LclRotation.Get();
		FbxDouble3 scaling = lRootNode->LclScaling.Get();
		pNewObject->m_xmf3Translate.x = static_cast<float>(translation.mData[0]);
		pNewObject->m_xmf3Translate.y = static_cast<float>(translation.mData[1]);
		pNewObject->m_xmf3Translate.z = static_cast<float>(translation.mData[2]);
		pNewObject->m_xmf3Rotate.x = static_cast<float>(rotation.mData[0]);
		pNewObject->m_xmf3Rotate.y = static_cast<float>(rotation.mData[1]);
		pNewObject->m_xmf3Rotate.z = static_cast<float>(rotation.mData[2]);
		pNewObject->m_xmf3Scale.x = static_cast<float>(scaling.mData[0]);
		pNewObject->m_xmf3Scale.y = static_cast<float>(scaling.mData[1]);
		pNewObject->m_xmf3Scale.z = static_cast<float>(scaling.mData[2]);
	}

	// loop for all childs and make object Hierarch
	for (int i = 0; i < lRootNode->GetChildCount(); ++i) {
		FbxNode* pChildNode = lRootNode->GetChild(i);
		if (pChildNode->GetNodeAttribute() == NULL) { 
			CFbx_V3::ObjectData* pNewChildObject = ProgressNodes(lRootNode->GetChild(i), lScene, fileName);
			pNewObject->m_vChildObjects.push_back(pNewChildObject);
			continue;
		}
		FbxNodeAttribute::EType AttributeType = pChildNode->GetNodeAttribute()->GetAttributeType();
		if (AttributeType != FbxNodeAttribute::eSkeleton &&
			AttributeType != FbxNodeAttribute::eMesh) {
			CFbx_V3::ObjectData* pNewChildObject = ProgressNodes(lRootNode->GetChild(i), lScene, fileName);
			pNewObject->m_vChildObjects.push_back(pNewChildObject);
		}
	}

	// Get Skeleton Hierarchy
	bool isSkeletonExist = false;
	m_pSkeleton = new CFbx_V3::Skeleton();
	for (int i = 0; i < lRootNode->GetChildCount(); ++i) {
		FbxNode* pChildNode = lRootNode->GetChild(i);
		if (pChildNode->GetNodeAttribute() == NULL) { continue; }
		FbxNodeAttribute::EType AttributeType = pChildNode->GetNodeAttribute()->GetAttributeType();
		if (AttributeType == FbxNodeAttribute::eSkeleton) {
			isSkeletonExist = true;
			LoadSkeletonHierarch(pChildNode, 0, -1);
		}
	}

	if (isSkeletonExist == true) {
		m_pSkeleton->m_vsBoneNames;
		m_pSkeleton->m_vxmf4x4BoneOffsetMat.resize(m_pSkeleton->m_vnBoneHierarcht.size());
	}
	else {
		delete m_pSkeleton;
		m_pSkeleton = nullptr;
	}

	// Get Mesh Data
	for (int i = 0; i < lRootNode->GetChildCount(); ++i) {
		FbxNode* pChildNode = lRootNode->GetChild(i);
		if (pChildNode->GetNodeAttribute() == NULL) { continue; }
		FbxNodeAttribute::EType AttributeType = pChildNode->GetNodeAttribute()->GetAttributeType();
		if (AttributeType == FbxNodeAttribute::eMesh) {
			FbxMesh* pMesh = pChildNode->GetMesh();

			LoadControlPoints(pChildNode);

			if (isSkeletonExist == true) {
				LoadAnimation(pChildNode, lScene, fileName, false);
				LoadVertexAndIndiceWithSkeleton(pChildNode);
			}
			else {
				LoadVertexAndIndiceNonSkeleton(pChildNode);
			}

			LoadMaterials(pChildNode);
		}
	}

	storeObjectData(pNewObject);
	return pNewObject;
}

CFbx_V3::ObjectData* CFbxLoader_V3::ProgressNodes_LoadOnlyMesh(FbxNode* lRootNode)
{
	// Make return object
	CFbx_V3::ObjectData* pNewObject = new CFbx_V3::ObjectData();

	// input translate data
	{
		FbxDouble3 translation = lRootNode->LclTranslation.Get();
		FbxDouble3 rotation = lRootNode->LclRotation.Get();
		FbxDouble3 scaling = lRootNode->LclScaling.Get();
		pNewObject->m_xmf3Translate.x = static_cast<float>(translation.mData[0]);
		pNewObject->m_xmf3Translate.y = static_cast<float>(translation.mData[1]);
		pNewObject->m_xmf3Translate.z = static_cast<float>(translation.mData[2]);
		pNewObject->m_xmf3Rotate.x = static_cast<float>(rotation.mData[0]);
		pNewObject->m_xmf3Rotate.y = static_cast<float>(rotation.mData[1]);
		pNewObject->m_xmf3Rotate.z = static_cast<float>(rotation.mData[2]);
		pNewObject->m_xmf3Scale.x = static_cast<float>(scaling.mData[0]);
		pNewObject->m_xmf3Scale.y = static_cast<float>(scaling.mData[1]);
		pNewObject->m_xmf3Scale.z = static_cast<float>(scaling.mData[2]);
	}

	// Get Curr Node Mesh
	if (lRootNode->GetNodeAttribute() != NULL) {
		FbxNodeAttribute::EType AttributeType = lRootNode->GetNodeAttribute()->GetAttributeType();

		if (AttributeType == FbxNodeAttribute::eMesh) {
			FbxMesh* pMesh = lRootNode->GetMesh();

			LoadControlPoints(lRootNode);

			LoadVertexAndIndiceNonSkeleton(lRootNode);

			LoadMaterials(lRootNode);
		}
	}
	storeObjectData(pNewObject);


	// loop for all childs and make object Hierarch
	for (int i = 0; i < lRootNode->GetChildCount(); ++i) {
		FbxNode* pChildNode = lRootNode->GetChild(i);
		if (pChildNode->GetNodeAttribute() == NULL) {
			CFbx_V3::ObjectData* pNewChildObject = ProgressNodes_LoadOnlyMesh(lRootNode->GetChild(i));
			pNewObject->m_vChildObjects.push_back(pNewChildObject);
			continue;
		}
		FbxNodeAttribute::EType AttributeType = pChildNode->GetNodeAttribute()->GetAttributeType();
		if (AttributeType != FbxNodeAttribute::eSkeleton) {
			CFbx_V3::ObjectData* pNewChildObject = ProgressNodes_LoadOnlyMesh(lRootNode->GetChild(i));
			pNewObject->m_vChildObjects.push_back(pNewChildObject);
		}
	}

	return pNewObject;
}

void CFbxLoader_V3::LoadSkeletonHierarch(FbxNode* inNode, int myIndex, int inParentIndex)
{
	m_pSkeleton->m_vnBoneHierarcht.push_back(inParentIndex);
	m_pSkeleton->m_vsBoneNames.push_back(inNode->GetName());

	for (int i = 0; i < inNode->GetChildCount(); ++i) {
		LoadSkeletonHierarch(inNode->GetChild(i), m_pSkeleton->m_vnBoneHierarcht.size(), myIndex);
	}
}

void CFbxLoader_V3::LoadControlPoints(FbxNode* inNode)
{
	FbxMesh* pMesh = inNode->GetMesh();

	unsigned int ctrlPointCount = pMesh->GetControlPointsCount();
	for (unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		CFbx_V3::ControlPoint* currCtrlPoint = new CFbx_V3::ControlPoint();

		XMFLOAT3 currPosition;
		currPosition.x = static_cast<float>(pMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = static_cast<float>(pMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = static_cast<float>(pMesh->GetControlPointAt(i).mData[2]);

		currCtrlPoint->m_xmf3Position = currPosition;
		m_mControlPoint[i] = currCtrlPoint;
	}
}

void CFbxLoader_V3::LoadVertexAndIndiceWithSkeleton(FbxNode* inNode)
{
	CFbx_V3::MeshData* pNewMeshData = new CFbx_V3::MeshData();

	// Load Vertex and Index With BlendingInfo

	FbxMesh* pMesh = inNode->GetMesh();
	int nPolygonCount = pMesh->GetPolygonCount();

	// TempData Container
	std::unordered_map<std::string, std::vector<int>> mNameIndexVectorMap;
	std::unordered_map<CFbx_V3::VertexData, int> mVertexIndexMap;

	int nVertexCounter = 0;
	for (int i = 0; i < nPolygonCount; ++i) {
		std::string currBoneName = m_mControlPoint[pMesh->GetPolygonVertex(i, 1)]->m_sBoneName;

		int nPolygonSize = pMesh->GetPolygonSize(i);
		for (int j = 0; j < nPolygonSize; ++j) {
			int nControlPointIndex = pMesh->GetPolygonVertex(i, j);

			CFbx_V3::ControlPoint* currControlPoint = m_mControlPoint[nControlPointIndex];

			// Normal
			FbxVector4 pNormal;
			pMesh->GetPolygonVertexNormal(i, j, pNormal);

			// UV
			//기본 TextureUV 만 로드
			int iUVcount = 1; // pMesh->GetElementUVCount();
			FbxStringList lUVNames;
			pMesh->GetUVSetNames(lUVNames);
			const char* lUVName = NULL;

			std::vector<FbxVector2> vUVList;
			vUVList.resize(iUVcount);
			for (int k = 0; k < iUVcount; ++k)
			{
				lUVName = lUVNames[k];

				bool bUnMappedUV;
				if (!pMesh->GetPolygonVertexUV(i, j, lUVName, vUVList[k], bUnMappedUV))
				{
					//MessageBox(0, L"UV not found", 0, 0);
					//std::cout << "UV Error ";
				}
			}


			// Position
			CFbx_V3::VertexData vertex;
			vertex.m_xmf3Position = currControlPoint->m_xmf3Position;

			// Normal
			vertex.m_xmf3Normal.x = static_cast<float>(pNormal.mData[0]);
			vertex.m_xmf3Normal.y = static_cast<float>(pNormal.mData[1]);
			vertex.m_xmf3Normal.z = static_cast<float>(pNormal.mData[2]);

			// UV
			vertex.m_xmf2UV.x = static_cast<float>(vUVList[0][0]);
			vertex.m_xmf2UV.y = 1.0f - static_cast<float>(vUVList[0][1]);


			// Index
			auto result = mVertexIndexMap.find(vertex);
			if (result != mVertexIndexMap.end()) { // check duplication
				mNameIndexVectorMap[currBoneName].push_back(result->second); // get index and input vector
			}
			else {
				int currIndex = nVertexCounter++;
				mVertexIndexMap[vertex] = currIndex;
				mNameIndexVectorMap[currBoneName].push_back(currIndex);

				// BlendigInfo (Bone)
				currControlPoint->SortBlendingInfoByWeight();
				for (int k = 0; k < currControlPoint->m_vBoneInfo.size(); ++k) {
					if (k >= 3) break;

					vertex.m_nlBlendingIndex[k] = currControlPoint->m_vBoneInfo[k].m_nBlendingIndex;
					vertex.m_flBlendingWeight[k] = currControlPoint->m_vBoneInfo[k].m_fBlendingWeight;
				}
				vertex.m_nlBlendingIndex[3] = currControlPoint->m_vBoneInfo[3].m_nBlendingIndex;
				vertex.m_flBlendingWeight[3] = 1.f
					- currControlPoint->m_vBoneInfo[0].m_fBlendingWeight
					- currControlPoint->m_vBoneInfo[1].m_fBlendingWeight
					- currControlPoint->m_vBoneInfo[2].m_fBlendingWeight;

				pNewMeshData->m_vVertices.push_back(vertex);
			}
		}
	}

	for (int i = 0; i < m_pSkeleton->m_vsBoneNames.size(); ++i) {
		std::vector<int>* indexVector = &mNameIndexVectorMap[m_pSkeleton->m_vsBoneNames[i]];
		pNewMeshData->m_nIndices.insert(pNewMeshData->m_nIndices.end(), indexVector->begin(), indexVector->end());
	}

	m_vpMeshs.push_back(pNewMeshData);
}

void CFbxLoader_V3::LoadVertexAndIndiceNonSkeleton(FbxNode* inNode)
{
	CFbx_V3::MeshData* pNewMeshData = new CFbx_V3::MeshData();

	// Load Vertex and Index Only Mesh

	FbxMesh* pMesh = inNode->GetMesh();
	int nPolygonCount = pMesh->GetPolygonCount();

	std::unordered_map<CFbx_V3::VertexData, int> mVertexIndexMap;

	int nVertexCounter = 0;
	for (int i = 0; i < nPolygonCount; ++i) {

		int nPolygonSize = pMesh->GetPolygonSize(i);
		for (int j = 0; j < nPolygonSize; ++j) {
			int nControlPointIndex = pMesh->GetPolygonVertex(i, j);

			CFbx_V3::ControlPoint* currControlPoint = m_mControlPoint[nControlPointIndex];

			// Normal
			FbxVector4 pNormal;
			pMesh->GetPolygonVertexNormal(i, j, pNormal);

			// UV
			//기본 TextureUV 만 로드
			int iUVcount = 1; // pMesh->GetElementUVCount();
			FbxStringList lUVNames;
			pMesh->GetUVSetNames(lUVNames);
			const char* lUVName = NULL;

			std::vector<FbxVector2> vUVList;
			vUVList.resize(iUVcount);
			for (int k = 0; k < iUVcount; ++k)
			{
				lUVName = lUVNames[k];

				bool bUnMappedUV;
				if (!pMesh->GetPolygonVertexUV(i, j, lUVName, vUVList[k], bUnMappedUV))
				{
					//MessageBox(0, L"UV not found", 0, 0);
					//std::cout << "UV Error ";
				}
			}


			// Position
			CFbx_V3::VertexData vertex;
			vertex.m_xmf3Position = currControlPoint->m_xmf3Position;

			// Normal
			vertex.m_xmf3Normal.x = static_cast<float>(pNormal.mData[0]);
			vertex.m_xmf3Normal.y = static_cast<float>(pNormal.mData[1]);
			vertex.m_xmf3Normal.z = static_cast<float>(pNormal.mData[2]);

			// UV
			vertex.m_xmf2UV.x = static_cast<float>(vUVList[0][0]);
			vertex.m_xmf2UV.y = 1.0f - static_cast<float>(vUVList[0][1]);


			// Index
			auto result = mVertexIndexMap.find(vertex);
			if (result != mVertexIndexMap.end()) { // check duplication
				pNewMeshData->m_nIndices.push_back(result->second); // get index and input vector
			}
			else {
				int currIndex = nVertexCounter++;
				mVertexIndexMap[vertex] = currIndex;
				pNewMeshData->m_nIndices.push_back(currIndex);

				pNewMeshData->m_vVertices.push_back(vertex);
			}
		}
	}

	m_vpMeshs.push_back(pNewMeshData);
}

void CFbxLoader_V3::LoadAnimation(FbxNode* inNode, FbxScene* lScene, const std::string& clipName, bool isOnlyGetAnim)
{
	FbxMesh* pMesh = inNode->GetMesh();
	FbxAMatrix geometryTransform = GetGeometryTransformation(inNode);

	CFbx_V3::AnimationClip* animData = new CFbx_V3::AnimationClip();

	animData->m_vBoneAnimations.resize(m_pSkeleton->m_vsBoneNames.size());

	bool isAlreadyLoaded = m_pSkeleton->m_mAnimations.contains(clipName);

	// Deformer
	int numOfDeformers = pMesh->GetDeformerCount();
	for (int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex) {
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(pMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!currSkin) continue;

		// Cluster
		int numOfClusters = currSkin->GetClusterCount();
		for (int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex) {
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);

			std::string currJointName = currCluster->GetLink()->GetName();
			int currJointIndex = FindJointIndexUsingName(currJointName);

			if (isOnlyGetAnim == false) {
				FbxAMatrix transformMatrix;
				FbxAMatrix transformLinkMatrix;
				FbxAMatrix globalBindposeInverseMatrix;

				currCluster->GetTransformMatrix(transformMatrix); // The transformation of the mesh at binding time
				currCluster->GetTransformLinkMatrix(transformLinkMatrix); // The transformation of the cluster(joint) at binding time from joint space to world space
				globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform; // we need this

				// Update the information in m_pSkeleton 
				XMFLOAT4X4 xmf4x4BindinverseMat;
				storeFbxAMat2XMFLOAT4x4(xmf4x4BindinverseMat, globalBindposeInverseMatrix);
				m_pSkeleton->m_vxmf4x4BoneOffsetMat[currJointIndex] = xmf4x4BindinverseMat;

				// Associate each joint with the control points it affects
				int* controlPointIndices = currCluster->GetControlPointIndices();
				int numOfIndices = currCluster->GetControlPointIndicesCount();
				for (int i = 0; i < numOfIndices; ++i)
				{
					CFbx_V3::BlendingIndexWeightPair currBlendingIndexWeightPair;
					currBlendingIndexWeightPair.m_nBlendingIndex = currJointIndex;
					currBlendingIndexWeightPair.m_fBlendingWeight = currCluster->GetControlPointWeights()[i];

					m_mControlPoint[controlPointIndices[i]]->m_vBoneInfo.push_back(currBlendingIndexWeightPair);
					m_mControlPoint[controlPointIndices[i]]->m_sBoneName = currJointName;
				}
			}

			// Get animation information
			if (isAlreadyLoaded == false) {
				FbxAnimStack* currAnimStack = lScene->GetSrcObject<FbxAnimStack>(0);
				if (currAnimStack) {
					FbxString animStackName = currAnimStack->GetName();

					FbxTakeInfo* takeInfo = lScene->GetTakeInfo(animStackName);
					FbxTime time = takeInfo->mLocalTimeSpan.GetDuration().GetFrameCount();

					FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
					FbxTime end = takeInfo->mLocalTimeSpan.GetStop();

					CFbx_V3::BoneAnimation boneAnim;

					//Keyframe** currAnim = &loadData->m_Skeleton.m_vJoints[currJointIndex].m_pAnimFrames;


					CFbx_V3::KeyFrame beforeKeyFrame;
					FbxAMatrix matBeforeMatrix;
					FbxAMatrix matCurrMatrix;

					for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
					{
						FbxTime currTime;
						currTime.SetFrame(i, FbxTime::eFrames24);

						matCurrMatrix = currCluster->GetLink()->EvaluateGlobalTransform(currTime);

						if (matCurrMatrix == matBeforeMatrix) continue; // 중복처리

						CFbx_V3::KeyFrame currAnim;
						currAnim.m_nFrameNum = i;

						FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) * geometryTransform;

						storeFbxAMat2XMFLOAT4x4(currAnim.m_xmf4x4AnimMat,
							currentTransformOffset.Inverse() * matCurrMatrix);
						matBeforeMatrix = matCurrMatrix;

						boneAnim.m_vKeyFrames.push_back(currAnim);
					}
					animData->m_vBoneAnimations[currJointIndex] = boneAnim;
				}
			}
		}
	}

	if (isOnlyGetAnim == false) {
		CFbx_V3::BlendingIndexWeightPair currBlendingIndexWeightPair;
		currBlendingIndexWeightPair.m_nBlendingIndex = 0;
		currBlendingIndexWeightPair.m_fBlendingWeight = 0;
		for (auto itr = m_mControlPoint.begin(); itr != m_mControlPoint.end(); ++itr)
		{
			for (unsigned int i = itr->second->m_vBoneInfo.size(); i < 4; ++i)
			{
				itr->second->m_vBoneInfo.push_back(currBlendingIndexWeightPair);
			}
		}
	}

	if (isAlreadyLoaded == false &&
		animData->m_vBoneAnimations.size() > 0 &&
		animData->m_vBoneAnimations[0].m_vKeyFrames.size() > 0) {
		m_pSkeleton->m_vAnimationNames.push_back(clipName);
		m_pSkeleton->m_mAnimations[clipName] = animData;
	}
}

void CFbxLoader_V3::LoadMaterials(FbxNode* inNode)
{
	int nMaterialCount = inNode->GetMaterialCount();

	for (int i = 0; i < nMaterialCount; ++i) {
		CFbx_V3::Material* tempMaterial = new CFbx_V3::Material();
		FbxSurfaceMaterial* pSurfaceMaterial = inNode->GetMaterial(i);

		LoadMaterialAttribute(pSurfaceMaterial, *tempMaterial);
		LoadMaterialTexture(pSurfaceMaterial, *tempMaterial);

		if (tempMaterial->Name != "") {
			m_vpMaterials.push_back(tempMaterial);
		}
	}
}

void CFbxLoader_V3::LoadMaterialAttribute(FbxSurfaceMaterial* pMaterial, CFbx_V3::Material& outMaterial)
{
	FbxDouble3 double3;
	FbxDouble double1;
	if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId)) {
		// Amibent Color
		double3 = reinterpret_cast<FbxSurfacePhong*>(pMaterial)->Ambient;
		outMaterial.Ambient.x = static_cast<float>(double3.mData[0]);
		outMaterial.Ambient.y = static_cast<float>(double3.mData[1]);
		outMaterial.Ambient.z = static_cast<float>(double3.mData[2]);

		// Diffuse Color
		double3 = reinterpret_cast<FbxSurfacePhong*>(pMaterial)->Diffuse;
		outMaterial.DiffuseAlbedo.x = static_cast<float>(double3.mData[0]);
		outMaterial.DiffuseAlbedo.y = static_cast<float>(double3.mData[1]);
		outMaterial.DiffuseAlbedo.z = static_cast<float>(double3.mData[2]);

		//// Roughness 
		//double1 = reinterpret_cast<FbxSurfacePhong*>(pMaterial)->Shininess;
		//outMaterial.Roughness = 1 - double1;

		//// Reflection
		//double3 = reinterpret_cast<FbxSurfacePhong*>(pMaterial)->Reflection;
		//outMaterial.FresnelR0.x = static_cast<float>(double3.mData[0]);
		//outMaterial.FresnelR0.y = static_cast<float>(double3.mData[1]);
		//outMaterial.FresnelR0.z = static_cast<float>(double3.mData[2]);

		//// Specular Color
		//double3 = reinterpret_cast<FbxSurfacePhong*>(pMaterial)->Specular;
		//outMaterial.Specular.x = static_cast<float>(double3.mData[0]);
		//outMaterial.Specular.y = static_cast<float>(double3.mData[1]);
		//outMaterial.Specular.z = static_cast<float>(double3.mData[2]);

		//// Emissive Color
		//double3 = reinterpret_cast<FbxSurfacePhong*>(pMaterial)->Emissive;
		//outMaterial.Emissive.x = static_cast<float>(double3.mData[0]);
		//outMaterial.Emissive.y = static_cast<float>(double3.mData[1]);
		//outMaterial.Emissive.z = static_cast<float>(double3.mData[2]);

		/*
		// Transparency Factor
		double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->TransparencyFactor;
		currMaterial->mTransparencyFactor = double1;

		// Specular Factor
		double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->SpecularFactor;
		currMaterial->mSpecularPower = double1;


		// Reflection Factor
	double1 = reinterpret_cast<FbxSurfacePhong *>(inMaterial)->ReflectionFactor;
		currMaterial->mReflectionFactor = double1;	*/
	}
	else if (pMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		// Amibent Color
		double3 = reinterpret_cast<FbxSurfaceLambert*>(pMaterial)->Ambient;
		outMaterial.Ambient.x = static_cast<float>(double3.mData[0]);
		outMaterial.Ambient.y = static_cast<float>(double3.mData[1]);
		outMaterial.Ambient.z = static_cast<float>(double3.mData[2]);

		// Diffuse Color
		double3 = reinterpret_cast<FbxSurfaceLambert*>(pMaterial)->Diffuse;
		outMaterial.DiffuseAlbedo.x = static_cast<float>(double3.mData[0]);
		outMaterial.DiffuseAlbedo.y = static_cast<float>(double3.mData[1]);
		outMaterial.DiffuseAlbedo.z = static_cast<float>(double3.mData[2]);

		//// Emissive Color
		//double3 = reinterpret_cast<FbxSurfaceLambert*>(pMaterial)->Emissive;
		//outMaterial.Emissive.x = static_cast<float>(double3.mData[0]);
		//outMaterial.Emissive.y = static_cast<float>(double3.mData[1]);
		//outMaterial.Emissive.z = static_cast<float>(double3.mData[2]);
	}
}

void CFbxLoader_V3::LoadMaterialTexture(FbxSurfaceMaterial* pMaterial, CFbx_V3::Material& outMaterial)
{
	unsigned int textureIndex = 0;
	FbxProperty property;

	FBXSDK_FOR_EACH_TEXTURE(textureIndex) {
		property = pMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[textureIndex]);
		if (property.IsValid())
		{
			unsigned int textureCount = property.GetSrcObjectCount<FbxTexture>();
			for (unsigned int i = 0; i < textureCount; ++i)
			{
				FbxLayeredTexture* layeredTexture = property.GetSrcObject<FbxLayeredTexture>(i);
				if (layeredTexture)
				{
					throw std::exception("Layered Texture is currently unsupported\n");
				}
				else
				{
					FbxTexture* texture = property.GetSrcObject<FbxTexture>(i);
					if (texture)
					{
						std::string textureType = property.GetNameAsCStr();
						FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);

						if (fileTexture)
						{
							if (textureType == "DiffuseColor")
							{
								std::string textureName = fileTexture->GetFileName();
								std::filesystem::path filePath(textureName);
								outMaterial.Name = filePath.filename().string();
							}
							/*else if (textureType == "SpecularColor")
							{
							Mat->mSpecularMapName = fileTexture->GetFileName();
							}
							else if (textureType == "Bump")
							{
							Mat->mNormalMapName = fileTexture->GetFileName();
							}*/
						}
					}
				}
			}
		}
	}
}

void CFbxLoader_V3::ExportFBXData(CFbx_V3::CFbxData* loadData, const std::string& fileName)
{
	std::ofstream fileOut("Resource/obData/" + fileName + ".obData", ios_base::out | ios_base::binary);
	if (loadData->m_nDataCount == 0) return;

	if (fileOut.is_open() == true) {
		fileOut.write(reinterpret_cast<const char*>(&loadData->m_nDataCount), sizeof(loadData->m_nDataCount));
		fileOut.write(reinterpret_cast<const char*>(&loadData->m_nTextureCount), sizeof(loadData->m_nTextureCount));
		ExportObjectData(loadData->m_pRootObjectData, &fileOut);

		size_t meshsCount = loadData->m_vpMeshs.size();
		fileOut.write(reinterpret_cast<const char*>(&meshsCount), sizeof(meshsCount));
		for (const auto& mesh : loadData->m_vpMeshs) {
			ExportMeshData(mesh, &fileOut);
		}


		fileOut.close();
	}
}

void CFbxLoader_V3::ExportMeshData(CFbx_V3::MeshData* pMeshData, std::ofstream* fileOut)
{
	using namespace CFbx_V3;

	size_t verticesCount = pMeshData->m_vVertices.size();
	fileOut->write(reinterpret_cast<const char*>(&verticesCount), sizeof(verticesCount));
	int counter = 0;
	for (const auto& vertex : pMeshData->m_vVertices) {
		fileOut->write(reinterpret_cast<const char*>(&vertex.m_xmf3Position), sizeof(vertex.m_xmf3Position));
		fileOut->write(reinterpret_cast<const char*>(&vertex.m_xmf3Normal), sizeof(vertex.m_xmf3Normal));
		fileOut->write(reinterpret_cast<const char*>(&vertex.m_xmf2UV), sizeof(vertex.m_xmf2UV));

		fileOut->write(reinterpret_cast<const char*>(vertex.m_flBlendingWeight), sizeof(vertex.m_flBlendingWeight));
		fileOut->write(reinterpret_cast<const char*>(vertex.m_nlBlendingIndex), sizeof(vertex.m_nlBlendingIndex));
	}

	size_t indicesCount = pMeshData->m_nIndices.size();
	fileOut->write(reinterpret_cast<const char*>(&indicesCount), sizeof(indicesCount));
	if (indicesCount > 0) {
		fileOut->write(reinterpret_cast<const char*>(pMeshData->m_nIndices.data()), indicesCount * sizeof(int));
	}
}

void CFbxLoader_V3::ExportObjectData(CFbx_V3::ObjectData* rootObject, std::ofstream* fileOut)
{
	using namespace CFbx_V3;

	std::stack<const ObjectData*> stack;
	stack.push(rootObject);

	while (!stack.empty()) {
		const ObjectData* currentObject = stack.top();
		stack.pop();

		// Save current object data
		fileOut->write(reinterpret_cast<const char*>(&currentObject->m_xmf3Translate), sizeof(currentObject->m_xmf3Translate));
		fileOut->write(reinterpret_cast<const char*>(&currentObject->m_xmf3Rotate), sizeof(currentObject->m_xmf3Rotate));
		fileOut->write(reinterpret_cast<const char*>(&currentObject->m_xmf3Scale), sizeof(currentObject->m_xmf3Scale));

		// Save mesh indices
		size_t meshIndicesCount = currentObject->m_vnMeshIndices.size();
		fileOut->write(reinterpret_cast<const char*>(&meshIndicesCount), sizeof(meshIndicesCount));
		if (meshIndicesCount > 0) {
			fileOut->write(reinterpret_cast<const char*>(currentObject->m_vnMeshIndices.data()), meshIndicesCount * sizeof(int));
		}

		// Save materials
		size_t materialsCount = currentObject->m_vpMaterials.size();
		fileOut->write(reinterpret_cast<const char*>(&materialsCount), sizeof(materialsCount));
		for (const auto& material : currentObject->m_vpMaterials) {
			size_t nameLength = material->Name.size();
			fileOut->write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
			fileOut->write(material->Name.data(), nameLength);
			fileOut->write(reinterpret_cast<const char*>(&material->Ambient), sizeof(material->Ambient));
			fileOut->write(reinterpret_cast<const char*>(&material->DiffuseAlbedo), sizeof(material->DiffuseAlbedo));
		}

		// Save skeleton presence flag
		bool hasSkeleton = (currentObject->m_pSkeleton != nullptr);
		fileOut->write(reinterpret_cast<const char*>(&hasSkeleton), sizeof(hasSkeleton));
		if (hasSkeleton == true) {
			ExportSkeletonData(currentObject->m_pSkeleton, fileOut);
		}

		// Save child objects
		size_t childCount = currentObject->m_vChildObjects.size();
		fileOut->write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));
		for (const auto& child : currentObject->m_vChildObjects) {
			stack.push(child);
		}
	}
}

void CFbxLoader_V3::ExportSkeletonData(CFbx_V3::Skeleton* pSkeleton, std::ofstream* fileOut)
{
	using namespace CFbx_V3;

	size_t boneNameCount = pSkeleton->m_vsBoneNames.size();
	fileOut->write(reinterpret_cast<const char*>(&boneNameCount), sizeof(boneNameCount));
	for (const auto& boneName : pSkeleton->m_vsBoneNames) {
		size_t nameLength = boneName.size();
		fileOut->write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
		fileOut->write(boneName.data(), nameLength);
	}

	size_t boneHierarchCount = pSkeleton->m_vnBoneHierarcht.size();
	fileOut->write(reinterpret_cast<const char*>(&boneHierarchCount), sizeof(boneHierarchCount));
	if (boneHierarchCount > 0) {
		fileOut->write(reinterpret_cast<const char*>(pSkeleton->m_vnBoneHierarcht.data()), boneHierarchCount * sizeof(int));
	}

	size_t boneOffsetMatCount = pSkeleton->m_vxmf4x4BoneOffsetMat.size();
	fileOut->write(reinterpret_cast<const char*>(&boneOffsetMatCount), sizeof(boneOffsetMatCount));
	for (size_t i = 0; i < boneOffsetMatCount; ++i) {
		fileOut->write(reinterpret_cast<const char*>(&pSkeleton->m_vxmf4x4BoneOffsetMat[i]), sizeof(XMFLOAT4X4));
	}

	size_t animationNameCount = pSkeleton->m_vAnimationNames.size();
	fileOut->write(reinterpret_cast<const char*>(&animationNameCount), sizeof(animationNameCount));
	if (animationNameCount > 0) {
		for (size_t i = 0; i < animationNameCount; ++i) {
			size_t nameLength = pSkeleton->m_vAnimationNames[i].size();
			fileOut->write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
			fileOut->write(pSkeleton->m_vAnimationNames[i].data(), nameLength);

			ExportAnimationClip(pSkeleton->m_mAnimations[pSkeleton->m_vAnimationNames[i]], pSkeleton->m_vAnimationNames[i]);
		}
	}
}

void CFbxLoader_V3::ExportAnimationClip(CFbx_V3::AnimationClip* pAnimClip, const std::string& clipName)
{
	std::ofstream fileOut("Resource/animClip/" + clipName + ".animClip", ios_base::out | ios_base::binary);

	if (fileOut.is_open() == true) {
		size_t boneAnimCount = pAnimClip->m_vBoneAnimations.size();
		fileOut.write(reinterpret_cast<const char*>(&boneAnimCount), sizeof(boneAnimCount));
		for (const auto& boneAnim : pAnimClip->m_vBoneAnimations) {
			size_t keyframeCount = boneAnim.m_vKeyFrames.size();
			fileOut.write(reinterpret_cast<const char*>(&keyframeCount), sizeof(keyframeCount));
			for (const auto& keyframe : boneAnim.m_vKeyFrames) {
				fileOut.write(reinterpret_cast<const char*>(&keyframe.m_nFrameNum), sizeof(keyframe.m_nFrameNum));
				fileOut.write(reinterpret_cast<const char*>(&keyframe.m_xmf4x4AnimMat), sizeof(keyframe.m_xmf4x4AnimMat));
			}
		}

		fileOut.close();
	}
}

CFbx_V3::CFbxData* CFbxLoader_V3::FileLoadFBXData(const std::string& fileName)
{
	std::ifstream fileIn("Resource/obData/" + fileName + ".obData", ios_base::in | ios_base::binary);

	if (fileIn.is_open() == true) {
		CFbx_V3::CFbxData* loadData = new CFbx_V3::CFbxData();
		loadData->m_sFileName = fileName;

		fileIn.read(reinterpret_cast<char*>(&loadData->m_nDataCount), sizeof(loadData->m_nDataCount));
		fileIn.read(reinterpret_cast<char*>(&loadData->m_nTextureCount), sizeof(loadData->m_nTextureCount));
		loadData->m_pRootObjectData = FileLoadObjectData(&fileIn);

		size_t meshsCount;
		fileIn.read(reinterpret_cast<char*>(&meshsCount), sizeof(meshsCount));
		loadData->m_vpMeshs.resize(meshsCount);
		for (size_t i = 0; i < meshsCount; ++i) {
			loadData->m_vpMeshs[i] = FileLoadMeshData(&fileIn);
		}


		fileIn.close();
		return loadData;
	}

	return nullptr;
}

CFbx_V3::MeshData* CFbxLoader_V3::FileLoadMeshData(std::ifstream* fileIn)
{
	using namespace CFbx_V3;

	MeshData* pMeshData = new MeshData();

	size_t meshVerticesCount;
	fileIn->read(reinterpret_cast<char*>(&meshVerticesCount), sizeof(meshVerticesCount));
	pMeshData->m_vVertices.resize(meshVerticesCount);
	VertexData* pVertex = nullptr;
	for (size_t i = 0; i < meshVerticesCount; ++i) {
		pVertex = &pMeshData->m_vVertices[i];
		fileIn->read(reinterpret_cast<char*>(&(pVertex->m_xmf3Position)), sizeof(pVertex->m_xmf3Position));
		fileIn->read(reinterpret_cast<char*>(&(pVertex->m_xmf3Normal)), sizeof(pVertex->m_xmf3Normal));
		fileIn->read(reinterpret_cast<char*>(&(pVertex->m_xmf2UV)), sizeof(pVertex->m_xmf2UV));

		fileIn->read(reinterpret_cast<char*>(pVertex->m_flBlendingWeight), sizeof(pVertex->m_flBlendingWeight));
		fileIn->read(reinterpret_cast<char*>(pVertex->m_nlBlendingIndex), sizeof(pVertex->m_nlBlendingIndex));
	}

	size_t meshIndicesCount;
	fileIn->read(reinterpret_cast<char*>(&meshIndicesCount), sizeof(meshIndicesCount));
	pMeshData->m_nIndices.resize(meshIndicesCount);
	if (meshIndicesCount > 0) {
		fileIn->read(reinterpret_cast<char*>(pMeshData->m_nIndices.data()), meshIndicesCount * sizeof(int));
	}

	return pMeshData;
}

CFbx_V3::ObjectData* CFbxLoader_V3::FileLoadObjectData(std::ifstream* fileIn)
{
	using namespace CFbx_V3;

	std::stack<ObjectData*> stack;
	ObjectData* rootObject = new ObjectData();
	stack.push(rootObject);

	while (!stack.empty()) {
		ObjectData* currentObject = stack.top();
		stack.pop();

		// Read current object data
		fileIn->read(reinterpret_cast<char*>(&currentObject->m_xmf3Translate), sizeof(currentObject->m_xmf3Translate));
		fileIn->read(reinterpret_cast<char*>(&currentObject->m_xmf3Rotate), sizeof(currentObject->m_xmf3Rotate));
		fileIn->read(reinterpret_cast<char*>(&currentObject->m_xmf3Scale), sizeof(currentObject->m_xmf3Scale));

		// Read mesh indices
		size_t meshIndicesCount;
		fileIn->read(reinterpret_cast<char*>(&meshIndicesCount), sizeof(meshIndicesCount));
		currentObject->m_vnMeshIndices.resize(meshIndicesCount);
		if (meshIndicesCount > 0) {
			fileIn->read(reinterpret_cast<char*>(currentObject->m_vnMeshIndices.data()), meshIndicesCount * sizeof(int));
		}

		// Read materials
		size_t materialsCount;
		fileIn->read(reinterpret_cast<char*>(&materialsCount), sizeof(materialsCount));
		currentObject->m_vpMaterials.resize(materialsCount);
		for (size_t i = 0; i < materialsCount; ++i) {
			Material* material = new Material();
			size_t nameLength;
			fileIn->read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
			material->Name.resize(nameLength);
			fileIn->read(&material->Name[0], nameLength);
			fileIn->read(reinterpret_cast<char*>(&material->Ambient), sizeof(material->Ambient));
			fileIn->read(reinterpret_cast<char*>(&material->DiffuseAlbedo), sizeof(material->DiffuseAlbedo));
			currentObject->m_vpMaterials[i] = material;
		}

		// Read skeleton presence flag
		bool hasSkeleton;
		fileIn->read(reinterpret_cast<char*>(&hasSkeleton), sizeof(hasSkeleton));
		if (hasSkeleton == true) {
			currentObject->m_pSkeleton = FileLoadSkeletonData(fileIn);
		}

		// Read child objects
		size_t childCount;
		fileIn->read(reinterpret_cast<char*>(&childCount), sizeof(childCount));
		currentObject->m_vChildObjects.resize(childCount);
		for (size_t i = 0; i < childCount; ++i) {
			ObjectData* childObject = new ObjectData();
			currentObject->m_vChildObjects[i] = childObject;
			stack.push(childObject);
		}
	}

	return rootObject;
}

CFbx_V3::Skeleton* CFbxLoader_V3::FileLoadSkeletonData(std::ifstream* fileIn)
{
	using namespace CFbx_V3;

	Skeleton* pSkeleton = new Skeleton();

	size_t boneNameCount;
	fileIn->read(reinterpret_cast<char*>(&boneNameCount), sizeof(boneNameCount));
	pSkeleton->m_vsBoneNames.resize(boneNameCount);
	for (size_t i = 0; i < boneNameCount; ++i) {
		size_t nameLegth;
		fileIn->read(reinterpret_cast<char*>(&nameLegth), sizeof(nameLegth));
		pSkeleton->m_vsBoneNames[i].resize(nameLegth);
		fileIn->read(&pSkeleton->m_vsBoneNames[i][0], nameLegth);
	}

	size_t boneHierarchCount;
	fileIn->read(reinterpret_cast<char*>(&boneHierarchCount), sizeof(boneHierarchCount));
	pSkeleton->m_vnBoneHierarcht.resize(boneHierarchCount);
	if (boneHierarchCount > 0) {
		fileIn->read(reinterpret_cast<char*>(pSkeleton->m_vnBoneHierarcht.data()), boneHierarchCount * sizeof(int));
	}

	size_t boneOffsetMatCount;
	fileIn->read(reinterpret_cast<char*>(&boneOffsetMatCount), sizeof(boneOffsetMatCount));
	pSkeleton->m_vxmf4x4BoneOffsetMat.resize(boneOffsetMatCount);
	for (size_t i = 0; i < boneOffsetMatCount; ++i) {
		fileIn->read(reinterpret_cast<char*>(&pSkeleton->m_vxmf4x4BoneOffsetMat[i]), sizeof(XMFLOAT4X4));
	}

	size_t animationNameCount;
	fileIn->read(reinterpret_cast<char*>(&animationNameCount), sizeof(animationNameCount));
	pSkeleton->m_vAnimationNames.resize(animationNameCount);
	if (animationNameCount > 0) {
		for (size_t i = 0; i < animationNameCount; ++i) {
			size_t nameLegth;
			fileIn->read(reinterpret_cast<char*>(&nameLegth), sizeof(nameLegth));
			pSkeleton->m_vAnimationNames[i].resize(nameLegth);
			fileIn->read(&pSkeleton->m_vAnimationNames[i][0], nameLegth);

			pSkeleton->m_mAnimations[pSkeleton->m_vAnimationNames[i]] = FlieLoadAnimationClip(pSkeleton->m_vAnimationNames[i]);
		}
	}

	return pSkeleton;
}

CFbx_V3::AnimationClip* CFbxLoader_V3::FlieLoadAnimationClip(const std::string& clipName)
{
	using namespace CFbx_V3;
	std::ifstream fileIn("Resource/animClip/" + clipName + ".animClip", ios_base::in | ios_base::binary);

	if (fileIn.is_open() == true) {
		AnimationClip* pNewAnimClip = new AnimationClip();
		BoneAnimation* pNewBoneAnim;
		KeyFrame* pNewKeyFrame;

		size_t boneAnimCount;
		fileIn.read(reinterpret_cast<char*>(&boneAnimCount), sizeof(boneAnimCount));
		pNewAnimClip->m_vBoneAnimations.resize(boneAnimCount);
		for (size_t i = 0; i < boneAnimCount; ++i) {
			pNewBoneAnim = &pNewAnimClip->m_vBoneAnimations[i];

			size_t keyframeCount;
			fileIn.read(reinterpret_cast<char*>(&keyframeCount), sizeof(keyframeCount));
			pNewBoneAnim->m_vKeyFrames.resize(keyframeCount);
			for (size_t j = 0; j < keyframeCount; ++j) {
				pNewKeyFrame = &pNewBoneAnim->m_vKeyFrames[j];
				
				fileIn.read(reinterpret_cast<char*>(&pNewKeyFrame->m_nFrameNum), sizeof(pNewKeyFrame->m_nFrameNum));
				fileIn.read(reinterpret_cast<char*>(&pNewKeyFrame->m_xmf4x4AnimMat), sizeof(pNewKeyFrame->m_xmf4x4AnimMat));
			}
		}

		fileIn.close();
		return pNewAnimClip;
	}

	return nullptr;
}

FbxAMatrix CFbxLoader_V3::GetGeometryTransformation(FbxNode* inNode)
{
	if (!inNode)
	{
		throw std::exception("Null for mesh geometry");
	}

	const FbxVector4 lT = inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = inNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = inNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

int CFbxLoader_V3::FindJointIndexUsingName(std::string JointName)
{
	if (m_pSkeleton) {
		for (int i = 0; i < m_pSkeleton->m_vsBoneNames.size(); ++i) {
			if (m_pSkeleton->m_vsBoneNames[i] == JointName)
				return i;
		}
	}
	return 0;
}

void CFbxLoader_V3::storeFbxAMat2XMFLOAT4x4(XMFLOAT4X4& dest, FbxAMatrix& sorce)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			dest.m[i][j] = static_cast<float>(sorce.Get(i, j));
}

void CFbxLoader_V3::storeObjectData(CFbx_V3::ObjectData* pObject)
{
	if(m_pSkeleton)
		pObject->m_pSkeleton = m_pSkeleton;

	bool isDifferent = true;
	for (int i = 0; i < m_vpMeshs.size(); ++i) {
		isDifferent = true;
		for (int j = 0; j < m_vpAllMeshs.size(); ++j) {
			if (*(m_vpMeshs[i]) == *(m_vpAllMeshs[j])) {
				isDifferent = false;
				pObject->m_vnMeshIndices.push_back(j);
				break;
			}
		}
		if (isDifferent) {
			pObject->m_vnMeshIndices.push_back(m_vpAllMeshs.size());
			m_vpAllMeshs.push_back(m_vpMeshs[i]);
		}
	}
	
	pObject->m_vpMaterials = m_vpMaterials;

	m_pSkeleton = nullptr;
	m_vpMeshs.clear();
	m_vpMaterials.clear();
	for (auto& a : m_mControlPoint) delete a.second;
	m_mControlPoint.clear();
}
