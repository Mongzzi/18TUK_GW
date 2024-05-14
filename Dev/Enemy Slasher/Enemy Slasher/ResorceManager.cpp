#include "ResorceManager.h"

CResorceManager::CResorceManager()
{
	m_pFBXLoader = new CFbxLoader_V2();
}

CResorceManager::~CResorceManager()
{
	if (m_pFBXLoader) delete m_pFBXLoader;
}

CGameObject* CResorceManager::LoadFBXObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* fileName)
{
	std::string fName(fileName);
	CFbxData* loadData = nullptr;

	if (m_mLoadedFBXDataMap.contains(fName) == false) {
		loadData = m_pFBXLoader->LoadFBX(fileName);

		CFBXObject* newGameObject;
		newGameObject = new CFBXObject(pd3dDevice, pd3dCommandList, NULL, ShaderType::NON);
		newGameObject->SetFbxData(pd3dDevice, pd3dCommandList, loadData);

		m_mLoadedFBXDataMap[fName] = newGameObject;
	}

	return m_mLoadedFBXDataMap[fName];
}

CAnimationData* CResorceManager::LoadAnimDataFromFBX(const char* fileName)
{
	std::string fName(fileName);
	CFbxData* loadData = nullptr;

	if (m_mLoadedAnimDataMap.contains(fName) == false) {
		loadData = m_pFBXLoader->LoadAnim(fileName);

		CAnimationData* newAnimData;
		newAnimData = new CAnimationData();
		newAnimData->m_nAnimLen = loadData->m_Skeleton.m_nAnimationLength;
		newAnimData->m_nJoint = loadData->m_Skeleton.m_vJoints.size();
		newAnimData->m_ppxmf4x4AnimMat = new XMFLOAT4X4*[newAnimData->m_nAnimLen];

		Keyframe* nowFrame;
		for (int frame = 0; frame < newAnimData->m_nAnimLen; ++frame) {
			newAnimData->m_ppxmf4x4AnimMat[frame] = new XMFLOAT4X4[newAnimData->m_nJoint];

			for (int joint = 0; joint < newAnimData->m_nJoint; ++joint) {
				nowFrame = loadData->m_Skeleton.m_vJoints[frame].m_pAnimFrames;
				for (int k = 0; k < frame; ++k)
					nowFrame = nowFrame->m_pNext;
				

				newAnimData->m_ppxmf4x4AnimMat[frame][joint] = Matrix4x4::Multiply(loadData->m_Skeleton.m_vJoints[joint].m_xmf4x4GlobalBindposeInverse, nowFrame->m_xmf4x4GlobalTransform);
			}
		}

		m_mLoadedAnimDataMap[fName] = newAnimData;
	}

	return m_mLoadedAnimDataMap[fName];
}
