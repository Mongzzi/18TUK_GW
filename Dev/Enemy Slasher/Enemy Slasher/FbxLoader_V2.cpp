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

		FbxGeometryConverter geometryConverter(m_plSdkManager);
		geometryConverter.Triangulate(lScene, true);

		FbxNode* lRootNode = lScene->GetRootNode();
		if (lRootNode)
		{
			loadData = new CFbxData;
			LoadControlPoints(lRootNode, loadData);
			ProcessSkeletonHierarchy(lRootNode, loadData);
			LoadMesh(lRootNode, lScene, loadData);
			// Clear CtrlPoints for manage memory
			loadData->m_vCtrlPoints.clear();

#ifdef _DEBUG
			FBXSDK_printf("Load Complete file : %s\n", fileName);
#endif // _DEBUG
		}

		m_mLoadedDataMap[fName] = loadData;
	}
	else {
#ifdef _DEBUG
		FBXSDK_printf("File already loaded : %s\n", fileName);
#endif // _DEBUG
	}

	return m_mLoadedDataMap[fName];
}

// LoadMesh Need loadData->ContrlPoint
void CFbxLoader_V2::LoadMesh(FbxNode* lRootNode, FbxScene* lScene, CFbxData* loadData)
{
	int iVertexCounter = 0;

	for (int i = 0; i < lRootNode->GetChildCount(); i++)
	{
		FbxNode* pFbxChildNode = lRootNode->GetChild(i);

		if (pFbxChildNode->GetNodeAttribute() == NULL)
			continue;

		FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

		if (AttributeType != FbxNodeAttribute::eMesh) {
			LoadMesh(pFbxChildNode, lScene, loadData);
			continue;
		}

		ProcessJointsAndAnimation(pFbxChildNode, lScene, loadData);

		FbxMesh* pMesh = pFbxChildNode->GetMesh();

		for (int j = 0; j < pMesh->GetPolygonCount(); j++)
		{
			int iNumVertices = pMesh->GetPolygonSize(j);
			assert(iNumVertices == 3);

			for (int k = 0; k < iNumVertices; k++) {
				int iControlPointIndex = pMesh->GetPolygonVertex(j, k);

				CFbxCtrlPoint* currCtrlPoint = &(loadData->m_vCtrlPoints[iControlPointIndex]);

				CFbxVertex vertex;
				vertex.m_xmf3Position = currCtrlPoint->m_xmf3Position;

				ReadNormal(pMesh, iControlPointIndex, iVertexCounter, vertex.m_xmf3Normal);

				//기본 TextureUV 만 로드
				for (int k = 0; k < 1; ++k)
				{
					ReadUV(pMesh, iControlPointIndex, pMesh->GetTextureUVIndex(i, j), k, vertex.m_xmf2UV);
				}


				// Copy the blending info from each control point
				for (unsigned int i = 0; i < currCtrlPoint->m_vBlendingInfo.size(); ++i)
				{
					BlendingIndexWeightPair currBlendingInfo;
					currBlendingInfo.m_nBlendingIndex = currCtrlPoint->m_vBlendingInfo[i].m_nBlendingIndex;
					currBlendingInfo.m_fBlendingWeight = currCtrlPoint->m_vBlendingInfo[i].m_fBlendingWeight;
					vertex.m_vBlendingInfo.push_back(currBlendingInfo);
				}


				loadData->m_vVertex.push_back(vertex);
				loadData->m_vIndices.push_back(iVertexCounter);
				++iVertexCounter;
			}
		}
	}
}

void CFbxLoader_V2::LoadControlPoints(FbxNode* lRootNode, CFbxData* loadData)
{
	int iVertexCounter = 0;

	for (int i = 0; i < lRootNode->GetChildCount(); i++)
	{
		FbxNode* pFbxChildNode = lRootNode->GetChild(i);

		if (pFbxChildNode->GetNodeAttribute() == NULL)
			continue;

		FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

		if (AttributeType != FbxNodeAttribute::eMesh) {
			LoadControlPoints(pFbxChildNode, loadData);
			continue;
		}

		FbxMesh* pMesh = pFbxChildNode->GetMesh();

		ReadControlPoints(pMesh, loadData);
	}
}

void CFbxLoader_V2::ReadControlPoints(FbxMesh* inMesh, CFbxData* loadData)
{
	unsigned int ctrlPointCount = inMesh->GetControlPointsCount();
	for (unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		CFbxCtrlPoint currCtrlPoint;
		XMFLOAT3 currPosition;
		currPosition.x = static_cast<float>(inMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = static_cast<float>(inMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = static_cast<float>(inMesh->GetControlPointAt(i).mData[2]);
		currCtrlPoint.m_xmf3Position = currPosition;
		loadData->m_vCtrlPoints.push_back(currCtrlPoint);
	}
}

void CFbxLoader_V2::ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outNormal)
{
	if (inMesh->GetElementNormalCount() < 1)
	{
		throw std::exception("Invalid Normal Number");
	}

	FbxGeometryElementNormal* vertexNormal = inMesh->GetElementNormal(0);
	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inCtrlPointIndex);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inVertexCounter);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void CFbxLoader_V2::ReadUV(FbxMesh* inMesh, int inCtrlPointIndex, int inUVPointIndex, int inUVLayerNum, XMFLOAT2& outUV)
{
	if (inMesh->GetElementUVCount() < 1)
	{
		throw std::exception("Invalid UV Number");
	}

	FbxGeometryElementUV* vertexUV = inMesh->GetElementUV(inUVLayerNum);
	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(inCtrlPointIndex);
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inUVPointIndex).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inUVPointIndex).mData[1]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(inUVPointIndex);
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}


void CFbxLoader_V2::ProcessSkeletonHierarchy(FbxNode* inRootNode, CFbxData* loadData)
{
	for (int childIndex = 0; childIndex < inRootNode->GetChildCount(); ++childIndex)
	{
		FbxNode* currNode = inRootNode->GetChild(childIndex);
		ProcessSkeletonHierarchyRecursively(currNode, 0, -1, loadData);
	}

	if (loadData->m_Skeleton.m_vJoints.size() > 0) loadData->m_bHasSkeleton = true;
}

void CFbxLoader_V2::ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int myIndex, int inParentIndex, CFbxData* loadData)
{
	if (inNode->GetNodeAttribute() &&
		inNode->GetNodeAttribute()->GetAttributeType() &&
		inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		CFbxJoint currJoint;
		currJoint.m_nParentIndex = inParentIndex;
		currJoint.m_sName = inNode->GetName();
		loadData->m_Skeleton.m_vJoints.push_back(currJoint);
	}
	for (int i = 0; i < inNode->GetChildCount(); i++)
	{
		ProcessSkeletonHierarchyRecursively(inNode->GetChild(i), loadData->m_Skeleton.m_vJoints.size(), myIndex, loadData);
	}
}




FbxAMatrix CFbxLoader_V2::GetGeometryTransformation(FbxNode* inNode)
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

unsigned int CFbxLoader_V2::FindJointIndexUsingName(std::string JointName, CFbxData* loadData)
{
	auto data = loadData->m_Skeleton.m_vJoints;
	for (int i = 0; i < data.size(); ++i) {
		if (data[i].m_sName == JointName)
			return i;
	}
	return -1;
}

void CFbxLoader_V2::storeFbxAMat2XMFLOAT4x4(XMFLOAT4X4& dest, FbxAMatrix& sorce)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			dest.m[i][j] = static_cast<float>(sorce.Get(i, j));
}

// this function need loaded contrlPoints and Skeleton
void CFbxLoader_V2::ProcessJoints(FbxNode* inNode, CFbxData* loadData)
{
	FbxMesh* currMesh = inNode->GetMesh();
	unsigned int numOfDeformers = currMesh->GetDeformerCount();

	FbxAMatrix geometryTransform = GetGeometryTransformation(inNode);

	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!currSkin) continue;

		unsigned int numOfClusters = currSkin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
			std::string currJointName = currCluster->GetLink()->GetName();
			unsigned int currJointIndex = FindJointIndexUsingName(currJointName, loadData);
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindposeInverseMatrix;

			currCluster->GetTransformMatrix(transformMatrix); // The transformation of the mesh at binding time
			currCluster->GetTransformLinkMatrix(transformLinkMatrix); // The transformation of the cluster(joint) at binding time from joint space to world space
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform; // we need this

			// Update the information in mSkeleton 
			XMFLOAT4X4 xmf4x4BindinverseMat;
			storeFbxAMat2XMFLOAT4x4(xmf4x4BindinverseMat, globalBindposeInverseMatrix);
			loadData->m_Skeleton.m_vJoints[currJointIndex].m_xmf4x4GlobalBindposeInverse = xmf4x4BindinverseMat;
			//loadData->m_Skeleton.m_vJoints[currJointIndex].mNode = currCluster->GetLink();


			// Associate each joint with the control points it affects
			unsigned int numOfIndices = currCluster->GetControlPointIndicesCount();
			for (unsigned int i = 0; i < numOfIndices; ++i)
			{
				BlendingIndexWeightPair currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.m_nBlendingIndex = currJointIndex;
				currBlendingIndexWeightPair.m_fBlendingWeight = currCluster->GetControlPointWeights()[i];
				loadData->m_vCtrlPoints[currCluster->GetControlPointIndices()[i]].m_vBlendingInfo.push_back(currBlendingIndexWeightPair);
			}
		}
	}

	// Some of the control points only have less than 4 joints
	// affecting them.
	// For a normal renderer, there are usually 4 joints
	// I am adding more dummy joints if there isn't enough
	BlendingIndexWeightPair currBlendingIndexWeightPair;
	currBlendingIndexWeightPair.m_nBlendingIndex = 0;
	currBlendingIndexWeightPair.m_fBlendingWeight = 0;
	for (auto itr = loadData->m_vCtrlPoints.begin(); itr != loadData->m_vCtrlPoints.end(); ++itr)
	{
		for (unsigned int i = itr->m_vBlendingInfo.size(); i <= 4; ++i)
		{
			itr->m_vBlendingInfo.push_back(currBlendingIndexWeightPair);
		}
	}
}

void CFbxLoader_V2::ProcessAnimations(FbxNode* inNode, FbxScene* lScene, CFbxData* loadData)
{
	FbxMesh* currMesh = inNode->GetMesh();
	unsigned int numOfDeformers = currMesh->GetDeformerCount();

	FbxAMatrix geometryTransform = GetGeometryTransformation(inNode);

	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!currSkin) continue;

		unsigned int numOfClusters = currSkin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
			std::string currJointName = currCluster->GetLink()->GetName();
			unsigned int currJointIndex = FindJointIndexUsingName(currJointName, loadData);
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindposeInverseMatrix;

			currCluster->GetTransformMatrix(transformMatrix); // The transformation of the mesh at binding time
			currCluster->GetTransformLinkMatrix(transformLinkMatrix); // The transformation of the cluster(joint) at binding time from joint space to world space
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform; // we need this

			// Get animation information
			FbxAnimStack* currAnimStack = lScene->GetSrcObject<FbxAnimStack>(0);

			FbxString animStackName = currAnimStack->GetName();
			loadData->m_Skeleton.m_sAnimationName = animStackName.Buffer();
			loadData->m_Skeleton.m_vAnimationNames.push_back(loadData->m_Skeleton.m_sAnimationName);

			FbxTakeInfo* takeInfo = lScene->GetTakeInfo(animStackName);
			FbxTime time = takeInfo->mLocalTimeSpan.GetDuration().GetFrameCount();

			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();

			loadData->m_Skeleton.m_nAnimationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
			loadData->m_Skeleton.m_vAnimationLengths.push_back(loadData->m_Skeleton.m_nAnimationLength);

			Keyframe** currAnim = &loadData->m_Skeleton.m_vJoints[currJointIndex].m_pAnimFrames;
			for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
			{
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eFrames24);
				*currAnim = new Keyframe();
				(*currAnim)->m_nFrameNum = i;

				FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) * geometryTransform;
				storeFbxAMat2XMFLOAT4x4((*currAnim)->m_xmf4x4GlobalTransform,
					currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime));
				currAnim = &((*currAnim)->m_pNext);
			}
		}
	}
}

void CFbxLoader_V2::ProcessJointsAndAnimation(FbxNode* inNode, FbxScene* lScene, CFbxData* loadData)
{
	FbxMesh* currMesh = inNode->GetMesh();
	unsigned int numOfDeformers = currMesh->GetDeformerCount();

	FbxAMatrix geometryTransform = GetGeometryTransformation(inNode);

	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!currSkin) continue;

		unsigned int numOfClusters = currSkin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
			std::string currJointName = currCluster->GetLink()->GetName();
			unsigned int currJointIndex = FindJointIndexUsingName(currJointName, loadData);
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindposeInverseMatrix;

			currCluster->GetTransformMatrix(transformMatrix); // The transformation of the mesh at binding time
			currCluster->GetTransformLinkMatrix(transformLinkMatrix); // The transformation of the cluster(joint) at binding time from joint space to world space
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform; // we need this

			// Update the information in mSkeleton 
			XMFLOAT4X4 xmf4x4BindinverseMat;
			storeFbxAMat2XMFLOAT4x4(xmf4x4BindinverseMat, globalBindposeInverseMatrix);
			loadData->m_Skeleton.m_vJoints[currJointIndex].m_xmf4x4GlobalBindposeInverse = xmf4x4BindinverseMat;
			//loadData->m_Skeleton.m_vJoints[currJointIndex].mNode = currCluster->GetLink();


			// Associate each joint with the control points it affects
			unsigned int numOfIndices = currCluster->GetControlPointIndicesCount();
			for (unsigned int i = 0; i < numOfIndices; ++i)
			{
				BlendingIndexWeightPair currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.m_nBlendingIndex = currJointIndex;
				currBlendingIndexWeightPair.m_fBlendingWeight = currCluster->GetControlPointWeights()[i];
				loadData->m_vCtrlPoints[currCluster->GetControlPointIndices()[i]].m_vBlendingInfo.push_back(currBlendingIndexWeightPair);
			}


			// Get animation information
			FbxAnimStack* currAnimStack = lScene->GetSrcObject<FbxAnimStack>(0);

			FbxString animStackName = currAnimStack->GetName();
			loadData->m_Skeleton.m_sAnimationName = animStackName.Buffer();
			loadData->m_Skeleton.m_vAnimationNames.push_back(loadData->m_Skeleton.m_sAnimationName);

			FbxTakeInfo* takeInfo = lScene->GetTakeInfo(animStackName);
			FbxTime time = takeInfo->mLocalTimeSpan.GetDuration().GetFrameCount();

			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();

			loadData->m_Skeleton.m_nAnimationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
			loadData->m_Skeleton.m_vAnimationLengths.push_back(loadData->m_Skeleton.m_nAnimationLength);

			Keyframe** currAnim = &loadData->m_Skeleton.m_vJoints[currJointIndex].m_pAnimFrames;
			for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
			{
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eFrames24);
				*currAnim = new Keyframe();
				(*currAnim)->m_nFrameNum = i;

				FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) * geometryTransform;
				storeFbxAMat2XMFLOAT4x4((*currAnim)->m_xmf4x4GlobalTransform,
					currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime));
				currAnim = &((*currAnim)->m_pNext);
			}
		}
	}

	// Some of the control points only have less than 4 joints
	// affecting them.
	// For a normal renderer, there are usually 4 joints
	// I am adding more dummy joints if there isn't enough

	//BlendingIndexWeightPair currBlendingIndexWeightPair;
	//currBlendingIndexWeightPair.m_nBlendingIndex = 0;
	//currBlendingIndexWeightPair.m_fBlendingWeight = 0;
	//for (auto itr = loadData->m_vCtrlPoints.begin(); itr != loadData->m_vCtrlPoints.end(); ++itr)
	//{
	//	for (unsigned int i = itr->m_vBlendingInfo.size(); i <= 4; ++i)
	//	{
	//		itr->m_vBlendingInfo.push_back(currBlendingIndexWeightPair);
	//	}
	//}
}

/*
struct Triangle {
	vector<int> mIndices;
};
struct CtrlPoint {
	XMFLOAT3 mPosition;
};

int mTriangleCount;
vector<Triangle> mTriangles;
vector<CtrlPoint*> mControlPoints;
void ProcessMesh(FbxNode* inNode)
{
	FbxMesh* currMesh = inNode->GetMesh();

	mTriangleCount = currMesh->GetPolygonCount();
	int vertexCounter = 0;
	mTriangles.reserve(mTriangleCount);

	for (unsigned int i = 0; i < mTriangleCount; ++i)
	{
		XMFLOAT3 normal[3];
		XMFLOAT3 tangent[3];
		XMFLOAT3 binormal[3];
		XMFLOAT2 UV[3][2];
		Triangle currTriangle;
		mTriangles.push_back(currTriangle);

		for (unsigned int j = 0; j < 3; ++j)
		{
			int ctrlPointIndex = currMesh->GetPolygonVertex(i, j);
			CtrlPoint* currCtrlPoint = mControlPoints[ctrlPointIndex];


			ReadNormal(currMesh, ctrlPointIndex, vertexCounter, normal[j]);
			// We only have diffuse texture
			for (int k = 0; k < 1; ++k)
			{
				ReadUV(currMesh, ctrlPointIndex, currMesh->GetTextureUVIndex(i, j), k, UV[j][k]);
			}


			PNTIWVertex temp;
			temp.mPosition = currCtrlPoint->mPosition;
			temp.mNormal = normal[j];
			temp.mUV = UV[j][0];
			// Copy the blending info from each control point
			for (unsigned int i = 0; i < currCtrlPoint->mBlendingInfo.size(); ++i)
			{
				VertexBlendingInfo currBlendingInfo;
				currBlendingInfo.mBlendingIndex = currCtrlPoint->mBlendingInfo[i].mBlendingIndex;
				currBlendingInfo.mBlendingWeight = currCtrlPoint->mBlendingInfo[i].mBlendingWeight;
				temp.mVertexBlendingInfos.push_back(currBlendingInfo);
			}
			// Sort the blending info so that later we can remove
			// duplicated vertices
			temp.SortBlendingInfoByWeight();

			mVertices.push_back(temp);
			mTriangles.back().mIndices.push_back(vertexCounter);
			++vertexCounter;
		}
	}

	// Now mControlPoints has served its purpose
	// We can free its memory
	for (auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
	{
		delete itr->second;
	}
	mControlPoints.clear();
}*/