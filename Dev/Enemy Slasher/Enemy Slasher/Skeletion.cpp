#include "Skeleton.h"


CSkeleton::CSkeleton(string name) {
	m_strName = name;
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