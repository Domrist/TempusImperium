#ifndef ROADCLUSTER_H
#define ROADCLUSTER_H

#include "Tile.h"
#include <iostream>
#include <vector>

class RoadCluster
{
	public:
		int clusterId;
		int treesCount;
		int rocksCount;
		RoadCluster();
		~RoadCluster();

		std::vector<Road*> clusterRoads;

		static int staticId;
		
		static std::vector<RoadCluster*> roadClusterVector;
		static void addRoadToCluster(Road* pRoad);
		static void createNewCluster();
		static void initVectorClusters();
		static int getLastClusterId();
		static void increaseLastClusterResourceCount(int type);
};

#endif