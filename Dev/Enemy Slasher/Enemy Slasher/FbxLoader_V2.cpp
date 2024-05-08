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
			LoadMesh(lRootNode, loadData);

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

void CFbxLoader_V2::LoadMesh(FbxNode* lRootNode, CFbxData* loadData)
{
	for (int i = 0; i < lRootNode->GetChildCount(); i++)
	{
		FbxNode* pFbxChildNode = lRootNode->GetChild(i);

		if (pFbxChildNode->GetNodeAttribute() == NULL)
			continue;

		FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

		if (AttributeType != FbxNodeAttribute::eMesh)
			continue;

		FbxMesh* pMesh = pFbxChildNode->GetMesh();

		FbxVector4* pVertices = pMesh->GetControlPoints();

		int iVertexCounter = 0;
		for (int j = 0; j < pMesh->GetPolygonCount(); j++)
		{
			int iNumVertices = pMesh->GetPolygonSize(j);
			assert(iNumVertices == 3);

			for (int k = 0; k < iNumVertices; k++) {
				int iControlPointIndex = pMesh->GetPolygonVertex(j, k);

				CFbxVertex vertex;
				vertex.m_Position.x = static_cast<float>(pVertices[iControlPointIndex].mData[0]);
				vertex.m_Position.y = static_cast<float>(pVertices[iControlPointIndex].mData[1]);
				vertex.m_Position.z = static_cast<float>(pVertices[iControlPointIndex].mData[2]);
				ReadNormal(pMesh, iControlPointIndex, iVertexCounter, vertex.m_Normal);

				//기본 TextureUV 만 로드
				for (int k = 0; k < 1; ++k)
				{
					ReadUV(pMesh, iControlPointIndex, pMesh->GetTextureUVIndex(i, j), k, vertex.m_UV);
				}

				loadData->m_vVertex.push_back(vertex);
				loadData->m_vIndices.push_back(iVertexCounter);
				++iVertexCounter;
			}
		}
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
}

// inNode is the Node in this FBX Scene that contains the mesh
// this is why I can use inNode->GetMesh() on it to get the mesh
void ProcessControlPoints(FbxNode* inNode)
{
	FbxMesh* currMesh = inNode->GetMesh();
	unsigned int ctrlPointCount = currMesh->GetControlPointsCount();
	for (unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		CtrlPoint* currCtrlPoint = new CtrlPoint();
		XMFLOAT3 currPosition;
		currPosition.x = static_cast<float>(currMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = static_cast<float>(currMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = static_cast<float>(currMesh->GetControlPointAt(i).mData[2]);
		currCtrlPoint->mPosition = currPosition;
		mControlPoints[i] = currCtrlPoint;
	}
}
*/