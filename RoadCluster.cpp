#include "RoadCluster.h"

#include <iostream>
#include <vector>

#include "Tile.h"

std::vector<RoadCluster*> RoadCluster::roadClusterVector;
int RoadCluster::staticId;

RoadCluster::RoadCluster(){
	this->clusterId = 0;
	this->treesCount = 0;
	this->rocksCount = 0;
}
RoadCluster::~RoadCluster(){

}

void RoadCluster::addRoadToCluster(Road* pRoad){
	pRoad->clusterIndex = RoadCluster::getLastClusterId();
	RoadCluster::roadClusterVector[RoadCluster::roadClusterVector.size()-1]->clusterRoads.push_back(pRoad);
}

void RoadCluster::createNewCluster(){
	RoadCluster::staticId++;
	RoadCluster::roadClusterVector.push_back(new RoadCluster());	
	RoadCluster::roadClusterVector[RoadCluster::roadClusterVector.size()-1]->clusterId = RoadCluster::staticId;
}

int RoadCluster::getLastClusterId(){
	return RoadCluster::roadClusterVector[RoadCluster::roadClusterVector.size()-1]->clusterId;
}

void RoadCluster::initVectorClusters(){
	RoadCluster::staticId = 0;
	if(RoadCluster::roadClusterVector.size() == 0){
		RoadCluster::roadClusterVector.push_back(new RoadCluster());
	}
	else if(RoadCluster::roadClusterVector.size() > 0){
		for(int i = 0;i < RoadCluster::roadClusterVector.size();i++){
			RoadCluster::roadClusterVector[i]->clusterRoads.clear();
			delete RoadCluster::roadClusterVector[i]; //вот тут хуй знает, может быть надо сделать нормальное удаление
		}
		RoadCluster::roadClusterVector.clear();
		RoadCluster::roadClusterVector.shrink_to_fit();
		RoadCluster::roadClusterVector.push_back(new RoadCluster());
	}
}

void RoadCluster::increaseLastClusterResourceCount(int type){
	if(type == 0){
		RoadCluster::roadClusterVector[RoadCluster::roadClusterVector.size()-1]->treesCount++;
	}
	if(type == 1){
		RoadCluster::roadClusterVector[RoadCluster::roadClusterVector.size()-1]->rocksCount++;
	}
}