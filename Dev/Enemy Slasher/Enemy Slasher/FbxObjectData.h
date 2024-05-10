#pragma once
#include "stdafx.h"
#include <vector>

class BlendingIndexWeightPair {
public:
	int m_nBlendingIndex;
	double m_fBlendingWeight;
};

class CFbxVertex {
public:
	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT2 m_xmf2UV;
	std::vector<BlendingIndexWeightPair> m_vBlendingInfo;
};

class CFbxCtrlPoint {
public:
	XMFLOAT3 m_xmf3Position;
	std::vector<BlendingIndexWeightPair> m_vBlendingInfo;
};

class Keyframe {
public:
	int m_nFrameNum;
	XMFLOAT4X4 m_xmf4x4GlobalTransform;
	Keyframe* m_pNext;
};

class CFbxJoint {
public:
	CFbxJoint() {};
	~CFbxJoint() {};

public:
	int m_nParentIndex = -1;
	string m_sName;
	XMFLOAT4X4 m_xmf4x4GlobalBindposeInverse;
	Keyframe* m_pAnimFrames;
};

class CFbxSkeletonData {
public:
	CFbxSkeletonData() {};
	~CFbxSkeletonData() {};

public:
	std::vector<CFbxJoint> m_vJoints;

	std::string m_sAnimationName;
	long long m_nAnimationLength;
	std::vector<std::string> m_vAnimationNames;
	std::vector<long long> m_vAnimationLengths;
};

class CFbxData {
public:
	CFbxData() {};
	~CFbxData() {};

public:
	std::vector<CFbxCtrlPoint> m_vCtrlPoints;
	std::vector<CFbxVertex> m_vVertex;
	std::vector<int> m_vIndices;
	CFbxSkeletonData m_Skeleton;
	bool m_bHasSkeleton = false;
};