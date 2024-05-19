#include "FbxLoader_V3.h"

CFbxLoader_V3::CFbxLoader_V3()
{
}

CFbxLoader_V3::~CFbxLoader_V3()
{
}

CFbx_V3::CFbxData* CFbxLoader_V3::LoadFbx(const std::string& filePath, const std::string& fileName)
{
	CFbx_V3::CFbxData* loadData = nullptr;

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
		loadData = new CFbx_V3::CFbxData;

		ProgressNodes(lRootNode, lScene, fileName);
	}

	return loadData;
}

void CFbxLoader_V3::LoadAnim(CFbx_V3::Skeleton* pTargetSkeleton, const std::string& filePath, const std::string& fileName)
{
	//m_pSkeleton = pTargetSkeleton;

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
}

void CFbxLoader_V3::ProgressNodes(FbxNode* lRootNode, FbxScene* lScene, const std::string& fileName)
{
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

CFbx_V3::MeshData* CFbxLoader_V3::LoadVertexAndIndiceWithSkeleton(FbxNode* inNode)
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
			//�⺻ TextureUV �� �ε�
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
					if (k >= 4) break;

					vertex.m_nlBlendingIndex[k] = currControlPoint->m_vBoneInfo[k].m_nBlendingIndex;
					if (k >= 3) continue;
					vertex.m_flBlendingWeight[k] = currControlPoint->m_vBoneInfo[k].m_fBlendingWeight;
				}

				pNewMeshData->m_vVertices.push_back(vertex);
			}
		}
	}

	for (int i = 0; i < m_pSkeleton->m_vsBoneNames.size(); ++i) {
		std::vector<int>* indexVector = &mNameIndexVectorMap[m_pSkeleton->m_vsBoneNames[i]];
		pNewMeshData->m_nIndices.insert(pNewMeshData->m_nIndices.end(), indexVector->begin(), indexVector->end());
	}

	return pNewMeshData;
}

CFbx_V3::MeshData* CFbxLoader_V3::LoadVertexAndIndiceNonSkeleton(FbxNode* inNode)
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
			//�⺻ TextureUV �� �ε�
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

	return pNewMeshData;
}

void CFbxLoader_V3::LoadAnimation(FbxNode* inNode, FbxScene* lScene, const std::string& clipName, bool isOnlyGetAnim)
{
	FbxMesh* pMesh = inNode->GetMesh();
	FbxAMatrix geometryTransform = GetGeometryTransformation(inNode);

	CFbx_V3::AnimationClip animData;

	animData.m_vBoneAnimations.resize(m_pSkeleton->m_vsBoneNames.size());

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

					if (matCurrMatrix == matBeforeMatrix) continue; // �ߺ�ó��

					CFbx_V3::KeyFrame currAnim;
					currAnim.m_nFrameNum = i;

					FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) * geometryTransform;

					storeFbxAMat2XMFLOAT4x4(currAnim.m_xmf4x4AnimMat,
						currentTransformOffset.Inverse() * matCurrMatrix);
					matBeforeMatrix = matCurrMatrix;

					boneAnim.m_vKeyFrames.push_back(currAnim);
				}
				animData.m_vBoneAnimations[currJointIndex] = boneAnim;
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

	m_pSkeleton->m_mAnimations[clipName] = animData;
}

void CFbxLoader_V3::LoadMaterials(FbxNode* inNode)
{
	int nMaterialCount = inNode->GetMaterialCount();

	for (int i = 0; i < nMaterialCount; ++i) {
		CFbx_V3::Material tempMaterial;
		FbxSurfaceMaterial* pSurfaceMaterial = inNode->GetMaterial(i);

		LoadMaterialAttribute(pSurfaceMaterial, tempMaterial);
		LoadMaterialTexture(pSurfaceMaterial, tempMaterial);

		if (tempMaterial.Name != "") {
			m_vMaterials.push_back(tempMaterial);
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
								outMaterial.Name = fileTexture->GetFileName();
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