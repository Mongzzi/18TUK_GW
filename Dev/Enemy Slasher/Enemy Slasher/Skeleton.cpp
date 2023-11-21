#include "Skeleton.h"


CSkeleton::CSkeleton()
{
}

CSkeleton::CSkeleton(string name) {
	m_strName = name;
}

CSkeleton::~CSkeleton()
{
}

void CSkeleton::LoadHierarchy(FbxNode* pNode)
{
	FbxNodeAttribute::EType lAttributeType;
	
	FbxNode* childNode = NULL;
	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		childNode = pNode->GetChild(i);

		if (childNode->GetNodeAttribute() == NULL)
		{
			//FBXSDK_printf("NULL Node Attribute\n\n");
		}
		else
		{
			lAttributeType = (childNode->GetNodeAttribute()->GetAttributeType());

			if (lAttributeType == FbxNodeAttribute::eSkeleton)
			{
				m_pChild = new CSkeleton(childNode->GetName());
				m_pChild->LoadHierarchy(childNode);
			}
		}
	}
}

void CSkeleton::SetData(int indiceNum, double weight, FbxAMatrix transformMatrix, FbxAMatrix transformLinkMatrix)
{
	m_iIndiceNUm = indiceNum;
	m_iWeight = weight;

	XMFLOAT4X4 result;

	FbxAMatrix lMatrix = transformMatrix;

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			// FbxAMatrix는 행 우선이므로 행과 열을 바꿔주어야 합니다.?
			result.m[row][col] = static_cast<float>(lMatrix[col][row]);
		}
	}
}