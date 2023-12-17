#include "Skeleton.h"


CSkeleton::CSkeleton()
{
}

CSkeleton::CSkeleton(string name, int childNum) {
	m_strName = name;
	m_inChildNum = childNum;
	if(m_inChildNum)
		m_ppChild = new CSkeleton*[m_inChildNum];
	else
		m_ppChild = NULL;
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
				m_ppChild[i] = new CSkeleton(childNode->GetName(), childNode->GetChildCount());
				m_ppChild[i]->LoadHierarchy(childNode);
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
			m_iIndicesCount = skelList[j].GetIndicesCount();
			m_ipIndices = skelList[j].GetIndices();
			m_dpWeights = skelList[j].GetWeights();
			m_xmf4x4TransformMatrix = skelList[j].GetTransformMatrix();
			m_xmf4x4TransformLinkMatrix = skelList[j].GetTransformLinkMatrix();
			break;
		}
	}
	if (m_ppChild)
	{
		for (int i = 0;i < m_inChildNum;i++)
		{
			m_ppChild[i]->LoadHierarchyFromMesh(pMesh);
		}
	}
}

void CSkeleton::SetData(string name, int indexCount, int* indices, double* weights, FbxAMatrix transformMatrix, FbxAMatrix transformLinkMatrix)
{
	m_strName = name;
	m_iIndicesCount = indexCount;
	m_ipIndices = indices;
	m_dpWeights = weights;

	XMFLOAT4X4 result;

	FbxAMatrix lMatrix = transformMatrix;
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			// FbxAMatrix는 행 우선이므로 행과 열을 바꿔주어야 합니다.? 아닐텐데?
			//result.m[row][col] = static_cast<float>(lMatrix[col][row]);
			result.m[row][col] = static_cast<float>(lMatrix[row][col]);
		}
	}
	m_xmf4x4TransformMatrix = result;

	lMatrix = transformLinkMatrix;
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			result.m[row][col] = static_cast<float>(lMatrix[row][col]);
		}
	}
	m_xmf4x4TransformLinkMatrix = result;
}

XMFLOAT4X4 CSkeleton::GetOffsetMatrix()
{
	XMFLOAT4X4 inverseMat = Matrix4x4::Inverse(m_xmf4x4TransformLinkMatrix);
	XMFLOAT4X4 offMat = Matrix4x4::Multiply(inverseMat, m_xmf4x4TransformMatrix);
	return offMat;
}
