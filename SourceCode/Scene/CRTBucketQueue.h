#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include "CRTRegion.h"
#include "CRTScene.h"

class CRTBucketQueue
{
	std::queue<CRTRegion> regionQueue;
	std::mutex queueMutex;
public:
	CRTBucketQueue(const CRTScene* scene);
	bool getRegion(CRTRegion& result);
};

