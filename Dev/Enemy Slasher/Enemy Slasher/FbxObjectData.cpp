#include "FbxObjectData.h"

void CFbxSkeletonData::deleteAnimData()
{
	for (int i = 0; i < m_vJoints.size(); ++i) {
		Keyframe* head = m_vJoints[i].m_pAnimFrames;

		while (head != nullptr) {
			Keyframe* curr = head;
			head = head->m_pNext;
			delete curr;
		}
	}
}