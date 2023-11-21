#include "Skeleton.h"


CSkeleton::CSkeleton()
{
}

CSkeleton::CSkeleton(string name, int childNum) {
	m_strName = name;
	m_inChildNum = childNum;
	if(m_inChildNum)
		m_pChild = new CSkeleton*[m_inChildNum];
	else
		m_pChild = NULL;
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
				m_pChild[i] = new CSkeleton(childNode->GetName(), childNode->GetChildCount());
				m_pChild[i]->LoadHierarchy(childNode);
			}
		}
	}
}
void CSkeleton::LoadHierarchyFromMesh(CFBXMesh* pMesh)
{
	CSkeleton* skelList;

	skelList = pMesh->GetSkeletonList();

	for (int j = 0;j < pMesh->GetClusterCount();j++)
	{
		if (m_strName == skelList[j].GetName())
		{
			m_iIndiceNUm = skelList[j].GetIndiceNum();
			m_iWeight = skelList[j].GetWeight();
			m_xmf4x4TransformMatrix = skelList[j].GetTransformMatrix();
			m_xmf4x4TransformLinkMatrix = skelList[j].GetTransformLinkMatrix();
			break;
		}
	}
	if (m_pChild)
	{
		for (int i = 0;i < m_inChildNum;i++)
		{
			m_pChild[i]->LoadHierarchyFromMesh(pMesh);
		}
	}
}

void CSkeleton::SetData(string name, int indiceNum, double weight, FbxAMatrix transformMatrix, FbxAMatrix transformLinkMatrix)
{
	m_strName = name;
	m_iIndiceNUm = indiceNum;
	m_iWeight = weight;

	XMFLOAT4X4 result;

	FbxAMatrix lMatrix = transformMatrix;

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			// FbxAMatrix는 행 우선이므로 행과 열을 바꿔주어야 합니다.? 아닐텐데?
			//result.m[row][col] = static_cast<float>(lMatrix[col][row]);
			result.m[row][col] = static_cast<float>(lMatrix[row][col]);
		}
	}
}