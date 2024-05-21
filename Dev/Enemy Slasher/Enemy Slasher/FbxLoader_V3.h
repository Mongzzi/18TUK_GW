#pragma once
#include "stdafx.h"
#include "fbxsdk.h"
#include <unordered_map>
#include <string>

namespace CFbx_V3 {
	struct BlendingIndexWeightPair {
		int m_nBlendingIndex;
		float m_fBlendingWeight;

		bool operator < (const BlendingIndexWeightPair& rhs)
		{
			return (m_fBlendingWeight > rhs.m_fBlendingWeight);
		}
	};

	struct VertexData {
		XMFLOAT3 m_xmf3Position;
		XMFLOAT3 m_xmf3Normal;
		XMFLOAT2 m_xmf2UV;

		int m_nlBlendingIndex[4];
		float m_flBlendingWeight[4];

		bool operator == (const VertexData& rhs) const {
			if (m_xmf3Position.x != rhs.m_xmf3Position.x ||
				m_xmf3Position.y != rhs.m_xmf3Position.y ||
				m_xmf3Position.z != rhs.m_xmf3Position.z) return false;

			if (m_xmf3Normal.x != rhs.m_xmf3Normal.x ||
				m_xmf3Normal.y != rhs.m_xmf3Normal.y ||
				m_xmf3Normal.z != rhs.m_xmf3Normal.z) return false;

			if (m_xmf2UV.x != rhs.m_xmf2UV.x ||
				m_xmf2UV.y != rhs.m_xmf2UV.y) return false;

			return true;
		}
	};

	struct MeshData {
		std::vector<VertexData> m_vVertices;
		std::vector<int> m_nIndices;
	};

	struct KeyFrame {
		int m_nFrameNum = 0;
		XMFLOAT4X4 m_xmf4x4AnimMat;
	};

	struct BoneAnimation {

		std::vector<KeyFrame> m_vKeyFrames;
	};

	struct AnimationClip {

		std::vector<BoneAnimation> m_vBoneAnimations;
	};

	struct Skeleton {
		// default skeleton data
		std::vector<std::string> m_vsBoneNames;
		std::vector<int> m_vnBoneHierarcht;
		std::vector<XMFLOAT4X4> m_vxmf4x4BoneOffsetMat;

		// animation datas
		std::vector<std::string> m_vAnimationNames;
		std::unordered_map<std::string, AnimationClip> m_mAnimations;
	};

	struct Material {
		std::string Name;

		XMFLOAT3 Ambient = { 0.0f, 0.0f, 0.0f };
		XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	struct ObjectData {
		ObjectData() {};
		~ObjectData() {
			for (auto& a : m_vpMeshs) delete a;
			for (auto& a : m_vChildObjects) delete a;
		};

		XMFLOAT3 m_xmf3Translate{ 0.f, 0.f, 0.f };
		XMFLOAT3 m_xmf3Rotate{ 0.f, 0.f, 0.f };
		XMFLOAT3 m_xmf3Scale{ 1.f, 1.f, 1.f };

		std::vector<MeshData*> m_vpMeshs;
		std::vector<Material*> m_vpMaterials;
		Skeleton* m_pSkeleton = nullptr;

		std::vector<ObjectData*> m_vChildObjects;
	};

	struct ControlPoint {
		XMFLOAT3 m_xmf3Position;
		std::vector<BlendingIndexWeightPair> m_vBoneInfo;
		std::string m_sBoneName;

		ControlPoint() {
			m_vBoneInfo.reserve(4);
		}

		void SortBlendingInfoByWeight() {
			std::sort(m_vBoneInfo.begin(), m_vBoneInfo.end());
		}
	};

	struct CFbxData {
		ObjectData* m_pRootObjectData;
	};
}

namespace std { // unordered map Using hash
	template <>
	struct hash<CFbx_V3::VertexData> {
		size_t operator()(const CFbx_V3::VertexData& vertex) const {
			size_t h1 = hash<float>()(vertex.m_xmf3Position.x) ^ (hash<float>()(vertex.m_xmf3Position.y) << 1) ^ (hash<float>()(vertex.m_xmf3Position.z) << 2);
			size_t h2 = hash<float>()(vertex.m_xmf3Normal.x) ^ (hash<float>()(vertex.m_xmf3Normal.y) << 1) ^ (hash<float>()(vertex.m_xmf3Normal.z) << 2);
			size_t h3 = hash<float>()(vertex.m_xmf2UV.x) ^ (hash<float>()(vertex.m_xmf2UV.y) << 1);
			return h1 ^ h2 ^ h3;
		}
	};
}

class CFbxLoader_V3
{
public:
	CFbxLoader_V3();
	~CFbxLoader_V3();

public:
	CFbx_V3::CFbxData* LoadFbx(
		const std::string& filePath,
		const std::string& fileName);

	void LoadAnim(
		CFbx_V3::Skeleton* pTargetSkeleton,
		const std::string& filePath,
		const std::string& fileName);

private:
	CFbx_V3::ObjectData* ProgressNodes(FbxNode* lRootNode, FbxScene* lScene, const std::string& fileName);

	void LoadSkeletonHierarch(FbxNode* inNode, int myIndex, int inParentIndex);

	void LoadControlPoints(FbxNode* inNode);
	void LoadVertexAndIndiceWithSkeleton(FbxNode* inNode);
	void LoadVertexAndIndiceNonSkeleton(FbxNode* inNode);
	void LoadAnimation(FbxNode* inNode, FbxScene* lScene, const std::string& clipName, bool isOnlyGetAnim);

	void LoadMaterials(FbxNode* inNode);
	void LoadMaterialAttribute(FbxSurfaceMaterial* pMaterial, CFbx_V3::Material& outMaterial);
	void LoadMaterialTexture(FbxSurfaceMaterial* pMaterial, CFbx_V3::Material& outMaterial);
private:
	FbxAMatrix GetGeometryTransformation(FbxNode* inNode);
	int FindJointIndexUsingName(std::string JointName);
	void storeFbxAMat2XMFLOAT4x4(XMFLOAT4X4& dest, FbxAMatrix& sorce);

	void storeObjectData(CFbx_V3::ObjectData* pObject);

private:
	FbxManager* m_plSdkManager = nullptr;

private:
	// ControlPoint
	std::unordered_map<unsigned int, CFbx_V3::ControlPoint*> m_mControlPoint;
	
	// Mesh Data
	std::vector<CFbx_V3::MeshData*> m_vpMeshs;

	// Animation Object Data
	CFbx_V3::Skeleton* m_pSkeleton;

	// Material
	std::vector<CFbx_V3::Material*> m_vpMaterials;
};