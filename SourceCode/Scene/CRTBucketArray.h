#pragma once
#include <vector>
#include "CRTRegion.h"
#include "CRTScene.h"
#include <atomic>

class CRTBucketArray
{
	std::vector<CRTRegion> regions;
	std::atomic_int index;
public:
	CRTBucketArray(CRTScene* scene);
	CRTRegion getRegion(int idx) const;
	bool nextRegion(CRTRegion& region);

	int size() const;
};

