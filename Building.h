#ifndef BUILDING_H
#define BUILDING_H

#include "GameObject.h"
#include <iostream>
#include <vector>
/*#include "Grid.h"
#include "Tile.h"*/
class Road;
class Cell;

enum class BuildingType
{
	Tree,
	Rock,
	Farm,
	Market,
	Fort,
	Castle,
	Enemy,
	Lake
};

class Building : public GameObject
{
	public:

		Cell* parentCell;
		int treeCount,rockCount;
		int points;
		Building(BuildingType buildingType);
		~Building();
		
		bool isConnected;
		bool isUsed;
		bool isKilled;
		bool isCounted; //срань для защиты от повторной проверки, нашли ли этот этот элемнет
						//защита для ферм при использовании рынка
						//защита для рынков и фортов при исользовании замка

		int roundUsed;  //вот жту херню можно вообще потом удалить
		std::vector<Road*> adjacentRoad;
		std::vector<Building*> adjacentsBuildings;
		void updateStatus();
		BuildingType buildingType;
		std::vector<void (Building::*)()> updateVector; //вот жто тоже удалить, уже есть указатель на метод, котораый очень даже неплохо справляется
		void (Building::*updateMethod)();
		
		void activateFarm();
		void activateMarket();
		void activateFort();
		void activateCastle();
		void killEnemy();
		
		std::string getBuildingType();
		void decreaseTreeCount();
		void decreaseRockCount();
};

#endif