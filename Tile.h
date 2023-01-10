#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdio.h>
#include <SFML/Graphics/Transformable.hpp>

#include "Building.h"

class Cell;

class Road : public GameObject
{
	public:
		
		int clusterIndex;

		bool isBuilded;
		bool isHorizontal;
		bool isVisited;

		int roadLength;

		Road* prevsRoad;

		static Road* longestRoad;
		static Road* beginLongestRoad;
		static std::vector<std::vector<Road*>> longestRoadClusters;

		int rowIndex;
		int colIndex;
		
		std::vector<void (Road::*)()> updateVector;
		
		Road(float x,float y,bool horVer);
		
		std::vector<Cell*> adjacentCells;
		
		void updateStatus();
		void selectCluster();
		void lookForNearesResources();

		void buildRoad();
		bool isRoadAdjacent(Road* nextRoad);

		bool isMouseIn();

		int isResourceAdjacent(BuildingType type);
		
		void findFarm(); //активируется при нажатии на рынок, меняем булево маркета, накидываем +1 голду за каждую ферму, соединённую с рынком
		void findEnemy(); //активируется при нажатии на форт
		//отписываем текущий форт от нажатия и ставим что он уже активировался
		//каждый найденный противник подписывается на функцию что при нажатии - убиваем этого врага(меняем булево и картинку),прибавляем 1 голду,
		//отписываем каждого противника от события при нажатии и идём на следующий ход

		void findMarketsAndForts(); //просто накидываем +2 золота за каждый маркет и форт что присоединён 

		void findLongestPath();
		void highlightLongestRoad();
		std::vector<Road*> adjacentRoads;
		
};

#endif