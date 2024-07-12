#include "CRTBucketQueue.h"

CRTBucketQueue::CRTBucketQueue(const CRTScene* scene)
{
	unsigned width = scene->getSettings().imageSettings.width;
	unsigned height = scene->getSettings().imageSettings.height;
	unsigned bucketSize = scene->getSettings().imageSettings.bucketSize;

	unsigned bucketsPerWidth = width / bucketSize;
	unsigned bucketsPerHeight = height / bucketSize;

	unsigned lastBucketWidth = bucketSize + (width % bucketSize);
	unsigned lastBucketHeight = bucketSize + (height % bucketSize); // to avoid leaving out pixels at the edges
	for (int i = 0; i < bucketsPerHeight; i++) {
		for (int j = 0; j < bucketsPerWidth; j++) {
			CRTRegion reg{ j * bucketSize, i * bucketSize, bucketSize, bucketSize };
			if (i == bucketsPerHeight - 1) { 
				// on the last bucket of the column we add the remaining pixels, so it's a bit bigger
				// this is in order to avoid having very small buckets that will require a lot of synchronization
				reg.height = lastBucketHeight;
			}
			if (j == bucketsPerWidth - 1) {
				reg.width = lastBucketWidth;
			}
			regionQueue.push(reg);
		}
	}
}

bool CRTBucketQueue::getRegion(CRTRegion& result)
{
	std::lock_guard<std::mutex> guard(queueMutex); // this locks the queue, so no other threads can access it
	if (regionQueue.empty()) {
		return false;// this is our signal that there's no more work to be done
	}
	result = regionQueue.front(); // we get the first available region in the queue if there's any
	regionQueue.pop();
	return true;
}
